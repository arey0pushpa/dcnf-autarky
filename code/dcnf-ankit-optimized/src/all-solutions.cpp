#include <algorithm>
#include <iterator>

#include "dcnf.h"
#include "defs.h"
#include "util.h"

/** Selected Boolean Function **/
void dcnf::selected_boolfunc(const coord_t aut_level) {
  selected_bf.clear();
  for (lit_t e : active_evars) {
    pairs_t t_vec;
    t_vec.emplace_back(e, no_of_vars + 1);  // false
    t_vec.emplace_back(e, no_of_vars + 2);  // true
    if (aut_level > 0) {
      cl_t dvar = dcnf_variables[e - 1].dependency;
      for (coord_t j = 0; j < dvar.size(); ++j) {
        if (!dcnf_variables[dvar[j] - 1].present) continue;
        t_vec.emplace_back(e, -dvar[j]);
        t_vec.emplace_back(e, dvar[j]);
      }
    }
    selected_bf.push_back(t_vec);
  }
}

/** Minimal Satisfying Clauses
 * Three cases to consider
 * 1. Basic case: handle all are e_variables
 * 2. Handle dependency case for all e_variable
 * 3. Handle e-var and a-var case */
void dcnf::min_satisfying_assgn(const coord_t aut_level) {
  // This is non-optimal; find a way to implement only for the present clauses
  for (coord_t i = 0; i < dcnf_clauses.size(); ++i) {
    cls_t m_ca;
    // 1. e-literals set to true
    cl_t elit_part = dcnf_clauses[i].elits;
    cl_t alit_part = dcnf_clauses[i].alits;
    cl_t evar_part = dcnf_clauses[i].evars;
    cl_t avar_part = dcnf_clauses[i].avars;
    for (lit_t e : elit_part) {
      if (e > 0) {
        m_ca.push_back(cl_t{e, no_of_vars + 2});
      } else {
        m_ca.push_back(cl_t{std::abs(e), no_of_vars + 1});
      }
    }

    if (aut_level > 0) {
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
          for (const lit_t ed1 : dep_e1) {
            for (const lit_t ed2 : dep_e2) {
              if (ed1 < ed2) {
                break;
              } else if (ed1 > ed2) {
                continue;
              } else {
                assert(ed1 == ed2);
                if (std::find(avar_part.begin(), avar_part.end(), ed1) !=
                    avar_part.end()) {
                  continue;
                } else {
                  const lit_t e1_lit = elit_part[e1];
                  const lit_t e2_lit = elit_part[e2];
                  if (e1_lit * e2_lit > 0) {
                    cl_t sat_ca1 = {std::abs(e1_lit), ed1, std::abs(e2_lit),
                                    -ed1};
                    m_ca.push_back(sat_ca1);
                    cl_t sat_ca2 = {std::abs(e1_lit), -ed1, std::abs(e2_lit),
                                    ed1};
                    m_ca.push_back(sat_ca2);
                  } else {
                    cl_t sat_ca1 = {std::abs(e1_lit), ed1, std::abs(e2_lit),
                                    ed1};
                    m_ca.push_back(sat_ca1);
                    cl_t sat_ca2 = {std::abs(e1_lit), -ed1, std::abs(e2_lit),
                                    -ed1};
                    m_ca.push_back(sat_ca2);
                  }
                }
              }
            }
          }
        }
      }

    }  // aut_level > 0 close
    minsat_clause_assgmt.push_back(m_ca);
  }
}
