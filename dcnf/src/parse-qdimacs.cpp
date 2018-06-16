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

/*
void semi_parse ( std::string filename, Vec1D& e_var ) {
  std::ifstream infile( filename );
  if (infile.good())
  {
    string sLine;
    std::getline( infile, sLine );
    std::cout << sLine << "\n";
  }
  infile.close();
}  */

void parse_qdimacs_file ( std::string filename, unsigned& dependencyVar, 
    Vec1D& e_var, Vec1D& a_var, Vec2D& dep_set, Vec2D& cnf_fml ) { 
  unsigned var_count;
  unsigned clause_count;

  std::ifstream file( filename );
  std::string line;
  
  if ( file.is_open() ) 
  {
    while( std::getline( file , line ) )
    {
      char s1 = line[0];
      switch (s1) {
        case 'c': break;
        case 'p': 
        { 
          auto vec_int = extract_int( line );
          assert ( vec_int.size() == 2 );
          var_count = vec_int[0];
          clause_count = vec_int[1];
          break;
        }   
        case 'e':
        {
          auto vec_int = extract_int( line );
          assert ( vec_int.size() >= 1 );
          for ( auto i : vec_int ) {
            e_var.push_back( i );
          }
          break;
        }
        case 'a':
        { 
          auto vec_int = extract_int( line );
          assert ( vec_int.size() >= 1 );
          for ( auto i : vec_int ) {
            a_var.push_back( i );
          }
          break;  
        }
        case 'd': 
        {           
          Vec1D inner_vec;
          dependencyVar += 1;
          auto vec_int = extract_int( line );
          auto vsize = vec_int.size(); 
          assert ( vsize >= 2 );
          for ( auto i : vec_int ) {  
            inner_vec.push_back( i );
          }
          dep_set.push_back( inner_vec );
          break;
        }
        
        default: 
        {
         auto vec_int = extract_int( line );
         cnf_fml.push_back( vec_int );
         break;
        }
      }
    } 
    file.close();
  }
  else {
    std::cout << "Unable to open file"; 
  }
  assert( var_count == (e_var.size() + a_var.size()) );  
}
