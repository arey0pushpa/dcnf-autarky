// ---------------------------------------------------------
// util.h
// basic utilities and helper functions for autarky reduction.
//
// Author: Ankit Shukla <ankit.shukla.jku.at>
// Last Modification: 20.4.2019
//
// (c) Ankit Shukla, 2019
// ----------------------------------------------------------

#ifndef UTIL_H
#define UTIL_H

/* #include <algorithm>
 * #include <bitset>
 * #include <cassert>
 * #include <iostream>
 * #include <limits>
 * #include <set>
 * #include <sstream> */

#include "defs.h"
#include "dcnf.h"

// command line
inline char *get_cmd_option(char **begin, char **end,
                            const std::string &option) {
  char **itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) return *itr;
  return 0;
}

inline bool cmd_option_exists(char **begin, char **end,
                              const std::string &option) {
  return std::find(begin, end, option) != end;
}

// Unique vector
template <typename t>
std::vector<std::vector<t>> unique_vectors(std::vector<std::vector<t>> input) {
  for (auto &i : input) {
    i.erase(i.begin());
  }
  std::sort(input.begin(), input.end());
  input.erase(std::unique(input.begin(), input.end()), input.end());
  return input;
}

// Basic util
//void at_most_one(cl_t &, cls_t &);
//void at_most_one_linear(cl_t &, cls_t &, coord_t &);
                                                      
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
  if (vec_int.back() != 0) {
    std::cerr
        << "Input format violation [Missing 0]. Every Line should end with 0"
        << '\n';
    exit(input_format_violation);
  }
  vec_int.pop_back();

  return vec_int;
}

/** Create lbf formula **/
inline cl_t lbf_fml(cl_t lbf_vars, lit_t bf_var) {
  coord_t blen = 0;
  boolv_t binary_repr;
  cl_t fml_repr;

  while (bf_var > 0) {
    binary_repr.push_back(bf_var % 2);
    bf_var = bf_var / 2;
    ++blen;
  }
  assert(blen <= lbf_vars.size());
  // Enforce the resultant vector is of size of lbf_vars
  for (coord_t i = blen; i < lbf_vars.size(); ++i) {
    binary_repr.push_back(0);
  }
  for (coord_t i = 0; i < lbf_vars.size(); ++i) {
    (binary_repr[i] == 0 ? fml_repr.push_back(-lbf_vars[i])
                         : fml_repr.push_back(lbf_vars[i]));
  }
  return fml_repr;
}

/** Vector intersection */
inline cl_t vector_intersection(cl_t &v1, cl_t &v2) {
  cl_t v;
  std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(),
                        std::back_inserter(v));
  return v;
}

/** Find an element in a vector **/
inline bool find_int_element(cl_t &vec, lit_t elem) {
  return std::find(vec.begin(), vec.end(), elem) != vec.end();
}

// Find a pair in a vector */
inline bool pair_present(pairs_t &V, pair_t elem) {
  if (std::find(V.begin(), V.end(), elem) != V.end()) {
    return true;
  } else {
    return false;
  }
}

/** Implement a template */
/** Return index of the element **/
inline lit_t find_index(cl_t &vec, lit_t elem) {
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
inline lit_t find_vector_index(cls_t &vec, cl_t &elem) {
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

/** Return index of the second element */
inline lit_t find_scd_index(pairs_t &vec, lit_t elem) {
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
inline std::string &ltrim(std::string &s, const char *t = " \t\n\r\f\v") {
  s.erase(0, s.find_first_not_of(t));
  return s;
}

// trim from right
inline std::string &rtrim(std::string &s, const char *t = " \t\n\r\f\v") {
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}

// trim from left & right
inline std::string &trim(std::string &s, const char *t = " \t\n\r\f\v") {
  return ltrim(rtrim(s, t), t);
}

/** Print n-dimentional vector **/
inline void print_1d_vector(cl_t vec) {
  for (coord_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i] << " ";
  }
}

inline void print_2d_vector(cls_t &vec) {
  for (coord_t i = 0; i < vec.size(); ++i) {
    print_1d_vector(vec[i]);
    std::cout << "\n";
  }
}

inline void print_3d_vector(minsat_ass &vec) {
  for (coord_t i = 0; i < vec.size(); ++i) {
    print_2d_vector(vec[i]);
    std::cout << "\n";
  }
}

inline void print_1d_vector_pair(std::vector<std::pair<lit_t, char>> &T) {
  for (const auto &p : T) {
    std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
  }
}

inline void print_1d_vector_int_pair(pairs_t &T) {
  for (pair_t &p : T) {
    std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
  }
}

inline void print_2d_vector_pair(sel_bf &T) {
  for (const pairs_t &i : T) {
    for (const pair_t &p : i) {
      std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
    }
    std::cout << "\n";
  }
}

#endif

