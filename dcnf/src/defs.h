#include <bitset> // std::bitset
#include <chrono>
#include <cmath>
#include <fstream>
#include <string>

#include "util.h"

void command_line_parsing(int, char *av[], std::string &, unsigned &,
                          unsigned &, bool &, bool &);

coord_t bfs_autarky(std::vector<Clauses> &dcnf_clauses,
                    std::vector<Variables> &dcnf_variables, sel_bf &selected_bf,
                    minsat_ass &minsat_clause_assgmt, cl_t e_vars,
                    std::string filename, std::string output_file_name,
                    coord_t dependency_var, coord_t level, coord_t s_level,
                    coord_t encoding);

coord_t e_autarky(std::vector<Clauses> &dcnf_clauses,
                  std::vector<Variables> &dcnf_variables, lit_t e);

void parse_qdimacs_file(std::string filename, cls_t &dcnf_fml, cls_t &dep_set,
                        cl_t &a_vars, cl_t &e_vars, coord_t &no_of_clauses,
                        coord_t &no_of_var, coord_t &dependency_var,
                        coord_t s_level, coord_t &min_dep_size,
                        coord_t &max_dep_size);

void set_all_solutions(std::vector<Clauses> &dcnf_clauses,
                       std::vector<Variables> &dcnf_variables,
                       sel_bf &selected_bf, minsat_ass &minsat_clause_assgmt,
                       const coord_t num_of_vars,
                       const coord_t level);

void quant_seperation(cl_t &, cl_t &, cl_t &);

void non_trivial_autarky(cl_t &, cls_t &);

void satisfied_clauses(coord_t encoding, coord_t cls_cnt, cl_t &lbf_vars,
                       std::vector<Clauses> dcnf_clauses,
                       std::vector<Variables> dcnf_variables, cls_t &bf_vars,
                       minsat_ass &pa_var_msat_ass,
                       cls_t &msat_concrete_var_map, sel_bf &selected_bf,
                       cls_t &cnf_fml, std::vector<bf_lbf_converter> &);

void touched_clauses(cl_t &cs_vars, cls_t &clausewise_pa_var_map,
                     cls_t &cnf_fml);

void untouched_clauses(coord_t encoding, cl_t &lbf_vars,
                       std::vector<Clauses> dcnf_clauses,
                       std::vector<Variables> dcnf_variables, cls_t &bf_vars,
                       cl_t &cs_vars, const coord_t &num_of_clause,
                       cls_t &cnf_fmls, std::vector<bf_lbf_converter> &);

void at_most_one(cl_t &, cls_t &);
void at_most_one_linear(cl_t &, cls_t &, coord_t &);
