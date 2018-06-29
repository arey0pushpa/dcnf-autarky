#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <algorithm>

typedef std::int64_t lit_t; // literals
typedef std::vector<lit_t> cl_t; // clauses
typedef std::vector<cl_t> cls_t; // clause-sets
typedef std::pair<lit_t,lit_t> pair_t;
typedef std::vector<pair_t> pairs_t;
typedef std::vector<pairs_t> sel_bf;      // represent bf var set
typedef std::vector<cls_t> minsat_ass;          // vector of clause set
typedef std::uint32_t coord_t; // coordinates

namespace {

enum Error_codes {
  file_reading_error = 1,
  file_writing_error = 2,
  file_pline_error = 3,
  num_vars_error = 4,
  allocation_error = 5,
  literal_read_error = 6,
  variable_value_error = 7,
  number_clauses_error = 8,
  empty_clause_error = 9,
  unit_clause_error = 11,
  input_format_violation = 12
};

class Clause {
 public:
  cl_t lits_c;
  cl_t evar_c;
  Clause (cl_t& l): lits_c(l) {}
  int length() {
    return lits_c.size();
  }
};

class Depset {
  public:
    coord_t var;
    cl_t deps_s; 
};

}

inline cl_t extract_int(std::string line) {
  cl_t vec_int;
  std::stringstream ss;
  ss << line;
  std::string temp;
  lit_t found;
  while (!ss.eof()) {
    ss >> temp;
    if (std::stringstream(temp) >> found) {
      vec_int.push_back(found);
    }
  }
  assert(!vec_string.empty());
  if (vec_int.back() != 0) {
    std::cerr
        << "Input format violation [Missing 0]. Every Line should end with 0"
        << '\n';
    exit(input_format_violation);
  }
  vec_int.pop_back();

  return vec_int;
}

/** Vector intersection */
inline cl_t vector_intersection(cl_t& v1, cl_t& v2) {
  cl_t v;
  std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(),
                        std::back_inserter(v));
  return v;
} 

/** Find an element in a vector **/
inline bool find_int_element(cl_t& vec, lit_t elem) {
  return std::find(vec.begin(), vec.end(), elem) != vec.end();
}

/** Implement a template */
/** Return index of the element **/
inline lit_t find_index(cl_t& vec, lit_t elem) {
  auto it = std::find(vec.begin(), vec.end(), elem);
  if (it == vec.end()) {
    // Todo: implement exception handling
    // std::cout << "Element not found. Exiting..." <<"\n";
    return -1;
    // exit(0);
  } else {
    auto index = std::distance(vec.begin(), it);
    return index;
  }
}

/** Return index of the element **/
inline lit_t find_vector_index(cls_t& vec, cl_t& elem) {
  auto it = std::find(vec.begin(), vec.end(), elem);
  if (it == vec.end()) {
    // Todo: implement exception handling
    // std::cout << "Element not found. Exiting..." <<"\n";
    return -1;
    // exit(0);
  } else {
    auto index = std::distance(vec.begin(), it);
    return index;
  }
}

/** Return index of the element */
inline lit_t find_scd_index(pairs_t& vec, lit_t elem) {
  for (coord_t i = 0; i < vec.size(); ++i) {
    if (vec[i].second == elem) {
      return i;
    }
  }
  // todo: error handle
  return -1;
}

/** Trim Operations */
// trim from left
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v") {
  s.erase(0, s.find_first_not_of(t));
  return s;
}

// trim from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v") {
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}

// trim from left & right
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v") {
  return ltrim(rtrim(s, t), t);
}

/** Print n-dimentional vector **/
inline void print_1d_vector(cl_t& vec) {
  for (coord_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i] << " ";
  }
}

inline void print_2d_vector(cls_t& vec) {
  for (coord_t i = 0; i < vec.size(); ++i) {
    print_1d_vector(vec[i]);
    std::cout << "\n";
  }
}


inline void print_3d_vector(minsat_ass& vec) {
  for (coord_t i = 0; i < vec.size(); ++i) {
    print_2d_vector(vec[i]);
    std::cout << "\n";
  }
}

inline void print_1d_vector_pair(std::vector<std::pair<lit_t, char> >& T) {
  for (const auto& p : T) {
    std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
  }
}

inline void print_1d_vector_int_pair(pairs_t& T) {
  for (pair_t& p : T) {
    std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
  }
}

inline void print_2d_vector_pair(sel_bf& T) {
  for (const pairs_t& i : T) {
    for (const pair_t& p : i) {
      std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
    }
    std::cout << "\n";
  }
}
