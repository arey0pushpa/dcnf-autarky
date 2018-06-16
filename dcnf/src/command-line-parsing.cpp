#include "defs.h"

// Include Boost Utilities 
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

using namespace boost;
using namespace boost::program_options;

void command_line_parsing ( int ac, char* av[], std::string& filename,  bool& inputFile ) {
  std::vector<std::string> inputFilename;
  
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
      exit(0);
    }

    if (vm.count("input-file")) {
      inputFilename = vm["input-file"].as< std::vector<std::string> >();
      std::cout << "Input file is = "
        << inputFilename[0] << "\n";
      filename = inputFilename[0];
      inputFile = true;
    } 
}  
