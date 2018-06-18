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

    Vec2D cnf_fml;   // dimacs/cnf variables  
    Vec1D cnf_vars;  // dimacs/cnf variables

    Vec1D cs_var; 
    Vec2D bf_var;
    Vec1D pa_var;

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

    std::cout << "\nThe s(v) is: " << "\n";
    print_2d_vector_pair( T );
    std::cout << "\nThe S(C) is: " << "\n";
    print_3d_vector( S );

    /** Create traslation Variables/ordering **/
    unsigned index = 1;

    // cs variable
    for ( unsigned i = 0; i < dcnf_fml.size(); i++ ) {
      cs_var.push_back( index );
      index += 1;
    }

    // bf variable
    std::cout << "The size of s(v) is: " << T.size() << "\n";
    for ( unsigned i = 0; i < T.size(); i++ ) {
      //print_1d_vector_int_pair  ( T[i] );
       for ( auto& p : T[i] ) {
       bf_var[i].push_back( index ); 
        index += 1;
      }
    }

    // pa variable 
    /** Straigten, sort and remove duplicates and then map **/ 
    

    /** Create Constraints **/
    // 4.5 Non trivial Autarky
    non_trivial_autarky ( cs_var, cnf_fml );

    // 4.1 At Most One Constraint
    for ( unsigned i = 0; i < cs_var.size(); i++ ) {
      at_most_one ( bf_var[i], cnf_fml );
    }
    
    //print_2d_vector ( cnf_fml );

    std::string fname = "/tmp/dcnf.dimacs";
    std::cout << "Writing the DIMACS file to .. " << fname << "\n";
    std::ofstream fout( fname );
    
    if ( !fout ) {
      std::cerr << "Error opening file..." << fname << "\n";
      return 1;
    }

    fout << "c" << "Writing the dcnf output in dimacs format";
    fout << "p cnf " << index - 1 << " " << cnf_fml.size() << "\n";
    
    for ( auto& C : cnf_fml ) {
      for ( auto lit : C ) {
        fout << lit << " ";
      }
      fout << "0" << "\n";
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "entire run took " << elapsed.count() << " secs\n";
  }

  catch ( const std::exception& ex ) {
    std::cout << "\n Nukes are launched --> " << ex.what() << "\n";
  }
  return 0;
}

