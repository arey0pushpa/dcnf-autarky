# Ankit Shukla, 22.June.2018 (Swansea)
# Copyright 2018 Ankit Shukla

PROGRAM = dcnf_autarky

CXXSOURCES = main.cpp constraints.cpp preprocess.cpp parse-qdimacs.cpp 
CXXOBJECTS = $(CXXSOURCES:.cpp=.o) 
CXX = g++

Standard_options = --std=c++11 -pedantic
Warning_options = -Wall
Optimisation_options = -Ofast -DNDEBUG
debug_options = -g


all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS)

main.o: main.cpp defs.h util.h constraints.o  preprocess.o  parse-qdimacs.o Makefile
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) -c -o main.o main.cpp

constraints.o: defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c -o constraints.o constraints.cpp

preprocess.o: defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c -o preprocess.o preprocess.cpp

parse-qdimacs.o: defs.h util.h
	$(CXX) $(Standard_options) $(Warning_options) $(Debug_options) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c -o parse-qdimacs.o parse-qdimacs.cpp

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)
	$(RM) count *.o *~

run:
	./$(PROGRAM)
