# CXX Make variable for compiler
CXX=g++
# Make variable for compiler options
#	-std=c++11  C/C++ variant to use, e.g. C++ 2011
#	-g          include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -g

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
pagingwithtlb : tracereader.o pageTable.o output.o main.o TLB.o
	$(CXX) $(CXXFLAGS) -o pagingwithtlb $^

tracereader.o : tracereader.h tracereader.cpp

pageTable.o : pageTable.h pageTable.cpp

output.o : output.h output.cpp

TLB.o : TLB.h TLB.cpp

main.o : tracereader.h tracereader.cpp pageTable.h pageTable.cpp output.h output.cpp TLB.h TLB.cpp

clean :
	rm *.o

