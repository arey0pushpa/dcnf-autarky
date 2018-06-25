# Ankit Shukla, 22.June.2018 (Swansea)
# Copyright 2018 Ankit Shukla

PROGRAM = dcnf_autarky

CXXSOURCES = main.cpp constraints.cpp preprocess.cpp parse-qdimacs.cpp 
CXXOBJECTS = $(CXXSOURCES:.cpp=.o) 
CXX = g++
CXXFLAGS  = --std=c++11 -pedantic -g -Wall

all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) 
	
main.o: main.cpp defs.h util.h constraints.o  preprocess.o  parse-qdimacs.o Makefile
	$(CXX) $(CXXFLAGS) -c main.cpp 

constraints.o: constraints.cpp defs.h  
	$(CXX) $(CXXFLAGS) -c constraints.cpp

preprocess.o: preprocess.cpp defs.h 
	$(CXX) $(CXXFLAGS) -c preprocess.cpp

parse-qdimacs.o: parse-qdimacs.cpp defs.h 
	$(CXX) $(CXXFLAGS) -c parse-qdimacs.cpp

clean: 
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)
	$(RM) count *.o *~
  
run:
	./$(PROGRAM)
