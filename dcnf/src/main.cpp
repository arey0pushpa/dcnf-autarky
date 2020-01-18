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
 * 7. printing results:
 *    - Homemade printing instead of the current default printing*
 *    - Handle input file structure errors in the case of the R file output
 */

#include <iterator> // std::advance

#include "dcnf.h"
#include "util.h"

void header() {
  std::cout << "c DCNF-Autarky [version 0.0.1]. \n"
               "c Copyright (c) 2018-2019 Swansea "
               "University. \n";
  std::cout << "c \n";
}

int main(int argc, char *argv[]) {
  cl_t e_vars;              // {exists-var}
  cl_t a_vars;              // {forall-var}
  cls_t dep_set;            // {{dep-var}...}
  cls_t dcnf_fml;           // Input Cnf formula {Clauses} := {{lit,...}...}
  coord_t aut_present = 10; // autarky present
  coord_t min_dep_size = 0; // Used in statistics collection
  coord_t max_dep_size = 0; // Used in stat collection
  coord_t dependency_var = 0;
  coord_t no_of_clauses = 0; // Cleaning: Remove it!!
  coord_t no_of_var = 0;

  dcnf_ptr d = std::shared_ptr<dcnf>(new dcnf());
  d->cmdline_parsing(argc, argv);
  // TODO: Remove these number of parameters
  parse_qdimacs_file(d->filename, dcnf_fml, dep_set, a_vars, e_vars,
                     no_of_clauses, no_of_var, dependency_var, d->s_level,
                     min_dep_size, max_dep_size);

  header();
  d->no_of_vars = no_of_var;
  d->dcnf_variables.resize(no_of_var);
  // Dependent_set and e-a-var sorted
  std::sort(dep_set.begin(), dep_set.end(),
            [](const cl_t &a, const cl_t &b) { return a[0] < b[0]; });
  std::sort(e_vars.begin(), e_vars.end());
  std::sort(a_vars.begin(), a_vars.end());
  assert(e_vars.size() == dep_set.size());
  cls_t unique_dep_set = unique_vectors(dep_set);
  lit_t dsize = dcnf_fml.size();

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

  // Initialize Clause Class with E, A Qvar
  lit_t cls_indx = 0;
  for (coord_t i = 0; i < dsize; ++i) {
    [&] { // Use of Lambda :)
      cl_t c_evars, c_elits, c_avars, c_alits;
      set_t posv, negv;
      for (lit_t l : dcnf_fml[i]) {
        lit_t indx = std::abs(l) - 1;
        if (l > 0) {
          posv.insert(indx);
          if (negv.count(indx)) { // tauto case
            d->ntaut = d->ntaut + 1;
            return;
          }
        } else {
          negv.insert(indx);
          if (posv.count(indx)) { // tauto case
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
      if (c_evars.size() == 0) { // All univ variable case
        d->result = "UNSAT";
        if (d->output_type == 0) {
          std::cout << "All univ variable case. The input formula is UNSAT."
                    << '\n';
        } else {
          d->display_rresult();
        }
        exit(0);
      }
      // Variable presence info update
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

  // handle empty clause (modulo tauto) case
  if (d->dcnf_clauses.size() == 0) {
    d->result = "SAT";
    if (d->output_type == 0) {
      std::cout
          << "Empty input clause (modulo tautology). Input formula is SAT\n";
    } else {
      d->display_rresult();
    }
    exit(0);
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
  for (coord_t i; i < cls_size; ++i) {
    d->dcnf_fml.push_back(d->dcnf_clauses[i].lits);
    d->present_clauses.insert(i);
  }

  for (lit_t e : e_vars) {
    if (!d->dcnf_variables[e - 1].present)
      continue;
    d->active_evars.push_back(e);
  }
  for (lit_t a : a_vars) {
    if (!d->dcnf_variables[a - 1].present)
      continue;
    d->active_avars.push_back(a);
  }

  d->min_satisfying_assgn(d->aut_level);
  d->old_cls_size = cls_size;
  d->updated_cls_size = 0;

  while (1) {
    // E_Autarky reduction
    if (d->reduction_type == 1 || d->reduction_type == 3) {
      d->selected_boolfunc(d->aut_level);
      cl_t iter_active_evars;
      if (d->output_type == 0)
        std::cout << "Performing E1 Autarky iteration...\n";
      for (lit_t e : d->active_evars) {
        aut_present = d->e_autarky(e);
        if (aut_present == 1) {
          iter_active_evars.push_back(e);
        } else if (aut_present == 10) {
          d->result = "SAT";
          d->update_avars();
          d->update_evars();
          if (d->output_type == 0)
            d->display_eresult(aut_present);
          else
            d->display_rresult();
          exit(0);
        }
      }
      if (d->output_type == 0) {
        std::cout << "Remaining clauses e_autarky reductions" << '\n';
        d->print_remaining_cls();
      }
      d->updated_cls_size = d->present_clauses.size();
      if (d->updated_cls_size == d->old_cls_size && d->reduction_type == 1) {
        if (d->output_type == 1)
          d->display_rresult();
        exit(0);
      } else if (d->updated_cls_size != d->old_cls_size) {
        d->result = "RED";
        d->old_cls_size = d->updated_cls_size;
      }
      d->active_evars = iter_active_evars;
      d->update_avars();
      d->update_evars();
    }
    // A_Autraky reduction
    if (d->reduction_type == 2 || d->reduction_type == 3) {
      if (d->output_type == 0) {
        std::cout << "Performing A1 Autarky iteration...\n";
      }
      d->selected_boolfunc(d->aut_level);
      aut_present = d->a_autarky(d->filename, d->output_file_name, d->encoding);
      d->display_result(aut_present, d->output_type);
    }
  }
  return 0;
}
