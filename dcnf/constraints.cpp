#include "defs.h"

/** 4.1. /\_f,f' !t(v,f) || !t(v,f') 
 * At Most One Constraint **/
void at_most_one(cl_t& bf_vars, cls_t& cnf_fml) {
  const unsigned N = bf_vars.size();
  if (N <= 1) return;
  for (unsigned i = 0; i < N - 1; i++) {
    for (unsigned j = i + 1; j < N; j++) {
      cnf_fml.push_back(cl_t{-bf_vars[i], -bf_vars[j]});
    }
  }
}

/** 4.2: t(phi) -> /\_v t(v,phi(v)) **/
void satisfied_clauses(const std::vector<Clauses> dcnf_clauses, const std::vector<Variables> dcnf_variables,
                       cl_t& pa_vars, cls_t& bf_vars, cls_t& pa_var_set,
                       sel_bf& selected_bf, cls_t& cnf_fml) {
  for (unsigned i = 0; i < pa_var_set.size(); ++i) {
    cl_t v2;
    v2.push_back(pa_vars[i]);
    for (unsigned j = 0; j < pa_var_set[i].size(); j = j + 2) {
      lit_t var = pa_var_set[i][j];
      lit_t depdt = pa_var_set[i][j + 1];
      coord_t t_indx = dcnf_variables[std::abs(var) - 1].fetch_eindex();
      coord_t indx = find_scd_index(selected_bf[t_indx], depdt);
      v2.push_back(-bf_vars[t_indx][indx]);
      cnf_fml.push_back(cl_t{-pa_vars[i], bf_vars[t_indx][indx]});
    }
    cnf_fml.push_back(v2);
  }
}

/** 4.3. !t(C) || \/_phi t(phi)  **/
void touched_clauses(const std::vector<Clauses> dcnf_clauses, const std::vector<Variables> dcnf_variables,
                     cl_t& cs_vars, cl_t& pa_vars, cls_t& pa_var_set,
                     minsat_ass& minsat_clause_assgmt, cls_t& cnf_fml) {
  cl_t t_phi_vector;
  for (unsigned i = 0; i < cs_vars.size(); ++i) {
    t_phi_vector.push_back(-cs_vars[i]);
    for (auto& c : minsat_clause_assgmt[i]) {
      coord_t id = find_vector_index(pa_var_set, c);
      t_phi_vector.push_back(pa_vars[id]);
    }
    cnf_fml.push_back(t_phi_vector);
    t_phi_vector.clear();
  }
}

/** 4.4. /\_v,f t(C) || !t(v,f) **/
void untouched_clauses(const std::vector<Clauses> dcnf_clauses, const std::vector<Variables> dcnf_variables,
                       cls_t& bf_vars, cl_t& cs_vars,
                       const coord_t& num_of_clause, cls_t& cnf_fml) {
  for (coord_t i = 0; i < num_of_clause; ++i) {
    cl_t clause = dcnf_clauses[i].fetch_evars();
    for (lit_t e : clause) {
      coord_t indx = dcnf_variables[e - 1].fetch_eindex();
      for (lit_t l : bf_vars[indx]) cnf_fml.push_back(cl_t{cs_vars[i], -l});
    }
  }
}

/** 4.5. t(C) **/
void non_trivial_autarky(cl_t& cs_vars, cls_t& cnf_fml) {
  cl_t dummy_vec;
  for (lit_t i : cs_vars) dummy_vec.push_back(i);
  cnf_fml.push_back(dummy_vec);
}
