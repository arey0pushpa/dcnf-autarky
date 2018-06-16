gt = git -c user.name="Auto" -c user.email="auto@auto.com" 

BUILDDIR = $(PWD)/build
SRCDIR = $(PWD)/src
LLVM_VERSION=4.0.0

#all : release

#.PHONY : release debug run clean patch test

release : $(BUILDDIR)/buildr/Makefile 
	 +make -C $(BUILDDIR)/buildr
	 cp -f $(BUILDDIR)/buildr/dcnf-autarky dcnf-autarky

#cc -o release $(SRCDIR)/main.o

debug : $(BUILDDIR)/buildd/Makefile 
	#$(SRCDIR)/main.o
	 +make -C $(BUILDDIR)/buildr
	 cp -f $(BUILDDIR)/buildr/dcnf-autarky dcnf-autarky

$(BUILDDIR)/buildr/Makefile: 
	  mkdir -p $(BUILDDIR)/buildr
		cd $(BUILDDIR)/buildr; cmake -DCMAKE_BUILD_TYPE=Release $(SRCDIR)

$(BUILDDIR)/buildd/Makefile: 
	  mkdir -p $(BUILDDIR)/buildr
		cd $(BUILDDIR)/buildr; cmake -DCMAKE_BUILD_TYPE=Release $(SRCDIR)

#$(SRCDIR)/main.o : $(SRCDIR)/main.c $(SRCDIR)/def.h
#		cc -c $(SRCDIR)/main.c

clean : 
	rm edit main.o
#---------------------------------------------------------------------------
# runtest:
# 	make -C examples/tester/

# test: release runtest
