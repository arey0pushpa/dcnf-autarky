#include "defs.h"

/** Constraint 1
 * At Most One Constraint **/
void at_most_one(cl_t& vec, cls_t& cnf_fml) {
  const unsigned N = vec.size();
  if (N <= 1) return;
  for (unsigned i = 0; i < N - 1; i++) {
    for (unsigned j = i + 1; j < N; j++) {
      cnf_fml.push_back(cl_t{-vec[i], -vec[j]});
    }
  }
}

/** Constraint 2 **/
void satisfied_clauses(Clauses dcnf_clauses[], Variables dcnf_variables[],
                       cl_t& pa_vars, cls_t& bf_vars, cls_t& pa_var_set,
                       sel_bf& selected_bf, cls_t& cnf_fml) {
  for (unsigned i = 0; i < pa_var_set.size(); i++) {
    for (unsigned j = 0; j < pa_var_set[i].size(); j = j + 2) {
      lit_t var = pa_var_set[i][j];
      lit_t depdt = pa_var_set[i][j + 1];
      coord_t t_indx = dcnf_variables[std::abs(var) - 1].fetch_eindex();
      coord_t indx = find_scd_index(selected_bf[t_indx], depdt);
      cnf_fml.push_back(cl_t{-pa_vars[i], bf_vars[t_indx][indx]});
    }
  }
}

/** Constraint 3 **/
void touched_clauses(cl_t& cs_vars, cl_t& pa_vars, cls_t& pa_var_set,
                     minsat_ass& S, cls_t& cnf_fml) {
  cl_t v3;
  for (unsigned i = 0; i < cs_vars.size(); i++) {
    v3.push_back(-cs_vars[i]);
    for (auto& c : S[i]) {
      auto id = find_vector_index(pa_var_set, c);
      v3.push_back(pa_vars[id]);
    }
    cnf_fml.push_back(v3);
    v3.clear();
  }
}

/** Constraint 4 **/
void untouched_clauses(Clauses dcnf_clauses[], Variables dcnf_variables[],
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

/** Constraint 5 **/
void non_trivial_autarky(cl_t& cs_vars, cls_t& cnf_fml) {
  cl_t dummy_vec;
  for (lit_t i : cs_vars) dummy_vec.push_back(i);
  cnf_fml.push_back(dummy_vec);
}
