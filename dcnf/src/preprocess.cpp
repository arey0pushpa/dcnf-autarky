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

void preprocess_fml(sel_bf& selected_bf, minsat_ass& minsat_clause_assgmt,
                    cls_t& dcnf_fml, cls_t& dep_set, cl_t& a_vars, cl_t& e_vars,
                    coord_t& level) {
  /** Create Complete Dependency List **/
  // Sort the dep set 
  std::sort(dep_set.begin(), dep_set.end(),
            [](const cl_t& a, const cl_t& b) { return a[0] < b[0]; });

  // Sort the dependent-e-var and a-var 
  std::sort(e_vars.begin(), e_vars.end());
  std::sort(a_vars.begin(), a_vars.end());
  
  // Complete the partial e_var List: e-var with no dependency
  cl_t evars_outermost; 

  /** Fill the dependency for all exists vars **
  coord_t ctr = 0;
  for (coord_t i = 0; i < no_of_var; ++i) {
    if (e == e_pr[ctr]) {
      ctr += 1;
    } else {
      auto dummy_vec = a_vars;
      dummy_vec.insert(dummy_vec.begin(), e);
      dep_set.push_back(dummy_vec);
    }
  } 

  std::sort(dep_set.begin(), dep_set.end(),
            [](const cl_t& a, const cl_t& b) {
              return a[0] < b[0];
            });
  for (coord_t i = 0; i < dep_set.size(); ++i) {
      print_1d_vector(dep_set[i]);
      std::cout << '\n';
  }
  exit(0);
  */

  /** Selected Boolean Function **/
  for (coord_t i = 0; i < e_vars.size(); ++i) {
    pairs_t t_vec;
    // Base Case [bf(0), bf(1)]
    t_vec.emplace_back(e_vars[i], 500);   // false
    t_vec.emplace_back(e_vars[i], 1000);  // true
    if (level > 0) {
      // Other Cases
      for (coord_t j = 0; j < dep_set[i].size(); ++j) {
        if (j == 0)
          continue;
        else {
          t_vec.emplace_back(e_vars[i], -dep_set[i][j]);
          t_vec.emplace_back(e_vars[i], dep_set[i][j]);
        }
      }
    }
    selected_bf.push_back(t_vec);
  }

  /*
  std::cout << "The T set is: ";
  print_2d_vector_pair( T );
  exit(0);
  */

  // Create a Union of both exists and forall :
  // todo: do it while parsing the file
  std::vector<std::pair<int, char> > union_var;

  for (auto i : e_vars) {
    union_var.emplace_back(i, 'e');
  }
  for (auto j : a_vars) {
    union_var.emplace_back(j, 'a');
  }

  /* Sort() before generating Min Sat Clause */
  std::sort(union_var.begin(), union_var.end());

  /*
  std::cout << " Printing Union Var: \n";
  for (coord_t i=0; i< union_var.size(); ++i) {
        std::cout << union_var[i].first << " "
             << union_var[i].second << "\n";
  }
  */
  /** Three cases to consider
   * 1. Basic case: handle all are e_varsiables
   * 2. Handle dependency case for all e_varsiable
   * 3. Handle e-var and a-var case
   */

  /** Minimal Satisfying Clauses **/
  for (cl_t c : dcnf_fml) {
    cls_t dummy_s;
    cl_t e_part;
    cl_t a_part;

    quant_seperation(c, e_part, a_part, union_var);

    /** All e-var case **/
    for (auto& e : e_part) {
      if (e > 0) {
        dummy_s.push_back(cl_t{e, 1000});
      } else {
        dummy_s.push_back(cl_t{abs(e), 500});
      }
    }

    if (level > 0) {
      /** e-var pairs case */
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

      /** e-var a-var case **/
      for (auto e : e_part) {
        const auto i = find_index(e_vars, abs(e));
        auto dep = dep_set[i];
        /** todo : implement with intersection **/
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
  }
}
