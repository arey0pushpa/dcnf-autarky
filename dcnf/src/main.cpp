#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <chrono>
#include <exception>

#include "defs.h"
#include <fstream>

// Include Boost Utilities 
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

using namespace boost;
using namespace boost::program_options;

int main ( int ac, char* av[] )
{
  try {
    bool inputFile = false;
    unsigned dependencyVar = 0;
    std::vector<std::string> inputFilename;
    std::string filename;

    options_description general("General options");
    general.add_options()
      ("help", "produce a help message");

    options_description options("Available options");
    options.add_options()
      ("input-file,i", value< std::vector<std::string> >(),
       "Specifies input file.");

    // Add all allowed options
    options_description all("Allowed options");
    all.add(general).add(options);

    options_description visible("DCNF-Autarky [version 0.0.1]. (C) Copyright 2018-2019 Swansea UNiversity. \nUsage: ./dcnf-autarky [--options] ");
    visible.add(general).add(options);

    variables_map vm;
    store(parse_command_line(ac, av, all), vm);

    positional_options_description p;
    p.add("input-file", -1);

    if (vm.count("help"))
    {
      std::cout << visible;
      return 0;
    }

    if (vm.count("input-file")) {
      inputFilename = vm["input-file"].as< std::vector<std::string> >();
      std::cout << "Input file is = "
        << inputFilename[0] << "\n";
      filename = inputFilename[0];
      inputFile = true;
    }

    if( inputFile == false ) {
      filename = "./examples/qbflib.qdimacs";
    } 

    auto start = std::chrono::high_resolution_clock::now();

    /** Global Variables **
     ** todo : Implement in a class 
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

