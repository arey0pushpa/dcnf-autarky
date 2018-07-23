#include <iterator>
#include <algorithm>

#include "defs.h"

void preprocess_fml( std::vector<Clauses>& dcnf_clauses,
                     std::vector<Variables>& dcnf_variables,
                    sel_bf& selected_bf, minsat_ass& minsat_clause_assgmt,
                    const coord_t num_of_clause, const coord_t num_of_vars,
                    const coord_t level) {
  /** Selected Boolean Function **/
  for (coord_t i = 0; i < num_of_vars; ++i) {
    pairs_t t_vec;
    if (dcnf_variables[i].qtype() == 'e') {
      // Todo: remove pair and only implement by second elem
      // Base Case [bf(0), bf(1)]; level == 0
      t_vec.emplace_back(i + 1, num_of_vars + 1);  // false
      t_vec.emplace_back(i + 1, num_of_vars + 2);  // true
      if (level > 0) {
        cl_t dvar = dcnf_variables[i].dependency();
        for (coord_t j = 0; j < dvar.size(); ++j) {
          t_vec.emplace_back(i + 1, dvar[j]);
          t_vec.emplace_back(i + 1, -dvar[j]);
        }
      }
      selected_bf.push_back(t_vec);
    }
  }

  /** Minimal Satisfying Clauses
   * Three cases to consider
   * 1. Basic case: handle all are e_variables
   * 2. Handle dependency case for all e_variable
   * 3. Handle e-var and a-var case
   */

  for (coord_t i = 0; i < num_of_clause; ++i) {
    cls_t m_ca;
    // 1. e-literals set to true
    cl_t elit_part = dcnf_clauses[i].elits();
    cl_t alit_part = dcnf_clauses[i].alits();
    cl_t evar_part = dcnf_clauses[i].evars();
    for (lit_t e : elit_part) {
      if (e > 0) {
        m_ca.push_back(cl_t{e, num_of_vars + 2});
      } else {
        m_ca.push_back(cl_t{std::abs(e), num_of_vars + 1});
      }
    }
    
    if (level > 0) {
      coord_t esize = evar_part.size();
      std::cout <<  "The esize is: " << esize << "\n";

      pairs_t V;
      // 2. e-literal as neg of a-literal case **
      for (lit_t e : evar_part) {
        cl_t dep_e = dcnf_variables[e - 1].dependency();
        for (lit_t a : alit_part) {
          if (std::find(dep_e.begin(), dep_e.end(), std::abs(a)) !=
              dep_e.end()) {
            if (a < 0) {
              m_ca.push_back(cl_t{e, std::abs(a)});
              V.emplace_back(cl_t{e, std::abs(a)});
            }
            else {
              m_ca.push_back(cl_t{e, -a});
              V.emplace_back(cl_t{e, -a});
            }
          }
        }
      }

      /* 3. e-literal negation of another e-literal case */
      for (coord_t e1 = 0; e1+1 < esize; ++e1) {
        for (coord_t e2 = e1 + 1; e2 < esize; ++e2) {
          cl_t dep_e1 = dcnf_variables[evar_part[e1] - 1].dependency();
          cl_t dep_e2 = dcnf_variables[evar_part[e2] - 1].dependency();
          cl_t common_dependency = vector_intersection(dep_e1, dep_e2);
          for (const lit_t& d : common_dependency) {
            const lit_t e1_var = evar_part[e1];
            const lit_t e2_var = evar_part[e2];
            if(std::find(V.begin(), V.end(), pair_t{e1_var,d}) != V.end()) {
              continue;
            }
            else if(std::find(V.begin(), V.end(), pair_t{e2_var,-d}) != V.end()) {
              continue;
            } else {
              cl_t sat_ca1 = {e1_var, d, e2_var, -d};
              m_ca.push_back(sat_ca1);
            }
            if(std::find(V.begin(), V.end(), pair_t{e1_var,-d}) != V.end()) {
              continue;
            }
            else if(std::find(V.begin(), V.end(), pair_t{e2_var,d}) != V.end()) {
              continue;
            }
            else {
              cl_t sat_ca2 = {e1_var, -d, e2_var, d};
              m_ca.push_back(sat_ca2);
            }
          }
          
        }
      }
    }  // level > 0 close
    minsat_clause_assgmt.push_back(m_ca);
  }
}
