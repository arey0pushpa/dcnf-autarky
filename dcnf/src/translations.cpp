#include <future>

#include "dcnf.h"
#include "util.h"

/** Remove the dead/inactive clauses from the active variable list **/
void dcnf::propagate_cls_removal(lit_t i) {
  for (lit_t l : dcnf_clauses[i].lits()) {
    if (!dcnf_variables[std::abs(l) - 1].var_present()) continue;
    if (l > 0) {
      dcnf_variables[std::abs(l) - 1].pos_cls.erase(i);
    } else {
      dcnf_variables[std::abs(l) - 1].neg_cls.erase(i);
    }
  }
}

// Print the remaining clauses in the system
void dcnf::print_remaining_cls() {
  for (lit_t l : present_clauses) {
    std::cout << l << '\t';
  }
  std::cout << "\n";
}

coord_t dcnf::a_autarky(std::string filename, std::string output_file_name,
                        const coord_t encoding) {
  // Traslation variables with ordering
  cl_t cs_vars;
  cls_t bf_vars;
  cl_t pa_vars;

  cls_t cnf_fml;  // dimacs/cnf fml {{lit...}...}
  cl_t cnf_vars;  // dimacs/cnf var {cnf-vars}

  coord_t index = 1;
  const coord_t no_of_var = active_evars.size() + active_avars.size();
  // const coord_t no_of_clauses = present_clauses.size();

  // cs variable := #no_of Active clauses
  for (coord_t i = 0; i < present_clauses.size(); ++i) {
    cs_vars.push_back(index);
    index += 1;
  }

  if (index == 1) {
    return 11;  // empty cls list; return SAT
  }

  // bf variable := two_dim [v] [f_v]
  coord_t lbf_var_size = 0;
  cl_t lbf_vars, s_bf;
  coord_t preindex = index;
  coord_t bf_var_count = 0;
  for (const lit_t e : active_evars) {
    // TODO: Implement using the active_evar_index
    for (coord_t j = 0; j < selected_bf[dcnf_variables[e - 1].m_eindex].size();
         ++j) {
      s_bf.push_back(index);
      index += 1;
    }
    bf_vars.push_back(s_bf);
    s_bf.clear();
  }

  // Additional 1 due to index count is incremented after last use.
  // TODO: Check the correct use of no_of_clauses, use the
  std::vector<bf_lbf_converter> bf2lbf_var_map(index -
                                               (dcnf_clauses.size() + 1));

  // AVOID LOG ENCODING for first iteration@!
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

  // Index Lookup for the evar
  cl_t active_evar_index(no_of_vars);
  coord_t eindx = 0;
  for (lit_t e : active_evars) {
    active_evar_index[e - 1] = eindx;
    ++eindx;
  }

  cl_t present_cls_index(dcnf_clauses.size());
  coord_t cindx = 0;
  for (lit_t c : present_clauses) {
    present_cls_index[c] = cindx;
    ++cindx;
  }

  // pa variable := Only consider unique mapping
  cls_t pa_var_set;
  minsat_ass pa_var_msat_ass(active_evars.size());  // bigbag to push all assmts
  cls_t msat_concrete_var_map(active_evars.size());  // dqbf Var to Cnf var Map
  cls_t clausewise_pa_var_map(
      present_clauses.size());  // create clausewise cnf vars
  coord_t msat_cntr = 1;
  for (lit_t i : present_clauses) {
    for (coord_t j = 0; j < minsat_clause_assgmt[i].size(); ++j) {
      cl_t dummy = minsat_clause_assgmt[i][j];
      // TODO: check dummy[0] is always will be absolute
      lit_t slit = std::abs(dummy[0]);
      // Extract the position of the existential var in evars
      lit_t elit = active_evar_index[slit];
      lit_t pa_indx = find_vector_index(pa_var_msat_ass[elit], dummy);
      if (pa_indx != -1) {
        clausewise_pa_var_map[i].push_back(
            msat_concrete_var_map[elit][pa_indx]);
      } else {
        pa_var_msat_ass[elit].push_back(dummy);
        msat_concrete_var_map[elit].push_back(index);
        ++msat_cntr;
        clausewise_pa_var_map[present_cls_index[i]].push_back(index);
        pa_vars.push_back(index);
        ++index;
      }
    }
  }

  // --- Build Constraints
  non_trivial_autarky(cs_vars, cnf_fml);  // (4.5)

  touched_clauses(cs_vars, clausewise_pa_var_map, cnf_fml);  // (4.3)

  satisfied_clauses(encoding, cs_vars.size(), lbf_vars, bf_vars,
                    pa_var_msat_ass, msat_concrete_var_map, cnf_fml,
                    bf2lbf_var_map, active_evar_index);  // (4.2)

  untouched_clauses(encoding, lbf_vars, bf_vars, cs_vars, cnf_fml,
                    bf2lbf_var_map, present_cls_index,
                    active_evar_index);  // (4.4)

  if (encoding == 0 || encoding == 2) {
    for (lit_t e : active_evars) {
      coord_t indx = dcnf_variables[e - 1].eindex();
      if (encoding == 0) {
        at_most_one(bf_vars[indx], cnf_fml);
      } else {
        at_most_one_linear(bf_vars[indx], cnf_fml, index);
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
    if (i < bf_vars.size() - 1) bf_var_size = bf_var_size + " +  ";
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

  // TODO: ASSERT! index - 1 = cs_var + pa_var + bf_vars _.size()

  for (const cl_t C : cnf_fml) {
    for (const lit_t l : C) {
      fout << l << " ";
    }
    fout << "0"
         << "\n";
  }
  fout.close();

  std::future<int> future = std::async(std::launch::async, []() {
    auto retVal = system(
        "./build/lingeling/lingeling -q /tmp/dcnfAutarky.dimacs > "
        "/tmp/a.out");
    return retVal;
  });

  cl_t var_assgn;
  std::cout << "Running Lingeling ... "
            << "\n";
  std::future_status status;

  status = future.wait_for(std::chrono::seconds(600));

  if (status == std::future_status::timeout) {
    std::cout << "TimeOut! \n";
    exit(0);
    std::terminate();
  }

  if (status == std::future_status::ready) {
    std::string filenm = "/tmp/a.out";
    std::string line;
    std::ifstream file(filenm);
    if (!file.is_open()) {
      perror(("Error while opening file " + filenm).c_str());
      exit(file_reading_error);
    }
    while (std::getline(file, line)) {
      char s1 = line[0];
      switch (s1) {
        case 'v': {
          line = line.substr(line.find_first_of(" \t") + 1);
          std::stringstream ss(line);
          for (lit_t i = 0; ss >> i;) {
            var_assgn.push_back(i);
          }
          break;
        }
        case 's': {
          if (line[2] == 'U') {
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
  // remove last 0 from the assignment
  var_assgn.pop_back();

  std::cout << "The satisfying assignment is... \n";
  print_1d_vector(var_assgn);
  std::cout << '\n';

  /*  Update the data structure   */
  coord_t vindx = 0;
  set_t update_present_cls;
  for (lit_t c : present_clauses) {
    if (var_assgn[vindx] > 0) {
      dcnf_clauses[c].present = 0;
      deleted_clauses.insert(c);
      propagate_cls_removal(c);
    } else {
      update_present_cls.insert(c);
    }
    ++vindx;
  }
  present_clauses = update_present_cls;
  updated_cls_size = cs_vars.size();

  // Relaying on the SAT solver to provide ordered assignment
  for (coord_t i = 0; i < bf_vars.size(); ++i) {
    for (coord_t j = 0; j < bf_vars[i].size(); ++j) {
      if (var_assgn[bf_vars[i][j] - 1] > 0) {
        final_assgmt.emplace_back(selected_bf[i][j]);
      }
    }
  }

  if (present_clauses.size() > 0) {
    return 10;
  } else {
    return 11;
  }
}
//  output(filename, output_file_name, level, s_level, encoding, no_of_var,
//         no_of_clauses, a_vars.size(), e_vars.size(),
//         unique_dep_set.size(), pa_vars.size(), total, cs_vars.size(),
//         index - 1, cnf_fml.size());

coord_t dcnf::e_autarky(lit_t e) {
  set_t intersect;
  set_t s1 = dcnf_variables[e - 1].pos_pol();
  set_t s2 = dcnf_variables[e - 1].neg_pol();
  set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                   std::inserter(intersect, intersect.begin()));
  if (!intersect.empty()) {
    for (lit_t j : s1) {
      if (!dcnf_clauses[j].cls_present()) continue;
      cl_t cls_s1 = dcnf_clauses[j].lits();
      set_t compl_C;
      set_t set_D;
      // Implement a func or change vector to a set
      for (lit_t l1 : cls_s1) {
        if (l1 > 0) {
          compl_C.insert(-l1);
        } else {
          compl_C.insert(l1);
        }
      }
      for (coord_t k : s2) {
        set_t intersect_cls;
        cl_t cls_s2 = dcnf_clauses[k].lits();
        for (lit_t l2 : cls_s2) {
          set_D.insert(l2);
        }
        set_intersection(compl_C.begin(), compl_C.end(), set_D.begin(),
                         set_D.end(),
                         std::inserter(intersect_cls, intersect_cls.begin()));
        assert(intersect_cls.size() >= 1);
        if (intersect_cls.size() < 2) {
          return 1;
        }
      }
    }
  }
  return 10;
}
