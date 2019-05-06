// ---------------------------------------------------------
// def.h
// basic types and definitions used in the autarky reduction.
//
// Author: Ankit Shukla <ankit.shukla.jku.at>
// Last Modification: 20.4.2019
//
// (c) Ankit Shukla, 2019
// ----------------------------------------------------------

#ifndef DEFS_H
#define DEFS_H

#include <iostream>
#include <vector>
#include <set>

#define LOG(x) std::cout << x << std::endl
typedef std::vector<bool> boolv_t;

typedef std::int64_t lit_t;       // literals
typedef std::vector<lit_t> cl_t;  // clauses
typedef std::vector<cl_t> cls_t;  // clause-sets
typedef std::pair<lit_t, lit_t> pair_t;
typedef std::vector<pair_t> pairs_t;
typedef std::vector<pairs_t> sel_bf;    // represent bf var set
typedef std::vector<cls_t> minsat_ass;  // vector of clause-set
typedef std::uint32_t coord_t;          // coordinates
typedef std::set<lit_t> set_t;          // coordinates
typedef std::vector<std::vector <std::pair<lit_t, cl_t>> > bflbf_t; // bflbf type

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
  input_format_violation = 12,
	free_var_occurrence = 13
};

inline void output(const std::string filename,
                   const std::string output_file_name, const coord_t level,
                   const coord_t s_level, const coord_t encoding,
                   const coord_t no_of_var, const coord_t no_of_clauses,
                   const coord_t no_of_avars, const coord_t no_of_evars,
                   const coord_t uni_dep_set, const coord_t pa_var,
                   const coord_t bf_var, const coord_t cs_var,
                   const coord_t vars_in_dimacs,
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


#endif

