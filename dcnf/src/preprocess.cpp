#include <iterator>
#include <iostream>
#include <algorithm>

#include "util.h"

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
   /** Minimal Satisfying Clauses **/
    for ( auto c : cnf_fml ) {
       Vec2D dummy_s;
       for ( auto l : c ) {
         auto index = union_var[ abs(l) ]; // get the lth value  
         auto var = std::get<1>( index );
          if ( var == 'e' ) {
             Vec1D inner_vec;
             if ( l > 0 ) {
              // Basic case
               inner_vec.push_back( l );
               inner_vec.push_back( true );
               
              // Other Cases: Take Care of Intersection
             } else {
              // Basic case
              inner_vec.push_back( l );
              inner_vec.push_back( false );
              // Other cases
             } 
          } else {
             continue;
          }  
        }
        S.push_back( dummy_s );
    }
}
