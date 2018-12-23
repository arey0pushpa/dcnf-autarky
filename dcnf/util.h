#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

// None of the following macros should exist:
#define LOG(x) std::cout << x << std::endl

typedef std::int64_t lit_t;      // literals
typedef std::vector<lit_t> cl_t; // clauses
typedef std::vector<cl_t> cls_t; // clause-sets
typedef std::pair<lit_t, lit_t> pair_t;
typedef std::vector<pair_t> pairs_t;
typedef std::vector<pairs_t> sel_bf;   // represent bf var set
typedef std::vector<cls_t> minsat_ass; // vector of clause-set
typedef std::uint32_t coord_t;         // coordinates

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

class Clauses {
  cl_t m_lits;

  cl_t m_evars;
  cl_t m_elits;

  cl_t m_avars;
  cl_t m_alits;

public:
  Clauses(){};
  void initialise_lits(cl_t c) { m_lits = c; }
  void initialise_evars(cl_t e) { m_evars = e; }
  void initialise_elits(cl_t e) { m_elits = e; }
  void initialise_avars(cl_t a) { m_avars = a; }
  void initialise_alits(cl_t a) { m_alits = a; }

  cl_t lits() const { return m_lits; }
  cl_t evars() const { return m_evars; }
  cl_t elits() const { return m_elits; }
  cl_t avars() const { return m_avars; }
  cl_t alits() const { return m_alits; }
};

class Variables {
  char m_quantype;
  cl_t m_dependency;
  coord_t m_eindex;

public:
  Variables() : m_quantype('a'), m_dependency({}) {}
  void initialise_qtype(char c) { m_quantype = c; }
  void initialise_eindex(coord_t i) { m_eindex = i; }
  void initialise_dependency(cl_t dep_var) { m_dependency = dep_var; }

  char qtype() const { return m_quantype; }
  cl_t dependency() const { return m_dependency; }
  coord_t eindex() const { return m_eindex; }
};

class bf_lbf_converter {

public:
  bool is_present;
  cl_t lbf_fml;
};

// command line
inline char *get_cmd_option(char **begin, char **end,
                            const std::string &option) {
  char **itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
    return *itr;
  return 0;
}

inline bool cmd_option_exists(char **begin, char **end,
                              const std::string &option) {
  return std::find(begin, end, option) != end;
}

// output
inline void
output(const std::string filename, const std::string output_file_name,
       const coord_t level, const coord_t s_level, const coord_t encoding,
       const coord_t no_of_var, const coord_t no_of_clauses,
       const coord_t no_of_avars, const coord_t no_of_evars,
       const coord_t uni_dep_set, const coord_t pa_var, const coord_t bf_var,
       const coord_t cs_var, const coord_t vars_in_dimacs,
       const coord_t matrix_size_dimacs)

{

  std::cout << "filename         " << filename << '\n';
  std::cout << "output_filename  " << output_file_name << '\n';
  std::cout << "autarky_level    " << level << '\n';
  std::cout << "conformity_level " << s_level << '\n';
  std::cout << "encoding_type    " << encoding << '\n';
  std::cout << "no_of_var        " << no_of_var << '\n';
  std::cout << "no_of_clauses    " << no_of_clauses << '\n';
  std::cout << "no_of_avars      " << no_of_avars << '\n';
  std::cout << "no_of_evars      " << no_of_evars << '\n';
  std::cout << "unique_dep_sets  " << uni_dep_set << '\n';
  std::cout << "cs_var           " << cs_var << '\n';
  std::cout << "bf_vars          " << bf_var << '\n';
  std::cout << "pa_vars          " << pa_var << '\n';
  std::cout << "vars_in_dimacs   " << vars_in_dimacs << '\n';
  std::cout << "mtx_size_dimacs  " << matrix_size_dimacs << '\n';
}

// Basic util
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
	cl_t binary_repr, fml_repr;
	coord_t blen = 0;
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
		(binary_repr[i] == 0 ? fml_repr.push_back(-lbf_vars[i]) : fml_repr.push_back(lbf_vars[i]));  
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
inline void print_1d_vector(cl_t &vec) {
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
