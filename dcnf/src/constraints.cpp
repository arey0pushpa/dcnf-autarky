#include "defs.h"

/** Constraint 1 
 * At Most One Constraint **/
void at_most_one ( Vec1D& vec, Vec2D& cnf_fml ) {
  const unsigned N = vec.size();
  if ( N <= 1 ) return; 
  for ( unsigned i = 0; i < N-1; i++ ) {
    for ( unsigned j = i+1; j < N; j++ ) {
      cnf_fml.push_back( Vec1D{-vec[i], -vec[j]} );
    }
  }
}

/** Constraint 2 **/


/** Constraint 3 **/


/** Constraint 4 **/


/** Constraint 5 **/
void non_trivial_autarky ( Vec1D& cs_var, Vec2D& cnf_fml ) {
  Vec1D dummy_vec;
  for ( auto i : cs_var ) {
    dummy_vec.push_back( i );
  }
  print_1d_vector( dummy_vec );
  cnf_fml.push_back( dummy_vec );
}
