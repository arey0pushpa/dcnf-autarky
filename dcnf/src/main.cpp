// Ankit Shukla, 22.June.2018 (Swansea)
/* Copyright 2018 Ankit Shukla
 * This is a free software.
 * */

#include <chrono>
#include <cmath>
#include <fstream>

#include "defs.h"

int main(int argc, char* argv[]) {
  std::string filename;
  bool input_file = false;
  bool level_func = false;
  coord_t dependency_var = 0;
  coord_t level = 0;
  coord_t encoding = 0;

  coord_t no_of_clauses = 0;
  coord_t no_of_var = 0;

  command_line_parsing(argc, argv, filename, level, encoding, input_file,
                       level_func);

  if (input_file == false) {
    filename = "./examples/qbflib.qdimacs";
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

  cl_t cs_var;
  cls_t bf_var;
  cl_t pa_vars;

  parse_qdimacs_file(filename, dcnf_fml, dep_set, a_vars, e_vars, no_of_clauses,
                     no_of_var, dependency_var);

  // Create no_of_var Objects and for each obj representing a
  // variable (uni and exist) set qtype of the var and fix it's dependency
  Variables dcnf_variables[no_of_var];

  std::sort(dep_set.begin(), dep_set.end(),
            [](const cl_t& a, const cl_t& b) { return a[0] < b[0]; });

  std::sort(e_vars.begin(), e_vars.end());
  std::sort(a_vars.begin(), a_vars.end());

  auto avar_iterator = a_vars.begin();
  auto evar_iterator = e_vars.begin();

  coord_t dep_index = 0;
  bool a_vars_end = false;
  bool e_vars_end = false;
  if (avar_iterator == a_vars.end()) a_vars_end = true;
  if (evar_iterator == e_vars.end()) e_vars_end = true;

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
      if (std::next(evar_iterator) == e_vars.end()) {
        e_vars_end = true;
      } else {
        evar_iterator = std::next(evar_iterator);
      }
    } else {
      dcnf_variables[i].initialise_qtype('e');
    }
  }

  // Create no_of_clauses Objects and initialise exits and forall quant var
  lit_t dsize = dcnf_fml.size();
  Clauses dcnf_clauses[dsize];

  for (coord_t i = 0; i < dsize; ++i) {
    cl_t c_evars;
    cl_t c_elits;
    cl_t c_avars;
    cl_t c_alits;
    dcnf_clauses[i].initialise_lits(dcnf_fml[i]);
    for (const lit_t l : dcnf_fml[i]) {
      if (dcnf_variables[std::abs(l) - 1].fetch_qtype() == 'e') {
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

  preprocess_fml(dcnf_clauses, dcnf_variables, selected_bf,
                 minsat_clause_assgmt, no_of_clauses, no_of_var, level);

  /** Create traslation Variables/ordering *
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
