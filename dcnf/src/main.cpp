// Ankit Shukla, 22.June.2018 (Swansea)
/* Copyright 2018 Ankit Shukla
 * This is a free software.
 * */

#include <chrono>
#include <cmath>
#include <fstream>

#include "defs.h"

int main(int argc, char* argv[]) {
  // Command line parsing var
  std::string filename;
  bool input_file = false;
  bool level_func = false;
  coord_t dependency_var = 0;
  coord_t level = 0;
  coord_t encoding = 0;

  coord_t no_of_clauses = 0;
  coord_t no_of_var = 0;

  /** Handle Command Line parsing **/
  command_line_parsing(argc, argv, filename, level, encoding, input_file,
                       level_func);

  if (input_file == false) {
    filename = "./examples/qbflib.qdimacs";
  }

  auto start = std::chrono::high_resolution_clock::now();

  /** Global Variables ***/

  cls_t dcnf_fml;  // Input Cnf formula {Clauses} := {{lit,...}...}

  cl_t e_vars;    // {exists-vars}
  cl_t a_vars;    // {forall-vars}
  cls_t dep_set;  // {{dep-var}...}

  sel_bf selected_bf;               // All bf (v,f) pairs {(e-var, )...}
  minsat_ass minsat_clause_assgmt;  // All S(C)'s: {<e-var,bf(k)>...}

  cls_t cnf_fml;  // dimacs/cnf fml {{lit...}...}
  cl_t cnf_vars;  // dimacs/cnf var {cnf-vars}

  cl_t cs_var;   // set of cs-var
  cls_t bf_var;  // set of set of bf-var
  cl_t pa_vars;  // set of pa-var

  /**** Implement the DQCNF Code ****/

  /** Parse Input file **/
  parse_qdimacs_file(filename, dcnf_fml, dep_set, a_vars, e_vars, no_of_var,
                     no_of_clauses, dependency_var);

  // std::cout << "Printing input cnf formula...\n";
  // print_2d_vector(dcnf_fml);

  if (e_vars.size() == dep_set.size()) {
    for (coord_t i = 0; i < e_vars.size(); ++i) {
      std::cout << "The e_vars " << e_vars[i] << " has dependency: ";
      print_1d_vector(dep_set[i]);
      std::cout << '\n';
    }
  }

  std::cout << "Only Handling the parsing now. Exiting." << '\n';

  /* Todo: Implement a dependency Scheme in case no dependency given
    if ( dependency_var == 0 ) {
      // Implement a dependency scheme
    } */

  /** Preprocessing 
  preprocess_fml(selected_bf, minsat_clause_assgmt, dcnf_fml, dep_set, a_vars,
                 e_vars, level);

  std::cout << "\nThe genearted bool func: s(v) is: "
            << "\n";
//  print_2d_vector_pair(selected_bf);
  std::cout << "\nThe generated min sat clause assgmt S(C) is: "
            << "\n";
//  print_3d_vector(minsat_clause_assgmt);

  ** Create traslation Variables/ordering *
  coord_t index = 1;

  // cs variable
  for (coord_t i = 0; i < dcnf_fml.size(); ++i) {
    cs_var.push_back(index);
    index += 1;
  }

  // bf variable
  // std::cout << "The size of s(v) is: " << selected_bf.size() << "\n";

  if (encoding == 0) {
    Vec1D dummy_vec;
    for (coord_t i = 0; i < selected_bf.size(); ++i) {
      for (coord_t j = 0; j < selected_bf[i].size(); ++j) {
        dummy_vec.push_back(index);
        index += 1;
      }
      bf_var.push_back(dummy_vec);
      dummy_vec.clear();
    }
  } else {
    coord_t m = ceil(log(selected_bf.size() + 1) / log(2));
  }

  // pa variable
  ** Improved Encoding than Blockwise encoding.
   * Straigten, sort and remove duplicates and then map
   * todo: add flattern function:
   *       2D-1D 3D-2D in util.cpp
  Vec2D dummy_pa;
  for (coord_t i = 0; i < minsat_clause_assgmt.size(); ++i) {
    for (coord_t j = 0; j < minsat_clause_assgmt[i].size(); ++j) {
      dummy_pa.push_back(minsat_clause_assgmt[i][j]);
    }
  }

  // todo: Implement using sets.
  // In the case of large duplicate that approach works better.
  sort(dummy_pa.begin(), dummy_pa.end());
  dummy_pa.erase(unique(dummy_pa.begin(), dummy_pa.end()), dummy_pa.end());

  std::cout << "Sorted S(C) set is : \n";
  print_2d_vector(dummy_pa);
  std::cout << "\n";

  for (coord_t i = 0; i < dummy_pa.size(); ++i) {
    pa_vars.push_back(index);
    index += 1;
  }

  std::cout << "cs-var are: ";
  print_1d_vector(cs_var);
  std::cout << "\n\n";

  std::cout << "bf-var are:\n";
  print_2d_vector(bf_var);
  std::cout << "\n";

  std::cout << "pa-var are : ";
  print_1d_vector(pa_vars);
  std::cout << "\n\n";

  * Create Constraints *
  // 4.5 Non trivial Autarky
  non_trivial_autarky(cs_var, cnf_fml);

  // 4.3 Selected clauses: t(C) -> P(C)
  touched_clauses(cs_var, pa_vars, dummy_pa, minsat_clause_assgmt, cnf_fml);

  if (encoding == 0) {
    // 4.2 pa-variable constraint
    satisfied_clauses(e_vars, pa_vars, dep_set, bf_var, dummy_pa, selected_bf,
                      cnf_fml);

    // 4.4. Untoched clauses: !t(C) -> N(C)
    untouched_clauses(e_vars, cs_var, bf_var, dcnf_fml, cnf_fml);

    // 4.1 At Most One Constraint
    for (coord_t i = 0; i < cs_var.size(); ++i) {
      at_most_one(bf_var[i], cnf_fml);
    }
  } else {
    // Lograthemic encoding.
  }

  // print_2d_vector ( cnf_fml );

  std::string fname = "/tmp/dcnf.dimacs";
  std::cout << "Writing the DIMACS file to .. " << fname << "\n";
  std::ofstream fout(fname);

  if (!fout) {
    std::cerr << "Error opening file..." << fname << "\n";
    return 1;
  }

  // fout << "c" << "Writing the dcnf output in dimacs format";
  fout << "p cnf " << index - 1 << " " << cnf_fml.size() << "\n";

  for (auto& C : cnf_fml) {
    for (auto lit : C) {
      fout << lit << " ";
    }
    fout << "0"
         << "\n";
  }
  */
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "entire run took " << elapsed.count() << " secs\n";

  return 0;
}
