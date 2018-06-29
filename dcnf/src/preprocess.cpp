#include <iterator>

#include "defs.h"

void quant_seperation(cl_t& c, cl_t& e_part, cl_t& a_part,
                      std::vector<std::pair<int, char> >& union_var) {
  for (auto l : c) {
    auto index = union_var[abs(l) - 1];  // get the lth value
    auto var = std::get<1>(index);
    if (var == 'e')
      e_part.push_back(l);
    else
      a_part.push_back(l);
  }
}

void preprocess_fml(Clauses dcnf_clauses[], Variables dcnf_variables[],
                    sel_bf& selected_bf, minsat_ass& minsat_clause_assgmt,
                    coord_t& num_of_vars, coord_t& level) {
  /** Selected Boolean Function **/
  for (coord_t i = 0; i < num_of_vars; ++i) {
    pairs_t t_vec;
    if (dcnf_variables[i].fetch_qtype() == 'e') {
      // Base Case [bf(0), bf(1)]; level == 0
      t_vec.emplace_back(i+1, 500);   // false
      t_vec.emplace_back(i+1, 1000);  // true
      if (level > 0) {
        cl_t dvar = dcnf_variables[i].fetch_dependency();
        for (coord_t j = 0; j < dvar.size(); ++j) {
          t_vec.emplace_back( i+1, dvar[j]);
          t_vec.emplace_back( i+1, -dvar[j]);
        }
      }
    }
    selected_bf.push_back(t_vec);
  }

  std::cout << "\nThe genearted bool func: s(v) is: "
            << "\n";
  print_2d_vector_pair(selected_bf);

  exit(0);

  /** Three cases to consider
   * 1. Basic case: handle all are e_varsiables
   * 2. Handle dependency case for all e_varsiable
   * 3. Handle e-var and a-var case
   *

  ** Minimal Satisfying Clauses **
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
