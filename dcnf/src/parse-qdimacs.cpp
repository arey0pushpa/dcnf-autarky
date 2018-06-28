#include <defs.h>
#include <fstream>

/** Varibles needs to be computed
 * e_var = []
 * a_var = []
 * dep_set = []
 *
 * func: add_dep_set()
 * func: create_sv()
 */

void parse_qdimacs_file(std::string filename, unsigned& dependency_var,
                        Vec1D& e_var, Vec1D& a_var, Vec2D& dep_set,
                        Vec2D& cnf_fml) {
  std::string line;
  unsigned var_count = 0;
  unsigned clause_count = 0;
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
    // If line is empty
    if (line == "") {
      std::cout << "Ignoring empty lines.\n";
      continue;
    }
    // handle spaces in the lines
    ltrim(line);
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
        var_count = v;
        clause_count = c;
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
          // std::cout << "E Line.\n";
        }
        auto vec_int = extract_int(line);
        assert(vec_int.size() >= 1);
        for (auto i : vec_int) {
          if (i >= 0 && abs(i) > var_count) {
            std::cerr << "Input format violation. atom > var_count." << '\n';
            exit(input_format_violation);
          }
          e_var.push_back(i);
          dep_set.push_back(a_var);
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
          // std::cout << "A Line.\n";
        }
        auto vec_int = extract_int(line);
        assert(vec_int.size() >= 1);
        for (auto i : vec_int) {
          if (i >= 0 && abs(i) > var_count) {
            std::cerr << "Input format violation. atom > var_count." << '\n';
            exit(input_format_violation);
          }
          a_var.push_back(i);
        }
        break;
      }
      case 'd': {
        Vec1D inner_vec;
        ++dependency_var;
        auto vec_int = extract_int(line);

        auto elem = vec_int.front();
        e_var.push_back(elem);

        vec_int.erase(vec_int.begin());

        assert(vec_int.size() >= 1);
        for (auto i : vec_int) {
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
        auto vec_int = extract_int(line);
        cnf_fml.push_back(vec_int);
        break;
      }
    }
  }

  if (file.bad()) {
    perror(("Error while reading file " + filename).c_str());
    exit(file_reading_error);
  }
  file.close();

  if (clause_count != matrix_cnt) {
    std::cerr << "Input format violation. clause count == #matrix lines"
              << '\n';
    exit(input_format_violation);
  } else if (!p_line) {
    std::cerr << "Input format violation. No p-line found!" << '\n';
    exit(input_format_violation);
  } else {
    std::cout << "Input parsed successfully.\n";
  }
}
