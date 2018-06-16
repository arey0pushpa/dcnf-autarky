#include <vector>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>

typedef std::vector<int> Vec1D; // clauses
typedef std::vector<Vec1D> Vec2D; // clause-sets
typedef std::vector<Vec2D> Vec3D; // vector of clause set 
typedef std::vector<std::vector<std::pair<int, int>>> Vec2DPair; // represent bf var set

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

inline std::vector<std::string> get_split_line ( std::string line ){
   remove_first_word ( line );
   auto vec_string = split_string ( line );
   assert ( !vec_string.empty() && vec_string.back() == '0' );
   vec_string.pop_back();
   return vec_string;
}

/** Without Boost Splitting **/
inline Vec1D extract_int ( std::string line )
{
  Vec1D vec_int;
	std::stringstream ss; 
	ss << line;
	std::string temp;
	int found;
	while ( !ss.eof() ) {
    ss >> temp;
		if ( std::stringstream(temp) >> found ) {
      vec_int.push_back( found );
    }
	}
  assert ( !vec_string.empty() );
  vec_int.pop_back();
  
  return vec_int;
}

inline void print_1d_vector ( Vec1D& vec ) {
  //for ( const auto& i : vec ) {
  for ( unsigned int i = 0; i < vec.size(); i++ ) {
    std::cout << vec[i] << " ";
  }
}
          
inline void print_2d_vector ( Vec2D& vec ) {
  for ( unsigned int i = 0; i < vec.size(); i++ ) {
    print_1d_vector( vec[i] );
      std::cout << "\n";
  }
}
 
inline void print_3d_vector ( Vec3D& vec ) {
  for ( unsigned int i = 0; i < vec.size(); i++ ) {
    print_2d_vector( vec[i] );
      std::cout << "\n";
  }
}

inline void print_2d_vector_pair ( Vec2DPair& T ) {
  for ( const auto& i : T ) {
    for ( const auto& p : i ) {
      std::cout << " (" << p.first << "," << p.second << ")" << std::endl;
    } 
  } 
}
