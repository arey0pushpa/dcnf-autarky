#include <future>

#include "defs.h"

coord_t bfs_autarky(std::string filename, std::string output_file_name,
                    coord_t dependency_var, coord_t level, coord_t s_level,
                    coord_t encoding) {

  /* Todo: Implement a dependency Scheme in case no dependency given
     if ( dependency_var == 0 ) {
       // Implement a dependency scheme
     } */
  set_all_solutions(dcnf_clauses, dcnf_variables, selected_bf,
                    minsat_clause_assgmt, no_of_clauses, no_of_var, level);

  // print_3d_vector(minsat_clause_assgmt);

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

  if (encoding == 1) { // LOG Encoding
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

  std::cout << "The cs_vars are: ";
  print_1d_vector(cs_vars);
  std::cout << "\n";

  // --- Build Constraints
  non_trivial_autarky(cs_vars, cnf_fml); // (4.5)

  touched_clauses(cs_vars, clausewise_pa_var_map, cnf_fml); // (4.3)

  satisfied_clauses(encoding, no_of_clauses, lbf_vars, dcnf_clauses,
                    dcnf_variables, bf_vars, pa_var_msat_ass,
                    msat_concrete_var_map, selected_bf, cnf_fml,
                    bf2lbf_var_map); // (4.2)

  untouched_clauses(encoding, lbf_vars, dcnf_clauses, dcnf_variables, bf_vars,
                    cs_vars, no_of_clauses, cnf_fml, bf2lbf_var_map); // (4.4)

  if (encoding == 0 || encoding == 2) {
    for (coord_t i = 0; i < no_of_var; ++i) { // (4.1)
      if (dcnf_variables[i].qtype() == 'e') {
        coord_t indx = dcnf_variables[i].eindex();
        if (encoding == 0) {
          at_most_one(bf_vars[indx], cnf_fml);
        } else {
          at_most_one_linear(bf_vars[indx], cnf_fml, index);
        }
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

  std::string bf_var_size;
  coord_t total = 0;

  for (coord_t i = 0; i < bf_vars.size(); ++i) {
    total += bf_vars[i].size();
    for (coord_t j = 0; j < bf_vars[i].size(); ++j) {
      bf_var_size = bf_var_size + std::to_string(bf_vars[i][j]) + " ";
    }
    if (i < bf_vars.size() - 1)
      bf_var_size = bf_var_size + " +  ";
  }

  if (encoding == 1) {
    fout << "c There are total " << total << " distinct lbf-variables. "
         << lbf_var_size;
  } else {
    fout << "c There are total " << total << " distinct bf-variables. "
         << bf_var_size;
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
  fout.close();

  std::future<int> future = std::async(std::launch::async, []() {
    auto retVal = system(
        "./build/lingeling/lingeling -q /tmp/dcnfAutarky.dimacs > /tmp/a.out");
    return retVal;
  });

  std::cout << "Running Lingeling ... "
            << "\n";
  std::future_status status;

  status = future.wait_for(std::chrono::seconds(600));

  if (status == std::future_status::timeout) {
    std::cout << "TimeOut! \n";
    exit(0);
    std::terminate();
    return 1;
  }

  if (status == std::future_status::ready) {
    std::string filenm = "/tmp/a.out";
    std::string line;
    std::ifstream file(filenm);
    coord_t csvar_index = 0;
    if (!file.is_open()) {
      perror(("Error while opening file " + filenm).c_str());
      exit(file_reading_error);
    }
    while (std::getline(file, line) || cs_var_index == cs_vars.size()) {
      char s1 = line[0];
      char s2 = line[2];
      switch (s1) {
      case 'v': {
        line = line.substr(line.find_first_of(" \t") + 1);
        std::stringstream ss;
        ss << line;
        std::string temp;
        while (!ss.eof() || (csvar_index == cs_vars.size())) {
          ss >> temp;
          if (std::stoi(temp) > 0) {
            dcnf_clauses[cs_var_index].update_presence(0);
          }
          ++csvar_index;
        }
        break;
      }
      case 's': {
        if (s2 == 'U') {
          return 20;
        }
        break;
      }
      }
    }
    if (file.bad()) {
      perror(("Error while reading file " + filenm).c_str());
      exit(file_reading_error);
    }
    file.close();
  }

  //  output(filename, output_file_name, level, s_level, encoding, no_of_var,
  //         no_of_clauses, a_vars.size(), e_vars.size(), unique_dep_set.size(),
  //         pa_vars.size(), total, cs_vars.size(), index - 1, cnf_fml.size());

  return 0;
}

coord_t e_autarky(std::vector<Clauses> &dcnf_clauses, lit e) {
  set_t intersect;
  lit_t s1 = dcnf_variables[e - 1].pos_pol();
  lit_t s2 = dcnf_variables[e - 1].neg_pol();
  set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                   std::inserter(intersect, intersect.begin()));
  if (intersect.empty()) {
    set_t vactive_cls = dcnf_variables[e - 1].var_active();
    for (coord_t i : vactive_cls) {
      // Check i or i-1
      dcnf_clauses[i].update_presence(0);
    }
  } else {
		for (coord_t j: s1) {
			for (coord_t k: s2) {
				// Remove the variable from the respective clauses and take intersection
			}
		}	
  }

}
