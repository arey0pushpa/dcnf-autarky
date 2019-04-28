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

#include <bitset>  // std::bitset
#include <chrono>
#include <cmath>
#include <fstream>
#include <iterator>  // std::advance
#include <string>

#include "dcnf.h"
#include "util.h"

int main(int argc, char *argv[]) {
  std::string filename;
  std::string output_file_name = "/tmp/dcnfAutarky.dimacs";
  coord_t dependency_var = 0;
  coord_t level = 1;
  coord_t s_level = 0;
  coord_t encoding = 0;
  coord_t reduction_type = 0;  // Start with search for e_var autarkies
  coord_t aut_present = 10;

  if (cmd_option_exists(argv, argv + argc, "-h")) {
    std::cout << "DCNF-Autarky [version 0.0.1]. (C) Copyright 2018-2019 "
                 "Swansea UNiversity. \nUsage: ./dcnf [-i filename] [-o "
                 "filename] [-l "
                 "level] [-e encoding] [-s strictness; 0:general, 1:strict] "
                 "[-r reduction; 0:e_autarky, 1:a_autarky]\n";
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
  cls_t dcnf_fml;  // Input Cnf formula {Clauses} := {{lit,...}...}

  cl_t e_vars;    // {exists-var}
  cl_t a_vars;    // {forall-var}
  cls_t dep_set;  // {{dep-var}...}

  // sel_bf selected_bf;               // All bf (v,f) pairs {(e-var, )...}
  // minsat_ass minsat_clause_assgmt;  // All S(C)'s: {<e-var,bf(k)>...}

  coord_t min_dep_size = 0;
  coord_t max_dep_size = 0;

  auto start = std::chrono::high_resolution_clock::now();
  dcnf_ptr d = std::shared_ptr<dcnf>(new dcnf());

  parse_qdimacs_file(filename, dcnf_fml, dep_set, a_vars, e_vars, no_of_clauses,
                     no_of_var, dependency_var, s_level, min_dep_size,
                     max_dep_size);

  // TEMP FIX. TODO:IMPLEMENT THE CASE OF VAR ABSENCE IN MATRIX CASE DIRECTLY
  no_of_var = e_vars.size() + a_vars.size();

	d->no_of_vars = no_of_var;
  // Create Variable Objects.
  d->dcnf_variables.resize(no_of_var);

  // make dep set sorted linearly accdn to evar
  std::sort(dep_set.begin(), dep_set.end(),
            [](const cl_t &a, const cl_t &b) { return a[0] < b[0]; });

  std::sort(e_vars.begin(), e_vars.end());
  std::sort(a_vars.begin(), a_vars.end());

  auto avar_iterator = a_vars.begin();
  auto evar_iterator = e_vars.begin();

  /*
  std::cout << "Printing universal variables" << '\n';
  print_1d_vector(a_vars);
  std::cout << "\nPrinting existenial variables" << '\n';
  print_1d_vector(e_vars);
  std::cout << "\n";
  */

  coord_t dep_index = 0;
  bool a_vars_end = false;
  bool e_vars_end = false;
  if (avar_iterator == a_vars.end()) a_vars_end = true;
  if (evar_iterator == e_vars.end()) e_vars_end = true;

  // Create a vector of Class Variables
  // attach add info and access based on their index
  // Structure is defined by the input dqdimacs file v_0,...,v_noofvars-1
  coord_t e_var_cntr = 0;
  for (coord_t i = 0; i < no_of_var; ++i) {
    if (!a_vars_end && i == *avar_iterator - 1) {
      if (std::next(avar_iterator) == a_vars.end()) {
        a_vars_end = true;
      } else {
        avar_iterator = std::next(avar_iterator);
      }
    } else if (!e_vars_end && i == *evar_iterator - 1) {
      d->dcnf_variables[i].initialise_qtype('e');
      cl_t d_s = dep_set[dep_index];
      // erase evar frm dep set: d_s shld be just dependent vars
      d_s.erase(d_s.begin());
      cl_t dep_vars = d_s;
      d->dcnf_variables[i].initialise_dependency(dep_vars);
      ++dep_index;
      d->dcnf_variables[i].initialise_eindex(e_var_cntr);
      ++e_var_cntr;
      if (std::next(evar_iterator) == e_vars.end()) {
        e_vars_end = true;
      } else {
        evar_iterator = std::next(evar_iterator);
      }
    } else {
      d->dcnf_variables[i].initialise_qtype('e');
      d->dcnf_variables[i].initialise_eindex(e_var_cntr);
      ++e_var_cntr;
    }
  }

  cls_t unique_dep_set = unique_vectors(dep_set);
  lit_t dsize = dcnf_fml.size();

  // Create vector of Clause Class to initialize E, A Qvar
  // std::vector<Clauses> dcnf_clauses;
  coord_t cls_indx = 0;
  for (coord_t i = 0; i < dsize; ++i) {
    [&] {  // Use of Lambda :) Yeahhh...
      cl_t c_evars, c_elits, c_avars, c_alits;
      set_t posv, negv;
      for (const lit_t l : dcnf_fml[i]) {
        coord_t indx = std::abs(l) - 1;
        if (l > 0) {
          posv.insert(indx);
          // If the clause is TAUTO ignore it
          if (negv.count(indx)) return;
        } else {
          negv.insert(indx);
          if (posv.count(indx)) return;
        }
        if (d->dcnf_variables[indx].qtype() == 'e') {
          c_evars.push_back(std::abs(l));
          c_elits.push_back(l);
        } else {
          c_avars.push_back(std::abs(l));
          c_alits.push_back(l);
        }
      }
      // Variable presence info update
      for (coord_t v : posv) {
        d->dcnf_variables[v].pos_polarity(cls_indx);
      }
      // TODO: misuse of coord_t conversion
      for (coord_t v : negv) {
        d->dcnf_variables[v].neg_polarity(cls_indx);
      }

      // Push the clause in the dcnf_clauses
      Clauses *cls = new Clauses;
      cls->initialise_lits(dcnf_fml[i]);

      cls->initialise_evars(c_evars);
      cls->initialise_elits(c_elits);

      cls->initialise_avars(c_avars);
      cls->initialise_alits(c_alits);

      d->dcnf_clauses.push_back(*cls);
      //delete cls;  // Avoid memory leak, My God!
      ++cls_indx;
    }();
  }

  // TODO: Check if not considering univ variable harm? NO, for now.
  // Every existential variable that do not occur anywhere in the formula is
  // not considered for the bf_vars
  for (const lit_t e : e_vars) {
    coord_t i = e - 1;
    if (d->dcnf_variables[i].pos_pol().empty() &&
        d->dcnf_variables[i].neg_pol().empty()) {
      d->dcnf_variables[i].update_presence(0);
    }
  }

  d->no_of_clauses = d->dcnf_clauses.size();
  d->e_vars = e_vars;
  for (coord_t i; i < d->dcnf_clauses.size(); ++i) {
    d->dcnf_fml.push_back(d->dcnf_clauses[i].m_lits);
    d->present_clauses.insert(i);
  }

  for (lit_t e : e_vars) {
    if (!d->dcnf_variables[e - 1].var_present()) continue;
    d->active_evars.push_back(e);
  }

  for (lit_t a : a_vars) {
    if (!d->dcnf_variables[a - 1].var_present()) continue;
    d->active_avars.push_back(a);
  }

  // Index Lookup for the evar
  //d->active_evar_index.resize(e_size);
   // Lookup for the present clauses
  //d->present_cls_index.resize(cls_size);

  // For evars and dcnf_clauses
  d->set_all_solutions(level);

  // TODO: Implement all three possible combinations of e_ and a_autarky
  while (1) {
/*     cl_t iter_active_evars;
 *     // reduction of e_autarky
 *     // TODO: Optimize the variables use
 *     for (lit_t e : d->active_evars) {
 *       if (d->dcnf_variables[e - 1].pos_cls.size() +
 *               d->dcnf_variables[e - 1].neg_cls.size() ==
 *           0) {
 *         aut_present = 10;
 *       } else {
 *         aut_present = d->e_autarky(e);
 *       }
 *       if (aut_present == 10) {
 *         d->assigned_evars.push_back(e);
 *         for (lit_t i : d->dcnf_variables[e - 1].pos_pol()) {
 *           d->dcnf_clauses[i].present = 0;
 *           d->present_clauses.erase(i);
 *           d->deleted_clauses.insert(i);
 *           d->propagate_cls_removal(i);
 *         }
 *         for (lit_t i : d->dcnf_variables[e - 1].neg_pol()) {
 *           d->dcnf_clauses[i].present = 0;
 *           d->present_clauses.erase(i);
 *           d->deleted_clauses.insert(i);
 *           d->propagate_cls_removal(i);
 *         }
 *       } else {
 *         iter_active_evars.push_back(e);
 *       }
 *
 *       if (d->present_clauses.size() == 0) {
 *         std::cout << "The input QBF formula is Satisfiable by an e_autarky "
 *                      "reduction.\n";
 *         // TODO: Print the satisfying assignments!!!
 *         exit(0);
 *       }
 *       // TODO: Add evar and avoid printing this everytime :)
 *       std::cout << "Remaining clauses e_autarky reductions" << '\n';
 *       d->print_remaining_cls();
 *     }
 *
 *     d->active_evars = iter_active_evars;
 *     iter_active_evars.clear();
 *
 *     if (d->active_evars.size() == 0) {
 *       std::cout << "All univ variable case. Fml SAT." << '\n';
 *       // Send it to a SAT solver?
 *       exit(0);
 *     } */
    // TODO: Check the code and make sure implemented correctly
    aut_present =
        d->a_autarky(filename, output_file_name, encoding);
    if (aut_present == 20) {
      std::cout << "The input QBF formula is UNSAT. \n";
      std::cout << "The UNSAT/remaining clauses are. \n";
      d->print_remaining_cls();
      exit(0);
    } else if (aut_present == 11) {
      std::cout << "The input QBF formula is Satisfiable by an a_autarky "
                   "reduction.\n ";
      // TODO: Print the satisfying assignments!!!
      exit(0);
    } else {
      // TODO: Propagate and update the data structure
      std::cout << "The remaining clauses after a_autarky reductions" << '\n';
      d->print_remaining_cls();
			//exit(0);
    }
  }
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "entire run took " << elapsed.count() << " secs\n";

  return 0;
}
