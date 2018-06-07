#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "defs.h"
#include "util.h"

void parse_qdimacs_file ( std::string filename, unsigned& dependencyVar ) {
  std::ifstream file( filename );
  std::string line;
  while( std::getline( file , line ) )
  {
    std::string s1 = line.substr(0, line.find(' '));
    //std::cout << "The value of s1 is : " << s1 << "\n";
    if ( s1 == "c" || s1 == "p" ) {
      continue;
    } else if ( s1 == "e" || s1 == "a" ) {
      std::string quant;
      if ( s1 == "e") {
        std::cout << "Inside the Existential block. \n";
        // do something
      } else {
        std::cout << "Inside the Universal block. \n";
        // do something
      }
    } else if ( s1 == "d" ) {
      dependencyVar += 1;
      remove_first_word ( line );
      auto vec_string = split_string ( line );
      auto vsize = vec_string.size();
      std::cout << "Inside the Dependency block. \n";
      if ( vsize == 2 ) {
        // Single variable dependence f( var )
      } else if ( vsize > 2 ) {
        // Implement a f ( var, var ) scheme 
      } else {
        std::cout << "Wrong input format of dependency. Exiting...\n";
        exit(0);
      }
      // do something
    } else {
      continue;
    }
  }
}


