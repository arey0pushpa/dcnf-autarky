# Ankit Shukla, 22.June.2018 (Swansea)
# Copyright 2018 Ankit Shukla, Oliver Kullmann

git = git -c user.name="Auto" -c user.email="auto@auto.com"

BUILDDIR = $(PWD)/build
SRCDIR = $(PWD)/src

PROGRAM = dcnf_autarky dcnf_autarky_debug

CXXSOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/constraints.cpp $(SRCDIR)/all-solutions.cpp $(SRCDIR)/parse-qdimacs.cpp $(SRCDIR)/translations.cpp
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXOBJECTS_debug = $(CXXSOURCES:.cpp=_debug.o)
CXX = g++

Standard_options = --std=c++11 -pedantic
Warning_options = -Wall -Werror
Optimisation_options = -Ofast -DNDEBUG
Debug_options = -g

.PHONY: all release

all: release dcnf_autarky 
#dcnf_autarky_debug

release: $(BUILDDIR)/buildr $(BUILDDIR)/cryptominisat

$(BUILDDIR)/cryptominisat: | $(BUILDDIR)
	cd $(dir $@); $(git) clone https://github.com/msoos/cryptominisat.git; cd cryptominisat; mkdir build && cd build; cmake ..; make -j4 

$(BUILDDIR) $(BUILDDIR)/buildr:
	mkdir -p $@

dcnf_autarky : $(CXXOBJECTS)
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(CXXOBJECTS)

dcnf_autarky_debug : $(CXXOBJECTS_debug)
	$(CXX) $(Debug_options) -o $@ $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(CXXOBJECTS)

main.o: main.cpp defs.h util.h constraints.o  all-solutions.o  parse-qdimacs.o translations.o Makefile
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) -c $<

constraints.o: constraints.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) -c $<

all-solutions.o: all-solutions.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) -c $<

parse-qdimacs.o: parse-qdimacs.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) -c $<

translations.o: translations.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Optimisation_options) $(CPPFLAGS) $(CXXFLAGS) -c $<

main_debug.o: main.cpp defs.h util.h constraints_debug.o  all-solutions_debug.o  parse-qdimacs_debug.o Makefile
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

constraints_debug.o: constraints.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

all-solutions_debug.o: all-solutions.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

parse-qdimacs_debug.o: parse-qdimacs.cpp defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)
	$(RM) count *.o *~

run:
	./$(PROGRAM)
