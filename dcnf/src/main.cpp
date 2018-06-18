#include <array>

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
    
    /** Handle Command Line parsing **/
    command_line_parsing ( ac, av, filename, inputFile );
    
    if( inputFile == false ) {
      filename = "./examples/qbflib.qdimacs";
    } 

    auto start = std::chrono::high_resolution_clock::now();

    /** Global Variables **
     ** todo : Implement in a class ... Template is ready ... 
     */

    Vec2D dcnf_fml;     // Input Cnf formula { Clause }

    Vec1D e_var;       // existential variable set 
    Vec1D a_var;       // Universal variable set
    Vec2D dep_set;     // List of dependent variable
    Vec2DPair T;       // All bf (v,f) pairs   
    Vec3D S;           // All S(C)'s

    Vec2D trans_fml;   // translation variables 
    Vec1D trans_vars;  // Set of translated variables
    Vec3D var_ord;     // Solver variable ordering

    /**** Implement the DQCNF Code ****/ 
    
    /** Parse Input file **/
    parse_qdimacs_file ( filename, dependencyVar, e_var, a_var, dep_set, dcnf_fml );
    
    // std::cout << "Printing input cnf formula...\n"; 
    // print_2d_vector ( cnf_fml );
    
    /* Todo: Implement a dependency Scheme in case no dependency given 
      if ( dependencyVar == 0 ) { 
        // Implement a dependency scheme
      } */

    /** Preprocessing **/
    preprocess_fml( e_var, a_var, dep_set, dcnf_fml, T, S );

    std::cout << "\nThe t is: " << "\n";
    print_2d_vector_pair( T );
    //print_3d_vector( S );

    /** Create Constraints **/
    // Create Variables
    unsigned index = 1;
    for ( unsigned i = 0; i < dcnf_fml.size(); i++ ) {
      trans_vars.push_back( index );
      index += 1;
    }
    trans_vars.clear();

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "entire run took " << elapsed.count() << " secs\n";
  }

  catch ( const std::exception& ex ) {
    std::cout << "\n Nukes are launched --> " << ex.what() << "\n";
  }
  return 0;
}

