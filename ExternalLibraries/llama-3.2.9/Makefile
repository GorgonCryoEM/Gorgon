TAR = tar
TARFLAGS = zcf

RM = rm -f
MV = mv
CP = cp
AR = ar
ARFLAGS = rs

CXX = g++ 
CXXFLAGS = -g -DEBUG -I./include -Wall
CC = gcc
CFLAGS = -g -DEBUG -O2 -I./include -Wall
FC = g77
FFLAGS = -O2

LOGO = llama2.gif
KDOC = kdoc
KDOCFLAGS = -f html -i --strip-h-path -d ../doc -n Llama -u		\
"http://www.tiem.utk.edu/~king/pub/llama/doc/" --html-logo "./llama2.gif"

DOCXX = doc++
DOCXXFLAGS = -d doc
DOCIFY = docify

LATEX = latex
PDFLATEX = pdflatex

LDLIBS = -L$(DESTDIR)/lib -llama -L/usr/local/lib -llapack -lcblas	\
-lf77blas -latlas -lrfftw -lfftw -lg2c -lm

SPEC = llama.spec

TEXT = README GPL INSTALL DISCLAIMER VERSION NEWS TODO Makefile	\
AUTHORS COPYING ChangeLog

AUX = configure.in Makefile.in install-sh ltmain.sh aclocal.m4	\
config.sub config.guess

HEADERS = include/llama.h include/Decls.h include/General.h		\
include/Exception.h include/AbstMatrix.h include/TinyVector.h		\
include/Matrix.h include/SMClosure.h include/MMClosure.h		\
include/GenMatrix.h include/RealMatrix.h include/ComplexMatrix.h	\
include/Transpose.h include/Range.h include/Submatrix.h			\
include/Diagonal.h include/FAClosure.h include/Random.h			\
include/EigenDecomposition.h include/LUDecomposition.h			\
include/SVDecomposition.h include/QRDecomposition.h			\
include/CholeskyDecomposition.h include/Statistics.h			\
include/AbstDiffeomorphism.h include/Diffeomorphism.h			\
include/AbstMapping.h include/Mapping.h include/AbstVectorfield.h	\
include/Vectorfield.h include/Odessa.h include/Minimization.h		\
include/Fourier.h include/f77fun.h

CSRC = src/rand.c odessa/d1mach.c odessa/pow_dd.c odessa/xerr.c

FSRC = odessa/addx.f odessa/cfode.f odessa/daxpy.f odessa/ddot.f	\
odessa/dgbfa.f odessa/dgbsl.f odessa/dgefa.f odessa/dgesl.f		\
odessa/dscal.f odessa/ewset.f odessa/idamax.f odessa/intdy.f		\
odessa/odessa.f odessa/prepdf.f odessa/prepj.f odessa/rscom.f		\
odessa/solsy.f odessa/sprime.f odessa/stesa.f odessa/stode.f		\
odessa/svcom.f odessa/vnorm.f odessa/xerr.f odessa/xsetf.f		\
odessa/xsetun.f

CXXSRC = src/EigenDecomposition.cc src/LUDecomposition.cc	\
src/SVDecomposition.cc src/QRDecomposition.cc			\
src/CholeskyDecomposition.cc src/Odessa.cc src/Minimization.cc

SRC = $(CSRC) $(CXXSRC) $(FSRC)

LIB = lib/liblama.a

default: $(LIB)

docs: $(HEADERS) $(LOGO)
	cd include; $(KDOC) $(KDOCFLAGS) $(notdir $(wildcard $(HEADERS)))
	$(CP) $(LOGO) doc

dist: $(TEXT) $(SRC) $(HEADERS) $(SPEC) src/machar.c $(AUX)
	-mkdir llama-$(shell cat VERSION)
	-mkdir llama-$(shell cat VERSION)/src
	-mkdir llama-$(shell cat VERSION)/include
	-mkdir llama-$(shell cat VERSION)/doc
	-mkdir llama-$(shell cat VERSION)/lib
	$(CP) $(TEXT) $(SPEC) $(AUX) llama-$(shell cat VERSION)
	$(CP) include/ChangeLog llama-$(shell cat VERSION)/include
	$(CP) src/ChangeLog llama-$(shell cat VERSION)/src
	$(CP) $(SRC) src/machar.c llama-$(shell cat VERSION)/src
	$(CP) $(HEADERS) llama-$(shell cat VERSION)/include
	$(CP) doc/* llama-$(shell cat VERSION)/doc
	cd llama-$(shell cat VERSION); autoconf
	$(TAR) $(TARFLAGS) llama-$(shell cat VERSION).tar.gz llama-$(shell cat VERSION)
	$(RM) -r llama-$(shell cat VERSION)

sign: dist
	gpg -abuBA039220 llama-$(shell cat VERSION).tar.gz 

include/machinu.h: src/machar.c
	$(CC) -O0 $< -lm -o machar
	./machar > $@
	$(RM) machar

$(LIB): $(LIB)($(addsuffix .o, $(basename $(SRC)))) 

$(LIB)(%.o): %.cc $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $*.cc -o $*.o
	$(AR) $(ARFLAGS) $(LIB) $*.o
	$(RM) $*.o

$(LIB)(%.o): %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(AR) $(ARFLAGS) $(LIB) $*.o
	$(RM) $*.o

%: %.c
	$(CC) $(CFLAGS) $*.c $(LDLIBS) -o $*

%: %.cc
	$(CXX) $(CFLAGS) $*.cc $(LDLIBS) -o $*

%: %.f
	$(FC) $(FFLAGS) $*.c $(LDLIBS) -o $*

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $*.cc -o $*.o

%.o: %.f
	$(FC) -c $(FFLAGS) $*.f -o $*.o

clean:
	 $(RM) $(LIB) include/machinu.h *.o

