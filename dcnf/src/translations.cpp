#include <future>

#include "dcnf.h"
#include "util.h"

/** Remove the dead/inactive clauses from the active variable list **/
void dcnf::propagate_cls_removal(lit_t i) {
  for (lit_t l : dcnf_clauses[i].lits) {
    if (!dcnf_variables[std::abs(l) - 1].present) continue;
    if (l > 0) {
      dcnf_variables[std::abs(l) - 1].pos_cls.erase(i);
    } else {
      dcnf_variables[std::abs(l) - 1].neg_cls.erase(i);
    }
  }
}

/** Create lbf formula **/
cl_t lbf_formula(cl_t& lbf_vars, lit_t bf_var) {
  coord_t blen = 0;
  boolv_t binary_repr;
  cl_t fml_repr;

  while (bf_var > 0) {
    binary_repr.push_back(bf_var % 2);
    bf_var = bf_var / 2;
    ++blen;
  }
  assert(blen <= lbf_vars.size());
  // Enforce the resultant vector is of size of lbf_vars
  for (coord_t i = blen; i < lbf_vars.size(); ++i) {
    binary_repr.push_back(0);
  }
  for (coord_t i = 0; i < lbf_vars.size(); ++i) {
    (binary_repr[i] == 0 ? fml_repr.push_back(-lbf_vars[i])
                         : fml_repr.push_back(lbf_vars[i]));
  }
  return fml_repr;
}

/** Print the remaining clauses in the system */
void dcnf::print_remaining_cls() {
  for (lit_t l : present_clauses) {
    std::cout << l << '\t';
  }
  std::cout << "\n";
}

// Print the total time taken
void display_running_time(
    std::chrono::time_point<std::chrono::high_resolution_clock> start) {
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "entire run took " << elapsed.count() << " secs\n";
}

/** handle output of an Aut_reduction based on aut_present */
void dcnf::display_aresult(coord_t aut_present) {
  if (aut_present == 20) {
    std::cout << "The input QBF formula is UNSAT. \n";
    std::cout << "The UNSAT/remaining clauses are. \n";
    print_remaining_cls();
    // display_running_time(start);
    exit(0);
  } else if (aut_present == 11) {
    std::cout << "The input QBF formula is Satisfiable by an a_autarky "
                 "reduction.\n ";
    std::cout << "The satisfying assignment is...\n";
    print_1d_vector_int_pair(final_assgmt);
    // display_running_time(start);
    exit(0);
  } else {
    std::cout << "The remaining clauses after a_autarky reductions" << '\n';
    print_remaining_cls();
    if (updated_cls_size == old_cls_size) {
      std::cout << "No further autarky is found.\n";
      std::cout << "The satisfying assignment is...\n";
      print_1d_vector_int_pair(final_assgmt);
      // display_running_time(start);
      exit(0);
    } else {
      old_cls_size = updated_cls_size;
    }
  }
}

/** Handle e_aut reduction based on aur_present**/
void dcnf::display_eresult(coord_t aut_present) {
  if (present_clauses.size() == 0) {
    std::cout << "The input QBF formula is Satisfiable by an e_autarky "
                 "reduction.\n ";
    // TODO: Print the satisfying assignments!!!
    exit(0);
  }
  if (active_evars.size() == 0) {
    std::cout << "All univ variable case. The input formula is UNSAT." << '\n';
    exit(0);
  }
  // TODO: Add evar and avoid printing this everytime :)
  std::cout << "Remaining clauses e_autarky reductions" << '\n';
  print_remaining_cls();
}

/**** A_Autarky ********/
coord_t dcnf::a_autarky(std::string filename, std::string output_file_name,
                        const coord_t encoding) {
  // Traslation variables with ordering
  cl_t cs_vars;
  cls_t bf_vars;
  cl_t pa_vars;

  cls_t cnf_fml;  // dimacs/cnf fml {{lit...}...}
  cl_t cnf_vars;  // dimacs/cnf var {cnf-vars}

  lit_t index = 1;

  // cs variable := #no_of Active clauses
  for (coord_t i = 0; i < present_clauses.size(); ++i) {
    cs_vars.push_back(index);
    index += 1;
  }

  if (index == 1) {
    return 11;  // empty cls list; return SAT
  }

  // bf variable := two_dim [v] [f_v]
  cl_t s_bf;
  bflbf_t bf2lbf_var_map;
  cls_t lbf_vars;
  coord_t preindex = index;
  for (coord_t i = 0; i < active_evars.size(); ++i) {
    for (coord_t j = 0; j < selected_bf[i].size(); ++j) {
      s_bf.push_back(index);
      index += 1;
    }
    bf_vars.push_back(s_bf);
    s_bf.clear();
  }

  if (encoding == 1) {  // LOG Encoding
    cl_t s_lbf;
    coord_t lbf_var_size = 0;
    coord_t lbf_enc = 0;
    index = preindex;
    for (coord_t i = 0; i < bf_vars.size(); ++i) {
      lbf_var_size = ceil(log(bf_vars[i].size() + 1) / log(2));
      for (coord_t k = 0; k < lbf_var_size; ++k) {
        s_lbf.push_back(index);
        index += 1;
      }
      lbf_vars.push_back(s_lbf);
      lbf_enc = 1;
      for (lit_t bv : bf_vars[i]) {
        bf2lbf_var_map[i].push_back(
            std::make_pair(bv, lbf_formula(s_lbf, lbf_enc)));
        ++lbf_enc;
      }
      s_lbf.clear();
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
  // bigbag to push all concrete bf variable assmts
  minsat_ass pa_var_msat_ass(active_evars.size());
  // dqbf Var to Cnf var Map; pa_vars for each bigbag element
  cls_t msat_concrete_var_map(active_evars.size());
  cls_t clausewise_pa_var_map(
      present_clauses.size());  // create clausewise cnf vars
  coord_t msat_cntr = 1;
  for (lit_t c : present_clauses) {
    for (coord_t j = 0; j < minsat_clause_assgmt[c].size(); ++j) {
      cl_t dummy = minsat_clause_assgmt[c][j];
      lit_t slit = std::abs(dummy[0]);
      // Extract the position of the existential var in evars
      lit_t elit = active_evar_index[slit - 1];
      lit_t pa_indx = find_vector_index(pa_var_msat_ass[elit], dummy);
      if (pa_indx != -1) {
        clausewise_pa_var_map[present_cls_index[c]].push_back(
            msat_concrete_var_map[elit][pa_indx]);
      } else {
        pa_var_msat_ass[elit].push_back(dummy);
        msat_concrete_var_map[elit].push_back(index);
        ++msat_cntr;
        clausewise_pa_var_map[present_cls_index[c]].push_back(index);
        pa_vars.push_back(index);
        ++index;
      }
    }
  }

  // --- Build Constraints
  non_trivial_autarky(cs_vars, cnf_fml);  // (4.5)

  touched_clauses(cs_vars, clausewise_pa_var_map, cnf_fml);  // (4.3)

  satisfied_clauses(lbf_vars, bf_vars, pa_var_msat_ass, msat_concrete_var_map,
                    cnf_fml, bf2lbf_var_map,
                    active_evar_index);  // (4.2)

  untouched_clauses(lbf_vars, bf_vars, cs_vars, cnf_fml, bf2lbf_var_map,
                    present_cls_index,
                    active_evar_index);  // (4.4)

  if (encoding == 0 || encoding == 2) {
    for (lit_t e : active_evars) {
      coord_t indx = active_evar_index[e - 1];
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
    // fout << "c There are total " << total << " distinct lbf-variables. "
    //     << lbf_var_size;
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

  std::cout << "The SAT solver's assignment is... \n";
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
  updated_cls_size = update_present_cls.size();

  // Relying on the SAT solver to provide ordered assignment
  for (coord_t i = 0; i < bf_vars.size(); ++i) {
    for (coord_t j = 0; j < bf_vars[i].size(); ++j) {
      if (var_assgn[bf_vars[i][j] - 1] > 0) {
        pair_t sbf = selected_bf[i][j];
        assigned_evars.push_back(sbf.first);
        final_assgmt.emplace_back(sbf);
      }
    }
  }

  cl_t tmp_active_evars;
  for (lit_t e : active_evars) {
    if ((dcnf_variables[e - 1].pos_cls.size() > 0) ||
        (dcnf_variables[e - 1].neg_cls.size() > 0)) {
      tmp_active_evars.push_back(e);
    } else {
      dcnf_variables[e - 1].present = 0;
    }
  }
  active_evars = tmp_active_evars;

  cl_t tmp_active_avars;
  for (lit_t a : active_avars) {
    if ((dcnf_variables[a - 1].pos_cls.size() > 0) ||
        (dcnf_variables[a - 1].neg_cls.size() > 0)) {
      tmp_active_avars.push_back(a);
    } else {
      dcnf_variables[a - 1].present = 0;
    }
  }
  active_avars = tmp_active_avars;

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

void dcnf::update_data_structure(lit_t e) {
  assigned_evars.push_back(e);
  for (lit_t i : dcnf_variables[e - 1].pos_cls) {
    dcnf_clauses[i].present = 0;
    present_clauses.erase(i);
    deleted_clauses.insert(i);
    propagate_cls_removal(i);
  }
  for (lit_t i : dcnf_variables[e - 1].neg_cls) {
    dcnf_clauses[i].present = 0;
    present_clauses.erase(i);
    deleted_clauses.insert(i);
    propagate_cls_removal(i);
  }
}

coord_t dcnf::e_autarky() {
  coord_t var_autarky_count = 0;
  cl_t iter_active_evars;
  for (lit_t e : active_evars) {
    [&] {
      // TODO: This check is redundant
      if (dcnf_variables[e - 1].pos_cls.size() +
              dcnf_variables[e - 1].neg_cls.size() ==
          0) {
        ++var_autarky_count;
        update_data_structure(e);
        return;
      }
      set_t intersect;
      set_t s1 = dcnf_variables[e - 1].pos_cls;
      set_t s2 = dcnf_variables[e - 1].neg_cls;
      set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                       std::inserter(intersect, intersect.begin()));
      if (intersect.empty()) {  // Pure Lit case
        ++var_autarky_count;
        update_data_structure(e);
        return;
      }
      for (lit_t j : s1) {
        // TODO: This check is redundant
        if (!dcnf_clauses[j].present) continue;
        cl_t cls_s1 = dcnf_clauses[j].lits;
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
          cl_t cls_s2 = dcnf_clauses[k].lits;
          for (lit_t l2 : cls_s2) {
            set_D.insert(l2);
          }
          set_intersection(compl_C.begin(), compl_C.end(), set_D.begin(),
                           set_D.end(),
                           std::inserter(intersect_cls, intersect_cls.begin()));
          assert(intersect_cls.size() >= 1);
          if (intersect_cls.size() < 2) {
            iter_active_evars.push_back(e);
            return;
          }
        }
      }
      ++var_autarky_count;
      update_data_structure(e);
    }();
  }
  active_evars = iter_active_evars;
  if (var_autarky_count > 0)
    return 10;
  else
    return 11;
}
