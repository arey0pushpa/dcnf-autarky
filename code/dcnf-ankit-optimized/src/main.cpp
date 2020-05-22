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
 * 0. Improve defense with const and ASSERTS and STATIC ASSERTS.
 *   - handle Segfault cases.
 *
 * 1. Cleaning:
 *    - Clean the class interface remove non-essential functions
 *    - Make the Clause and variable initialization as a seperate function call
 *    - Multiple library uploads removal
 *    - Improve parsing code and flow
 *    - Check the update_avars case for e_autarky
 *
 * 2. Check and update the implementation of LOG encoding
 *
 * 3. Input format should be described in details and explain the strict vs
 *    loose.
 *
 * 4. Fix the MakeFile. Avoid heavy compiling everytime for debugging.
 *
 * 5. Manage Shared pointer correctly. Avoid memory leaks.
 *
 * 6. Output the statistics.
 *
 */

#include <iterator>  // std::advance

#include <chrono>
#include "dcnf.h"
#include "util.h"

void banner() {
  std::cout << "c DCNF-Autarky [version 0.0.1]. \n"
               "c Copyright (c) 2018-2019 Swansea "
               "University. \n";
  std::cout << "c \n";
}

std::string getFileName(std::string filePath, bool withExtension = false,
                        char seperator = '/') {
  // Get last dot position
  std::size_t dotPos = filePath.rfind('.');
  std::size_t sepPos = filePath.rfind(seperator);

  if (sepPos != std::string::npos) {
    return filePath.substr(
        sepPos + 1,
        filePath.size() -
            (withExtension || dotPos != std::string::npos ? 1 : dotPos));
  }
  return "";
}

int main(int argc, char *argv[]) {
  // ** Avoid Global Variables; use of CONST, MOVE, Value orientataion
  cl_t e_vars;       // {exists-var}
  cl_t a_vars;       // {forall-var}
  cls_t dep_set;     // {{dep-var}...}
  cls_t dcnf_fml;    // Input Cnf formula {Clauses} := {{lit,...}...}
  vstr_t aed_lines;  // Input formula uni existential and dependency lines

  // ** Add scope enum
  coord_t aut_present = 10;  // autarky present
  coord_t min_dep_size = 0;  // Used in statistics collection
  coord_t max_dep_size = 0;  // Used in stat collection
  coord_t dependency_var = 0;
  coord_t no_of_clauses = 0;  // Cleaning: Remove it!!
  coord_t no_of_var = 0;

  // ** Check this new!!
  dcnf_ptr d = std::shared_ptr<dcnf>(new dcnf());
  d->start_time = std::chrono::high_resolution_clock::now();

  d->cmdline_parsing(argc, argv);
  // TODO: Remove these number of parameters
  parse_qdimacs_file(d->filename, aed_lines, dcnf_fml, dep_set, a_vars, e_vars,
                     no_of_clauses, no_of_var, dependency_var, d->s_level,
                     min_dep_size, max_dep_size);

  if (d->output_type == 0) {
    banner();
  }

  d->fname = getFileName(d->filename);
  d->output_file_name = "/tmp/" + d->fname + "-dcnfAutarky.dimacs";

  d->no_of_vars = no_of_var;
  d->dcnf_variables.resize(no_of_var);
  d->aed_lines = aed_lines;

  // Dependent_set and e-a-var sorted
  // ** Encapsulate this operation!
  std::sort(dep_set.begin(), dep_set.end(),
            [](const cl_t &a, const cl_t &b) { return a[0] < b[0]; });
  std::sort(e_vars.begin(), e_vars.end());
  std::sort(a_vars.begin(), a_vars.end());
  assert(e_vars.size() == dep_set.size());
  cls_t unique_dep_set = unique_vectors(dep_set);
  lit_t dsize = dcnf_fml.size();

  // ** Add computation to do these!!! No computation in the main!!
  // ** Try to make it 1 based!
  // ** Try to implement using Pointers: share value: cashing!!
  // Dependency set should be a SET shared across the variables
  // Attach info to Variables Class (0 based: use e-1 to refer to var e)
  coord_t e_var_cntr = 0;
  for (lit_t e : e_vars) {
    d->dcnf_variables[e - 1].initialise_qtype('e');
    cl_t d_s = dep_set[e_var_cntr];
    assert(e == *d_s.begin());
    d_s.erase(d_s.begin());
    cl_t dep_vars = d_s;
    d->dcnf_variables[e - 1].initialise_dependency(dep_vars);
    d->dcnf_variables[e - 1].initialise_eindex(e_var_cntr);
    ++e_var_cntr;
  }

  // ** This should not be here!!! Abstract
  // Initialize Clause Class with E, A Qvar
  lit_t cls_indx = 0;
  for (coord_t i = 0; i < dsize; ++i) {
    [&] {  // Use of Lambda :)
      cl_t c_evars, c_elits, c_avars, c_alits;
      set_t posv, negv;
      for (lit_t l : dcnf_fml[i]) {
        lit_t indx = std::abs(l) - 1;
        if (l > 0) {
          posv.insert(indx);
          if (negv.count(indx)) {  // tauto case
            d->ntaut = d->ntaut + 1;
            return;
          }
        } else {
          negv.insert(indx);
          if (posv.count(indx)) {  // tauto case
            d->ntaut = d->ntaut + 1;
            return;
          }
        }
        if (d->dcnf_variables[indx].quantype == 'e') {
          c_evars.push_back(std::abs(l));
          c_elits.push_back(l);
        } else {
          c_avars.push_back(std::abs(l));
          c_alits.push_back(l);
        }
      }

      if (c_evars.size() == 0) {  // All univ variable case
        d->result = "UNSAT";
        if (d->output_type == 0) {
          std::cout << "c All univ variable case. \n"
                    << "c The input formula is UNSAT."
                    << "\nc\n";
          d->output();
        }
        if (d->output_type == 0 || d->output_type == 1) {
          d->display_rresult();
        }
      }
      // Variable presence info update
      // ** Expensive process check if usefullness; Remove!!
      for (lit_t v : posv) {
        d->dcnf_variables[v].pos_cls.insert(cls_indx);
      }
      for (lit_t v : negv) {
        d->dcnf_variables[std::abs(v)].neg_cls.insert(cls_indx);
      }
      // Push the clause in the dcnf_clauses
      Clauses *cls = new Clauses;
      cls->initialise_lits(dcnf_fml[i]);

      cls->initialise_evars(c_evars);
      cls->initialise_elits(c_elits);
      cls->initialise_avars(c_avars);
      cls->initialise_alits(c_alits);
      d->dcnf_clauses.push_back(*cls);
      delete cls;
      ++cls_indx;
    }();
  }

  // Empty clause (modulo tauto) case
  if (d->dcnf_clauses.size() == 0) {
    d->result = "SAT";
    if (d->output_type == 0) {
      std::cout << "c Empty input clause (modulo tautology).\n"
                << "c Input formula is SAT.\nc\n";
      d->output();
    }
    if (d->output_type == 0 || d->output_type == 1) {
      d->display_rresult();
    }
  }
  // Ignore non occuring evars for bf_vars
  for (const lit_t e : e_vars) {
    coord_t i = e - 1;
    if (d->dcnf_variables[i].pos_cls.empty() &&
        d->dcnf_variables[i].neg_cls.empty()) {
      d->dcnf_variables[i].update_presence(0);
    }
  }

  const coord_t cls_size = d->dcnf_clauses.size();
  d->no_of_clauses = cls_size;
  d->e_vars = e_vars;
  for (coord_t i = 0; i < cls_size; ++i) {
    d->dcnf_fml.push_back(d->dcnf_clauses[i].lits);
    d->present_clauses.insert(i);
  }

  for (lit_t e : e_vars) {
    if (!d->dcnf_variables[e - 1].present) continue;
    d->active_evars.push_back(e);
  }
  for (lit_t a : a_vars) {
    if (!d->dcnf_variables[a - 1].present) continue;
    d->active_avars.push_back(a);
  }

  if (d->gen > 0) {
    d->existential_subset();
  }

  if (d->output_type == 0) {
    std::cout << "c Input QBF/DQBF path: " << d->filename << "\n";
    std::cout << "c Output SAT Translation Path: " << d->output_file_name
              << "\n";
    std::cout << "c Input Clause Count (tautology free): " << cls_size
              << "\nc\n";
  }

  if (d->reduction_type == 2 || d->reduction_type == 3) {
    d->min_satisfying_assgn(d->aut_level);
  }
  d->old_cls_size = cls_size;
  coord_t updated_cls_size = 0;

  while (1) {
    // E_Autarky reduction
    if (d->reduction_type == 1 || d->reduction_type == 3) {
      auto t1 = std::chrono::high_resolution_clock::now();
      if (d->reduction_type == 3) {
        d->selected_boolfunc(d->aut_level);
      }
      cl_t iter_active_evars;
      if (d->output_type == 0)
        std::cout << "c Performing E1-Autarky iteration.\n";
      for (lit_t e : d->active_evars) {
        aut_present = d->e_autarky(e);
        if (aut_present == 0) {
          iter_active_evars.push_back(e);
        } else if (aut_present == 10) {
          d->result = "SAT";
          // Use of next two lines?
          d->update_avars();
          d->update_evars();
          d->print_results();
        }
      }
      if (d->output_type == 0) {
        std::cout << "c Remaining clauses count after E-Reduction: "
                  << d->present_clauses.size() << "\n";
      }
      updated_cls_size = d->present_clauses.size();
      auto t2 = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
      std::cout << "c This E_Autarky iteration took: " << duration
                << " secs.\nc\n";
      if (updated_cls_size == d->old_cls_size &&
          (d->reduction_type == 1 || d->result == "UNSAT")) {
        d->print_results();
      } else if (updated_cls_size != d->old_cls_size) {
        d->ever_reduced = 1;
        d->ered_last = 1;
        d->result = "RED";
        d->naut += 1;
        if (d->reduction_type == 1) {
          d->old_cls_size = updated_cls_size;
        }
      }
      d->active_evars = iter_active_evars;
      d->update_avars();
      d->update_evars();
    }
    // A_Autraky reduction
    if (d->reduction_type == 2 || d->reduction_type == 3) {
      if (d->output_type == 0) {
        std::cout << "c Performing A1-Autarky iteration.\n";
      }
      auto t3 = std::chrono::high_resolution_clock::now();
      // Only recalculate in case not already calculated. TEST!!
      assert(d->present_clauses.size() <= d->old_cls_size);
      if ((d->present_clauses.size() != d->old_cls_size) ||
          d->reduction_type == 2) {
        d->selected_boolfunc(d->aut_level);
      }

      aut_present = d->a_autarky(d->filename, d->output_file_name, d->encoding);
      auto t4 = std::chrono::high_resolution_clock::now();
      auto duration1 =
          std::chrono::duration_cast<std::chrono::seconds>(t4 - t3).count();
      if (d->output_type == 0) {
        std::cout << "c This A1_Autarky iteration took: " << duration1
                  << " secs.\n";
      }
      if (d->output_type == 0) {
        std::cout << "c Remaining clauses count after A-Reduction: "
                  << d->present_clauses.size() << "\nc\n";
      }
      d->display_result(aut_present, d->output_type);
    }
  }
  return 0;
}
