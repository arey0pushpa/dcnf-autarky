#include <vector>
#include <boost/algorithm/string.hpp>

typedef int lit_t; // literals
typedef std::vector<lit_t> Vec1D; // clauses
typedef std::vector<Vec1D> Vec2D; // clause-sets
typedef std::vector<Vec2D> Vec3D; // vector of clause set 
typedef std::vector<std::vector<std::pair<int, int>>> Vec2DPair; // represent bf var set

void parse_qdimacs_file ( std::string filename, unsigned& dependencyVar, 
    Vec1D& e_var, Vec1D& a_var, Vec2D& dep_set, 
    Vec2D& cnf_fml, Vec2DPair& T, Vec3D& S );

inline void remove_first_word ( std::string &sentence ) {
  std::string::size_type n = 0;
  n = sentence.find_first_not_of( " \t", n );
  n = sentence.find_first_of( " \t", n );
  sentence.erase( 0,  sentence.find_first_not_of( " \t", n ) );
}

inline std::vector<std::string> split_string ( std::string text, bool splitOnUnder = true ) {
  std::vector<std::string> results;
  boost::split(results, text, [](char c){ return c == ' '; });
  return results;
}

inline std::vector<std::string> get_split_line ( std::string& line ){
   remove_first_word ( line );
   auto vec_string = split_string ( line );
   assert ( !vec_string.empty() && vec_string.back() == '0' );
   vec_string.pop_back();
   return vec_string;
}

inline void print_1d_vector ( Vec1D& vec ) {
 for ( const auto& i : vec ) {
   std::cout << i << "\n";
 }
}
          
inline void print_2d_vector (Vec2D& vec) {
  for ( const auto& i : vec ) {
    for ( const auto& j : i ) {
        std::cout << j << " ";
    }
  }
}  
