#include "util.h"
namespace {

// --- General input and output ---

const std::string version = "2.8.0";
const std::string date = "29.5.2018";

const std::string program = "tawSolver";

enum Error_codes {
  file_reading_error=1,
  file_writing_error=2,
  file_pline_error=3,
  num_vars_error=4,
  allocation_error=5,
  literal_read_error=6,
  variable_value_error=7,
  number_clauses_error=8,
  empty_clause_error=9,
  unit_clause_error=11,
  input_format_violation=12
};
}

void command_line_parsing ( int, char* av[], std::string&, unsigned&, unsigned&, bool&, bool& );

void parse_qdimacs_file ( std::string, unsigned&, 
    Vec1D&, Vec1D&, Vec2D&, Vec2D& );

void preprocess_fml ( Vec1D&, Vec1D&, Vec2D&, 
    Vec2D&, Vec2DPair&, Vec3D&, unsigned);

void quant_seperation ( Vec1D&, Vec1D&, Vec1D& );

void non_trivial_autarky ( Vec1D&, Vec2D& );

void satisfied_clauses ( Vec1D&, Vec1D&, Vec2D&, 
   Vec2D&, Vec2D&, Vec2DPair&, Vec2D& );

void touched_clauses ( Vec1D&, Vec1D&, Vec2D&, Vec3D&, Vec2D& );

void untouched_clauses ( Vec1D&,  Vec1D&, Vec2D&, Vec2D&, Vec2D& );

void at_most_one ( Vec1D&, Vec2D& );

/*
// DCNF [ Universal_var_count, Existential_var_count, Number_Of_clauses, Number_of_var ] 
class dcnf{
  public:
    dcnf( cl_t _A, cl_t _E, cl_t _D, vec_p T, ord_t _S )
      : A(_A), E(_E), D(_D), T(_T), S(_S)  
    {
      init_dcnf();
    }

  private:
  
    cl_t A;       // Universal_var_count 
    cl_t E;       // Existential_var_count
    cl_t D;       // Number_Of_clauses
    vec_p T;       // Number_of_var  
    ord_t S;
    
    void init_dcnf();

    // Variable of the DCNF Translation 
    // True:: f is chosen for v. Length = (2 * E_vars) + 2
    Vec2D bf_var;

    // Make each P (C) a clause. Length == MayBe( #of E_vars ) 
    Vec1D pa_var;

    // True: C is satisfied. Length == #of clause
    Vec2D clause_sel_var;
    
  public:
};
*/
