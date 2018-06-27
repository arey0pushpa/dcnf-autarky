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
  std::cout << "Trying to open and read... " << filename << '\n';
  std::ifstream file(filename);

  if (!file.is_open()) {
    perror(("Error while opening file " + filename).c_str());
    exit(file_reading_error);
  }

  while (std::getline(file, line)) {
    char s1 = line[0];
    switch (s1) {
      case 'c':
        break;
      case 'p': {
        auto vec_int = extract_int(line);
        assert(vec_int.size() == 2);
        if (vec_int.size() < 2 || vec_int.size() > 2) {
          std::cerr
              << "Input format violation [p-line]. Accepted format: p cnf n1 n2"
              << '\n';
          exit(input_format_violation);
        }
        break;
      }
      case 'e': {
        auto vec_int = extract_int(line);
        assert(vec_int.size() >= 1);
        for (auto i : vec_int) {
          e_var.push_back(i);
        }
        break;
      }
      case 'a': {
        auto vec_int = extract_int(line);
        assert(vec_int.size() >= 1);
        for (auto i : vec_int) {
          a_var.push_back(i);
        }
        break;
      }
      case 'd': {
        Vec1D inner_vec;
        dependency_var += 1;
        auto vec_int = extract_int(line);
        assert(vec_int.size() >= 2);
        for (auto i : vec_int) {
          inner_vec.push_back(i);
        }
        dep_set.push_back(inner_vec);
        break;
      }

      default: {
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
}
