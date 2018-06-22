# Ankit Shukla, 22.June.2018 (Swansea)
# Copyright 2018 Ankit Shukla

git = git -c user.name="Auto" -c user.email="auto@auto.com" 

PROGRAM = dcnf_autarky

CXXSOURCES = main.cpp constraints.cpp preprocess.cpp parse-qdimacs.cpp # list of source files
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)  # expands to list of object files
CXX = g++
CXXFLAGS  = -g -Wall

all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) 
	
main.o: defs.o constraints.o  preprocess.o  parse-qdimacs.o 
	$(CXX) $(CXXFLAGS) -c -o main.o main.cpp

defs.o: util.h
	$(CXX) $(CXXFLAGS) -c -o defs.o defs.h

util.o: 
	$(CXX) $(CXXFLAGS) -c -o util.o util.h

constraints.o: defs.h 
	$(CXX) $(CXXFLAGS) -c -o constraints.o constraints.cpp

preprocess.o: defs.h 
	$(CXX) $(CXXFLAGS) -c -o preprocess.o preprocess.cpp

parse-qdimacs.o: defs.h 
	$(CXX) $(CXXFLAGS) -c -o parse-qdimacs.o parse-qdimacs.cpp

clean: 
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)
	$(RM) count *.o *~
  
run:
	./$(PROGRAM)
