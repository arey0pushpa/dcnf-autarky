// dcnfAutarky -- A basic implementation for autarky search in DQCNF
// Ankit Shukla 22.June.2018 (Swansea)

/* Copyright 2018 Oliver Kullmann, Ankit Shukla
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * */

/* Todo list
 *
 * 1. Implement linear encoding.
 *   - Add the "Command-variable" Encoding for AMO constraint.
 *   - Make sure that uep is preserved.
 *
 *   Sequential Commander encoding:
 *   The encoding  uses 3n-6 binary clauses (without uep) and < n/2 variables.
 *   Given V = {v1,..,vn}
 *   seco(v1, ..., vn) is defined recursively
 *
 *    i.  Base case n <= 4:
 *       seco(v1,v2,v3,v4) = the binomial(4,2)=6 prime-clauses for
 *                           amo(v1,v2,v3,v4)
 *    ii.  Recursion for n >= 5:
 *       seco(v1,...,vn) = Conjunction of
 *                       binomial(3,2)=3 prime-clauses for amo(v1,v2,v3)
 *                       and v1->w, v2->w, v3->w for the commander-variable w
 *                       and seco(w,v3,...,vn).
 *		Handle UEP:
 *      The problem is that v1=...=vn=0 admits many solutions.
 *
 *	 	For each commander-variable w(x,y,z) add
 *       w -> x v y v z (i.e., the 4-clause {-w,x,y,z}).
 *
 * 2. Handle empty clause and tautology.
 *    - Add checks to avoid basic SAT and UNSAT cases.
 *    - Add code to remove non occuring variables in the matrix.
 *
 * 3. Cleaning:
 *    - The use of index is not clear or what is the use of cnf_vars.
 *       The purpose should be clear.
 *
 * 4. Namespace creation: do using the classes or namespace.
 *    - Passing the parameters all the time looks ugly.
 *
 * 5. Implement the one existential variable form.
 *
 * 6. Autarky reduction algorithm:
 * 		- Add code in MakeFile to download the SAT solver locally.
 * 		- Add code for using SAT solver and extracting the assignment.
 * 		- Create a Wrapper for doing Autarky reduction.
 */

#include <bitset> // std::bitset
#include <chrono>
#include <cmath>
#include <fstream>
#include <iterator> // std::advance
#include <string>

#include "defs.h"

int main(int argc, char *argv[]) {
  std::string filename;
  std::string output_file_name = "/tmp/dcnfAutarky.dimacs";
  coord_t dependency_var = 0;
  coord_t level = 1;
  coord_t s_level = 0;
  coord_t encoding = 0;
  coord_t reduction_type = 0;
  coord_t aut_present = 10;

  if (cmd_option_exists(argv, argv + argc, "-h")) {
    std::cout << "DCNF-Autarky [version 0.0.1]. (C) Copyright 2018-2019 "
                 "Swansea UNiversity. \nUsage: ./dcnf [-i filename] [-o "
                 "filename] [-l "
                 "level] [-e encoding] [-s strictness; 0:general, 1:strict] "
                 "[-r reduction; 0:bf_autarky, 1:e_autarky]\n";
    exit(0);
  }

  char *file_name = get_cmd_option(argv, argv + argc, "-i");
  char *output_file = get_cmd_option(argv, argv + argc, "-o");
  char *level_set = get_cmd_option(argv, argv + argc, "-l");
  char *encoding_chosen = get_cmd_option(argv, argv + argc, "-e");
  char *strict_level = get_cmd_option(argv, argv + argc, "-s");
  char *red_type = get_cmd_option(argv, argv + argc, "-r");

  if (file_name) {
    filename = file_name;
  } else {
    std::cout << "Please provide an input file. Use [-i filename] or see help "
                 "[-h] for more options\n";
    exit(0);
  }

  if (output_file) {
    output_file_name = output_file;
  }

  if (filename == output_file_name) {
    std::cout << "Please provide differnt filenames for input and output file.";
    exit(0);
  }

  if (level_set) {
    level = std::stoi(level_set);
  }

  if (strict_level) {
    s_level = std::stoi(strict_level);
  }

  if (encoding_chosen) {
    encoding = std::stoi(encoding_chosen);
  }

  if (red_type) {
    reduction_type = std::stoi(red_type);
  }

  coord_t no_of_clauses = 0;
  coord_t no_of_var = 0;

  /** Global Variables ***/
  cls_t dcnf_fml; // Input Cnf formula {Clauses} := {{lit,...}...}

  cl_t e_vars;   // {exists-var}
  cl_t a_vars;   // {forall-var}
  cls_t dep_set; // {{dep-var}...}

  sel_bf selected_bf;              // All bf (v,f) pairs {(e-var, )...}
  minsat_ass minsat_clause_assgmt; // All S(C)'s: {<e-var,bf(k)>...}

  coord_t min_dep_size = 0;
  coord_t max_dep_size = 0;

  auto start = std::chrono::high_resolution_clock::now();

  parse_qdimacs_file(filename, dcnf_fml, dep_set, a_vars, e_vars, no_of_clauses,
                     no_of_var, dependency_var, s_level, min_dep_size,
                     max_dep_size);

  // TEMP FIX. TODO:IMPLEMENT THE CASE OF VAR ABSENCE IN MATRIX CASE DIRECTLY
  no_of_var = e_vars.size() + a_vars.size();
  // Create no_of_var Objects and for each obj representing a
  // variable (uni and exist) set qtype of the var and fix it's dependency
  std::vector<Variables> dcnf_variables;
  dcnf_variables.resize(no_of_var);

  std::sort(dep_set.begin(), dep_set.end(),
            [](const cl_t &a, const cl_t &b) { return a[0] < b[0]; });

  std::sort(e_vars.begin(), e_vars.end());
  std::sort(a_vars.begin(), a_vars.end());

  auto avar_iterator = a_vars.begin();
  auto evar_iterator = e_vars.begin();

  coord_t dep_index = 0;
  bool a_vars_end = false;
  bool e_vars_end = false;
  if (avar_iterator == a_vars.end())
    a_vars_end = true;
  if (evar_iterator == e_vars.end())
    e_vars_end = true;

  coord_t e_var_cntr = 0;
  // Create a class vector for the Variables
  // attach add info and access based on their index
  for (coord_t i = 0; i < no_of_var; ++i) {
    if (!a_vars_end && i == *avar_iterator - 1) {
      if (std::next(avar_iterator) == a_vars.end()) {
        a_vars_end = true;
      } else {
        avar_iterator = std::next(avar_iterator);
      }
    } else if (!e_vars_end && i == *evar_iterator - 1) {
      dcnf_variables[i].initialise_qtype('e');
      cl_t d_s = dep_set[dep_index];
      d_s.erase(d_s.begin());
      cl_t dep_vars = d_s;
      dcnf_variables[i].initialise_dependency(dep_vars);
      ++dep_index;
      dcnf_variables[i].initialise_eindex(e_var_cntr);
      ++e_var_cntr;
      if (std::next(evar_iterator) == e_vars.end()) {
        e_vars_end = true;
      } else {
        evar_iterator = std::next(evar_iterator);
      }
    } else {
      dcnf_variables[i].initialise_qtype('e');
      dcnf_variables[i].initialise_eindex(e_var_cntr);
      ++e_var_cntr;
    }
  }

  cls_t unique_dep_set = unique_vectors(dep_set);

  // create Clause vector objects and initialize E, A Qvar
  lit_t dsize = dcnf_fml.size();
  std::vector<Clauses> dcnf_clauses;
  for (coord_t i = 0; i < dsize; ++i) {
    [&] {
      cl_t c_evars, c_elits, c_avars, c_alits;
      set_t actv, posv, negv;
      for (const lit_t l : dcnf_fml[i]) {
        coord_t indx = std::abs(l) - 1;
        actv.insert(i);
        if (l > 0) {
          posv.insert(l);
          if (negv.count(l))
            return;
        } else {
          negv.insert(l);
          if (posv.count(l))
            return;
        }
        if (dcnf_variables[indx].qtype() == 'e') {
          c_evars.push_back(std::abs(l));
          c_elits.push_back(l);
        } else {
          c_avars.push_back(std::abs(l));
          c_alits.push_back(l);
        }
      }
      Clauses *cls = new Clauses;
      cls->initialise_lits(dcnf_fml[i]);

      cls->initialise_evars(c_evars);
      cls->initialise_elits(c_elits);

      cls->initialise_avars(c_avars);
      cls->initialise_alits(c_alits);

      dcnf_clauses.push_back(*cls);
    }();
  }

  // Every existential variable that do not occur in the clause is
  // not considered for the bf_vars
  for (const lit_t e : e_vars) {
    // check the e or e-1
    if (dcnf_variables[e - 1].act_cls().empty()) {
      dcnf_variables[e - 1].update_presence(0);
    }
  }

  // Create a Clause list of the present clauses
  // no_of_clauses = cls_indx;
  // boolv_t present_clauses [no_of_clauses];

  /* Todo: Implement a dependency Scheme in case no dependency given */

  if (reduction_type == 0) {
    set_all_solutions(dcnf_clauses, dcnf_variables, selected_bf,
                      minsat_clause_assgmt, no_of_clauses, no_of_var, level);

    while (aut_present == 10) {
      aut_present =
          bfs_autarky(dcnf_clauses, dcnf_variables, selected_bf,
                      minsat_clause_assgmt, e_vars, filename, output_file_name,
                      dependency_var, level, s_level, encoding);
    }
    if (aut_present == 20) {
      std::cout << "The input QBF formula is UNSAT. \n";
      std::cout << "The UNSAT/remaining clauses are. \n";
      for (coord_t i = 0; i < no_of_clauses; ++i) {
        if (dcnf_clauses[i].cls_present() == 0)
          continue;
        print_1d_vector(dcnf_clauses[i].lits());
        std::cout << "\n";
      }
    } else if (aut_present == 11) {
      std::cout << "The input QBF formula is Satisfiable.\n";
    }
  } else { // reduction of e_autarky
    for (lit_t &e : e_vars) {
      aut_present = e_autarky(dcnf_clauses, dcnf_variables, e);
    }

    cls_t unsat_cls;
    for (coord_t i = 0; i < no_of_clauses; ++i) {
      if (dcnf_clauses[i].cls_present() == 1) {
        unsat_cls.push_back(dcnf_clauses[i].lits());
      }
    }
    if (unsat_cls.size() > 0) {
      std::cout << "The input QBF formula is UNSAT. \n";
      std::cout << "The UNSAT/remaining clauses are. \n";
      for (cl_t &c : unsat_cls) {
        print_1d_vector(c);
        std::cout << "\n";
      }
    } else {
      std::cout << "The input QBF formula is Satisfiable.\n";
    }
  }

  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "entire run took " << elapsed.count() << " secs\n";

  return 0;
}
