#include <iterator>

#include "defs.h"

void preprocess_fml(Clauses dcnf_clauses[], Variables dcnf_variables[],
                    sel_bf& selected_bf, minsat_ass& minsat_clause_assgmt,
                    const coord_t num_of_clause, const coord_t num_of_vars,
                    const coord_t level) {
  /** Selected Boolean Function **/
  for (coord_t i = 0; i < num_of_vars; ++i) {
    pairs_t t_vec;
    if (dcnf_variables[i].fetch_qtype() == 'e') {
      // Base Case [bf(0), bf(1)]; level == 0
      t_vec.emplace_back(i + 1, 500);   // false
      t_vec.emplace_back(i + 1, 1000);  // true
      if (level > 0) {
        cl_t dvar = dcnf_variables[i].fetch_dependency();
        for (coord_t j = 0; j < dvar.size(); ++j) {
          t_vec.emplace_back(i + 1, dvar[j]);
          t_vec.emplace_back(i + 1, -dvar[j]);
        }
      }
    }
    selected_bf.push_back(t_vec);
  }

  /** Minimal Satisfying Clauses
   * Three cases to consider
   * 1. Basic case: handle all are e_variables
   * 2. Handle dependency case for all e_variable
   * 3. Handle e-var and a-var case
   */

  for (coord_t i = 0; i < num_of_clause; ++i) {
    cls_t m_ca;
    // 1. basic Case
    cl_t elit_part = dcnf_clauses[i].fetch_elits();
    cl_t alit_part = dcnf_clauses[i].fetch_alits();
    cl_t evar_part = dcnf_clauses[i].fetch_evars();
    for (lit_t e : elit_part) {
      if (e > 0) {
        m_ca.push_back(cl_t{e, 1000});
      } else {
        m_ca.push_back(cl_t{std::abs(e), 500});
      }
    }
    if (level > 0) {
      coord_t esize = evar_part.size();
      // 2. e-var pairs case
      for (coord_t e1 = 0; e1 < esize - 1; ++e1) {
        for (coord_t e2 = e1 + 1; e2 < esize; ++e2) {
          cl_t dep_e1 = dcnf_variables[evar_part[e1] - 1].fetch_dependency();
          cl_t dep_e2 = dcnf_variables[evar_part[e2] - 1].fetch_dependency();
          cl_t common_dependency = vector_intersection(dep_e1, dep_e2);
          for (const lit_t& d : common_dependency) {
            cl_t sat_ca1 = {evar_part[e1], d, evar_part[e2], -d};
            cl_t sat_ca2 = {evar_part[e1], -d, evar_part[e2], d};
            m_ca.push_back(sat_ca1);
            m_ca.push_back(sat_ca2);
          }
        }
      }
      // 3. e-var a-var case **
      for (lit_t e : evar_part) {
        cl_t dep_e = dcnf_variables[e-1].fetch_dependency();
        for (lit_t a : alit_part) {
          if (std::find(dep_e.begin(), dep_e.end(), std::abs(a)) !=
              dep_e.end()) {
            if (a < 0)
              m_ca.push_back(cl_t{e, a});
            else
              m_ca.push_back(cl_t{e, -a});
          }
        }
      }
    }  // level > 0 close
    minsat_clause_assgmt.push_back(m_ca);
  }

  for (coord_t i = 0; i < num_of_clause; ++i) {
    auto l = dcnf_clauses[i].fetch_evars();
    std::cout << "The " << i << "th lit-set is: ";
    print_1d_vector(l);
    std::cout << '\n';
  }

  std::cout << "\nThe generated min sat clause assgmt S(C) is: "
            << "\n";
  print_3d_vector(minsat_clause_assgmt);
}
