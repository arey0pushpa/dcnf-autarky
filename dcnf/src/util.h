#include <vector>
#include <iostream>
#include <sstream> 
#include <cassert>

#include <boost/algorithm/string.hpp>

typedef std::pair<int, int> Pair;
typedef std::vector<Pair> Vec1Dpair; 
typedef std::vector<int> Vec1D; // clauses
typedef std::vector<Vec1D> Vec2D; // clause-sets
typedef std::vector<Vec2D> Vec3D; // vector of clause set 
typedef std::vector< Vec1Dpair> Vec2DPair; // represent bf var set

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
  if(vec_string.back() != '0') {
    std::cerr
              << "Input format violation[Missing 0]. Every Line should end with 0"
              << '\n';
          exit(input_format_violation);
  }
  vec_int.pop_back();
  
  return vec_int;
}

/** Vector intersection **/
inline Vec1D vector_intersection( Vec1D &v1, Vec1D &v2 ) {
  Vec1D v;
  std::set_intersection( v1.begin(), v1.end(), v2.begin(), v2.end(),
                        std::back_inserter(v) );
  return v;
}

/** Find an element in a vector **/
inline bool find_int_element ( Vec1D& vec,  int elem ) {
  return std::find( vec.begin(), vec.end(), elem ) != vec.end();
}

/** Implement a template */
/** Return index of the element **/
inline int find_index ( Vec1D& vec, int elem ) { 
  auto it = std::find( vec.begin(), vec.end(), elem );
  if ( it == vec.end() ) {
    // Todo: implement exception handling
    // std::cout << "Element not found. Exiting..." <<"\n";
    return -1;
    // exit(0);
  } else {
    auto index = std::distance( vec.begin(), it );
    return index;
  }
}

/** Return index of the element **/
inline int find_vector_index ( Vec2D& vec, Vec1D& elem ) { 
  auto it = std::find( vec.begin(), vec.end(), elem );
  if ( it == vec.end() ) {
    // Todo: implement exception handling
    // std::cout << "Element not found. Exiting..." <<"\n";
    return -1;
    // exit(0);
  } else {
    auto index = std::distance( vec.begin(), it );
    return index;
  }
}

/** Return index of the element */
inline int find_scd_index ( Vec1Dpair& vec, int elem ) { 
  for ( unsigned i = 0; i < vec.size(); i++ ) {
    if ( vec[i].second == elem ) {
      return i;
    }
  }
  // todo: error handle
  return -1;
} 

/** Print n-dimentional vector **/
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

// todo: use template
inline void print_1d_vector_pair ( std::vector<std::pair <int, char> >& T ) {
  for ( const auto& p : T ) {
    std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
  }  
}

inline void print_1d_vector_int_pair ( std::vector<std::pair <int, int> >& T ) {
  for ( const auto& p : T ) {
    std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
  }  
}

inline void print_2d_vector_pair ( Vec2DPair& T ) {
  for ( const auto& i : T ) {
    for ( const auto& p : i ) {
      std::cout << "(" << p.first << "," << p.second << ")" << std::endl;
    } 
    std::cout << "\n";
  } 
}


