#include "util.h"

void command_line_parsing(int, char* av[], std::string&, unsigned&, unsigned&,
                          bool&, bool&);

void parse_qdimacs_file(std::string filename, cls_t& dcnf_fml, cls_t& dep_set,
                        cl_t& a_vars, cl_t& e_vars, coord_t& no_of_var,
                        coord_t& no_of_clauses, coord_t& dependency_var);

void preprocess_fml(Clauses dcnf_clauses[], Variables dcnf_variables[], sel_bf& selected_bf, minsat_ass& minsat_clause_assgmt,
                    coord_t& num_var, coord_t& level);

void quant_seperation(cl_t&, cl_t&, cl_t&);

void non_trivial_autarky(cl_t&, cls_t&);

void satisfied_clauses(cl_t&, cl_t&, cls_t&, cls_t&, cls_t&, sel_bf&, cls_t&);

void touched_clauses(cl_t&, cl_t&, cls_t&, minsat_ass&, cls_t&);

void untouched_clauses(cl_t&, cl_t&, cls_t&, cls_t&, cls_t&);

void at_most_one(cl_t&, cls_t&);
