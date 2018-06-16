#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <chrono>
#include <exception>

#include "defs.h"
#include <fstream>

int main ( int ac, char* av[] )
{
  try {
    bool inputFile = false;
    unsigned dependencyVar = 0;
    std::string filename;

    command_line_parsing ( ac, av, filename, inputFile );
    
    if( inputFile == false ) {
      filename = "./examples/qbflib.qdimacs";
    } 

    auto start = std::chrono::high_resolution_clock::now();

    /** Global Variables **
     ** todo : Implement in a class ... Template is ready ... 
     */

    Vec1D e_var;       // existential variable set 
    Vec1D a_var;       // Universal variable set
    Vec2D dep_set;    // List of dependent variable
    Vec2D cnf_fml;
    Vec2DPair T;         
    Vec3D S;        

    /** Implement the DQCNF Code i
    *  Given: fml = vector< 
    * **/ 
    
    parse_qdimacs_file ( filename, dependencyVar, e_var, a_var, dep_set, cnf_fml, T, S );

    /* Implement a dependency Scheme in case no dependency given
      if ( dependencyVar == 0 ) 
        // Implement a dependency scheme
    */

    /** Preprocessing **/

    /** Create Constraints **/

    printf("Hello World \n"); 

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "entire run took " << elapsed.count() << " secs\n";
  }

  catch ( const std::exception& ex ) {
    std::cout << "\n Nukes are launched --> " << ex.what() << "\n";
  }

  return 0;
}

