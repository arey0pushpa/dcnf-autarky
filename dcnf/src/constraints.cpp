#include "dcnf.h"
#include "defs.h"
#include "util.h"

/** Create lbf formula **/
 cl_t lbf_formula(cl_t lbf_vars, lit_t bf_var) {
   coord_t blen = 0;
   boolv_t binary_repr;
   cl_t fml_repr;

   while (bf_var > 0) {
     binary_repr.push_back(bf_var % 2);
     bf_var = bf_var / 2;
     ++blen;
   }
   assert(blen <= lbf_vars.size());
   // Enforce the resultant vector is of size of lbf_vars
   for (coord_t i = blen; i < lbf_vars.size(); ++i) {
     binary_repr.push_back(0);
   }
   for (coord_t i = 0; i < lbf_vars.size(); ++i) {
     (binary_repr[i] == 0 ? fml_repr.push_back(-lbf_vars[i])
                          : fml_repr.push_back(lbf_vars[i]));
   }
   return fml_repr;
 }


/** 4.1. /\_f,f' !t(v,f) || !t(v,f')
 * At Most One Constraint **/
void dcnf::at_most_one(cl_t &tbf_vars, cls_t &cnf_fml) {
  const unsigned N = tbf_vars.size();
  if (N <= 1) return;
  for (unsigned i = 0; i < N - 1; i++) {
    for (unsigned j = i + 1; j < N; j++) {
      cnf_fml.push_back(cl_t{-tbf_vars[i], -tbf_vars[j]});
    }
  }
}

/** 4.1. Linear encoding: AMO constraint, **/
void dcnf::at_most_one_linear(cl_t &tbf_vars, cls_t &cnf_fml, lit_t &index) {
  cl_t dummy_tbf_vars = tbf_vars;
  const unsigned N = tbf_vars.size();
  if (N <= 1) return;
  // Base case: seco(v1,v2,v3,v4)
  if (N <= 4) {
    at_most_one(tbf_vars, cnf_fml);
    return;
  }
  // TODO: Avoid reverse and dropping the element
  std::reverse(dummy_tbf_vars.begin(), dummy_tbf_vars.end());
  // AMO for first 3 vars: amo(v1,v2,v3)
  lit_t v1 = dummy_tbf_vars[N - 1];
  dummy_tbf_vars.pop_back();
  lit_t v2 = dummy_tbf_vars[N - 2];
  dummy_tbf_vars.pop_back();
  lit_t v3 = dummy_tbf_vars[N - 3];
  dummy_tbf_vars.pop_back();
  cl_t binomial_vars = {v1, v2, v3};
  at_most_one(binomial_vars, cnf_fml);

  // Commander variable constraint: v1->w, v2->w, v3->w
  cnf_fml.push_back(cl_t{-v1, index});
  cnf_fml.push_back(cl_t{-v2, index});
  cnf_fml.push_back(cl_t{-v3, index});

  // uep constraint
  cnf_fml.push_back(cl_t{-index, v1, v2, v3});

  // Add w to the front
  dummy_tbf_vars.push_back(index);

  ++index;

  // recursive call: seco(w,v3,...,vn)
  at_most_one_linear(dummy_tbf_vars, cnf_fml, index);
}

/** 4.2: t(phi) -> /\_v t(v,phi(v)) **/
// TODO: Check the code for Varibles mis-match and wrong var use
void dcnf::satisfied_clauses(cl_t &lbf_vars, cls_t &bf_vars,
                             minsat_ass &pa_var_msat_ass,
                             cls_t &msat_concrete_var_map, cls_t &cnf_fml,
                             std::vector<bf_lbf_converter> &bf2lbf_var_map,
                             cl_t &active_evar_index) {
  for (unsigned i = 0; i < pa_var_msat_ass.size(); ++i) {
    for (unsigned j = 0; j < pa_var_msat_ass[i].size(); ++j) {
      cl_t v2;
      v2.push_back(msat_concrete_var_map[i][j]);
      for (unsigned k = 0; k < pa_var_msat_ass[i][j].size(); k = k + 2) {
        lit_t var = pa_var_msat_ass[i][j][k];
        lit_t depdt = pa_var_msat_ass[i][j][k + 1];
        coord_t v_indx = active_evar_index[std::abs(var) - 1];
        coord_t d_indx = find_scd_index(selected_bf[v_indx], depdt);
        lit_t current_bf_var = bf_vars[v_indx][d_indx];
        if (encoding == 1) {
          // In case of LOG encoding bf_var = lbf_var1 && ... && lbf_varm
					// TODO: check off by one error in the logic
          coord_t bf_id =
              current_bf_var - present_clauses.size(); 
          if (bf2lbf_var_map[bf_id].is_present == 0) {
            bf2lbf_var_map[bf_id].is_present = 1;
            bf2lbf_var_map[bf_id].lbf_fml = lbf_formula(lbf_vars, bf_id);
          }
          cl_t cls_lbf = bf2lbf_var_map[bf_id].lbf_fml;
          for (lit_t li : cls_lbf) {
            v2.push_back(-li);
            cnf_fml.push_back(cl_t{-msat_concrete_var_map[i][j], li});
          }
        } else {
          v2.push_back(-current_bf_var);
          cnf_fml.push_back(cl_t{-msat_concrete_var_map[i][j], current_bf_var});
        }
      }
      cnf_fml.push_back(v2);
    }
  }
}

/** 4.3. !t(C) || \/_phi t(phi)  **/
void dcnf::touched_clauses(cl_t &cs_vars, cls_t &clausewise_pa_var_map,
                           cls_t &cnf_fml) {
  cl_t t_phi_vector;
  for (coord_t i = 0; i < cs_vars.size(); ++i) {
    t_phi_vector.push_back(-cs_vars[i]);
    for (coord_t j = 0; j < clausewise_pa_var_map[i].size(); ++j) {
      t_phi_vector.push_back(clausewise_pa_var_map[i][j]);
    }
    cnf_fml.push_back(t_phi_vector);
    t_phi_vector.clear();
  }
}

/** 4.4. /\_v,f t(C) || !t(v,f) **/
void dcnf::untouched_clauses(cl_t &lbf_vars, cls_t &bf_vars, cl_t &cs_vars,
                             cls_t &cnf_fml,
                             std::vector<bf_lbf_converter> &bf2lbf_var_map,
                             cl_t &present_cls_index, cl_t &active_evar_index) {
  for (const lit_t c : present_clauses) {
    cl_t clause = dcnf_clauses[c].evars;
    for (lit_t e : clause) {
      coord_t indx = active_evar_index[e - 1];
      if (encoding == 1) {
        // TODO: Make sure overflowing bits too are the bottom
        for (lit_t l : lbf_vars) {
          cnf_fml.push_back(cl_t{cs_vars[present_cls_index[c]], -l});
        }
      } else {
        for (lit_t l : bf_vars[indx])
          cnf_fml.push_back(cl_t{cs_vars[present_cls_index[c]], -l});
      }
    }
  }
}

/** 4.5. t(C) **/
void dcnf::non_trivial_autarky(cl_t &cs_vars, cls_t &cnf_fml) {
  cl_t dummy_vec;
  for (lit_t i : cs_vars) dummy_vec.push_back(i);
  cnf_fml.push_back(dummy_vec);
}
