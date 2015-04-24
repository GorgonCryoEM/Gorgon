arch=default
include ../../Make.$(arch)
mach=default
dcc=default
mcc=gcc
cc=gcc

mmd = $(TOPdir)/CONFIG/ARCHS/$(mach)/$(mcc)/gemm
syd = $(TOPdir)/CONFIG/ARCHS/$(mach)/$(cc)/misc

tarfile : $(mach)
	- rm $(mach).tgz $(mach).tar
	tar cvf $(mach).tar $(mach)
	gzip --best $(mach).tar
	mv $(mach).tar.gz $(mach).tgz

$(mach)/$(dcc) :
	mkdir $(mach)/$(dcc)
	cd $(mach)/$(dcc) ; mkdir gemm misc
machsub:
	$(MAKE) $(mach)/$(mcc) dcc=$(mcc) mach=$(mach) arch=$(arch)
	$(MAKE) $(mach)/$(cc) dcc=$(cc) mach=$(mach) arch=$(arch)
$(mach) :
	mkdir $(mach)
	$(MAKE) machsub mcc=$(mcc) cc=$(cc) mach=$(mach) arch=$(arch)

xnegflt : negflt.c
	gcc -O -o $@ negflt.c
#xnegfloat : negfloat.c
#	gcc -O -o xnegfloat negfloat.c

ArchNew : $(mach) machsub xnegflt
	- rm -f $(syd)/*
	- rm -f $(mmd)/*
	- cp $(SYSdir)/res/?MULADD $(syd)/.
	./xnegflt $(syd)/?MULADD
	- cp $(TOPdir)/tune/blas/gemv/$(ARCH)/res/?MVRES $(syd)/.
	- cp $(TOPdir)/tune/blas/ger/$(ARCH)/res/?R1RES $(syd)/.
	./xnegflt $(syd)/?R1RES $(syd)/?MVRES
	- cp $(SYSdir)/res/L1CacheSize $(syd)/.
	- cp $(L1Tdir)/res/*_SUMM $(syd)/.
	- cp $(MMTdir)/res/?MMRES $(mmd)/.
	- cp $(MMTdir)/res/?gMMRES $(mmd)/.
	- cp $(MMTdir)/res/?Clean[M,N,K] $(mmd)/.
	- cp $(MMTdir)/res/?best[N,T][N,T]_* $(mmd)/.
	- cp $(MMTdir)/res/?uMMRES $(mmd)/.
	- cp $(MMTdir)/res/?uClean[M,N,K] $(mmd)/.
	- cp $(MMTdir)/res/?uClean[M,N,K]F $(mmd)/.
	./xnegflt $(mmd)/*
	- cp $(MMTdir)/res/?nreg $(mmd)/.
	- cp $(MMTdir)/res/?NB $(mmd)/.
	- cp $(MMTdir)/res/?NCNB $(mmd)/.
	- cp $(INCdir)/atlas_cacheedge.h $(mmd)/.
	- cp $(INCdir)/?Xover.h $(mmd)/.
	rm -f xnegflt
