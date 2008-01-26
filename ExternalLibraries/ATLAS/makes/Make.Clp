include Make.inc

sobj = clapack_sgesv.o clapack_sgetrf.o clapack_sgetri.o clapack_sgetrs.o \
       clapack_slauum.o clapack_sposv.o clapack_spotrf.o clapack_spotri.o \
       clapack_spotrs.o clapack_strtri.o

dobj = clapack_dgesv.o clapack_dgetrf.o clapack_dgetri.o clapack_dgetrs.o \
       clapack_dlauum.o clapack_dposv.o clapack_dpotrf.o clapack_dpotri.o \
       clapack_dpotrs.o clapack_dtrtri.o

cobj = clapack_cgesv.o clapack_cgetrf.o clapack_cgetri.o clapack_cgetrs.o \
       clapack_clauum.o clapack_cposv.o clapack_cpotrf.o clapack_cpotri.o \
       clapack_cpotrs.o clapack_ctrtri.o

zobj = clapack_zgesv.o clapack_zgetrf.o clapack_zgetri.o clapack_zgetrs.o \
       clapack_zlauum.o clapack_zposv.o clapack_zpotrf.o clapack_zpotri.o \
       clapack_zpotrs.o clapack_ztrtri.o

all : lib
lib : dlib slib zlib clib
clean : dclean sclean zclean cclean

sclean :
	rm -f $(sobj) slib.grd
slib :
	$(PMAKE) slib.grd
slib.grd : $(sobj)
	$(ARCHIVER) $(ARFLAGS) $(LAPACKlib) $(sobj)
	$(RANLIB) $(LAPACKlib)
	touch slib.grd

dclean :
	rm -f $(dobj) dlib.grd
dlib :
	$(PMAKE) dlib.grd
dlib.grd : $(dobj)
	$(ARCHIVER) $(ARFLAGS) $(LAPACKlib) $(dobj)
	$(RANLIB) $(LAPACKlib)
	touch dlib.grd

cclean :
	rm -f $(cobj) clib.grd
clib :
	$(PMAKE) clib.grd
clib.grd : $(cobj)
	$(ARCHIVER) $(ARFLAGS) $(LAPACKlib) $(cobj)
	$(RANLIB) $(LAPACKlib)
	touch clib.grd

zclean :
	rm -f $(zobj) zlib.grd
zlib :
	$(PMAKE) zlib.grd
zlib.grd : $(zobj)
	$(ARCHIVER) $(ARFLAGS) $(LAPACKlib) $(zobj)
	$(RANLIB) $(LAPACKlib)
	touch zlib.grd


clapack_sgetri.o : ../clapack_sgetri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_sgetri.c
clapack_spotri.o : ../clapack_spotri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_spotri.c
clapack_slauum.o : ../clapack_slauum.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_slauum.c
clapack_strtri.o : ../clapack_strtri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_strtri.c
clapack_sgetrs.o : ../clapack_sgetrs.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_sgetrs.c
clapack_sgetrf.o : ../clapack_sgetrf.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_sgetrf.c
clapack_sgesv.o : ../clapack_sgesv.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_sgesv.c
clapack_spotrs.o : ../clapack_spotrs.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_spotrs.c
clapack_spotrf.o : ../clapack_spotrf.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_spotrf.c
clapack_sposv.o : ../clapack_sposv.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_sposv.c

clapack_dgetri.o : ../clapack_dgetri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dgetri.c
clapack_dpotri.o : ../clapack_dpotri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dpotri.c
clapack_dlauum.o : ../clapack_dlauum.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dlauum.c
clapack_dtrtri.o : ../clapack_dtrtri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dtrtri.c
clapack_dgetrs.o : ../clapack_dgetrs.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dgetrs.c
clapack_dgetrf.o : ../clapack_dgetrf.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dgetrf.c
clapack_dgesv.o : ../clapack_dgesv.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dgesv.c
clapack_dpotrs.o : ../clapack_dpotrs.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dpotrs.c
clapack_dpotrf.o : ../clapack_dpotrf.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dpotrf.c
clapack_dposv.o : ../clapack_dposv.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_dposv.c

clapack_cgetri.o : ../clapack_cgetri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_cgetri.c
clapack_cpotri.o : ../clapack_cpotri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_cpotri.c
clapack_clauum.o : ../clapack_clauum.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_clauum.c
clapack_ctrtri.o : ../clapack_ctrtri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_ctrtri.c
clapack_cgetrs.o : ../clapack_cgetrs.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_cgetrs.c
clapack_cgetrf.o : ../clapack_cgetrf.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_cgetrf.c
clapack_cgesv.o : ../clapack_cgesv.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_cgesv.c
clapack_cpotrs.o : ../clapack_cpotrs.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_cpotrs.c
clapack_cpotrf.o : ../clapack_cpotrf.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_cpotrf.c
clapack_cposv.o : ../clapack_cposv.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_cposv.c

clapack_zgetri.o : ../clapack_zgetri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zgetri.c
clapack_zpotri.o : ../clapack_zpotri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zpotri.c
clapack_zlauum.o : ../clapack_zlauum.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zlauum.c
clapack_ztrtri.o : ../clapack_ztrtri.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_ztrtri.c
clapack_zgetrs.o : ../clapack_zgetrs.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zgetrs.c
clapack_zgetrf.o : ../clapack_zgetrf.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zgetrf.c
clapack_zgesv.o : ../clapack_zgesv.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zgesv.c
clapack_zpotrs.o : ../clapack_zpotrs.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zpotrs.c
clapack_zpotrf.o : ../clapack_zpotrf.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zpotrf.c
clapack_zposv.o : ../clapack_zposv.c
	$(CC) $(CCFLAGS) -o $@ -c ../clapack_zposv.c

