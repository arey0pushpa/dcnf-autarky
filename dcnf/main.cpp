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
 * 1. Implement Lograthemic encoding.
 * 2. Implement linear encoding.
 * 2. Handle empty clause and tautology.
 *    - Add checks to avoid basic SAT and UNSAT cases.
 * 3. namespace creation : do using the classes or namespace.
 *    - Passing the parameters all the time looks ugly.
 */

#include <bitset>  // std::bitset
#include <chrono>
#include <cmath>
#include <fstream>
#include <string>

#include "defs.h"

template <typename t>
std::vector<std::vector<t>> unique_vectors(std::vector<std::vector<t>> input) {
  for (auto &i : input) {
    i.erase(i.begin());
  }
  std::sort(input.begin(), input.end());
  input.erase(std::unique(input.begin(), input.end()), input.end());
  return input;
}

int main(int argc, char *argv[]) {
  std::string filename;
  std::string output_file_name = "/tmp/a.out";
  coord_t dependency_var = 0;
  coord_t level = 1;
  coord_t s_level = 0;
  coord_t encoding = 0;

  coord_t no_of_clauses = 0;
  coord_t no_of_var = 0;

  if (cmd_option_exists(argv, argv + argc, "-h")) {
    std::cout << "DCNF-Autarky [version 0.0.1]. (C) Copyright 2018-2019 "
                 "Swansea UNiversity. \nUsage: ./dcnf [-i filename] [-o "
                 "filename] [-l "
                 "level] [-e encoding] [-s strictness; 0:general, 1:strict]\n";
    exit(0);
  }

  char *file_name = get_cmd_option(argv, argv + argc, "-i");
  char *output_file = get_cmd_option(argv, argv + argc, "-o");
  char *level_set = get_cmd_option(argv, argv + argc, "-l");
  char *encoding_chosen = get_cmd_option(argv, argv + argc, "-e");
  char *strict_level = get_cmd_option(argv, argv + argc, "-s");

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

  auto start = std::chrono::high_resolution_clock::now();

  /** Global Variables ***/
  cls_t dcnf_fml;  // Input Cnf formula {Clauses} := {{lit,...}...}

  cl_t e_vars;    // {exists-var}
  cl_t a_vars;    // {forall-var}
  cls_t dep_set;  // {{dep-var}...}

  sel_bf selected_bf;               // All bf (v,f) pairs {(e-var, )...}
  minsat_ass minsat_clause_assgmt;  // All S(C)'s: {<e-var,bf(k)>...}

  cls_t cnf_fml;  // dimacs/cnf fml {{lit...}...}
  cl_t cnf_vars;  // dimacs/cnf var {cnf-vars}

  cl_t cs_vars;
  cls_t bf_vars;
  cl_t pa_vars;

  coord_t min_dep_size = 0;
  coord_t max_dep_size = 0;

  parse_qdimacs_file(filename, dcnf_fml, dep_set, a_vars, e_vars, no_of_clauses,
                     no_of_var, dependency_var, s_level, min_dep_size,
                     max_dep_size);

  // TEMP FIX. IMPLEMENT THE CASE OF VAR ABSENCE IN MATRIX CASE DIRECTLY
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
  if (avar_iterator == a_vars.end()) a_vars_end = true;
  if (evar_iterator == e_vars.end()) e_vars_end = true;

  coord_t e_var_cntr = 0;
  // Create a big vector[used Classes to attach additional info]
  // of all variables access based on their index.
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

  // Create no_of_clauses Objects and initialise exits and forall quant var
  lit_t dsize = dcnf_fml.size();
  std::vector<Clauses> dcnf_clauses;
  dcnf_clauses.resize(dsize);

  for (coord_t i = 0; i < dsize; ++i) {
    cl_t c_evars;
    cl_t c_elits;
    cl_t c_avars;
    cl_t c_alits;
    dcnf_clauses[i].initialise_lits(dcnf_fml[i]);
    for (const lit_t l : dcnf_fml[i]) {
      if (dcnf_variables[std::abs(l) - 1].qtype() == 'e') {
        c_evars.push_back(std::abs(l));
        c_elits.push_back(l);
      } else {
        c_avars.push_back(std::abs(l));
        c_alits.push_back(l);
      }
    }
    dcnf_clauses[i].initialise_evars(c_evars);
    dcnf_clauses[i].initialise_elits(c_elits);

    dcnf_clauses[i].initialise_avars(c_avars);
    dcnf_clauses[i].initialise_alits(c_alits);
  }

  /* Todo: Implement a dependency Scheme in case no dependency given
    if ( dependency_var == 0 ) {
      // Implement a dependency scheme
    } */
  set_all_solutions(dcnf_clauses, dcnf_variables, selected_bf,
                    minsat_clause_assgmt, no_of_clauses, no_of_var, level);

  //print_3d_vector(minsat_clause_assgmt);

  /** Traslation variables with ordering */
  coord_t index = 1;

  // cs variable := #no_of_clauses -----------------------------
  for (coord_t i = 0; i < no_of_clauses; ++i) {
    cs_vars.push_back(index);
    index += 1;
  }

  coord_t lbf_var_size = 0;
  cl_t lbf_vars, s_bf;
  // bf variable := two_dim [v] [f_v] -------------------------
  coord_t preindex = index;
  coord_t bf_var_count = 0;

  for (coord_t i = 0; i < selected_bf.size(); ++i) {
    for (coord_t j = 0; j < selected_bf[i].size(); ++j) {
      s_bf.push_back(index);
      index += 1;
    }
    bf_vars.push_back(s_bf);
    s_bf.clear();
  }

  // Additional 1 due to index count is incremented after last use.
  std::vector<bf_lbf_converter> bf2lbf_var_map(index - (no_of_clauses + 1));

  if (encoding == 1) {  // LOG Encoding
    index = preindex;
    for (coord_t i = 0; i < selected_bf.size(); ++i) {
      bf_var_count += selected_bf[i].size();
    }
    // m := |bf_vars| for the log encoding
    lbf_var_size = ceil(log(bf_var_count + 1) / log(2));
    for (coord_t k = 0; k < lbf_var_size; ++k) {
      lbf_vars.push_back(index);
      index += 1;
    }
  }

  // pa variable := Only consider unique mapping
  cls_t pa_var_set;
  minsat_ass pa_var_msat_ass(e_vars.size());
  cls_t msat_concrete_var_map(e_vars.size());
  cls_t clausewise_pa_var_map(no_of_clauses);
  coord_t msat_cntr = 1;
  for (coord_t i = 0; i < minsat_clause_assgmt.size(); ++i) {
    for (coord_t j = 0; j < minsat_clause_assgmt[i].size(); ++j) {
      cl_t dummy = minsat_clause_assgmt[i][j];
      lit_t slit = std::abs(dummy[0]);
      assert(slit > 0);
      lit_t elit = dcnf_variables[slit - 1].eindex();
      lit_t pa_indx = find_vector_index(pa_var_msat_ass[elit], dummy);
      if (pa_indx != -1) {
        clausewise_pa_var_map[i].push_back(
            msat_concrete_var_map[elit][pa_indx]);
      } else {
        pa_var_msat_ass[elit].push_back(dummy);
        msat_concrete_var_map[elit].push_back(index);
        ++msat_cntr;
        clausewise_pa_var_map[i].push_back(index);
        pa_vars.push_back(index);
        ++index;
      }
    }
  }

  std::cout << "selected Bf_vars is : ";
  print_2d_vector(bf_vars);
  std::cout << "\n";

  std::cout << "pa var m ass is : ";
  print_3d_vector(pa_var_msat_ass);
  std::cout << "\n";

  std::cout << "selected Bf_vars is : ";
  print_2d_vector_pair(selected_bf);
  std::cout << "\n";

  std::cout << "Min sat concrete var map is : ";
  print_2d_vector(msat_concrete_var_map);
  std::cout << "\n";

  // --- Build Constraints
  non_trivial_autarky(cs_vars, cnf_fml);  // (4.5)

  touched_clauses(cs_vars, clausewise_pa_var_map, cnf_fml);  // (4.3)

  satisfied_clauses(encoding, no_of_clauses, lbf_vars, dcnf_clauses,
                    dcnf_variables, bf_vars, pa_var_msat_ass,
                    msat_concrete_var_map, selected_bf, cnf_fml,
                    bf2lbf_var_map);  // (4.2)

	std::cout << "The 4.2 constratints are done!... \n";

  untouched_clauses(encoding, lbf_vars, dcnf_clauses, dcnf_variables, bf_vars,
                    cs_vars, no_of_clauses, cnf_fml, bf2lbf_var_map);  // (4.4)

  if (encoding == 0) {  // Quadratic encoding has AtMostOne() constraint
    for (coord_t i = 0; i < no_of_var; ++i) {  // (4.1)
      if (dcnf_variables[i].qtype() == 'e') {
        coord_t indx = dcnf_variables[i].eindex();
        at_most_one(bf_vars[indx], cnf_fml);
      }
    }
  }

  std::ofstream fout(output_file_name);

  if (!fout) {
    std::cerr << "Error opening file..." << output_file_name << "\n";
    return 1;
  }

  // Writing the dcnf output in dimacs format
  fout << "c This is a output dimacs file of input file: " << filename << '\n';
  fout << "c There are total " << cs_vars.size() << " cs-variables. ";
  for (lit_t c : cs_vars) {
    fout << c << " ";
  }

  fout << "\n";

  std::string bf_var_line;
  coord_t total = 0;

  for (coord_t i = 0; i < bf_vars.size(); ++i) {
    total += bf_vars[i].size();
    for (coord_t j = 0; j < bf_vars[i].size(); ++j) {
      bf_var_line = bf_var_line + std::to_string(bf_vars[i][j]) + " ";
    }
    if (i < bf_vars.size() - 1) bf_var_line = bf_var_line + " +  ";
  }

  if (encoding == 1) {
    fout << "c There are total " << total << " distinct lbf-variables. "
         << lbf_var_size;
  } else {
    fout << "c There are total " << total << " distinct bf-variables. "
         << bf_var_line;
  }
  fout << "\n";
  fout << "c There are total " << pa_vars.size() << " distinct pa-variables. ";
  for (coord_t i = 0; i < pa_vars.size(); ++i) {
    fout << pa_vars[i] << " ";
  }
  fout << '\n';
  fout << "p cnf " << index - 1 << " " << cnf_fml.size() << "\n";

  for (auto &C : cnf_fml) {
    for (auto lit : C) {
      fout << lit << " ";
    }
    fout << "0"
         << "\n";
  }

  output(filename, output_file_name, level, s_level, encoding, no_of_var,
         no_of_clauses, a_vars.size(), e_vars.size(), unique_dep_set.size(),
         pa_vars.size(), total, cs_vars.size(), index - 1, cnf_fml.size());

  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "entire run took " << elapsed.count() << " secs\n";
  return 0;
}
