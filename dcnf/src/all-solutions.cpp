#include <algorithm>
#include <iterator>

#include "dcnf.h"
#include "defs.h"
#include "util.h"

void dcnf::set_all_solutions(const coord_t level) {
  /** Selected Boolean Function **/
  for (lit_t e : e_vars) {
    pairs_t t_vec;
    // Todo: remove pair and only implement by second elem
    // Base Case [bf(0), bf(1)]; level == 0
    t_vec.emplace_back(e, no_of_vars + 1);  // false
    t_vec.emplace_back(e, no_of_vars + 2);  // true
    if (level > 0) {
      cl_t dvar = dcnf_variables[e - 1].dependency;
      for (coord_t j = 0; j < dvar.size(); ++j) {
        t_vec.emplace_back(e, dvar[j]);
        t_vec.emplace_back(e, -dvar[j]);
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

  // This is non-optimal; find a way to implement only for the present clauses
  for (coord_t i = 0; i < dcnf_clauses.size(); ++i) {
    cls_t m_ca;
    // 1. e-literals set to true
    cl_t elit_part = dcnf_clauses[i].elits;
    cl_t alit_part = dcnf_clauses[i].alits;
    cl_t evar_part = dcnf_clauses[i].evars;
    for (lit_t e : elit_part) {
      if (e > 0) {
        m_ca.push_back(cl_t{e, no_of_vars + 2});
      } else {
        m_ca.push_back(cl_t{std::abs(e), no_of_vars + 1});
      }
    }

    if (level > 0) {
      coord_t esize = evar_part.size();
      pairs_t V;
      // 2. e-literal as neg of a-literal case **
      for (lit_t e : elit_part) {
        cl_t dep_e = dcnf_variables[std::abs(e) - 1].dependency;
        for (lit_t a : alit_part) {
          if (std::find(dep_e.begin(), dep_e.end(), std::abs(a)) !=
              dep_e.end()) {
            if (e * a >= 1) {
              m_ca.push_back(cl_t{std::abs(e), -std::abs(a)});
              V.emplace_back(std::abs(e), -std::abs(a));
            } else {
              m_ca.push_back(cl_t{std::abs(e), std::abs(a)});
              V.emplace_back(std::abs(e), std::abs(a));
            }
          }
        }
      }
      /* 3. e-literal negation of another e-literal case */
      for (coord_t e1 = 0; e1 + 1 < esize; ++e1) {
        for (coord_t e2 = e1 + 1; e2 < esize; ++e2) {
          cl_t dep_e1 = dcnf_variables[evar_part[e1] - 1].dependency;
          cl_t dep_e2 = dcnf_variables[evar_part[e2] - 1].dependency;
          cl_t common_dependency = vector_intersection(dep_e1, dep_e2);
          for (const lit_t& d : common_dependency) {
            const lit_t e1_lit = elit_part[e1];
            const lit_t e2_lit = elit_part[e2];
            pair_t p1 = std::make_pair(std::abs(e1_lit), d);
            pair_t p2 = std::make_pair(std::abs(e2_lit), -d);
            pair_t p3 = std::make_pair(std::abs(e1_lit), -d);
            pair_t p4 = std::make_pair(std::abs(e2_lit), d);
            if (e1_lit * e2_lit >= 1) {
              if (!(pair_present(V, p1) || pair_present(V, p2))) {
                cl_t sat_ca1 = {std::abs(e1_lit), d, std::abs(e2_lit), -d};
                m_ca.push_back(sat_ca1);
              }
              if (!(pair_present(V, p3) || pair_present(V, p4))) {
                cl_t sat_ca2 = {std::abs(e1_lit), -d, std::abs(e2_lit), d};
                m_ca.push_back(sat_ca2);
              }
            } else {
              if (!(pair_present(V, p1) || pair_present(V, p4))) {
                cl_t sat_ca1 = {std::abs(e1_lit), d, std::abs(e2_lit), d};
                m_ca.push_back(sat_ca1);
              }
              if (!(pair_present(V, p2) || pair_present(V, p3))) {
                cl_t sat_ca2 = {std::abs(e1_lit), -d, std::abs(e2_lit), -d};
                m_ca.push_back(sat_ca2);
              }
            }
          }
        }
      }

    }  // level > 0 close
    minsat_clause_assgmt.push_back(m_ca);
  }
}
