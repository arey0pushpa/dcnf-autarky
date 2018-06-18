#include "defs.h"

/** Constraint 1 
 * At Most One Constraint **/
void at_most_one ( Vec1D& vec, Vec2D& fml ) {
  const unsigned N = vec.size();
  if ( N <= 1 ) return; 
  for ( unsigned i = 0; i < N-1; i++ ) {
    for ( unsigned j = i+1; j < N; j++ ) {
      fml.push_back( Vec1D{-vec[i], -vec[j]} );
    }
  }
}

/** Constraint 2 **/


/** Constraint 3 **/


/** Constraint 4 **/


/** Constraint 5 **/
