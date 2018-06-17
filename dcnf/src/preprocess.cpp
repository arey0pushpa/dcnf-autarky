#include <iterator>
#include <iostream>
#include <algorithm>

#include "util.h"

void quant_seperation ( Vec1D& c, Vec1D& e_part, Vec1D& a_part , std::vector<std::pair <int, char> >& union_var ) {
  for ( auto& l : c ) {
    auto index = union_var[ abs(l) ]; // get the lth value  
    auto var = std::get<1>( index );
    if ( var == 'e' ) 
      e_part.push_back( l ); 
    else 
      a_part.push_back( l ); 
  }
}

void preprocess_fml ( Vec1D& e_var, Vec1D& a_var, Vec2D& dep_set, 
    Vec2D& cnf_fml, Vec2DPair& T, Vec3D& S ) {

  /** Create Complete Dependency List **/
  std::sort(dep_set.begin(), dep_set.end(),
      [](const std::vector<int>& a, const std::vector<int>& b) {
      return a[0] < b[0];
      });

  assert ( e_var.size() >= dep_set.size() );
  Vec1D e_pr;
  for ( auto& i : dep_set ) {
    e_pr.push_back( i[0] );
  }
  //print_1d_vector( e_pr );

  /** Come Back and Complete **/
  unsigned ctr = 0;
  for ( auto e : e_var ) {
    if ( e == e_pr[ctr] ) {
      ctr += 1;
    } else {
      auto dummy_vec = a_var;
      dummy_vec.insert( dummy_vec.begin(), e );
      dep_set.push_back( dummy_vec ); 
    } 
  }

  std::sort(dep_set.begin(), dep_set.end(),
      [](const std::vector<int>& a, const std::vector<int>& b) {
      return a[0] < b[0];
      });

  /** Selected Boolean Function **/
  for ( unsigned i = 0; i < e_var.size(); i++  ) {
    std::vector< std::pair <int,int> > t_vec;
    // Base Case [bf(0), bf(1)]
    t_vec.emplace_back( e_var[i], 1000 ); //true 
    t_vec.emplace_back( e_var[i], 500 );  //false
    // Other Cases
    for ( unsigned j = 0; j < dep_set[i].size(); j++ ) {
      if ( j == 0 ) 
        continue;
      else  {
        t_vec.emplace_back( e_var[i], dep_set[i][j] ); 
        t_vec.emplace_back( e_var[i], -dep_set[i][j] ); 
      } 
    }
    T.push_back( t_vec );
  }

  std::vector<std::pair <int, char> > union_var;

  for ( auto i : e_var ) {
    union_var.emplace_back( i, 'e');      
  }
  for ( auto j : a_var ) {
    union_var.emplace_back( j, 'a');      
  }

  /*
     std::sort(union_var.begin(), union_var.end(),
     [](const std::vector<int>& a, const std::vector<int>& b) {
     return a[0] < b[0];
     });
  */

  /** Three cases to consider 
   * 1. Basic case: handle all are e_variables 
   * 2. Handle dependency case for all e_variable 
   * 3. Handle e-var and a-var case
   */

  /** Minimal Satisfying Clauses **/
  for ( auto c : cnf_fml ) {
    Vec2D dummy_s;
    Vec1D e_part;
    Vec1D a_part;
    quant_seperation( c, e_part, a_part, union_var );

    /** All e-var case **/
    for ( auto& e : e_part ) {
      if ( e > 0 ) {
        Vec1D inner_vec = { e, 1000 };
        dummy_s.push_back( inner_vec );
      } else {
        Vec1D inner_vec = { e, 500 };
        dummy_s.push_back( inner_vec );
      }
    }

    /** e-var pairs case **/
    auto& size = e_part.size();
    for ( unsigned i = 0; i < size-1; i++ ) {
      // Implement the scheme to find the dependency set of a e-var.
      auto& dep1 = dep_set[i]; // get the dependent set of the e variables. 
      for ( unsigned j = i+1; j < size; j++ ) {
        auto& dep2 = dep_set[j]; // get the dependent set of the e variables. 
        // Implement Intersection of two vectors.
        auto& d_vec = vector_intersection( dep1, dep2 );
        for ( auto& d : d_vec ) {
          Vec1D inner_vec1 = { e_part[i], d, e_part[j], -d };
          Vec1D inner_vec2 = { e_part[i], -d, e_part[j], d };
          dummy_s.push_back( inner_vec1 );
          dummy_s.push_back( inner_vec2 );
        }
      }
    }

    /** e-var a-var case **/
    for ( auto& e : e_part ) {
      auto& dep = dep_set[]; // get the dependent set of the e variables. 
      /** todo : try using intersection **/
      for ( auto& a : a_part ) {
        auto& presence_a = std::find( dep.begin(), dep.end(), abs(a) ) != dep.end(); 
        if ( presence_a ) { 
          Vec1D inner_vec = { e, -a };
          dummy_s.push_back( inner_vec );
        }
      }
    }
    // Do final push on the S
    S.push_back( dummy_s );
  }
}

