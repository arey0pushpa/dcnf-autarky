// ---------------------------------------------------------
// dcnf.h
// class File for dcnf formula manipulation and associated operations.
//
// Author: Ankit Shukla <ankit.shukla.jku.at>
// Last Modification: 20.4.2019
//
// (c) Ankit Shukla, 2019
// ----------------------------------------------------------

#ifndef DCNF_H
#define DCNF_H

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <sstream>
#include <string>

#include <chrono>
#include "defs.h"

/* Clauses class provide information of each of the clauses
 * attached with each clause
 * 1. set of literals: lits
 * 2. set of existential variables: evars
 * 3. set of existential literals: elits
 * 4. set of uni variables: avars
 * 5. set of uni literals: alits
 * 6. activity of the clause: present
 * */
class Clauses {
 public:
  cl_t lits;

  cl_t evars;
  cl_t elits;

  cl_t avars;
  cl_t alits;

  bool present;

  Clauses() : present(1){};
  void initialise_lits(cl_t c) { lits = c; }
  void initialise_evars(cl_t e) { evars = e; }
  void initialise_elits(cl_t e) { elits = e; }

  void initialise_avars(cl_t a) { avars = a; }
  void initialise_alits(cl_t a) { alits = a; }
  void update_presence(bool p) { present = p; }
};

/* Class Variables provides information of each variables
 *   1. The quantifier type of the variable : quantype
 *   In case of E_Var
 *   2. Var dependency list: dependency
 *   3. 0 based: Index of the existenial quantifier in e1,..,en: eindex
 *       Do you need same for the univ variable?
 *   4. positive occurence in clauses: pos_cls
 *   5. neg occurence in clauses: pos_cls
 *   6. The variable is present in the formula after autarky reduction?: present
 */
class Variables {
 public:
  char quantype;
  cl_t dependency;  // absolute var list
  coord_t eindex;   // 0 based USED!!!

  set_t pos_cls;
  set_t neg_cls;

  bool present;

  Variables() : quantype('a'), dependency({}), present(1) {}
  void initialise_qtype(char c) { quantype = c; }
  void initialise_eindex(coord_t i) { eindex = i; }
  void initialise_dependency(cl_t dep_var) { dependency = dep_var; }

  void update_presence(bool p) { present = p; }
  void pos_polarity(coord_t cls) { pos_cls.insert(cls); }
  void neg_polarity(coord_t cls) { neg_cls.insert(cls); }
};

/**
class bf_lbf_converter {
public:
  bool is_present;
  cl_t lbf_fml;
  bf_lbf_converter() : is_present(false) {}
}; **/

void command_line_parsing(int, char *av[], std::string &, unsigned &,
                          unsigned &, bool &, bool &);

void parse_qdimacs_file(std::string filename, vstr_t &aed_lines,
                        cls_t &dcnf_fml, cls_t &dep_set, cl_t &a_vars,
                        cl_t &e_vars, coord_t &no_of_clauses,
                        coord_t &no_of_var, coord_t &dependency_var,
                        coord_t s_level, coord_t &min_dep_size,
                        coord_t &max_dep_size);

// void display_running_time(
//    std::chrono::time_point<std::chrono::high_resolution_clock> start);

/* A class used for namescpace for the code.
 */
class dcnf {
 public:
  dcnf() {
    aut_level = 1;             // 1-level Autarky
    s_level = 0;               // relaxed, non-strict
    gen = 0;                   // relaxed, non-strict
    encoding = 2;              // Linear encoding
    reduction_type = 2;        // a-autarky
    coord_t aut_present = 10;  // autarky present
    coord_t output_type = 0;   // commandline o/p
    result = "NONE";
    ever_reduced = 0;
    naut = 0;
  }

  // Variables
  std::string filename;          // Input file path
  std::string fname;             // Input filename
  std::string output_file_name;  // output file
  coord_t aut_level;             // Autarky level
  coord_t s_level;               // Input DQDIMACS file strictness
  coord_t gen;         // Genralisation of grounded autarkies; symbolic encoding
  coord_t encoding;              // Chosen encoding
  coord_t reduction_type;        // Autarky reduction type
  coord_t
      output_type;  // commandline o/p
                    // std::chrono::time_point<std::chrono::high_resolution_clock>
                    // start;

  coord_t ntaut;          // No of tautological clauses
  coord_t naut;           // No of autarkies found.
  std::string result;     // SAT, UNSAT, RED, NONE, T/O, ERR
  coord_t no_of_vars;     // No Of variables in the input
  coord_t no_of_clauses;  // Input clause set
  cl_t e_vars;            // e variables in the input
  std::vector<Variables>
      dcnf_variables;  // Data Struture for info about input variables
  std::vector<Clauses>
      dcnf_clauses;  // Data structure for info about each input clause

  vstr_t aed_lines;       // input uni, existential and d lines
  cls_t dcnf_fml;         // Input cnf fml
  set_t present_clauses;  // Present clauses at current iteration
  set_t deleted_clauses;  // Set of deleted clause at current iteration
  //  cl_t active_evar_index; //  Index Lookup for the evar
  //  cl_t present_cls_index; // Lookup for the present clauses
  coord_t old_cls_size;
  // coord_t updated_cls_size;
  coord_t ever_reduced;
  std::chrono::high_resolution_clock::time_point start_time;

  cl_t active_evars;    // Current evar set of active variables
  cl_t assigned_evars;  // Evar variables that got assigned
  cl_t active_avars;    // Current avar set of active variables
  cl_t assigned_avars;  // Avar variables that got assigned

  sel_bf selected_bf;               // All bf (v,f) pairs {(e-var, )...}
  minsat_ass minsat_clause_assgmt;  // All S(C)'s: {<e-var,bf(k)>...}
  cls_t final_assgmt;               // Assignmt for the autarky

  // std::vector<bf_lbf_converter> bf2lbf_var_map; // bf to lbf var map

  // helper functions
  void update_avars();
  void update_evars();
  void propagate_cls_removal(lit_t i, lit_t e);
  void update_data_structure(lit_t e);
  coord_t a_autarky(std::string filename, std::string output_file_name,
                    coord_t encoding);
  coord_t e_autarky(lit_t e);
  void cmdline_parsing(int argc, char *argv[]);
  void display_result(coord_t aut_present, coord_t output_result);
  void display_rresult();

  // void set_all_solutions(const coord_t level);
  void selected_boolfunc(const coord_t level);
  void min_satisfying_assgn(const coord_t level);

  void quant_seperation(cl_t &, cl_t &, cl_t &);

  void non_trivial_autarky(cl_t &, cls_t &);

  void at_most_one(cl_t &, cls_t &);
  void at_most_one_linear(cl_t &, cls_t &, lit_t &);

  void satisfied_clauses(cls_t &lbf_vars, cls_t &bf_vars,
                         minsat_ass &pa_var_msat_ass,
                         cls_t &msat_concrete_var_map, cls_t &cnf_fml,
                         bflbf_t &bf2lbf_var_map, cl_t &active_evar_index);

  void touched_clauses(cl_t &cs_vars, cls_t &clausewise_pa_var_map,
                       cls_t &cnf_fml);

  void untouched_clauses(cls_t &lbf_vars, cls_t &bf_vars, cl_t &cs_vars,
                         cls_t &cnf_fmls, bflbf_t &, cl_t &present_cls_index,
                         cl_t &active_evar_index);
  void print_remaining_cls();
  void output();
  void print_results();
  void clean_tmp_files(std::string &sat_out);

  //  friend std::ostream &operator<<(std::ostream &os, const dcnf &m);
};

// typedef std::unique_ptr<dcnf> dcnf_ptr;
typedef std::shared_ptr<dcnf> dcnf_ptr;

#endif

