#include <fstream>

#include "dcnf.h"
#include "defs.h"
#include "util.h"

void dcnf::cmdline_parsing(int argc, char* argv[]) {
  if (cmd_option_exists(argv, argv + argc, "-h")) {
    std::cout
        << "DCNF-Autarky [version 0.0.1]. (C) Copyright 2018-2019 "
           "Swansea UNiversity. \nUsage: ./dcnf [-i filename] [-o "
           "filename] [-l "
           "aut_level] [-e encoding 0:Binomial, 1:Log, 2:Linear] [-s strictness; "
           "0:general, "
           "1:strict] "
           "[-r reduction; 1:e_autarky, 2:a_autarky 3: Both e+a_autarky]\n";
    exit(0);
  }

  char* file_name = get_cmd_option(argv, argv + argc, "-i");
  char* output_file = get_cmd_option(argv, argv + argc, "-o");
  char* level_set = get_cmd_option(argv, argv + argc, "-l");
  char* encoding_chosen = get_cmd_option(argv, argv + argc, "-e");
  char* strict_level = get_cmd_option(argv, argv + argc, "-s");
  char* red_type = get_cmd_option(argv, argv + argc, "-r");

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
}

void parse_qdimacs_file(std::string filename, cls_t& dcnf_fml, cls_t& dep_set,
                        cl_t& a_vars, cl_t& e_vars, coord_t& no_of_clauses,
                        coord_t& no_of_var, coord_t& dependency_var,
                        coord_t s_level, coord_t& min_dep_size,
                        coord_t& max_dep_size) {
  set_t prefix_vars;
  set_t cls_vars;
  std::string line;
  unsigned matrix_cnt = 0;
  bool p_line = false;
  char q_line = 'q';
  std::ifstream file(filename);
  if (!file.is_open()) {
    perror(("Error while opening file " + filename).c_str());
    exit(file_reading_error);
  }
  while (std::getline(file, line)) {
    if (line == "") {
      std::cout << "Ignoring empty lines.\n";
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
          std::cerr
              << "Input format violation [p-line]. Accepted format: p cnf n1 n2"
              << '\n';
          exit(input_format_violation);
        }
        break;
      }
      case 'e': {
        if (q_line == 'e') {
          std::cerr << "Input format violation [e-line]. Consecutive e lines "
                    << '\n';
          exit(input_format_violation);
        } else if (matrix_cnt > 0) {
          std::cerr << "Input format violation [e-line]. e-line not allowed "
                       "after matrix starts. "
                    << '\n';
          exit(input_format_violation);
        } else {
          q_line = 'e';
        }
        cl_t clause = extract_int(line);
        assert(clause.size() >= 1);
        for (lit_t i : clause) {
          if (unsigned(abs(i)) > no_of_var) {
            std::cerr << "Input format violation. atom > no_of_var." << '\n';
            exit(input_format_violation);
          }
          if (prefix_vars.find(i) != prefix_vars.end()) {
            std::cerr << "Input format violation. Multiple var entry in prefix."
                      << '\n';
            exit(input_format_violation);
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
          std::cerr << "Input format violation [a-line]. Consecutive a lines."
                    << '\n';
          exit(input_format_violation);
        }
        if (matrix_cnt > 0) {
          std::cerr << "Input format violation [a-line]. a-line not allowed "
                       "after matrix line. "
                    << '\n';
          exit(input_format_violation);
        }
        q_line = 'a';
        cl_t clause = extract_int(line);
        assert(clause.size() >= 1);
        for (lit_t i : clause) {
          if (unsigned(abs(i)) > no_of_var) {
            std::cerr << "Input format violation. atom > no_of_var." << '\n';
            exit(input_format_violation);
          }
          if (prefix_vars.find(i) != prefix_vars.end()) {
            std::cerr << "Input format violation. Multiple var entry in prefix."
                      << '\n';
            exit(input_format_violation);
          }
          a_vars.push_back(i);
          prefix_vars.insert(i);
        }
        break;
      }
      case 'd': {
        if (q_line == 'q') {
          std::cerr << "Input format violation. Starting d line. Require at "
                       "least one a line."
                    << '\n';
          exit(input_format_violation);
        } else {
          q_line = 'd';
        }
        cl_t inner_vec;
        ++dependency_var;
        cl_t clause = extract_int(line);
        lit_t elem = clause.front();
        if (prefix_vars.find(elem) != prefix_vars.end()) {
          std::cerr << "Input format violation. Multiple var entry in prefix."
                    << '\n';
          exit(input_format_violation);
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
          std::cerr << "Input format violation [e-line]. Last quant line "
                       "should be an e-line."
                    << '\n';
          exit(input_format_violation);
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
    perror(("Error while reading file " + filename).c_str());
    exit(file_reading_error);
  }
  file.close();

  if (no_of_clauses != matrix_cnt) {
    std::cerr << "Input format violation. clause count == #matrix lines"
              << '\n';
    exit(input_format_violation);
  } else if (!p_line) {
    std::cerr << "Input format violation. No p-line found!" << '\n';
    exit(input_format_violation);
  }
  for (lit_t c : cls_vars) {
    if (prefix_vars.find(std::abs(c)) == prefix_vars.end()) {
      std::cerr << "Free variable occurence. Due to literal " << c << '\n';
      exit(free_var_occurrence);
    }
  }
}
