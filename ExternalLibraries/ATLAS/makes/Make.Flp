include Make.inc

sobj = ATL_f77wrap_sgesv.o ATL_f77wrap_sgetnb.o ATL_f77wrap_sgetrf.o \
       ATL_f77wrap_sgetri.o ATL_f77wrap_sgetrs.o ATL_f77wrap_slauum.o \
       ATL_f77wrap_sposv.o ATL_f77wrap_spotrf.o ATL_f77wrap_spotrs.o \
       ATL_f77wrap_strtri.o ieeeck.o ilaenv.o sgesv.o sgetrf.o sgetri.o \
       sgetrs.o slauum.o sposv.o spotrf.o spotri.o spotrs.o strtri.o

dobj = ATL_f77wrap_dgesv.o ATL_f77wrap_dgetnb.o ATL_f77wrap_dgetrf.o \
       ATL_f77wrap_dgetri.o ATL_f77wrap_dgetrs.o ATL_f77wrap_dlauum.o \
       ATL_f77wrap_dposv.o ATL_f77wrap_dpotrf.o ATL_f77wrap_dpotrs.o \
       ATL_f77wrap_dtrtri.o dgesv.o dgetrf.o dgetri.o dgetrs.o dlauum.o \
       dposv.o dpotrf.o dpotri.o dpotrs.o dtrtri.o ieeeck.o ilaenv.o

cobj = ATL_f77wrap_cgesv.o ATL_f77wrap_cgetnb.o ATL_f77wrap_cgetrf.o \
       ATL_f77wrap_cgetri.o ATL_f77wrap_cgetrs.o ATL_f77wrap_clauum.o \
       ATL_f77wrap_cposv.o ATL_f77wrap_cpotrf.o ATL_f77wrap_cpotrs.o \
       ATL_f77wrap_ctrtri.o cgesv.o cgetrf.o cgetri.o cgetrs.o clauum.o \
       cposv.o cpotrf.o cpotri.o cpotrs.o ctrtri.o ieeeck.o ilaenv.o

zobj = ATL_f77wrap_zgesv.o ATL_f77wrap_zgetnb.o ATL_f77wrap_zgetrf.o \
       ATL_f77wrap_zgetri.o ATL_f77wrap_zgetrs.o ATL_f77wrap_zlauum.o \
       ATL_f77wrap_zposv.o ATL_f77wrap_zpotrf.o ATL_f77wrap_zpotrs.o \
       ATL_f77wrap_ztrtri.o ieeeck.o ilaenv.o zgesv.o zgetrf.o zgetri.o \
       zgetrs.o zlauum.o zposv.o zpotrf.o zpotri.o zpotrs.o ztrtri.o

all : lib
lib : dlib slib zlib clib
clean : dclean sclean zclean cclean

sclean :
	rm -f $(sobj) slib.grd
slib : slib.grd
slib.grd : $(sobj)
	$(ARCHIVER) $(ARFLAGS) $(LAPACKlib) $(sobj)
	$(RANLIB) $(LAPACKlib)
	touch slib.grd

dclean :
	rm -f $(dobj) dlib.grd
dlib : dlib.grd
dlib.grd : $(dobj)
	$(ARCHIVER) $(ARFLAGS) $(LAPACKlib) $(dobj)
	$(RANLIB) $(LAPACKlib)
	touch dlib.grd

cclean :
	rm -f $(cobj) clib.grd
clib : clib.grd
clib.grd : $(cobj)
	$(ARCHIVER) $(ARFLAGS) $(LAPACKlib) $(cobj)
	$(RANLIB) $(LAPACKlib)
	touch clib.grd

zclean :
	rm -f $(zobj) zlib.grd
zlib : zlib.grd
zlib.grd : $(zobj)
	$(ARCHIVER) $(ARFLAGS) $(LAPACKlib) $(zobj)
	$(RANLIB) $(LAPACKlib)
	touch zlib.grd


spotri.o : ../spotri.f
	$(F77) -o $@ -c $(F77FLAGS) ../spotri.f
sgetri.o : ../sgetri.f
	$(F77) -o $@ -c $(F77FLAGS) ../sgetri.f
ATL_f77wrap_sgetri.o : ../f77wrap/ATL_f77wrap_getri.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_getri.c
slauum.o : ../slauum.f
	$(F77) -o $@ -c $(F77FLAGS) ../slauum.f
ATL_f77wrap_slauum.o : ../f77wrap/ATL_f77wrap_lauum.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_lauum.c
strtri.o : ../strtri.f
	$(F77) -o $@ -c $(F77FLAGS) ../strtri.f
ATL_f77wrap_strtri.o : ../f77wrap/ATL_f77wrap_trtri.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_trtri.c
sgetrs.o : ../sgetrs.f
	$(F77) -o $@ -c $(F77FLAGS) ../sgetrs.f
ATL_f77wrap_sgetrs.o : ../f77wrap/ATL_f77wrap_getrs.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_getrs.c
sgetrf.o : ../sgetrf.f
	$(F77) -o $@ -c $(F77FLAGS) ../sgetrf.f
ATL_f77wrap_sgetrf.o : ../f77wrap/ATL_f77wrap_getrf.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_getrf.c
sgesv.o : ../sgesv.f
	$(F77) -o $@ -c $(F77FLAGS) ../sgesv.f
ATL_f77wrap_sgesv.o : ../f77wrap/ATL_f77wrap_gesv.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_gesv.c
spotrs.o : ../spotrs.f
	$(F77) -o $@ -c $(F77FLAGS) ../spotrs.f
ATL_f77wrap_spotrs.o : ../f77wrap/ATL_f77wrap_potrs.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_potrs.c
spotrf.o : ../spotrf.f
	$(F77) -o $@ -c $(F77FLAGS) ../spotrf.f
ATL_f77wrap_spotrf.o : ../f77wrap/ATL_f77wrap_potrf.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_potrf.c
sposv.o : ../sposv.f
	$(F77) -o $@ -c $(F77FLAGS) ../sposv.f
ATL_f77wrap_sposv.o : ../f77wrap/ATL_f77wrap_posv.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_posv.c
ATL_f77wrap_sgetnb.o : ../f77wrap/ATL_f77wrap_getnb.c
	$(CC) $(CCFLAGS) -o $@ -c -DSREAL ../f77wrap/ATL_f77wrap_getnb.c

dpotri.o : ../dpotri.f
	$(F77) -o $@ -c $(F77FLAGS) ../dpotri.f
dgetri.o : ../dgetri.f
	$(F77) -o $@ -c $(F77FLAGS) ../dgetri.f
ATL_f77wrap_dgetri.o : ../f77wrap/ATL_f77wrap_getri.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_getri.c
dlauum.o : ../dlauum.f
	$(F77) -o $@ -c $(F77FLAGS) ../dlauum.f
ATL_f77wrap_dlauum.o : ../f77wrap/ATL_f77wrap_lauum.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_lauum.c
dtrtri.o : ../dtrtri.f
	$(F77) -o $@ -c $(F77FLAGS) ../dtrtri.f
ATL_f77wrap_dtrtri.o : ../f77wrap/ATL_f77wrap_trtri.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_trtri.c
dgetrs.o : ../dgetrs.f
	$(F77) -o $@ -c $(F77FLAGS) ../dgetrs.f
ATL_f77wrap_dgetrs.o : ../f77wrap/ATL_f77wrap_getrs.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_getrs.c
dgetrf.o : ../dgetrf.f
	$(F77) -o $@ -c $(F77FLAGS) ../dgetrf.f
ATL_f77wrap_dgetrf.o : ../f77wrap/ATL_f77wrap_getrf.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_getrf.c
dgesv.o : ../dgesv.f
	$(F77) -o $@ -c $(F77FLAGS) ../dgesv.f
ATL_f77wrap_dgesv.o : ../f77wrap/ATL_f77wrap_gesv.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_gesv.c
dpotrs.o : ../dpotrs.f
	$(F77) -o $@ -c $(F77FLAGS) ../dpotrs.f
ATL_f77wrap_dpotrs.o : ../f77wrap/ATL_f77wrap_potrs.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_potrs.c
dpotrf.o : ../dpotrf.f
	$(F77) -o $@ -c $(F77FLAGS) ../dpotrf.f
ATL_f77wrap_dpotrf.o : ../f77wrap/ATL_f77wrap_potrf.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_potrf.c
dposv.o : ../dposv.f
	$(F77) -o $@ -c $(F77FLAGS) ../dposv.f
ATL_f77wrap_dposv.o : ../f77wrap/ATL_f77wrap_posv.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_posv.c
ATL_f77wrap_dgetnb.o : ../f77wrap/ATL_f77wrap_getnb.c
	$(CC) $(CCFLAGS) -o $@ -c -DDREAL ../f77wrap/ATL_f77wrap_getnb.c

cpotri.o : ../cpotri.f
	$(F77) -o $@ -c $(F77FLAGS) ../cpotri.f
cgetri.o : ../cgetri.f
	$(F77) -o $@ -c $(F77FLAGS) ../cgetri.f
ATL_f77wrap_cgetri.o : ../f77wrap/ATL_f77wrap_getri.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_getri.c
clauum.o : ../clauum.f
	$(F77) -o $@ -c $(F77FLAGS) ../clauum.f
ATL_f77wrap_clauum.o : ../f77wrap/ATL_f77wrap_lauum.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_lauum.c
ctrtri.o : ../ctrtri.f
	$(F77) -o $@ -c $(F77FLAGS) ../ctrtri.f
ATL_f77wrap_ctrtri.o : ../f77wrap/ATL_f77wrap_trtri.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_trtri.c
cgetrs.o : ../cgetrs.f
	$(F77) -o $@ -c $(F77FLAGS) ../cgetrs.f
ATL_f77wrap_cgetrs.o : ../f77wrap/ATL_f77wrap_getrs.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_getrs.c
cgetrf.o : ../cgetrf.f
	$(F77) -o $@ -c $(F77FLAGS) ../cgetrf.f
ATL_f77wrap_cgetrf.o : ../f77wrap/ATL_f77wrap_getrf.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_getrf.c
cgesv.o : ../cgesv.f
	$(F77) -o $@ -c $(F77FLAGS) ../cgesv.f
ATL_f77wrap_cgesv.o : ../f77wrap/ATL_f77wrap_gesv.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_gesv.c
cpotrs.o : ../cpotrs.f
	$(F77) -o $@ -c $(F77FLAGS) ../cpotrs.f
ATL_f77wrap_cpotrs.o : ../f77wrap/ATL_f77wrap_potrs.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_potrs.c
cpotrf.o : ../cpotrf.f
	$(F77) -o $@ -c $(F77FLAGS) ../cpotrf.f
ATL_f77wrap_cpotrf.o : ../f77wrap/ATL_f77wrap_potrf.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_potrf.c
cposv.o : ../cposv.f
	$(F77) -o $@ -c $(F77FLAGS) ../cposv.f
ATL_f77wrap_cposv.o : ../f77wrap/ATL_f77wrap_posv.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_posv.c
ATL_f77wrap_cgetnb.o : ../f77wrap/ATL_f77wrap_getnb.c
	$(CC) $(CCFLAGS) -o $@ -c -DSCPLX ../f77wrap/ATL_f77wrap_getnb.c

zpotri.o : ../zpotri.f
	$(F77) -o $@ -c $(F77FLAGS) ../zpotri.f
zgetri.o : ../zgetri.f
	$(F77) -o $@ -c $(F77FLAGS) ../zgetri.f
ATL_f77wrap_zgetri.o : ../f77wrap/ATL_f77wrap_getri.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_getri.c
zlauum.o : ../zlauum.f
	$(F77) -o $@ -c $(F77FLAGS) ../zlauum.f
ATL_f77wrap_zlauum.o : ../f77wrap/ATL_f77wrap_lauum.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_lauum.c
ztrtri.o : ../ztrtri.f
	$(F77) -o $@ -c $(F77FLAGS) ../ztrtri.f
ATL_f77wrap_ztrtri.o : ../f77wrap/ATL_f77wrap_trtri.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_trtri.c
zgetrs.o : ../zgetrs.f
	$(F77) -o $@ -c $(F77FLAGS) ../zgetrs.f
ATL_f77wrap_zgetrs.o : ../f77wrap/ATL_f77wrap_getrs.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_getrs.c
zgetrf.o : ../zgetrf.f
	$(F77) -o $@ -c $(F77FLAGS) ../zgetrf.f
ATL_f77wrap_zgetrf.o : ../f77wrap/ATL_f77wrap_getrf.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_getrf.c
zgesv.o : ../zgesv.f
	$(F77) -o $@ -c $(F77FLAGS) ../zgesv.f
ATL_f77wrap_zgesv.o : ../f77wrap/ATL_f77wrap_gesv.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_gesv.c
zpotrs.o : ../zpotrs.f
	$(F77) -o $@ -c $(F77FLAGS) ../zpotrs.f
ATL_f77wrap_zpotrs.o : ../f77wrap/ATL_f77wrap_potrs.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_potrs.c
zpotrf.o : ../zpotrf.f
	$(F77) -o $@ -c $(F77FLAGS) ../zpotrf.f
ATL_f77wrap_zpotrf.o : ../f77wrap/ATL_f77wrap_potrf.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_potrf.c
zposv.o : ../zposv.f
	$(F77) -o $@ -c $(F77FLAGS) ../zposv.f
ATL_f77wrap_zposv.o : ../f77wrap/ATL_f77wrap_posv.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_posv.c
ATL_f77wrap_zgetnb.o : ../f77wrap/ATL_f77wrap_getnb.c
	$(CC) $(CCFLAGS) -o $@ -c -DDCPLX ../f77wrap/ATL_f77wrap_getnb.c


ieeeck.o : ../ieeeck.f
	$(F77) -o $@ -c $(F77FLAGS) ../ieeeck.f
ilaenv.o : ../ilaenv.f
	$(F77) -o $@ -c $(F77FLAGS) ../ilaenv.f
