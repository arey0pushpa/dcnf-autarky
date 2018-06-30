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
void satisfied_clauses(cl_t& e_var, cl_t& pa_var, cls_t& dep_set, cls_t& bf_var,
                       cls_t& dummy_pa, sel_bf& T, cls_t& cnf_fml) {
  for (unsigned i = 0; i < dummy_pa.size(); i++) {
    for (unsigned j = 0; j < dummy_pa[i].size(); j = j + 2) {
      auto var = dummy_pa[i][j];
      auto depdt = dummy_pa[i][j + 1];
      auto t_indx = find_index(e_var, var);
      auto indx = find_scd_index(T[t_indx], depdt);
      cnf_fml.push_back(cl_t{-pa_var[i], bf_var[t_indx][indx]});
    }
  }
}

/** Constraint 3 **/
void touched_clauses(cl_t& cs_var, cl_t& pa_var, cls_t& dummy_pa, minsat_ass& S,
                     cls_t& cnf_fml) {
  cl_t v3;
  for (unsigned i = 0; i < cs_var.size(); i++) {
    v3.push_back(-cs_var[i]);
    for (auto& c : S[i]) {
      auto id = find_vector_index(dummy_pa, c);
      // std::cout << "the chosen pa-var:" << pa_var[id] << "\n";
      v3.push_back(pa_var[id]);
    }
    cnf_fml.push_back(v3);
    v3.clear();
  }
}

/** Constraint 4 **/
void untouched_clauses(cl_t& e_var, cl_t& cs_var, cls_t& bf_var,
                       cls_t& dcnf_fml, cls_t& cnf_fml) {
  for (unsigned i = 0; i < dcnf_fml.size(); i++) {
    for (auto d : dcnf_fml[i]) {
      auto id = find_index(e_var, abs(d));
      if (id < 0) {
        continue;
      } else {
        for (auto l : bf_var[id]) cnf_fml.push_back(cl_t{cs_var[i], -l});
      }
    }
  }
}

/** Constraint 5 **/
void non_trivial_autarky(cl_t& cs_var, cls_t& cnf_fml) {
  cl_t dummy_vec;
  for (auto i : cs_var) dummy_vec.push_back(i);
  cnf_fml.push_back(dummy_vec);
}
