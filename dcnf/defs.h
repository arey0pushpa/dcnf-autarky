#include "util.h"

void command_line_parsing(int, char* av[], std::string&, unsigned&, unsigned&,
                          bool&, bool&);

void parse_qdimacs_file(std::string filename, cls_t& dcnf_fml, cls_t& dep_set,
                        cl_t& a_vars, cl_t& e_vars, coord_t& no_of_clauses, coord_t& no_of_var,
                        coord_t& dependency_var);

void preprocess_fml(std::vector<Clauses>& dcnf_clauses, std::vector<Variables>& dcnf_variables, sel_bf& selected_bf, minsat_ass& minsat_clause_assgmt,
                   const coord_t num_of_clause, const coord_t num_of_vars, const coord_t level);

void quant_seperation(cl_t&, cl_t&, cl_t&);

void non_trivial_autarky(cl_t&, cls_t&);

void satisfied_clauses(std::vector<Clauses> dcnf_clauses, std::vector<Variables> dcnf_variables,
                       cl_t& pa_vars, cls_t& bf_vars, cls_t& pa_var_set,
                       sel_bf& selected_bf, cls_t& cnf_fml);

void touched_clauses(const std::vector<Clauses> dcnf_clauses, const std::vector<Variables> dcnf_variables,
                     cl_t& cs_vars, cl_t& pa_vars, cls_t& pa_var_set,
                     minsat_ass& minsat_clause_assgmt, cls_t& cnf_fml);

void untouched_clauses(std::vector<Clauses> dcnf_clauses, std::vector<Variables> dcnf_variables,
                       cls_t& bf_vars, cl_t& cs_vars, const coord_t& num_of_clause, cls_t& cnf_fmls);

void at_most_one(cl_t&, cls_t&);
