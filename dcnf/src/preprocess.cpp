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
    for (lit_t e : elit_part) {
      if (e > 0) {
        m_ca.push_back(cl_t{e, 1000});
      } else {
        m_ca.push_back(cl_t{std::abs(e), 500});
      }
    }

    if (level > 0) {
    }
    minsat_clause_assgmt.push_back(m_ca);
  }

  /*
  for (cl_t& c : dcnf_fml) {
    cls_t dummy_s;
    cl_t e_part;
    cl_t a_part;

    quant_seperation(c, e_part, a_part, union_var);

    ** All e-var case **
    for (auto& e : e_part) {
      if (e > 0) {
        dummy_s.push_back(cl_t{e, 1000});
      } else {
        dummy_s.push_back(cl_t{abs(e), 500});
      }
    }

    if (level > 0) {
      ** e-var pairs case *
      // todo: check with variations: May have Bugs
      auto size = e_part.size();
      for (coord_t i = 0; i < size - 1; ++i) {
        auto index = find_index(e_vars, abs(e_part[i]));
        auto dep1 = dep_set[index];
        for (coord_t j = i + 1; j < size; ++j) {
          auto index = find_index(e_vars, abs(e_part[j]));
          auto dep2 = dep_set[index];
          cl_t d_vec = vector_intersection(dep1, dep2);
          for (auto& d : d_vec) {
            cl_t inner_vec1 = {abs(e_part[i]), d, abs(e_part[j]), -d};
            cl_t inner_vec2 = {abs(e_part[i]), -d, abs(e_part[j]), d};
            dummy_s.push_back(inner_vec1);
            dummy_s.push_back(inner_vec2);
          }
        }
      }

      ** e-var a-var case **
      for (auto e : e_part) {
        const auto i = find_index(e_vars, abs(e));
        auto dep = dep_set[i];
        ** todo : implement with intersection **
        for (auto a : a_part) {
          auto presence_a = find_int_element(dep, abs(a));
          if (presence_a) {
            dummy_s.push_back(cl_t{abs(e), -a});
          }
        }
      }
    }
    // final push on the S
    minsat_clause_assgmt.push_back(dummy_s);
  } */
}
