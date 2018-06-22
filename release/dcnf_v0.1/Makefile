# Ankit Shukla, 22.June.2018 (Swansea)
# Copyright 2018 Ankit Shukla

PROGRAM = dcnf_autarky

CXXSOURCES = main.cpp constraints.cpp preprocess.cpp parse-qdimacs.cpp 
CXXOBJECTS = $(CXXSOURCES:.cpp=.o) 
CXX = g++
CXXFLAGS  = -g -Wall

all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) 
	
main.o: main.cpp defs.h util.h constraints.o  preprocess.o  parse-qdimacs.o Makefile
	$(CXX) $(CXXFLAGS) -c -o main.o main.cpp

constraints.o: defs.h util.h 
	$(CXX) $(CXXFLAGS) -c -o constraints.o constraints.cpp

preprocess.o: defs.h util.h 
	$(CXX) $(CXXFLAGS) -c -o preprocess.o preprocess.cpp

parse-qdimacs.o: defs.h util.h 
	$(CXX) $(CXXFLAGS) -c -o parse-qdimacs.o parse-qdimacs.cpp

clean: 
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)
	$(RM) count *.o *~
  
run:
	./$(PROGRAM)
