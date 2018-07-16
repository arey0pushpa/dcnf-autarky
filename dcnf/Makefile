# Ankit Shukla, 22.June.2018 (Swansea)
# Copyright 2018 Ankit Shukla

PROGRAM = dcnf_autarky dcnf_autarky_debug

CXXSOURCES = main.cpp constraints.cpp preprocess.cpp parse-qdimacs.cpp 
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXOBJECTS_debug = $(CXXSOURCES:.cpp=_debug.o)
CXX = g++

Standard_options = --std=c++11 -pedantic
Warning_options = -Wall -Werror
Optimisation_options = -Ofast -DNDEBUG
Debug_options = -g


all: dcnf_autarky dcnf_autarky_debug

dcnf_autarky : $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS)

dcnf_autarky_debug : $(CXXOBJECTS_debug)
	$(CXX) -g -o $@ $(CXXOBJECTS)

main.o: main.cpp defs.h util.h constraints.o  preprocess.o  parse-qdimacs.o Makefile
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) -c $<

constraints.o: constraints.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c $<

preprocess.o: preprocess.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c $<

parse-qdimacs.o: parse-qdimacs.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c $<

main_debug.o: main.cpp defs.h util.h constraints_debug.o  preprocess_debug.o  parse-qdimacs_debug.o Makefile
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

constraints_debug.o: constraints.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

preprocess_debug.o: preprocess.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

parse-qdimacs_debug.o: parse-qdimacs.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)
	$(RM) count *.o *~

run:
	./$(PROGRAM)
