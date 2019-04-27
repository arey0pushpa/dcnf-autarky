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
#include <iostream>
#include <limits>
#include <set>
#include <sstream>

//#include <chrono>
#include <cmath>
#include <fstream>
#include <memory>
#include <string>

#include "defs.h"

/* Clauses class provide information of each of the clauses
 * attached with each clause
 * 1. set of literals: mLits
 * 2. set of existential variables: e_vars
 * 3. set of existential literals: e_lits
 * 4. set of uni variables: a_vars
 * 5. set of uni literals: a_lits
 * 6. activity of the clause: present
 * */

class Clauses {
 public:
  cl_t m_lits;

  cl_t m_evars;
  cl_t m_elits;

  cl_t m_avars;
  cl_t m_alits;

  bool present;

  Clauses() : present(1){};
  void initialise_lits(cl_t c) { m_lits = c; }
  void initialise_evars(cl_t e) { m_evars = e; }
  void initialise_elits(cl_t e) { m_elits = e; }

  void initialise_avars(cl_t a) { m_avars = a; }
  void initialise_alits(cl_t a) { m_alits = a; }
  void update_presence(bool p) { present = p; }

  cl_t lits() const { return m_lits; }
  cl_t evars() const { return m_evars; }
  cl_t elits() const { return m_elits; }
  cl_t avars() const { return m_avars; }
  cl_t alits() const { return m_alits; }
  bool cls_present() const { return present; }
};

/* Class Variables provides information of each variables
 *   1. The quantifier type of the variable : quantype
 *   In case of E_Var
 *   2. Var dependency list: dependency
 *   3. 0 based: Index of the existenial quantifier in e1,..,en: eindex
 *       Do you need same for the univ variable?
 *   4. 0 based: Clauses the variable is active/present: active_cls
 *   5. positive occurence in clauses: pos_cls
 *   6. neg occurence in clauses: pos_cls
 *   7. The variable is present in the formula after autarky reduction?: present
 */
class Variables {
 public:
  char m_quantype;
  cl_t m_dependency;  // absolute var list
  coord_t m_eindex;   // 0 based USED!!!

  set_t active_cls;  // 0 based
  set_t pos_cls;
  set_t neg_cls;

  bool present;

  Variables() : m_quantype('a'), m_dependency({}), present(1) {}
  void initialise_qtype(char c) { m_quantype = c; }
  void initialise_eindex(coord_t i) { m_eindex = i; }
  void initialise_dependency(cl_t dep_var) { m_dependency = dep_var; }

  void update_presence(bool p) { present = p; }
  void activein_cls(coord_t cls) { active_cls.insert(cls); }
  void pos_polarity(coord_t cls) { pos_cls.insert(cls); }
  void neg_polarity(coord_t cls) { neg_cls.insert(cls); }

  char qtype() const { return m_quantype; }
  cl_t dependency() const { return m_dependency; }
  coord_t eindex() const { return m_eindex; }
  set_t act_cls() const { return active_cls; }
  set_t pos_pol() const { return pos_cls; }
  set_t neg_pol() const { return neg_cls; }
  bool var_present() const { return present; }
};

class bf_lbf_converter {
 public:
  bool is_present;
  cl_t lbf_fml;
  bf_lbf_converter() : is_present(false) {}
};

void command_line_parsing(int, char *av[], std::string &, unsigned &,
                          unsigned &, bool &, bool &);

void parse_qdimacs_file(std::string filename, cls_t &dcnf_fml, cls_t &dep_set,
                        cl_t &a_vars, cl_t &e_vars, coord_t &no_of_clauses,
                        coord_t &no_of_var, coord_t &dependency_var,
                        coord_t s_level, coord_t &min_dep_size,
                        coord_t &max_dep_size);
/* A class used for namescpace for the code.
 */
class dcnf {
 public:
  // variables
  coord_t no_of_clauses;  // Input clause set
  coord_t no_of_vars;     // No Of variables in the input
  cl_t e_vars;            // No Of e variables in the input
  std::vector<Variables>
      dcnf_variables;  // Data Struture for info about input variables
  std::vector<Clauses>
      dcnf_clauses;  // Data structure for info about each input clause

  cls_t dcnf_fml;         // Input cnf fml
  set_t present_clauses;  // Present clauses at current iteration
  set_t deleted_clauses;  // Set of deleted clause at current iteration
  //  cl_t active_evar_index; //  Index Lookup for the evar
  //  cl_t present_cls_index; // Lookup for the present clauses

  cl_t active_evars;    // Current evar set of active variables
  cl_t assigned_evars;  // Evar variables that got assigned
  cl_t active_avars;    // Current avar set of active variables
  cl_t assigned_avars;  // Avar variables that got assigned

  sel_bf selected_bf;               // All bf (v,f) pairs {(e-var, )...}
  minsat_ass minsat_clause_assgmt;  // All S(C)'s: {<e-var,bf(k)>...}

  // helper functions

  void propagate_cls_removal(lit_t i);

  coord_t a_autarky(std::string filename, std::string output_file_name,
                    coord_t encoding);

  coord_t e_autarky(lit_t e);

  void set_all_solutions(const coord_t level);

  void quant_seperation(cl_t &, cl_t &, cl_t &);

  void non_trivial_autarky(cl_t &, cls_t &);

  void at_most_one(cl_t &, cls_t &);
  void at_most_one_linear(cl_t &, cls_t &, coord_t &);

  void satisfied_clauses(coord_t encoding, coord_t cls_cnt, cl_t &lbf_vars,
                         cls_t &bf_vars, minsat_ass &pa_var_msat_ass,
                         cls_t &msat_concrete_var_map, cls_t &cnf_fml,
                         std::vector<bf_lbf_converter> &bf2lbf_var_map,
                         cl_t &active_evar_index);

  void touched_clauses(cl_t &cs_vars, cls_t &clausewise_pa_var_map,
                       cls_t &cnf_fml);

  void untouched_clauses(coord_t encoding, cl_t &lbf_vars, cls_t &bf_vars,
                         cl_t &cs_vars, cls_t &cnf_fmls,
                         std::vector<bf_lbf_converter> &,
                         cl_t &present_cls_index, cl_t &active_evar_index);
  void print_remaining_cls();

  friend std::ostream &operator<<(std::ostream &os, const dcnf &m);
};

/* std::ostream& operator<< (std::ostream &theStream, dcnf &d)
 * {
 *     theStream<<" fml: "<< d.dcnf_fml<<'\n';
 *     return theStream;
 * } */
typedef std::shared_ptr<dcnf> dcnf_ptr;

#endif

