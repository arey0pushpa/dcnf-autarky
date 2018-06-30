#include <defs.h>
#include <fstream>

void parse_qdimacs_file(std::string filename, cls_t& dcnf_fml, cls_t& dep_set,
                        cl_t& a_vars, cl_t& e_vars, coord_t& no_of_clauses,
                        coord_t& no_of_var, coord_t& dependency_var) {
  std::string line;
  unsigned matrix_cnt = 0;
  bool p_line = false;
  char q_line = 'q';
  std::cout << "Trying to open and read [" << filename << "]\n";
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
          std::cout << "E Line.\n";
        }
        cl_t clause = extract_int(line);
        assert(clause.size() >= 1);
        for (lit_t i : clause) {
          if (i >= 0 && abs(i) > no_of_var) {
            std::cerr << "Input format violation. atom > no_of_var." << '\n';
            exit(input_format_violation);
          }
          cl_t dummy_dep = a_vars;
          dummy_dep.insert(dummy_dep.begin(), i);

          e_vars.push_back(i);
          dep_set.push_back(dummy_dep);
        }
        break;
      }

      case 'a': {
        if (q_line == 'a') {
          std::cerr << "Input format violation [a-line]. Consecutive a lines."
                    << '\n';
          exit(input_format_violation);
        } else if (matrix_cnt > 0) {
          std::cerr << "Input format violation [a-line]. a-line not allowed "
                       "after matrix line. "
                    << '\n';
          exit(input_format_violation);
        } else {
          q_line = 'a';
          std::cout << "A Line.\n";
        }
        cl_t clause = extract_int(line);
        assert(clause.size() >= 1);
        for (lit_t i : clause) {
          if (i >= 0 && abs(i) > no_of_var) {
            std::cerr << "Input format violation. atom > no_of_var." << '\n';
            exit(input_format_violation);
          }
          a_vars.push_back(i);
        }
        break;
      }
      case 'd': {
        std::cout << "D Line.\n";
        cl_t inner_vec;
        ++dependency_var;
        cl_t clause = extract_int(line);
        lit_t elem = clause.front();
        e_vars.push_back(elem);
        // clause.erase(clause.begin());
        assert(clause.size() >= 1);
        for (lit_t i : clause) {
          inner_vec.push_back(i);
        }
        dep_set.push_back(inner_vec);
        break;
      }

      default: {
        if (q_line != 'e') {
          std::cerr << "Input format violation [e-line]. Last quant line "
                       "should be an e-line."
                    << '\n';
          exit(input_format_violation);
        }
        ++matrix_cnt;
        cl_t clause = extract_int(line);
        dcnf_fml.push_back(clause);
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
  } else {
    std::cout << "\nInput parsed successfully.\n";
  }
}
