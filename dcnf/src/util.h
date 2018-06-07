#include "defs.h"

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




