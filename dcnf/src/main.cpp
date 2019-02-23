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
#include <string>

#include "defs.h"

int main(int argc, char *argv[]) {
  std::string filename;
  std::string output_file_name = "/tmp/dcnfAutarky.dimacs";
  coord_t dependency_var = 0;
  coord_t level = 1;
  coord_t s_level = 0;
  coord_t encoding = 0;

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

  bfs_autarky ( filename, output_file_name, 
                dependency_var, level, s_level, encoding);
              
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "entire run took " << elapsed.count() << " secs\n";
  return 0;
}
