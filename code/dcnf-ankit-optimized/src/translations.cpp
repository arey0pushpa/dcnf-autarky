#include <future>

#include "dcnf.h"
#include "util.h"

/** Remove the dead/inactive clauses from the active variable list **/
void dcnf::propagate_cls_removal(lit_t cls_indx) {
  // TODO: Check for the use of cls_indx
  for (lit_t l : dcnf_clauses[cls_indx].lits) {
    // Not sure if this check is required
    if (!dcnf_variables[std::abs(l) - 1].present)
      continue;
    if (l > 0) {
      dcnf_variables[std::abs(l) - 1].pos_cls.erase(cls_indx);
    } else {
      dcnf_variables[std::abs(l) - 1].neg_cls.erase(cls_indx);
    }
  }
}

void dcnf::update_avars() {
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
}

void dcnf::update_evars() {
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
}

/** Create lbf formula **/
cl_t lbf_formula(cl_t &lbf_vars, lit_t bf_var) {
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

/** Home made printing final assignment **/
void print_final_assignment(cls_t &final_assgmt, coord_t no_of_vars) {
  for (cl_t c : final_assgmt) {
    std::cout << "< " << c[0] << "->";
    for (coord_t i = 1; i < c.size(); ++i) {
      if (i == c.size()) {
        c[i] > no_of_vars ? (c[i] == no_of_vars + 1 ? std::cout << "False >"
                                                    : std::cout << "True >")
                          : std::cout << c[i] << " >\n";
      } else {
        c[i] > no_of_vars ? (c[i] == no_of_vars + 1 ? std::cout << "False v"
                                                    : std::cout << "True v ")
                          : std::cout << c[i] << " v ";
      }
    }
  }
}

// Final Output prinitng
void dcnf::output() {
  std::cout << "c Input Parameter (command line, file): \n";
  std::cout << "c input filename   " << filename << '\n';
  std::cout << "c output filename  " << output_file_name << '\n';
  std::cout << "c autarky level    " << aut_level << '\n';
  std::cout << "c conformity level " << s_level << '\n';
  std::cout << "c encoding type    " << encoding << '\n';
  std::cout << "c no.of var        " << no_of_vars << '\n';
  std::cout << "c no.of clauses    " << no_of_clauses << '\n';
  std::cout << "c no.of taut cls   " << ntaut << '\n';
  // std::cout << "no_of_avars      " << no_of_avars << '\n';
  /// std::cout << "no_of_evars      " << e_vars << '\n';
  // std::cout << "unique_dep_sets  " << uni_dep_set << '\n';
  // std::cout << "cs_var           " << cs_var << '\n';
  // std::cout << "bf_vars          " << bf_var << '\n';
  // std::cout << "pa_vars          " << pa_var << '\n';
  // std::cout << "vars_in_dimacs   " << vars_in_dimacs << '\n';
  // std::cout << "mtx_size_dimacs  " << matrix_size_dimacs << '\n';

  std::cout << "c \n";
  std::cout << "c Output Parameters: \n";
  // std::cout << "remaining no.of avars " << active_avars << '\n';
  // std::cout << "remaining no.of evars " << active_evars << '\n';
  std::cout << "c remaining no.of clauses  " << present_clauses.size() << '\n';
  // std::exit(0);
}

// Print the total time taken
lit_t running_time(
    std::chrono::time_point<std::chrono::high_resolution_clock> start) {
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  return elapsed.count();
}

void dcnf::print_results() {
  if (output_type == 0)
    output();
  if (output_type == 0 || output_type == 1)
    display_rresult();
}

/** handle output of an Aut_reduction based on aut_present */
void dcnf::display_result(coord_t aut_present, coord_t output_type) {
  if ((aut_present == 10 && result != "RED") || result == "SAT") {
    result = "SAT";
    print_results();
  }
  if ((aut_present == 20 && result != "RED") ||
      (aut_present == 20 && reduction_type == 2)) {
    if (ever_reduced) {
      result = "RED";
    } else {
      result = "NONE";
    }
    print_results();
  }
  old_cls_size = present_clauses.size();
  result = "RED";
}

std::string display_string(cl_t &container) {
  std::string str;
  // for (auto const &c : container) {
  for (coord_t i = 0; i < container.size(); ++i) {
    if (i == container.size() - 1) {
      str += std::to_string(container[i]);
    } else {
      str += std::to_string(container[i]) + " ";
    }
  }
  return str;
}

/** display R result */
void dcnf::display_rresult() {
	 // std::string str = "sha1sum ";
  // str = str + filename;
  // const char *command = str.c_str();
  // system(command);
  std::string aut_type;
  if (reduction_type == 1) {
    aut_type = "E1";
  } else if (reduction_type == 2) {
    aut_type = "A1";
  } else {
    aut_type = "E1+A1";
  }
  std::string r_out = "";
  r_out += "\"" + filename + "\"" + " ";
  r_out += std::to_string(no_of_vars) + " ";
  r_out += std::to_string(no_of_clauses) + " ";
  r_out += "\"" + aut_type + "\"" + " ";
  //r_out += "[" + display_string(assigned_evars) + "] ";
  r_out += std::to_string(ntaut) + " ";
  // r_out += "[ " + display_string(active_avars) + "] ";
  r_out += std::to_string(active_avars.size()) + " ";
  // r_out += "[ " + display_string(active_evars) + "] ";
  r_out += std::to_string(active_evars.size()) + " ";
  r_out += std::to_string(no_of_clauses - present_clauses.size()) + " ";
  r_out += "\"" + result + "\"" + " ";
  // r_out += std::to_string(running_time(start)) + " ";
  std::cout << "c\nc filename pn pc autarky ntaut rpa rpe rpcdiff result\n";
  std::cout << "c " << r_out << "\n";
  std::cout << "p cnf " << no_of_vars << " "
            << present_clauses.size() << "\n";
  if (present_clauses.size() == 0) std::exit(0);
  for (coord_t i = 0; i < aed_lines.size(); ++i) {
   std::cout << aed_lines[i] << "\n";
  }
  for (lit_t c : present_clauses) {
    assert(c >= 0);
    cl_t c1 = dcnf_clauses[c].lits;
    print_1d_vector(c1);
    std::cout << "0\n";
  }
  std::exit(0);
}


/**** A_Autarky ********/
coord_t dcnf::a_autarky(std::string filename, std::string output_file_name,
                        const coord_t encoding) {
  // Traslation variables with ordering
  cl_t cs_vars;
  cls_t bf_vars;
  cl_t pa_vars;

  cls_t cnf_fml; // dimacs/cnf fml {{lit...}...}
  cl_t cnf_vars; // dimacs/cnf var {cnf-vars}

  lit_t index = 1;

  // cs variable := #no_of Active clauses
  for (coord_t i = 0; i < present_clauses.size(); ++i) {
    cs_vars.push_back(index);
    index += 1;
  }

  if (index == 1) {
    result = "SAT";
    return 10; // empty cls list; return SAT
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

  if (encoding == 1) { // LOG Encoding
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
  // bigbag to push all concrete bf variable assmts
  minsat_ass pa_var_msat_ass(active_evars.size());
  // Create a Set for each concete var assignemnt
  vecsets_t pavar_msat_ea(active_evars.size());
  vecsets_cls pavar_msat_ee(active_evars.size());

  cls_t msat_ea(active_evars.size());
  minsat_ass msat_ee(active_evars.size());

  lit_t current_size = 0;
  lit_t pa_indx;
  lit_t pos = 0;

  // dqbf Var to Cnf var Map; pa_vars for each bigbag element
  cls_t msat_concrete_var_map(active_evars.size());
  cls_t msat_concrete_var_ea(active_evars.size());
  cls_t msat_concrete_var_ee(active_evars.size());

  cls_t clausewise_pa_var_map(
      present_clauses.size()); // create clausewise cnf vars

  for (lit_t c : present_clauses) {
    for (coord_t j = 0; j < minsat_clause_assgmt[c].size(); ++j) {
      cl_t dummy = minsat_clause_assgmt[c][j];
      assert(dummy.size() == 2 || dummy.size() == 4);
      lit_t slit = std::abs(dummy[0]);
      lit_t univar = std::abs(dummy[1]);
      lit_t unilit = dummy[1];
      // Check if you need Active clause
      if (!dcnf_variables[univar - 1].present && univar != no_of_vars + 1 &&
          univar != no_of_vars + 2) {
        continue;
      }

      lit_t elit = active_evar_index[slit - 1];
      if (dummy.size() == 2) {
        current_size = pavar_msat_ea[elit].size();
        pavar_msat_ea[elit].insert(unilit);
        if (pavar_msat_ea[elit].size() > current_size) {
          msat_ea[elit].push_back(unilit);
          msat_concrete_var_ea[elit].push_back(index);
          clausewise_pa_var_map[present_cls_index[c]].push_back(index);
          pa_vars.push_back(index);
          ++index;
        } else {
          pos = std::distance(
              msat_ea[elit].begin(),
              std::find(msat_ea[elit].begin(), msat_ea[elit].end(), unilit));
          clausewise_pa_var_map[present_cls_index[c]].push_back(
              msat_concrete_var_ea[elit][pos]);
        }
      } else {
        current_size = pavar_msat_ee[elit].size();
        pavar_msat_ee[elit].insert(dummy);
        if (pavar_msat_ee[elit].size() > current_size) {
          msat_ee[elit].push_back(dummy);
          msat_concrete_var_ee[elit].push_back(index);
          clausewise_pa_var_map[present_cls_index[c]].push_back(index);
          pa_vars.push_back(index);
          ++index;
        } else {
          pos = std::distance(
              msat_ee[elit].begin(),
              std::find(msat_ee[elit].begin(), msat_ee[elit].end(), dummy));
          clausewise_pa_var_map[present_cls_index[c]].push_back(
              msat_concrete_var_ee[elit][pos]);
        }
      }
    }
  }

  // Put everything back in pavar_msat_ass
  for (coord_t i = 0; i < active_evars.size(); ++i) {
    for (lit_t j : msat_ea[i]) {
      pa_var_msat_ass[i].push_back({active_evars[i], j});
    }
    for (cl_t k : msat_ee[i]) {
      pa_var_msat_ass[i].push_back(k);
    }
    for (lit_t l : msat_concrete_var_ea[i]) {
      msat_concrete_var_map[i].push_back(l);
    }
    for (lit_t m : msat_concrete_var_ee[i]) {
      msat_concrete_var_map[i].push_back(m);
    }
  }

  // --- Build Constraints
  non_trivial_autarky(cs_vars, cnf_fml); // (4.5)

  satisfied_clauses(lbf_vars, bf_vars, pa_var_msat_ass, msat_concrete_var_map,
                    cnf_fml, bf2lbf_var_map,
                    active_evar_index); // (4.2)

  touched_clauses(cs_vars, clausewise_pa_var_map, cnf_fml); // (4.3)

  untouched_clauses(lbf_vars, bf_vars, cs_vars, cnf_fml, bf2lbf_var_map,
                    present_cls_index,
                    active_evar_index); // (4.4)

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
    result = "ERR";
    if (output_type == 0) {
      std::cerr << "c Error opening file..." << output_file_name << "\n";
    }
    if (output_type == 0 || output_type == 1) {
      display_rresult();
    }
  }

  // Writing the dcnf output in dimacs format
  fout << "c This is a output dimacs file of input file: " << filename << '\n';
  fout << "c There are total " << cs_vars.size() << " cs-variables. ";
  for (lit_t c : cs_vars) {
    fout << c << " ";
  }

  fout << "\n";

  std::string bf_var_size;
  coord_t total_bfvars = 0;
  coord_t total_pavars = 0;

  // Create a String (bf_var_size) representing the list of bf_vars
  for (coord_t i = 0; i < bf_vars.size(); ++i) {
    total_bfvars += bf_vars[i].size();
    for (coord_t j = 0; j < bf_vars[i].size(); ++j) {
      bf_var_size = bf_var_size + std::to_string(bf_vars[i][j]) + " ";
    }
    if (i < bf_vars.size() - 1)
      bf_var_size = bf_var_size + " +  ";
  }

  if (encoding == 1) {
    // fout << "c There are total " << total << " distinct lbf-variables. "
    //     << lbf_var_size;
  } else {
    fout << "c There are total " << total_bfvars << " distinct bf-variables. "
         << bf_var_size;
  }
  fout << "\n";
  fout << "c There are total " << pa_vars.size() << " distinct pa-variables. ";
  for (coord_t i = 0; i < pa_vars.size(); ++i) {
    ++total_pavars;
    fout << pa_vars[i] << " ";
  }
  fout << '\n';
  fout << "p cnf " << index - 1 << " " << cnf_fml.size() << "\n";

  // Add support for the new auxilary variables by AMO encoding
  assert(index >= cs_vars.size() + total_pavars + total_bfvars + 1);

  for (const cl_t C : cnf_fml) {
    for (const lit_t l : C) {
      fout << l << " ";
    }
    fout << "0"
         << "\n";
  }
  fout.close();

  std::string sat_out = "/tmp/" + fname + ".out";
  std::string cmd("./build/lingeling/lingeling -q ");
  cmd += output_file_name;
  cmd += " > ";
  cmd += sat_out;
  std::future<int> future = std::async(std::launch::async, [=]() {
    auto retVal = system(cmd.c_str());
    return retVal;
  });

  cl_t var_assgn;
  if (output_type == 0) {
    std::cout << "c Running Lingeling ... "
              << "\nc\n";
  }
  std::future_status status;

  status = future.wait_for(std::chrono::seconds(12000));

  // Handle timout and chek for the MemoryOut
  if (status == std::future_status::timeout) {
    result = "T/O";
    if (output_type == 0) {
      std::cout << "c TimeOut! \n";
    }
    if (output_type == 0 || output_type == 1) {
      display_rresult();
    }
    std::terminate();
  }

  if (status == std::future_status::ready) {
    std::string filenm = sat_out;
    std::string line;
    std::ifstream file(filenm);
    if (!file.is_open()) {
      perror(("c Error while opening file " + filenm).c_str());
      std::exit(file_reading_error);
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
          result = "UNSAT";
          return 20;
        }
        break;
      }
      }
    }
    if (file.bad()) {
      result = "ERR";
      if (output_type == 0) {
        perror(("c Error while reading file " + filenm).c_str());
      }
      if (output_type == 0 || output_type == 1) {
        display_rresult();
      }
      std::exit(file_reading_error);
    }
    file.close();
  }
  // remove last 0 from the assignment
  var_assgn.pop_back();

  /*  Update the data structure   */
  // For each satisfied clause, remove them from cls list and add them to
  // deleted list Update the variable occurance list
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

  // Relying on the SAT solver to provide ordered assignment
  // Register Assignments of the variables
  for (coord_t i = 0; i < bf_vars.size(); ++i) {
    for (coord_t j = 0; j < bf_vars[i].size(); ++j) {
      if (var_assgn[bf_vars[i][j] - 1] > 0) {
        pair_t sbf = selected_bf[i][j];
        assigned_evars.push_back(sbf.first);
        final_assgmt.push_back({sbf.first, sbf.second});
      }
    }
  }

  // Update the A and E active vars
  update_avars();
  update_evars();

  if (present_clauses.size() > 0) {
    ever_reduced = 1;
    return 11;
  } else {
    result = "SAT";
    return 10;
  }
}

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

coord_t dcnf::e_autarky(lit_t e) {
  if (dcnf_variables[e - 1].pos_cls.size() +
          dcnf_variables[e - 1].neg_cls.size() ==
      0) {
    // remove. No use!
    update_data_structure(e);
    return 11;
  }
  set_t intersect;
  set_t s1 = dcnf_variables[e - 1].pos_cls;
  set_t s2 = dcnf_variables[e - 1].neg_cls;
  if (s1.size() == 0 || s2.size() == 0) { // Pure Lit case
    update_data_structure(e);
    final_assgmt.push_back({e, s1.size() ? no_of_vars + 2 : no_of_vars + 1});
    if (present_clauses.size() > 0)
      return 11;
    else
      return 10;
  }
  cl_t vec = dcnf_variables[e - 1].dependency;
  // OPTIMISATION: If a clause in s1 do not have any dep variable of e just
  // QUIT!!
  for (lit_t j : s1) {
    // if (!dcnf_clauses[j].present) continue;
    cl_t cls_s1 = dcnf_clauses[j].lits;
    set_t compl_C;
    set_t set_D;
    // Implement a func or change vector to a set
    for (lit_t l1 : cls_s1) {
      if (std::abs(l1) == e)
        continue;
      if (l1 > 0) {
        compl_C.insert(-l1);
      } else {
        compl_C.insert(std::abs(l1));
      }
    }
    for (coord_t k : s2) {
      set_D.clear();
      set_t intersect_cls;
      cl_t cls_s2 = dcnf_clauses[k].lits;
      for (lit_t l2 : cls_s2) {
        if (std::find(vec.begin(), vec.end(), std::abs(l2)) == vec.end())
          continue;
        set_D.insert(l2);
      }
      set_intersection(compl_C.begin(), compl_C.end(), set_D.begin(),
                       set_D.end(),
                       std::inserter(intersect_cls, intersect_cls.begin()));
      if (intersect_cls.size() < 1) {
        return 0;
      }
    }
  }
  update_data_structure(e);
  // Add the chosen assgnmnet: first clause
  cl_t vassgnmt;
  vassgnmt.push_back(e);
  for (lit_t l : dcnf_clauses[*s1.begin()].lits) {
    if (std::abs(l) == e)
      continue;
    if (std::find(vec.begin(), vec.end(), std::abs(l)) == vec.end())
      continue;
    vassgnmt.push_back(l ? -l : std::abs(l));
  }
  final_assgmt.push_back(vassgnmt);
  if (present_clauses.size() > 0)
    return 11;
  else
    return 10;
}
