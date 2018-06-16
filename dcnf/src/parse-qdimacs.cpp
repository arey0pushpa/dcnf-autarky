#include <defs.h>
#include <fstream>

/** Varibles needs to be fixed 
 * e_var = []
 * a_var = []
 * dep_set = []
 *
 * func: add_dep_set()
 * func: create_sv()
 */

void parse_qdimacs_file ( std::string filename, unsigned& dependencyVar, 
    Vec1D& e_var, Vec1D& a_var, Vec2D& dep_set, 
    Vec2D& cnf_fml, Vec2DPair& T, Vec3D& S ) { 
  unsigned var_count;
  unsigned clause_count;

  std::ifstream file( filename );
  std::string line;
  
  /** todo: 1. handle conversion to each string to int in get_split_line() **/
  if ( file.is_open() ) 
  {
    while( std::getline( file , line ) )
    {
      char s1 = line[0];
      switch (s1) {
        case 'c': break;
        case 'p': 
        { 
          auto vec_string = get_split_line( line ); 
          assert ( vec_string.size() == 3 );
          var_count = std::stoi( vec_string[1] );
          clause_count = std::stoi( vec_string[2] );
          break;
        }   
        case 'e':
        {
          auto vec_string = get_split_line( line );
          assert ( vec_string.size() >= 1 );
          for ( auto i : vec_string ) { 
            e_var.push_back( std::stoi( i ) );
          }
          break;
        }
        case 'a':
        { 
          auto vec_string = get_split_line( line );
          assert ( vec_string.size() >= 1 );
          for ( auto i : vec_string ) {
            a_var.push_back( std::stoi( i ) );
          }
          break;  
        }
        case 'd': 
        { 
          dependencyVar += 1;
          
          auto vec_string = get_split_line( line );
          auto vsize = vec_string.size(); 
          assert ( vsize >= 2 );
          
          auto var = std::stoi( vec_string[0] );
          assert( !vec_string.empty() );
          vec_string.erase( vec_string.begin() );
      
          Vec1D inner_vec;
          for ( auto i : vec_string ) {  
            inner_vec.push_back( std::stoi(i) );
          }
          dep_set.push_back( inner_vec );
          break;
        }
        
        default: continue;
      }
    } 
    file.close();
  }
  else {
    std::cout << "Unable to open file"; 
  }
  assert( var_count == (e_var.size() + a_var.size()) );  
}
