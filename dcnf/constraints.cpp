#include "defs.h"

/** 4.1. /\_f,f' !t(v,f) || !t(v,f')
 * At Most One Constraint **/
void at_most_one(cl_t &bf_vars, cls_t &cnf_fml) {
  const unsigned N = bf_vars.size();
  if (N <= 1)
    return;
  for (unsigned i = 0; i < N - 1; i++) {
    for (unsigned j = i + 1; j < N; j++) {
      cnf_fml.push_back(cl_t{-bf_vars[i], -bf_vars[j]});
    }
  }
}

/** 4.2: t(phi) -> /\_v t(v,phi(v)) **/
void satisfied_clauses(coord_t encoding, coord_t no_of_clauses, cl_t& lbf_vars,
                       std::vector<Clauses> dcnf_clauses,
                       std::vector<Variables> dcnf_variables, cls_t &bf_vars,
                       minsat_ass &pa_var_msat_ass,
                       cls_t &msat_concrete_var_map, sel_bf &selected_bf,
                       cls_t &cnf_fml,
                       std::vector<bf_lbf_converter> &bf2lbf_var_map) {
  for (unsigned i = 0; i < pa_var_msat_ass.size(); ++i) {
    for (unsigned j = 0; j < pa_var_msat_ass[i].size(); ++j) {
      cl_t v2;
      v2.push_back(msat_concrete_var_map[i][j]);
      for (unsigned k = 0; k < pa_var_msat_ass[i][j].size(); k = k + 2) {
        lit_t var = pa_var_msat_ass[i][j][k];
				//std::cout << "Existential min ass var is: " << var << "\n";
        lit_t depdt = pa_var_msat_ass[i][j][k + 1];
				//std::cout << "Univ min ass dep var is: " << depdt << "\n";
        coord_t t_indx = dcnf_variables[std::abs(var) - 1].eindex();
				//std::cout << "Existential min ass var index is: " << t_indx << "\n";
        coord_t indx = find_scd_index(selected_bf[t_indx], depdt);
				//std::cout << "Univ min ass dep var index  is: " << indx << "\n";
        lit_t current_bf_var = bf_vars[t_indx][indx];
				//std::cout << "The current bf var is: " << current_bf_var << "\n\n";
        // In case of LOG encoding bf_var = lbf_var1 && ... && lbf_varm
        coord_t bf_id = current_bf_var - no_of_clauses;
        if (encoding == 1) {
          if (bf2lbf_var_map[bf_id].is_present == 0) {
            bf2lbf_var_map[bf_id].is_present = 1;
            bf2lbf_var_map[bf_id].lbf_fml = lbf_fml(lbf_vars, current_bf_var);
          }
          cl_t cls_lbf = bf2lbf_var_map[bf_id].lbf_fml;
          for (lit_t li : cls_lbf) {
            v2.push_back(-li);
          }
        } else {
          v2.push_back(-current_bf_var);
        }
        if (encoding == 1) {
          cl_t cls_lbf = bf2lbf_var_map[bf_id].lbf_fml;
          for (lit_t li : cls_lbf) {
            cnf_fml.push_back(cl_t{-msat_concrete_var_map[i][j], li});
          }
        } else {
          cnf_fml.push_back(
              cl_t{-msat_concrete_var_map[i][j], current_bf_var});
        }
      }
      cnf_fml.push_back(v2);
    }
  }
}

/** 4.3. !t(C) || \/_phi t(phi)  **/
void touched_clauses(cl_t &cs_vars, cls_t &clausewise_pa_var_map,
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
void untouched_clauses(const coord_t encoding, cl_t& lbf_vars,
                       const std::vector<Clauses> dcnf_clauses,
                       const std::vector<Variables> dcnf_variables,
                       cls_t &bf_vars, cl_t &cs_vars,
                       const coord_t &num_of_clause, cls_t &cnf_fml,
                       std::vector<bf_lbf_converter> &bf2lbf_var_map) {
  for (coord_t i = 0; i < num_of_clause; ++i) {
    cl_t clause = dcnf_clauses[i].evars();
    for (lit_t e : clause) {
      coord_t indx = dcnf_variables[e - 1].eindex();
      if (encoding == 1) {
        for (lit_t l : lbf_vars) {
          cnf_fml.push_back(cl_t{cs_vars[i], -l});
        }
      } else {
        for (lit_t l : bf_vars[indx])
          cnf_fml.push_back(cl_t{cs_vars[i], -l});
      }
    }
  }
}

/** 4.5. t(C) **/
void non_trivial_autarky(cl_t &cs_vars, cls_t &cnf_fml) {
  cl_t dummy_vec;
  for (lit_t i : cs_vars)
    dummy_vec.push_back(i);
  cnf_fml.push_back(dummy_vec);
}
