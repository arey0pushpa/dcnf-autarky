#include <fstream>

#include "dcnf.h"
#include "defs.h"
#include "util.h"

void print_usage() {
  std::cout << "DCNF-Autarky [version 0.0.1]. (C) Copyright 2018-2019 "
               "Swansea University. \nUsage: ./dcnf [-i filename] "
               "[-o filename] [-l aut_level] "
               "[-e encoding] "
               "[-s strictness] "
               "[-r reduction] "
               "[-t output_type] \n";
}

void print_cmd_options() {
  std::cout << "  encoding = [0 (Binomial); 1 (Log); 2 (Linear {default})] \n"
               "  strictness = [0 (general {default}); 1 (strict)]\n"
               "  output_type = [0 (commandline based {default}); 1 (R based "
               "file)]\n"
               "  reduction = [1 (E_Autarky); 2 (A_Autarky {default}); 3 (Both "
               "E+A_Autarky)] \n  output_type = [0 (detailed output with "
               "statistics); 1 (R-Style output line)]";
}

void print_filename(std::string filename) {
  std::cout << "c input filename   " << filename << "\n";
}

void dcnf::cmdline_parsing(int argc, char *argv[]) {
  if (cmd_option_exists(argv, argv + argc, "-h")) {
    print_usage();
    std::exit(0);
  }
  if (cmd_option_exists(argv, argv + argc, "--help")) {
    print_usage();
    std::cout << "\n";
    print_cmd_options();
    std::cout << "\n";
    std::exit(0);
  }

  char *file_name = get_cmd_option(argv, argv + argc, "-i");
  char *output_file = get_cmd_option(argv, argv + argc, "-o");
  char *level_set = get_cmd_option(argv, argv + argc, "-l");
  char *encoding_chosen = get_cmd_option(argv, argv + argc, "-e");
  char *strict_level = get_cmd_option(argv, argv + argc, "-s");
  char *red_type = get_cmd_option(argv, argv + argc, "-r");
  char *op_type = get_cmd_option(argv, argv + argc, "-t");

  if (file_name) {
    filename = file_name;
  } else {
    std::cout
        << "c Please provide an input file.\nc Use [-i filename] or see help "
           "[-h] for more options\n";
    std::exit(0);
  }

  if (output_file) {
    output_file_name = output_file;
    std::cout << "The output file is: " << output_file_name << std::endl;
  }

  if (filename == output_file_name) {
    std::cout << "Please provide differnt filenames for input and output file.";
    std::exit(0);
  }

  if (level_set) {
    aut_level = std::stoi(level_set);
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
  if (op_type) {
    output_type = std::stoi(op_type);
  }
}

void parse_qdimacs_file(std::string filename, vstr_t &aed_lines,
                        cls_t &dcnf_fml, cls_t &dep_set, cl_t &a_vars,
                        cl_t &e_vars, coord_t &no_of_clauses,
                        coord_t &no_of_var, coord_t &dependency_var,
                        coord_t s_level, coord_t &min_dep_size,
                        coord_t &max_dep_size) {
  set_t prefix_vars;
  set_t cls_vars;
  std::string line;
  unsigned matrix_cnt = 0;
  bool p_line = false;
  char q_line = 'q';
  std::ifstream file(filename);
  if (!file.is_open()) {
    perror(("c Error while opening file " + filename).c_str());
    std::exit(file_reading_error);
  }
  while (std::getline(file, line)) {
    if (line == "") {
      // std::cout << "Ignoring empty lines.\n";
      continue;
    }
    trim(line);
    char s1 = line[0];
    switch (s1) {
      case 'c': {
        break;
      }
      case 'p': {
        std::string s2;
        char ef = '\0';
        unsigned v, c;
        p_line = true;
        std::stringstream iss(line);
        iss >> s2 >> s2 >> v >> c >> ef;
        no_of_var = v;
        no_of_clauses = c;
        if (s2 != "cnf" || ef != '\0') {
          print_filename(filename);
          std::cerr << "c Input format violation [p-line]. \nc Accepted "
                       "format: p cnf n1 n2"
                    << '\n';
          std::exit(input_format_violation);
        }
        break;
      }
      case 'e': {
        if (q_line == 'e') {
          print_filename(filename);
          std::cerr
              << "c Input format violation [e-line]. \nc Consecutive e lines "
              << '\n';
          exit(input_format_violation);
        } else if (matrix_cnt > 0) {
          print_filename(filename);
          std::cerr
              << "c Input format violation [e-line]. \nc e-line not allowed "
                 "after matrix starts. "
              << '\n';
          std::exit(input_format_violation);
        } else {
          q_line = 'e';
        }

        aed_lines.push_back(line);

        cl_t clause = extract_int(line);
        assert(clause.size() >= 1);
        for (lit_t i : clause) {
          if (unsigned(abs(i)) > no_of_var) {
            print_filename(filename);
            std::cerr << "c Input format violation. \nc atom > no_of_var."
                      << '\n';
            std::exit(input_format_violation);
          }
          if (prefix_vars.find(i) != prefix_vars.end()) {
            print_filename(filename);
            std::cerr
                << "c Input format violation. \nc Multiple var entry in prefix."
                << '\n';
            std::exit(input_format_violation);
          }
          cl_t dummy_dep = a_vars;
          dummy_dep.insert(dummy_dep.begin(), i);

          e_vars.push_back(i);
          dep_set.push_back(dummy_dep);
          prefix_vars.insert(i);
        }
        break;
      }

      case 'a': {
        if (s_level == 1 && q_line == 'a') {
          print_filename(filename);
          std::cerr
              << "c Input format violation [a-line]. \nc Consecutive a lines."
              << '\n';
          std::exit(input_format_violation);
        }
        if (matrix_cnt > 0) {
          print_filename(filename);
          std::cerr
              << "c Input format violation [a-line].\nc a-line not allowed "
                 "after matrix line. "
              << '\n';
          std::exit(input_format_violation);
        }

        aed_lines.push_back(line);

        q_line = 'a';
        cl_t clause = extract_int(line);
        // assert(clause.size() >= 1);
        for (lit_t i : clause) {
          if (unsigned(abs(i)) > no_of_var) {
            print_filename(filename);
            std::cerr << "c Input format violation. \nc atom > no_of_var."
                      << '\n';
            exit(input_format_violation);
          }
          if (prefix_vars.find(i) != prefix_vars.end()) {
            print_filename(filename);
            std::cerr
                << "c Input format violation. \nc Multiple var entry in prefix."
                << '\n';
            std::exit(input_format_violation);
          }
          a_vars.push_back(i);
          prefix_vars.insert(i);
        }
        break;
      }
      case 'd': {
        if (q_line == 'q') {
          print_filename(filename);
          std::cerr
              << "c Input format violation.\nc Starting d line. Require at "
                 "least one a line."
              << '\n';
          std::exit(input_format_violation);
        } else {
          q_line = 'd';
        }

        aed_lines.push_back(line);

        cl_t inner_vec;
        ++dependency_var;
        cl_t clause = extract_int(line);
        lit_t elem = clause.front();
        if (prefix_vars.find(elem) != prefix_vars.end()) {
          print_filename(filename);
          std::cerr
              << "c Input format violation.\nc Multiple var entry in prefix."
              << '\n';
          std::exit(input_format_violation);
        }
        prefix_vars.insert(elem);
        e_vars.push_back(elem);
        assert(clause.size() >= 1);
        for (lit_t i : clause) {
          inner_vec.push_back(i);
        }
        dep_set.push_back(inner_vec);
        break;
      }

      default: {
        if (q_line != 'e' && q_line != 'd') {
          print_filename(filename);
          std::cerr << "c Input format violation [e-line]. \nc Last quant line "
                       "should be an e-line."
                    << '\n';
          std::exit(input_format_violation);
        }
        ++matrix_cnt;
        cl_t clause = extract_int(line);
        dcnf_fml.push_back(clause);
        for (lit_t clsvar : clause) {
          cls_vars.insert(clsvar);
        }
        break;
      }
    }
  }

  if (file.bad()) {
    perror(("c Error while reading file " + filename).c_str());
    std::exit(file_reading_error);
  }
  file.close();

  if (no_of_clauses != matrix_cnt) {
    print_filename(filename);
    std::cerr << "c Input format violation. \nc clause count == #matrix lines"
              << '\n';
    std::exit(input_format_violation);
  } else if (!p_line) {
    print_filename(filename);
    std::cerr << "c Input format violation. \nc No p-line found!" << '\n';
    std::exit(input_format_violation);
  }
  for (lit_t c : cls_vars) {
    if (prefix_vars.find(std::abs(c)) == prefix_vars.end()) {
      print_filename(filename);
      std::cerr << "c Free variable occurence.\nc Due to literal " << c << '\n';
      std::exit(free_var_occurrence);
    }
  }
}
