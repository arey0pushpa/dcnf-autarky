#include <string>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <ostream>
#include <sstream>
#include <map>

#include <boost/regex.hpp>

#include "util.h"

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
    Vec2Expr bf_var;

    // Make each P (C) a clause. Length == MayBe( #of E_vars ) 
    VecExpr pa_var;

    // True: C is satisfied. Length == #of clause
    VecExpr clause_sel_var;

    void popl1( VecExpr&, unsigned,  std::string);
    void popl2( Vec2Expr&, unsigned, unsigned, std::string);
    void popl3( Vec3Expr&, unsigned, unsigned, unsigned, std::string);
    void popl4( Vec4Expr&, unsigned, unsigned, unsigned, unsigned, std::string);
    
  public:
};
*/
