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
#include <set>
#include <vector>

#define LOG(x) std::cout << x << std::endl
typedef std::vector<bool> boolv_t;

typedef std::int64_t lit_t;      // literals
typedef std::vector<lit_t> cl_t; // clauses
typedef std::vector<cl_t> cls_t; // clause-sets
typedef std::pair<lit_t, lit_t> pair_t;
typedef std::vector<pair_t> pairs_t;
typedef std::vector<pairs_t> sel_bf;   // represent bf var set
typedef std::vector<cls_t> minsat_ass; // vector of clause-set
typedef std::uint32_t coord_t;         // coordinates
typedef std::set<lit_t> set_t;         // coordinates
typedef std::set<set_t> sets_t;         // coordinates
typedef std::set<std::set<cl_t>> sets_cls;         // coordinates
typedef std::vector<std::vector<std::pair<lit_t, cl_t>>> bflbf_t; // bflbf type

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
  free_var_occurrence = 13,
  normal_exit = 100
};

#endif

