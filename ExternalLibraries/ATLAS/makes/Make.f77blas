########################################################################
#  
#  -- Automatically Tuned Linear Algebra Software (ATLAS)
#     University of Tennessee - Innovative Computing Laboratory
#     Knoxville TN, 37996-1301 USA.
#     (C) Copyright 2000 All Rights Reserved
#
#  -- Suggestions,  comments,  bugs reports should be sent to the follo-
#     wing e-mail address: atlas@cs.utk.edu
#  
########################################################################
#
#  -- Copyright notice and Licensing terms:
#
#  Redistribution  and  use in  source and binary forms, with or without
#  modification, are  permitted provided  that the following  conditions
#  are met:
#
#  1. Redistributions  of  source  code  must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce  the above copyright
#     notice,  this list of conditions, and the  following disclaimer in
#     the documentation and/or other materials provided with the distri-
#     bution.
#  3. The name of the University,  the ATLAS group,  or the names of its
#     contributors  may not be used to endorse or promote products deri-
#     ved from this software without specific written permission.
#
#  -- Disclaimer:
#
#  THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
#  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPE-
#  CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
#  TO,  PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
#  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEO-
#  RY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (IN-
#  CLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#  
########################################################################
#
include Make.inc
#
########################################################################
#
INCdep = \
   $(INCdir)/../atlas_enum.h        $(INCdir)/../atlas_f77.h       \
   $(INCdir)/../atlas_refalias1.h   $(INCdir)/../atlas_refalias2.h \
   $(INCdir)/../atlas_refalias3.h   $(INCdir)/../atlas_ptalias1.h  \
   $(INCdir)/../atlas_ptalias2.h    $(INCdir)/../atlas_ptalias3.h  \
   $(INCdir)/../atlas_level1.h      $(INCdir)/../atlas_level2.h    \
   $(INCdir)/../atlas_level3.h
#
## Object files ########################################################
# 
aobj     = \
   dcabs1.o lsame.o xerbla.o
#
slvl1obj = \
   srotg.o srotmg.o sscal.o snrm2.o sasum.o isamax.o saxpy.o scopy.o sswap.o \
   srot.o srotm.o sdot.o dsdot.o sdsdot.o
#
dlvl1obj = \
   drotg.o drotmg.o dscal.o dnrm2.o dasum.o idamax.o daxpy.o dcopy.o dswap.o \
   drot.o drotm.o ddot.o
#
clvl1obj = \
   crotg.o cscal.o csscal.o scnrm2.o scasum.o icamax.o caxpy.o ccopy.o \
   cswap.o csrot.o cdotc.o cdotu.o
#
zlvl1obj = \
   zrotg.o zscal.o zdscal.o dznrm2.o dzasum.o izamax.o zaxpy.o zcopy.o \
   zswap.o zdrot.o zdotc.o zdotu.o
#
slvl2obj = \
   sgbmv.o sgemv.o sger.o ssbmv.o sspmv.o sspr.o sspr2.o ssymv.o ssyr.o \
   ssyr2.o stbmv.o stbsv.o stpmv.o stpsv.o strmv.o strsv.o
#
dlvl2obj = \
   dgbmv.o dgemv.o dger.o dsbmv.o dspmv.o dspr.o dspr2.o dsymv.o dsyr.o \
   dsyr2.o dtbmv.o dtbsv.o dtpmv.o dtpsv.o dtrmv.o dtrsv.o
#
clvl2obj = \
   cgbmv.o cgemv.o cgerc.o cgeru.o chbmv.o chemv.o cher.o cher2.o chpmv.o \
   chpr.o chpr2.o ctbmv.o ctbsv.o ctpmv.o ctpsv.o ctrmv.o ctrsv.o
#
zlvl2obj = \
   zgbmv.o zgemv.o zgerc.o zgeru.o zhbmv.o zhemv.o zher.o zher2.o zhpmv.o \
   zhpr.o zhpr2.o ztbmv.o ztbsv.o ztpmv.o ztpsv.o ztrmv.o ztrsv.o
#
slvl3obj = \
   sgemm.o ssymm.o ssyr2k.o ssyrk.o strmm.o strsm.o
#
dlvl3obj = \
   dgemm.o dsymm.o dsyr2k.o dsyrk.o dtrmm.o dtrsm.o
#
clvl3obj = \
   cgemm.o chemm.o cher2k.o cherk.o csymm.o csyr2k.o csyrk.o ctrmm.o ctrsm.o
#
zlvl3obj = \
   zgemm.o zhemm.o zher2k.o zherk.o zsymm.o zsyr2k.o zsyrk.o ztrmm.o ztrsm.o
#
ATL_slvl1obj = \
   ATL_F77wrap_srotg.o ATL_F77wrap_srotmg.o ATL_F77wrap_sscal.o \
   ATL_F77wrap_snrm2.o ATL_F77wrap_sasum.o ATL_F77wrap_isamax.o \
   ATL_F77wrap_saxpy.o ATL_F77wrap_scopy.o ATL_F77wrap_sswap.o \
   ATL_F77wrap_srot.o ATL_F77wrap_srotm.o ATL_F77wrap_sdot.o \
   ATL_F77wrap_dsdot.o ATL_F77wrap_sdsdot.o fatlas_saxpby.o fatlas_sset.o
#
ATL_dlvl1obj = \
   ATL_F77wrap_drotg.o ATL_F77wrap_drotmg.o ATL_F77wrap_dscal.o \
   ATL_F77wrap_dnrm2.o ATL_F77wrap_dasum.o ATL_F77wrap_idamax.o \
   ATL_F77wrap_daxpy.o ATL_F77wrap_dcopy.o ATL_F77wrap_dswap.o \
   ATL_F77wrap_drot.o ATL_F77wrap_drotm.o ATL_F77wrap_ddot.o fatlas_daxpby.o \
   fatlas_dset.o
#
ATL_clvl1obj = \
   ATL_F77wrap_crotg.o ATL_F77wrap_cscal.o ATL_F77wrap_csscal.o \
   ATL_F77wrap_scnrm2.o ATL_F77wrap_scasum.o ATL_F77wrap_icamax.o \
   ATL_F77wrap_caxpy.o ATL_F77wrap_ccopy.o ATL_F77wrap_cswap.o \
   ATL_F77wrap_csrot.o ATL_F77wrap_cdotc.o ATL_F77wrap_cdotu.o \
   fatlas_caxpby.o fatlas_cset.o
#
ATL_zlvl1obj = \
   ATL_F77wrap_zrotg.o ATL_F77wrap_zscal.o ATL_F77wrap_zdscal.o \
   ATL_F77wrap_dznrm2.o ATL_F77wrap_dzasum.o ATL_F77wrap_izamax.o \
   ATL_F77wrap_zaxpy.o ATL_F77wrap_zcopy.o ATL_F77wrap_zswap.o \
   ATL_F77wrap_zdrot.o ATL_F77wrap_zdotc.o ATL_F77wrap_zdotu.o \
   fatlas_zaxpby.o fatlas_zset.o
#
ATL_sptlvl1obj = \
   ATL_F77wrap_sptrotg.o ATL_F77wrap_sptrotmg.o ATL_F77wrap_sptscal.o \
   ATL_F77wrap_sptnrm2.o ATL_F77wrap_sptasum.o ATL_F77wrap_isptamax.o \
   ATL_F77wrap_sptaxpy.o ATL_F77wrap_sptcopy.o ATL_F77wrap_sptswap.o \
   ATL_F77wrap_sptrot.o ATL_F77wrap_sptrotm.o ATL_F77wrap_sptdot.o \
   ATL_F77wrap_dsptdot.o ATL_F77wrap_sdsptdot.o fatlas_sptaxpby.o \
   fatlas_sptset.o
#
ATL_dptlvl1obj = \
   ATL_F77wrap_dptrotg.o ATL_F77wrap_dptrotmg.o ATL_F77wrap_dptscal.o \
   ATL_F77wrap_dptnrm2.o ATL_F77wrap_dptasum.o ATL_F77wrap_idptamax.o \
   ATL_F77wrap_dptaxpy.o ATL_F77wrap_dptcopy.o ATL_F77wrap_dptswap.o \
   ATL_F77wrap_dptrot.o ATL_F77wrap_dptrotm.o ATL_F77wrap_dptdot.o \
   fatlas_dptaxpby.o fatlas_dptset.o
#
ATL_cptlvl1obj = \
   ATL_F77wrap_cptrotg.o ATL_F77wrap_cptscal.o ATL_F77wrap_csptscal.o \
   ATL_F77wrap_scptnrm2.o ATL_F77wrap_scptasum.o ATL_F77wrap_icptamax.o \
   ATL_F77wrap_cptaxpy.o ATL_F77wrap_cptcopy.o ATL_F77wrap_cptswap.o \
   ATL_F77wrap_csptrot.o ATL_F77wrap_cptdotc.o ATL_F77wrap_cptdotu.o \
   fatlas_cptaxpby.o fatlas_cptset.o
#
ATL_zptlvl1obj = \
   ATL_F77wrap_zptrotg.o ATL_F77wrap_zptscal.o ATL_F77wrap_zdptscal.o \
   ATL_F77wrap_dzptnrm2.o ATL_F77wrap_dzptasum.o ATL_F77wrap_izptamax.o \
   ATL_F77wrap_zptaxpy.o ATL_F77wrap_zptcopy.o ATL_F77wrap_zptswap.o \
   ATL_F77wrap_zdptrot.o ATL_F77wrap_zptdotc.o ATL_F77wrap_zptdotu.o \
   fatlas_zptaxpby.o fatlas_zptset.o
#
ATL_slvl2obj = \
   ATL_F77wrap_sgbmv.o ATL_F77wrap_sgemv.o ATL_F77wrap_sger.o \
   ATL_F77wrap_ssbmv.o ATL_F77wrap_sspmv.o ATL_F77wrap_sspr.o \
   ATL_F77wrap_sspr2.o ATL_F77wrap_ssymv.o ATL_F77wrap_ssyr.o \
   ATL_F77wrap_ssyr2.o ATL_F77wrap_stbmv.o ATL_F77wrap_stbsv.o \
   ATL_F77wrap_stpmv.o ATL_F77wrap_stpsv.o ATL_F77wrap_strmv.o \
   ATL_F77wrap_strsv.o
#
ATL_dlvl2obj = \
   ATL_F77wrap_dgbmv.o ATL_F77wrap_dgemv.o ATL_F77wrap_dger.o \
   ATL_F77wrap_dsbmv.o ATL_F77wrap_dspmv.o ATL_F77wrap_dspr.o \
   ATL_F77wrap_dspr2.o ATL_F77wrap_dsymv.o ATL_F77wrap_dsyr.o \
   ATL_F77wrap_dsyr2.o ATL_F77wrap_dtbmv.o ATL_F77wrap_dtbsv.o \
   ATL_F77wrap_dtpmv.o ATL_F77wrap_dtpsv.o ATL_F77wrap_dtrmv.o \
   ATL_F77wrap_dtrsv.o
#
ATL_clvl2obj = \
   ATL_F77wrap_cgbmv.o ATL_F77wrap_cgemv.o ATL_F77wrap_cgerc.o \
   ATL_F77wrap_cgeru.o ATL_F77wrap_chbmv.o ATL_F77wrap_chemv.o \
   ATL_F77wrap_cher.o ATL_F77wrap_cher2.o ATL_F77wrap_chpmv.o \
   ATL_F77wrap_chpr.o ATL_F77wrap_chpr2.o ATL_F77wrap_ctbmv.o \
   ATL_F77wrap_ctbsv.o ATL_F77wrap_ctpmv.o ATL_F77wrap_ctpsv.o \
   ATL_F77wrap_ctrmv.o ATL_F77wrap_ctrsv.o
#
ATL_zlvl2obj = \
   ATL_F77wrap_zgbmv.o ATL_F77wrap_zgemv.o ATL_F77wrap_zgerc.o \
   ATL_F77wrap_zgeru.o ATL_F77wrap_zhbmv.o ATL_F77wrap_zhemv.o \
   ATL_F77wrap_zher.o ATL_F77wrap_zher2.o ATL_F77wrap_zhpmv.o \
   ATL_F77wrap_zhpr.o ATL_F77wrap_zhpr2.o ATL_F77wrap_ztbmv.o \
   ATL_F77wrap_ztbsv.o ATL_F77wrap_ztpmv.o ATL_F77wrap_ztpsv.o \
   ATL_F77wrap_ztrmv.o ATL_F77wrap_ztrsv.o
#
ATL_sptlvl2obj = \
   ATL_F77wrap_sptgbmv.o ATL_F77wrap_sptgemv.o ATL_F77wrap_sptger.o \
   ATL_F77wrap_sptsbmv.o ATL_F77wrap_sptspmv.o ATL_F77wrap_sptspr.o \
   ATL_F77wrap_sptspr2.o ATL_F77wrap_sptsymv.o ATL_F77wrap_sptsyr.o \
   ATL_F77wrap_sptsyr2.o ATL_F77wrap_spttbmv.o ATL_F77wrap_spttbsv.o \
   ATL_F77wrap_spttpmv.o ATL_F77wrap_spttpsv.o ATL_F77wrap_spttrmv.o \
   ATL_F77wrap_spttrsv.o
#
ATL_dptlvl2obj = \
   ATL_F77wrap_dptgbmv.o ATL_F77wrap_dptgemv.o ATL_F77wrap_dptger.o \
   ATL_F77wrap_dptsbmv.o ATL_F77wrap_dptspmv.o ATL_F77wrap_dptspr.o \
   ATL_F77wrap_dptspr2.o ATL_F77wrap_dptsymv.o ATL_F77wrap_dptsyr.o \
   ATL_F77wrap_dptsyr2.o ATL_F77wrap_dpttbmv.o ATL_F77wrap_dpttbsv.o \
   ATL_F77wrap_dpttpmv.o ATL_F77wrap_dpttpsv.o ATL_F77wrap_dpttrmv.o \
   ATL_F77wrap_dpttrsv.o
#
ATL_cptlvl2obj = \
   ATL_F77wrap_cptgbmv.o ATL_F77wrap_cptgemv.o ATL_F77wrap_cptgerc.o \
   ATL_F77wrap_cptgeru.o ATL_F77wrap_cpthbmv.o ATL_F77wrap_cpthemv.o \
   ATL_F77wrap_cpther.o ATL_F77wrap_cpther2.o ATL_F77wrap_cpthpmv.o \
   ATL_F77wrap_cpthpr.o ATL_F77wrap_cpthpr2.o ATL_F77wrap_cpttbmv.o \
   ATL_F77wrap_cpttbsv.o ATL_F77wrap_cpttpmv.o ATL_F77wrap_cpttpsv.o \
   ATL_F77wrap_cpttrmv.o ATL_F77wrap_cpttrsv.o
#
ATL_zptlvl2obj = \
   ATL_F77wrap_zptgbmv.o ATL_F77wrap_zptgemv.o ATL_F77wrap_zptgerc.o \
   ATL_F77wrap_zptgeru.o ATL_F77wrap_zpthbmv.o ATL_F77wrap_zpthemv.o \
   ATL_F77wrap_zpther.o ATL_F77wrap_zpther2.o ATL_F77wrap_zpthpmv.o \
   ATL_F77wrap_zpthpr.o ATL_F77wrap_zpthpr2.o ATL_F77wrap_zpttbmv.o \
   ATL_F77wrap_zpttbsv.o ATL_F77wrap_zpttpmv.o ATL_F77wrap_zpttpsv.o \
   ATL_F77wrap_zpttrmv.o ATL_F77wrap_zpttrsv.o
#
ATL_slvl3obj = \
   ATL_F77wrap_sgemm.o ATL_F77wrap_ssymm.o ATL_F77wrap_ssyr2k.o \
   ATL_F77wrap_ssyrk.o ATL_F77wrap_strmm.o ATL_F77wrap_strsm.o
#
ATL_dlvl3obj = \
   ATL_F77wrap_dgemm.o ATL_F77wrap_dsymm.o ATL_F77wrap_dsyr2k.o \
   ATL_F77wrap_dsyrk.o ATL_F77wrap_dtrmm.o ATL_F77wrap_dtrsm.o
#
ATL_clvl3obj = \
   ATL_F77wrap_cgemm.o ATL_F77wrap_chemm.o ATL_F77wrap_cher2k.o \
   ATL_F77wrap_cherk.o ATL_F77wrap_csymm.o ATL_F77wrap_csyr2k.o \
   ATL_F77wrap_csyrk.o ATL_F77wrap_ctrmm.o ATL_F77wrap_ctrsm.o
#
ATL_zlvl3obj = \
   ATL_F77wrap_zgemm.o ATL_F77wrap_zhemm.o ATL_F77wrap_zher2k.o \
   ATL_F77wrap_zherk.o ATL_F77wrap_zsymm.o ATL_F77wrap_zsyr2k.o \
   ATL_F77wrap_zsyrk.o ATL_F77wrap_ztrmm.o ATL_F77wrap_ztrsm.o
#
ATL_sptlvl3obj = \
   ATL_F77wrap_sptgemm.o ATL_F77wrap_sptsymm.o ATL_F77wrap_sptsyr2k.o \
   ATL_F77wrap_sptsyrk.o ATL_F77wrap_spttrmm.o ATL_F77wrap_spttrsm.o
#
ATL_dptlvl3obj = \
   ATL_F77wrap_dptgemm.o ATL_F77wrap_dptsymm.o ATL_F77wrap_dptsyr2k.o \
   ATL_F77wrap_dptsyrk.o ATL_F77wrap_dpttrmm.o ATL_F77wrap_dpttrsm.o
#
ATL_cptlvl3obj = \
   ATL_F77wrap_cptgemm.o ATL_F77wrap_cpthemm.o ATL_F77wrap_cpther2k.o \
   ATL_F77wrap_cptherk.o ATL_F77wrap_cptsymm.o ATL_F77wrap_cptsyr2k.o \
   ATL_F77wrap_cptsyrk.o ATL_F77wrap_cpttrmm.o ATL_F77wrap_cpttrsm.o
#
ATL_zptlvl3obj = \
   ATL_F77wrap_zptgemm.o ATL_F77wrap_zpthemm.o ATL_F77wrap_zpther2k.o \
   ATL_F77wrap_zptherk.o ATL_F77wrap_zptsymm.o ATL_F77wrap_zptsyr2k.o \
   ATL_F77wrap_zptsyrk.o ATL_F77wrap_zpttrmm.o ATL_F77wrap_zpttrsm.o
#
## Targets #############################################################
#
all              : lib
#
lib              : slib dlib clib zlib
#
lvl1             : slvl1 dlvl1 clvl1 zlvl1
lvl2             : slvl2 dlvl2 clvl2 zlvl2
lvl3             : slvl3 dlvl3 clvl3 zlvl3
#
slib             : slvl1 slvl2 slvl3
dlib             : dlvl1 dlvl2 dlvl3
clib             : clvl1 clvl2 clvl3
zlib             : zlvl1 zlvl2 zlvl3
#
ptall            : ptlib
#
ptlib            : sptlib dptlib cptlib zptlib
#
ptlvl1           : sptlvl1 dptlvl1 cptlvl1 zptlvl1
ptlvl2           : sptlvl2 dptlvl2 cptlvl2 zptlvl2
ptlvl3           : sptlvl3 dptlvl3 cptlvl3 zptlvl3
#
sptlib         : sptlvl1 sptlvl2 sptlvl3
#  sptlib           : sptlvl3
dptlib         : dptlvl1 dptlvl2 dptlvl3
#  dptlib           : dptlvl3
cptlib         : cptlvl1 cptlvl2 cptlvl3
#  cptlib           : cptlvl3
zptlib         : zptlvl1 zptlvl2 zptlvl3
#  zptlib           : zptlvl3
#
########################################################################
#
slvl1            : 
	$(PMAKE) slvl1.grd
dlvl1            : 
	$(PMAKE) dlvl1.grd
clvl1            : 
	$(PMAKE) clvl1.grd
zlvl1            : 
	$(PMAKE) zlvl1.grd
#
slvl2            : 
	$(PMAKE) slvl2.grd
dlvl2            : 
	$(PMAKE) dlvl2.grd
clvl2            : 
	$(PMAKE) clvl2.grd
zlvl2            : 
	$(PMAKE) zlvl2.grd
#
slvl3            : 
	$(PMAKE) slvl3.grd
dlvl3            : 
	$(PMAKE) dlvl3.grd
clvl3            : 
	$(PMAKE) clvl3.grd
zlvl3            : 
	$(PMAKE) zlvl3.grd
#
sptlvl1          : 
	$(PMAKE) sptlvl1.grd
dptlvl1          : 
	$(PMAKE) dptlvl1.grd
cptlvl1          : 
	$(PMAKE) cptlvl1.grd
zptlvl1          : 
	$(PMAKE) zptlvl1.grd
#
sptlvl2          : 
	$(PMAKE) sptlvl2.grd
dptlvl2          : 
	$(PMAKE) dptlvl2.grd
cptlvl2          : 
	$(PMAKE) cptlvl2.grd
zptlvl2          : 
	$(PMAKE) zptlvl2.grd
#
sptlvl3          : 
	$(PMAKE) sptlvl3.grd
dptlvl3          : 
	$(PMAKE) dptlvl3.grd
cptlvl3          : 
	$(PMAKE) cptlvl3.grd
zptlvl3          : 
	$(PMAKE) zptlvl3.grd
#
slvl1.grd        : $(slvl1obj) $(ATL_slvl1obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(slvl1obj) \
        $(ATL_slvl1obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch slvl1.grd
#
dlvl1.grd        : $(dlvl1obj) $(ATL_dlvl1obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(dlvl1obj) \
        $(ATL_dlvl1obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch dlvl1.grd
#
clvl1.grd        : $(clvl1obj) $(ATL_clvl1obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(clvl1obj) \
        $(ATL_clvl1obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch clvl1.grd
#
zlvl1.grd        : $(zlvl1obj) $(ATL_zlvl1obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(zlvl1obj) \
        $(ATL_zlvl1obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch zlvl1.grd
#
slvl2.grd        : $(slvl2obj) $(ATL_slvl2obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(slvl2obj) \
        $(ATL_slvl2obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch slvl2.grd
#
dlvl2.grd        : $(dlvl2obj) $(ATL_dlvl2obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(dlvl2obj) \
        $(ATL_dlvl2obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch dlvl2.grd
#
clvl2.grd        : $(clvl2obj) $(ATL_clvl2obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(clvl2obj) \
        $(ATL_clvl2obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch clvl2.grd
#
zlvl2.grd        : $(zlvl2obj) $(ATL_zlvl2obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(zlvl2obj) \
        $(ATL_zlvl2obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch zlvl2.grd
#
slvl3.grd        : $(slvl3obj) $(ATL_slvl3obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(slvl3obj) \
        $(ATL_slvl3obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch slvl3.grd
#
dlvl3.grd        : $(dlvl3obj) $(ATL_dlvl3obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(dlvl3obj) \
        $(ATL_dlvl3obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch dlvl3.grd
#
clvl3.grd        : $(clvl3obj) $(ATL_clvl3obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(clvl3obj) \
        $(ATL_clvl3obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch clvl3.grd
#
zlvl3.grd        : $(zlvl3obj) $(ATL_zlvl3obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(F77BLASlib) $(zlvl3obj) \
        $(ATL_zlvl3obj) $(aobj)
	$(RANLIB) $(F77BLASlib)
	touch zlvl3.grd
#
#
sptlvl1.grd      : $(slvl1obj) $(ATL_sptlvl1obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(slvl1obj) \
        $(ATL_sptlvl1obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch sptlvl1.grd
#  
dptlvl1.grd      : $(dlvl1obj) $(ATL_dptlvl1obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(dlvl1obj) \
        $(ATL_dptlvl1obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch dptlvl1.grd
#  
cptlvl1.grd      : $(clvl1obj) $(ATL_cptlvl1obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(clvl1obj) \
        $(ATL_cptlvl1obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch cptlvl1.grd
#  
zptlvl1.grd      : $(zlvl1obj) $(ATL_zptlvl1obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(zlvl1obj) \
        $(ATL_zptlvl1obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch zptlvl1.grd
#  
sptlvl2.grd      : $(slvl2obj) $(ATL_sptlvl2obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(slvl2obj) \
        $(ATL_sptlvl2obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch sptlvl2.grd
#  
dptlvl2.grd      : $(dlvl2obj) $(ATL_dptlvl2obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(dlvl2obj) \
        $(ATL_dptlvl2obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch dptlvl2.grd
#  
cptlvl2.grd      : $(clvl2obj) $(ATL_cptlvl2obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(clvl2obj) \
        $(ATL_cptlvl2obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch cptlvl2.grd
#  
zptlvl2.grd      : $(zlvl2obj) $(ATL_zptlvl2obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(zlvl2obj) \
        $(ATL_zptlvl2obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch zptlvl2.grd
#  
sptlvl3.grd      : $(slvl3obj) $(ATL_sptlvl3obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(slvl3obj) \
        $(ATL_sptlvl3obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch sptlvl3.grd
#  
dptlvl3.grd      : $(dlvl3obj) $(ATL_dptlvl3obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(dlvl3obj) \
        $(ATL_dptlvl3obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch dptlvl3.grd
#  
cptlvl3.grd      : $(clvl3obj) $(ATL_cptlvl3obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(clvl3obj) \
        $(ATL_cptlvl3obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch cptlvl3.grd
#  
zptlvl3.grd      : $(zlvl3obj) $(ATL_zptlvl3obj) $(aobj)
	$(ARCHIVER) $(ARFLAGS) $(PTF77BLASlib) $(zlvl3obj) \
        $(ATL_zptlvl3obj) $(aobj)
	$(RANLIB) $(PTF77BLASlib)
	touch zptlvl3.grd
#  
########################################################################
#
dcabs1.o             : ../dcabs1.f            
	$(F77) -o $@ -c $(F77FLAGS) ../dcabs1.f            
xerbla.o             : ../xerbla.f            
	$(F77) -o $@ -c $(F77FLAGS) ../xerbla.f            
lsame.o              : ../lsame.f             
	$(F77) -o $@ -c $(F77FLAGS) ../lsame.f             
#
srotg.o              : ../srotg.f             
	$(F77) -o $@ -c $(F77FLAGS) ../srotg.f             
srotmg.o             : ../srotmg.f            
	$(F77) -o $@ -c $(F77FLAGS) ../srotmg.f            
sscal.o              : ../sscal.f             
	$(F77) -o $@ -c $(F77FLAGS) ../sscal.f             
snrm2.o              : ../snrm2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../snrm2.f             
sasum.o              : ../sasum.f             
	$(F77) -o $@ -c $(F77FLAGS) ../sasum.f             
isamax.o             : ../isamax.f            
	$(F77) -o $@ -c $(F77FLAGS) ../isamax.f            
saxpy.o              : ../saxpy.f             
	$(F77) -o $@ -c $(F77FLAGS) ../saxpy.f             
scopy.o              : ../scopy.f             
	$(F77) -o $@ -c $(F77FLAGS) ../scopy.f             
sswap.o              : ../sswap.f             
	$(F77) -o $@ -c $(F77FLAGS) ../sswap.f             
srot.o               : ../srot.f              
	$(F77) -o $@ -c $(F77FLAGS) ../srot.f              
srotm.o              : ../srotm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../srotm.f             
sdot.o               : ../sdot.f              
	$(F77) -o $@ -c $(F77FLAGS) ../sdot.f              
dsdot.o              : ../dsdot.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dsdot.f             
sdsdot.o             : ../sdsdot.f            
	$(F77) -o $@ -c $(F77FLAGS) ../sdsdot.f            
#
drotg.o              : ../drotg.f             
	$(F77) -o $@ -c $(F77FLAGS) ../drotg.f             
drotmg.o             : ../drotmg.f            
	$(F77) -o $@ -c $(F77FLAGS) ../drotmg.f            
dscal.o              : ../dscal.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dscal.f             
dnrm2.o              : ../dnrm2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dnrm2.f             
dasum.o              : ../dasum.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dasum.f             
idamax.o             : ../idamax.f            
	$(F77) -o $@ -c $(F77FLAGS) ../idamax.f            
daxpy.o              : ../daxpy.f             
	$(F77) -o $@ -c $(F77FLAGS) ../daxpy.f             
dcopy.o              : ../dcopy.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dcopy.f             
dswap.o              : ../dswap.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dswap.f             
drot.o               : ../drot.f              
	$(F77) -o $@ -c $(F77FLAGS) ../drot.f              
drotm.o              : ../drotm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../drotm.f             
ddot.o               : ../ddot.f              
	$(F77) -o $@ -c $(F77FLAGS) ../ddot.f              
#
crotg.o              : ../crotg.f             
	$(F77) -o $@ -c $(F77FLAGS) ../crotg.f             
cscal.o              : ../cscal.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cscal.f             
csscal.o             : ../csscal.f            
	$(F77) -o $@ -c $(F77FLAGS) ../csscal.f            
scnrm2.o             : ../scnrm2.f            
	$(F77) -o $@ -c $(F77FLAGS) ../scnrm2.f            
scasum.o             : ../scasum.f            
	$(F77) -o $@ -c $(F77FLAGS) ../scasum.f            
icamax.o             : ../icamax.f            
	$(F77) -o $@ -c $(F77FLAGS) ../icamax.f            
caxpy.o              : ../caxpy.f             
	$(F77) -o $@ -c $(F77FLAGS) ../caxpy.f             
ccopy.o              : ../ccopy.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ccopy.f             
cswap.o              : ../cswap.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cswap.f             
csrot.o              : ../csrot.f             
	$(F77) -o $@ -c $(F77FLAGS) ../csrot.f             
cdotc.o              : ../cdotc.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cdotc.f             
cdotu.o              : ../cdotu.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cdotu.f             
#
zrotg.o              : ../zrotg.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zrotg.f             
zscal.o              : ../zscal.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zscal.f             
zdscal.o             : ../zdscal.f            
	$(F77) -o $@ -c $(F77FLAGS) ../zdscal.f            
dznrm2.o             : ../dznrm2.f            
	$(F77) -o $@ -c $(F77FLAGS) ../dznrm2.f            
dzasum.o             : ../dzasum.f            
	$(F77) -o $@ -c $(F77FLAGS) ../dzasum.f            
izamax.o             : ../izamax.f            
	$(F77) -o $@ -c $(F77FLAGS) ../izamax.f            
zaxpy.o              : ../zaxpy.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zaxpy.f             
zcopy.o              : ../zcopy.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zcopy.f             
zswap.o              : ../zswap.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zswap.f             
zdrot.o              : ../zdrot.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zdrot.f             
zdotc.o              : ../zdotc.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zdotc.f             
zdotu.o              : ../zdotu.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zdotu.f             
#
sgbmv.o              : ../sgbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../sgbmv.f             
sgemv.o              : ../sgemv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../sgemv.f             
sger.o               : ../sger.f              
	$(F77) -o $@ -c $(F77FLAGS) ../sger.f              
ssbmv.o              : ../ssbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ssbmv.f             
sspmv.o              : ../sspmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../sspmv.f             
ssymv.o              : ../ssymv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ssymv.f             
sspr.o               : ../sspr.f              
	$(F77) -o $@ -c $(F77FLAGS) ../sspr.f              
ssyr.o               : ../ssyr.f              
	$(F77) -o $@ -c $(F77FLAGS) ../ssyr.f              
sspr2.o              : ../sspr2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../sspr2.f             
ssyr2.o              : ../ssyr2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ssyr2.f             
stbmv.o              : ../stbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../stbmv.f             
stpmv.o              : ../stpmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../stpmv.f             
strmv.o              : ../strmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../strmv.f             
stbsv.o              : ../stbsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../stbsv.f             
stpsv.o              : ../stpsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../stpsv.f             
strsv.o              : ../strsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../strsv.f             
#
dgbmv.o              : ../dgbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dgbmv.f             
dgemv.o              : ../dgemv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dgemv.f             
dger.o               : ../dger.f              
	$(F77) -o $@ -c $(F77FLAGS) ../dger.f              
dsbmv.o              : ../dsbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dsbmv.f             
dspmv.o              : ../dspmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dspmv.f             
dsymv.o              : ../dsymv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dsymv.f             
dspr.o               : ../dspr.f              
	$(F77) -o $@ -c $(F77FLAGS) ../dspr.f              
dsyr.o               : ../dsyr.f              
	$(F77) -o $@ -c $(F77FLAGS) ../dsyr.f              
dspr2.o              : ../dspr2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dspr2.f             
dsyr2.o              : ../dsyr2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dsyr2.f             
dtbmv.o              : ../dtbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dtbmv.f             
dtpmv.o              : ../dtpmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dtpmv.f             
dtrmv.o              : ../dtrmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dtrmv.f             
dtbsv.o              : ../dtbsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dtbsv.f             
dtpsv.o              : ../dtpsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dtpsv.f             
dtrsv.o              : ../dtrsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dtrsv.f             
#
cgbmv.o              : ../cgbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cgbmv.f             
cgemv.o              : ../cgemv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cgemv.f             
cgerc.o              : ../cgerc.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cgerc.f             
cgeru.o              : ../cgeru.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cgeru.f             
chbmv.o              : ../chbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../chbmv.f             
chpmv.o              : ../chpmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../chpmv.f             
chemv.o              : ../chemv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../chemv.f             
chpr.o               : ../chpr.f              
	$(F77) -o $@ -c $(F77FLAGS) ../chpr.f              
cher.o               : ../cher.f              
	$(F77) -o $@ -c $(F77FLAGS) ../cher.f              
chpr2.o              : ../chpr2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../chpr2.f             
cher2.o              : ../cher2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cher2.f             
ctbmv.o              : ../ctbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ctbmv.f             
ctpmv.o              : ../ctpmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ctpmv.f             
ctrmv.o              : ../ctrmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ctrmv.f             
ctbsv.o              : ../ctbsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ctbsv.f             
ctpsv.o              : ../ctpsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ctpsv.f             
ctrsv.o              : ../ctrsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ctrsv.f             
#
zgbmv.o              : ../zgbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zgbmv.f             
zgemv.o              : ../zgemv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zgemv.f             
zgerc.o              : ../zgerc.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zgerc.f             
zgeru.o              : ../zgeru.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zgeru.f             
zhbmv.o              : ../zhbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zhbmv.f             
zhpmv.o              : ../zhpmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zhpmv.f             
zhemv.o              : ../zhemv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zhemv.f             
zhpr.o               : ../zhpr.f              
	$(F77) -o $@ -c $(F77FLAGS) ../zhpr.f              
zher.o               : ../zher.f              
	$(F77) -o $@ -c $(F77FLAGS) ../zher.f              
zhpr2.o              : ../zhpr2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zhpr2.f             
zher2.o              : ../zher2.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zher2.f             
ztbmv.o              : ../ztbmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ztbmv.f             
ztpmv.o              : ../ztpmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ztpmv.f             
ztrmv.o              : ../ztrmv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ztrmv.f             
ztbsv.o              : ../ztbsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ztbsv.f             
ztpsv.o              : ../ztpsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ztpsv.f             
ztrsv.o              : ../ztrsv.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ztrsv.f             
#
sgemm.o              : ../sgemm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../sgemm.f             
ssymm.o              : ../ssymm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ssymm.f             
ssyrk.o              : ../ssyrk.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ssyrk.f             
ssyr2k.o             : ../ssyr2k.f            
	$(F77) -o $@ -c $(F77FLAGS) ../ssyr2k.f            
strmm.o              : ../strmm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../strmm.f             
strsm.o              : ../strsm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../strsm.f             
#
dgemm.o              : ../dgemm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dgemm.f             
dsymm.o              : ../dsymm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dsymm.f             
dsyrk.o              : ../dsyrk.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dsyrk.f             
dsyr2k.o             : ../dsyr2k.f            
	$(F77) -o $@ -c $(F77FLAGS) ../dsyr2k.f            
dtrmm.o              : ../dtrmm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dtrmm.f             
dtrsm.o              : ../dtrsm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../dtrsm.f             
#
cgemm.o              : ../cgemm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cgemm.f             
chemm.o              : ../chemm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../chemm.f             
cherk.o              : ../cherk.f             
	$(F77) -o $@ -c $(F77FLAGS) ../cherk.f             
cher2k.o             : ../cher2k.f            
	$(F77) -o $@ -c $(F77FLAGS) ../cher2k.f            
csymm.o              : ../csymm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../csymm.f             
csyrk.o              : ../csyrk.f             
	$(F77) -o $@ -c $(F77FLAGS) ../csyrk.f             
csyr2k.o             : ../csyr2k.f            
	$(F77) -o $@ -c $(F77FLAGS) ../csyr2k.f            
ctrmm.o              : ../ctrmm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ctrmm.f             
ctrsm.o              : ../ctrsm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ctrsm.f             
#
zgemm.o              : ../zgemm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zgemm.f             
zhemm.o              : ../zhemm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zhemm.f             
zherk.o              : ../zherk.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zherk.f             
zher2k.o             : ../zher2k.f            
	$(F77) -o $@ -c $(F77FLAGS) ../zher2k.f            
zsymm.o              : ../zsymm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zsymm.f             
zsyrk.o              : ../zsyrk.f             
	$(F77) -o $@ -c $(F77FLAGS) ../zsyrk.f             
zsyr2k.o             : ../zsyr2k.f            
	$(F77) -o $@ -c $(F77FLAGS) ../zsyr2k.f            
ztrmm.o              : ../ztrmm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ztrmm.f             
ztrsm.o              : ../ztrsm.f             
	$(F77) -o $@ -c $(F77FLAGS) ../ztrsm.f             
#
ATL_F77wrap_srotg.o      : ../f77wrap/ATL_F77wrap_rotg.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_rotg.c  
ATL_F77wrap_srotmg.o     : ../f77wrap/ATL_F77wrap_rotmg.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_rotmg.c 
ATL_F77wrap_sscal.o      : ../f77wrap/ATL_F77wrap_scal.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_scal.c  
ATL_F77wrap_snrm2.o      : ../f77wrap/ATL_F77wrap_nrm2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_nrm2.c  
ATL_F77wrap_sasum.o      : ../f77wrap/ATL_F77wrap_asum.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_asum.c  
ATL_F77wrap_isamax.o     : ../f77wrap/ATL_F77wrap_amax.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_amax.c  
ATL_F77wrap_saxpy.o      : ../f77wrap/ATL_F77wrap_axpy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_axpy.c  
ATL_F77wrap_scopy.o      : ../f77wrap/ATL_F77wrap_copy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_copy.c  
ATL_F77wrap_sswap.o      : ../f77wrap/ATL_F77wrap_swap.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_swap.c  
ATL_F77wrap_srot.o       : ../f77wrap/ATL_F77wrap_rot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_rot.c   
ATL_F77wrap_srotm.o      : ../f77wrap/ATL_F77wrap_rotm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_rotm.c  
ATL_F77wrap_sdot.o       : ../f77wrap/ATL_F77wrap_dot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_dot.c   
ATL_F77wrap_dsdot.o      : ../f77wrap/ATL_F77wrap_dsdot.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_dsdot.c 
ATL_F77wrap_sdsdot.o     : ../f77wrap/ATL_F77wrap_sdsdot.c $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_sdsdot.c
#
ATL_F77wrap_drotg.o      : ../f77wrap/ATL_F77wrap_rotg.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_rotg.c  
ATL_F77wrap_drotmg.o     : ../f77wrap/ATL_F77wrap_rotmg.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_rotmg.c 
ATL_F77wrap_dscal.o      : ../f77wrap/ATL_F77wrap_scal.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_scal.c  
ATL_F77wrap_dnrm2.o      : ../f77wrap/ATL_F77wrap_nrm2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_nrm2.c  
ATL_F77wrap_dasum.o      : ../f77wrap/ATL_F77wrap_asum.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_asum.c  
ATL_F77wrap_idamax.o     : ../f77wrap/ATL_F77wrap_amax.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_amax.c  
ATL_F77wrap_daxpy.o      : ../f77wrap/ATL_F77wrap_axpy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_axpy.c  
ATL_F77wrap_dcopy.o      : ../f77wrap/ATL_F77wrap_copy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_copy.c  
ATL_F77wrap_dswap.o      : ../f77wrap/ATL_F77wrap_swap.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_swap.c  
ATL_F77wrap_drot.o       : ../f77wrap/ATL_F77wrap_rot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_rot.c   
ATL_F77wrap_drotm.o      : ../f77wrap/ATL_F77wrap_rotm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_rotm.c  
ATL_F77wrap_ddot.o       : ../f77wrap/ATL_F77wrap_dot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_dot.c   
#
ATL_F77wrap_crotg.o      : ../f77wrap/ATL_F77wrap_rotg.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_rotg.c  
ATL_F77wrap_cscal.o      : ../f77wrap/ATL_F77wrap_scal.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_scal.c  
ATL_F77wrap_csscal.o     : ../f77wrap/ATL_F77wrap_rscal.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_rscal.c 
ATL_F77wrap_scnrm2.o     : ../f77wrap/ATL_F77wrap_nrm2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_nrm2.c  
ATL_F77wrap_scasum.o     : ../f77wrap/ATL_F77wrap_asum.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_asum.c  
ATL_F77wrap_icamax.o     : ../f77wrap/ATL_F77wrap_amax.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_amax.c  
ATL_F77wrap_caxpy.o      : ../f77wrap/ATL_F77wrap_axpy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_axpy.c  
ATL_F77wrap_ccopy.o      : ../f77wrap/ATL_F77wrap_copy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_copy.c  
ATL_F77wrap_cswap.o      : ../f77wrap/ATL_F77wrap_swap.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_swap.c  
ATL_F77wrap_csrot.o      : ../f77wrap/ATL_F77wrap_rot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_rot.c   
ATL_F77wrap_cdotc.o      : ../f77wrap/ATL_F77wrap_dotc.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_dotc.c  
ATL_F77wrap_cdotu.o      : ../f77wrap/ATL_F77wrap_dotu.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_dotu.c  
#
ATL_F77wrap_zrotg.o      : ../f77wrap/ATL_F77wrap_rotg.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_rotg.c  
ATL_F77wrap_zscal.o      : ../f77wrap/ATL_F77wrap_scal.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_scal.c  
ATL_F77wrap_zdscal.o     : ../f77wrap/ATL_F77wrap_rscal.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_rscal.c 
ATL_F77wrap_dznrm2.o     : ../f77wrap/ATL_F77wrap_nrm2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_nrm2.c  
ATL_F77wrap_dzasum.o     : ../f77wrap/ATL_F77wrap_asum.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_asum.c  
ATL_F77wrap_izamax.o     : ../f77wrap/ATL_F77wrap_amax.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_amax.c  
ATL_F77wrap_zaxpy.o      : ../f77wrap/ATL_F77wrap_axpy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_axpy.c  
ATL_F77wrap_zcopy.o      : ../f77wrap/ATL_F77wrap_copy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_copy.c  
ATL_F77wrap_zswap.o      : ../f77wrap/ATL_F77wrap_swap.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_swap.c  
ATL_F77wrap_zdrot.o      : ../f77wrap/ATL_F77wrap_rot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_rot.c   
ATL_F77wrap_zdotc.o      : ../f77wrap/ATL_F77wrap_dotc.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_dotc.c  
ATL_F77wrap_zdotu.o      : ../f77wrap/ATL_F77wrap_dotu.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_dotu.c  
#
ATL_F77wrap_sptrotg.o    : ../f77wrap/ATL_F77wrap_rotg.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_rotg.c  
ATL_F77wrap_sptrotmg.o   : ../f77wrap/ATL_F77wrap_rotmg.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_rotmg.c 
ATL_F77wrap_sptscal.o    : ../f77wrap/ATL_F77wrap_scal.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_scal.c  
ATL_F77wrap_sptnrm2.o    : ../f77wrap/ATL_F77wrap_nrm2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_nrm2.c  
ATL_F77wrap_sptasum.o    : ../f77wrap/ATL_F77wrap_asum.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_asum.c  
ATL_F77wrap_isptamax.o   : ../f77wrap/ATL_F77wrap_amax.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_amax.c  
ATL_F77wrap_sptaxpy.o    : ../f77wrap/ATL_F77wrap_axpy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_axpy.c  
ATL_F77wrap_sptcopy.o    : ../f77wrap/ATL_F77wrap_copy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_copy.c  
ATL_F77wrap_sptswap.o    : ../f77wrap/ATL_F77wrap_swap.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_swap.c  
ATL_F77wrap_sptrot.o     : ../f77wrap/ATL_F77wrap_rot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_rot.c   
ATL_F77wrap_sptrotm.o    : ../f77wrap/ATL_F77wrap_rotm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_rotm.c  
ATL_F77wrap_sptdot.o     : ../f77wrap/ATL_F77wrap_dot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_dot.c   
ATL_F77wrap_dsptdot.o    : ../f77wrap/ATL_F77wrap_dsdot.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_dsdot.c 
ATL_F77wrap_sdsptdot.o   : ../f77wrap/ATL_F77wrap_sdsdot.c $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_sdsdot.c
#
ATL_F77wrap_dptrotg.o    : ../f77wrap/ATL_F77wrap_rotg.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_rotg.c  
ATL_F77wrap_dptrotmg.o   : ../f77wrap/ATL_F77wrap_rotmg.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_rotmg.c 
ATL_F77wrap_dptscal.o    : ../f77wrap/ATL_F77wrap_scal.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_scal.c  
ATL_F77wrap_dptnrm2.o    : ../f77wrap/ATL_F77wrap_nrm2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_nrm2.c  
ATL_F77wrap_dptasum.o    : ../f77wrap/ATL_F77wrap_asum.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_asum.c  
ATL_F77wrap_idptamax.o   : ../f77wrap/ATL_F77wrap_amax.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_amax.c  
ATL_F77wrap_dptaxpy.o    : ../f77wrap/ATL_F77wrap_axpy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_axpy.c  
ATL_F77wrap_dptcopy.o    : ../f77wrap/ATL_F77wrap_copy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_copy.c  
ATL_F77wrap_dptswap.o    : ../f77wrap/ATL_F77wrap_swap.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_swap.c  
ATL_F77wrap_dptrot.o     : ../f77wrap/ATL_F77wrap_rot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_rot.c   
ATL_F77wrap_dptrotm.o    : ../f77wrap/ATL_F77wrap_rotm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_rotm.c  
ATL_F77wrap_dptdot.o     : ../f77wrap/ATL_F77wrap_dot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_dot.c   
#
ATL_F77wrap_cptrotg.o    : ../f77wrap/ATL_F77wrap_rotg.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_rotg.c  
ATL_F77wrap_cptscal.o    : ../f77wrap/ATL_F77wrap_scal.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_scal.c  
ATL_F77wrap_csptscal.o   : ../f77wrap/ATL_F77wrap_rscal.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_rscal.c 
ATL_F77wrap_scptnrm2.o   : ../f77wrap/ATL_F77wrap_nrm2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_nrm2.c  
ATL_F77wrap_scptasum.o   : ../f77wrap/ATL_F77wrap_asum.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_asum.c  
ATL_F77wrap_icptamax.o   : ../f77wrap/ATL_F77wrap_amax.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_amax.c  
ATL_F77wrap_cptaxpy.o    : ../f77wrap/ATL_F77wrap_axpy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_axpy.c  
ATL_F77wrap_cptcopy.o    : ../f77wrap/ATL_F77wrap_copy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_copy.c  
ATL_F77wrap_cptswap.o    : ../f77wrap/ATL_F77wrap_swap.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_swap.c  
ATL_F77wrap_csptrot.o    : ../f77wrap/ATL_F77wrap_rot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_rot.c   
ATL_F77wrap_cptdotc.o    : ../f77wrap/ATL_F77wrap_dotc.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_dotc.c  
ATL_F77wrap_cptdotu.o    : ../f77wrap/ATL_F77wrap_dotu.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_dotu.c  
#
ATL_F77wrap_zptrotg.o    : ../f77wrap/ATL_F77wrap_rotg.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_rotg.c  
ATL_F77wrap_zptscal.o    : ../f77wrap/ATL_F77wrap_scal.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_scal.c  
ATL_F77wrap_zdptscal.o   : ../f77wrap/ATL_F77wrap_rscal.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_rscal.c 
ATL_F77wrap_dzptnrm2.o   : ../f77wrap/ATL_F77wrap_nrm2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_nrm2.c  
ATL_F77wrap_dzptasum.o   : ../f77wrap/ATL_F77wrap_asum.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_asum.c  
ATL_F77wrap_izptamax.o   : ../f77wrap/ATL_F77wrap_amax.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_amax.c  
ATL_F77wrap_zptaxpy.o    : ../f77wrap/ATL_F77wrap_axpy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_axpy.c  
ATL_F77wrap_zptcopy.o    : ../f77wrap/ATL_F77wrap_copy.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_copy.c  
ATL_F77wrap_zptswap.o    : ../f77wrap/ATL_F77wrap_swap.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_swap.c  
ATL_F77wrap_zdptrot.o    : ../f77wrap/ATL_F77wrap_rot.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_rot.c   
ATL_F77wrap_zptdotc.o    : ../f77wrap/ATL_F77wrap_dotc.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_dotc.c  
ATL_F77wrap_zptdotu.o    : ../f77wrap/ATL_F77wrap_dotu.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_dotu.c  
#
ATL_F77wrap_sgbmv.o      : ../f77wrap/ATL_F77wrap_gbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_gbmv.c  
ATL_F77wrap_sgemv.o      : ../f77wrap/ATL_F77wrap_gemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_gemv.c  
ATL_F77wrap_sger.o       : ../f77wrap/ATL_F77wrap_ger.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_ger.c   
ATL_F77wrap_ssbmv.o      : ../f77wrap/ATL_F77wrap_sbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_sbmv.c  
ATL_F77wrap_sspmv.o      : ../f77wrap/ATL_F77wrap_spmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_spmv.c  
ATL_F77wrap_ssymv.o      : ../f77wrap/ATL_F77wrap_symv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_symv.c  
ATL_F77wrap_sspr.o       : ../f77wrap/ATL_F77wrap_spr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_spr.c   
ATL_F77wrap_ssyr.o       : ../f77wrap/ATL_F77wrap_syr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_syr.c   
ATL_F77wrap_sspr2.o      : ../f77wrap/ATL_F77wrap_spr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_spr2.c  
ATL_F77wrap_ssyr2.o      : ../f77wrap/ATL_F77wrap_syr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_syr2.c  
ATL_F77wrap_stbmv.o      : ../f77wrap/ATL_F77wrap_tbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_tbmv.c  
ATL_F77wrap_stpmv.o      : ../f77wrap/ATL_F77wrap_tpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_tpmv.c  
ATL_F77wrap_strmv.o      : ../f77wrap/ATL_F77wrap_trmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_trmv.c  
ATL_F77wrap_stbsv.o      : ../f77wrap/ATL_F77wrap_tbsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_tbsv.c  
ATL_F77wrap_stpsv.o      : ../f77wrap/ATL_F77wrap_tpsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_tpsv.c  
ATL_F77wrap_strsv.o      : ../f77wrap/ATL_F77wrap_trsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_trsv.c  
#
ATL_F77wrap_dgbmv.o      : ../f77wrap/ATL_F77wrap_gbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_gbmv.c  
ATL_F77wrap_dgemv.o      : ../f77wrap/ATL_F77wrap_gemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_gemv.c  
ATL_F77wrap_dger.o       : ../f77wrap/ATL_F77wrap_ger.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_ger.c   
ATL_F77wrap_dsbmv.o      : ../f77wrap/ATL_F77wrap_sbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_sbmv.c  
ATL_F77wrap_dspmv.o      : ../f77wrap/ATL_F77wrap_spmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_spmv.c  
ATL_F77wrap_dsymv.o      : ../f77wrap/ATL_F77wrap_symv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_symv.c  
ATL_F77wrap_dspr.o       : ../f77wrap/ATL_F77wrap_spr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_spr.c   
ATL_F77wrap_dsyr.o       : ../f77wrap/ATL_F77wrap_syr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_syr.c   
ATL_F77wrap_dspr2.o      : ../f77wrap/ATL_F77wrap_spr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_spr2.c  
ATL_F77wrap_dsyr2.o      : ../f77wrap/ATL_F77wrap_syr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_syr2.c  
ATL_F77wrap_dtbmv.o      : ../f77wrap/ATL_F77wrap_tbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_tbmv.c  
ATL_F77wrap_dtpmv.o      : ../f77wrap/ATL_F77wrap_tpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_tpmv.c  
ATL_F77wrap_dtrmv.o      : ../f77wrap/ATL_F77wrap_trmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_trmv.c  
ATL_F77wrap_dtbsv.o      : ../f77wrap/ATL_F77wrap_tbsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_tbsv.c  
ATL_F77wrap_dtpsv.o      : ../f77wrap/ATL_F77wrap_tpsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_tpsv.c  
ATL_F77wrap_dtrsv.o      : ../f77wrap/ATL_F77wrap_trsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_trsv.c  
#
ATL_F77wrap_cgbmv.o      : ../f77wrap/ATL_F77wrap_gbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_gbmv.c  
ATL_F77wrap_cgemv.o      : ../f77wrap/ATL_F77wrap_gemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_gemv.c  
ATL_F77wrap_cgerc.o      : ../f77wrap/ATL_F77wrap_gerc.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_gerc.c  
ATL_F77wrap_cgeru.o      : ../f77wrap/ATL_F77wrap_geru.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_geru.c  
ATL_F77wrap_chbmv.o      : ../f77wrap/ATL_F77wrap_hbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_hbmv.c  
ATL_F77wrap_chpmv.o      : ../f77wrap/ATL_F77wrap_hpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_hpmv.c  
ATL_F77wrap_chemv.o      : ../f77wrap/ATL_F77wrap_hemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_hemv.c  
ATL_F77wrap_chpr.o       : ../f77wrap/ATL_F77wrap_hpr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_hpr.c   
ATL_F77wrap_cher.o       : ../f77wrap/ATL_F77wrap_her.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_her.c   
ATL_F77wrap_chpr2.o      : ../f77wrap/ATL_F77wrap_hpr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_hpr2.c  
ATL_F77wrap_cher2.o      : ../f77wrap/ATL_F77wrap_her2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_her2.c  
ATL_F77wrap_ctbmv.o      : ../f77wrap/ATL_F77wrap_tbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_tbmv.c  
ATL_F77wrap_ctpmv.o      : ../f77wrap/ATL_F77wrap_tpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_tpmv.c  
ATL_F77wrap_ctrmv.o      : ../f77wrap/ATL_F77wrap_trmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_trmv.c  
ATL_F77wrap_ctbsv.o      : ../f77wrap/ATL_F77wrap_tbsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_tbsv.c  
ATL_F77wrap_ctpsv.o      : ../f77wrap/ATL_F77wrap_tpsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_tpsv.c  
ATL_F77wrap_ctrsv.o      : ../f77wrap/ATL_F77wrap_trsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_trsv.c  
#
ATL_F77wrap_zgbmv.o      : ../f77wrap/ATL_F77wrap_gbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_gbmv.c  
ATL_F77wrap_zgemv.o      : ../f77wrap/ATL_F77wrap_gemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_gemv.c  
ATL_F77wrap_zgerc.o      : ../f77wrap/ATL_F77wrap_gerc.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_gerc.c  
ATL_F77wrap_zgeru.o      : ../f77wrap/ATL_F77wrap_geru.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_geru.c  
ATL_F77wrap_zhbmv.o      : ../f77wrap/ATL_F77wrap_hbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_hbmv.c  
ATL_F77wrap_zhpmv.o      : ../f77wrap/ATL_F77wrap_hpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_hpmv.c  
ATL_F77wrap_zhemv.o      : ../f77wrap/ATL_F77wrap_hemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_hemv.c  
ATL_F77wrap_zhpr.o       : ../f77wrap/ATL_F77wrap_hpr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_hpr.c   
ATL_F77wrap_zher.o       : ../f77wrap/ATL_F77wrap_her.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_her.c   
ATL_F77wrap_zhpr2.o      : ../f77wrap/ATL_F77wrap_hpr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_hpr2.c  
ATL_F77wrap_zher2.o      : ../f77wrap/ATL_F77wrap_her2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_her2.c  
ATL_F77wrap_ztbmv.o      : ../f77wrap/ATL_F77wrap_tbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_tbmv.c  
ATL_F77wrap_ztpmv.o      : ../f77wrap/ATL_F77wrap_tpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_tpmv.c  
ATL_F77wrap_ztrmv.o      : ../f77wrap/ATL_F77wrap_trmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_trmv.c  
ATL_F77wrap_ztbsv.o      : ../f77wrap/ATL_F77wrap_tbsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_tbsv.c  
ATL_F77wrap_ztpsv.o      : ../f77wrap/ATL_F77wrap_tpsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_tpsv.c  
ATL_F77wrap_ztrsv.o      : ../f77wrap/ATL_F77wrap_trsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_trsv.c  
#
ATL_F77wrap_sptgbmv.o    : ../f77wrap/ATL_F77wrap_gbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_gbmv.c  
ATL_F77wrap_sptgemv.o    : ../f77wrap/ATL_F77wrap_gemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_gemv.c  
ATL_F77wrap_sptger.o     : ../f77wrap/ATL_F77wrap_ger.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_ger.c   
ATL_F77wrap_sptsbmv.o    : ../f77wrap/ATL_F77wrap_sbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_sbmv.c  
ATL_F77wrap_sptspmv.o    : ../f77wrap/ATL_F77wrap_spmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_spmv.c  
ATL_F77wrap_sptsymv.o    : ../f77wrap/ATL_F77wrap_symv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_symv.c  
ATL_F77wrap_sptspr.o     : ../f77wrap/ATL_F77wrap_spr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_spr.c   
ATL_F77wrap_sptsyr.o     : ../f77wrap/ATL_F77wrap_syr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_syr.c   
ATL_F77wrap_sptspr2.o    : ../f77wrap/ATL_F77wrap_spr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_spr2.c  
ATL_F77wrap_sptsyr2.o    : ../f77wrap/ATL_F77wrap_syr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_syr2.c  
ATL_F77wrap_spttbmv.o    : ../f77wrap/ATL_F77wrap_tbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_tbmv.c  
ATL_F77wrap_spttpmv.o    : ../f77wrap/ATL_F77wrap_tpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_tpmv.c  
ATL_F77wrap_spttrmv.o    : ../f77wrap/ATL_F77wrap_trmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_trmv.c  
ATL_F77wrap_spttbsv.o    : ../f77wrap/ATL_F77wrap_tbsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_tbsv.c  
ATL_F77wrap_spttpsv.o    : ../f77wrap/ATL_F77wrap_tpsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_tpsv.c  
ATL_F77wrap_spttrsv.o    : ../f77wrap/ATL_F77wrap_trsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_trsv.c  
#
ATL_F77wrap_dptgbmv.o    : ../f77wrap/ATL_F77wrap_gbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_gbmv.c  
ATL_F77wrap_dptgemv.o    : ../f77wrap/ATL_F77wrap_gemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_gemv.c  
ATL_F77wrap_dptger.o     : ../f77wrap/ATL_F77wrap_ger.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_ger.c   
ATL_F77wrap_dptsbmv.o    : ../f77wrap/ATL_F77wrap_sbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_sbmv.c  
ATL_F77wrap_dptspmv.o    : ../f77wrap/ATL_F77wrap_spmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_spmv.c  
ATL_F77wrap_dptsymv.o    : ../f77wrap/ATL_F77wrap_symv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_symv.c  
ATL_F77wrap_dptspr.o     : ../f77wrap/ATL_F77wrap_spr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_spr.c   
ATL_F77wrap_dptsyr.o     : ../f77wrap/ATL_F77wrap_syr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_syr.c   
ATL_F77wrap_dptspr2.o    : ../f77wrap/ATL_F77wrap_spr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_spr2.c  
ATL_F77wrap_dptsyr2.o    : ../f77wrap/ATL_F77wrap_syr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_syr2.c  
ATL_F77wrap_dpttbmv.o    : ../f77wrap/ATL_F77wrap_tbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_tbmv.c  
ATL_F77wrap_dpttpmv.o    : ../f77wrap/ATL_F77wrap_tpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_tpmv.c  
ATL_F77wrap_dpttrmv.o    : ../f77wrap/ATL_F77wrap_trmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_trmv.c  
ATL_F77wrap_dpttbsv.o    : ../f77wrap/ATL_F77wrap_tbsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_tbsv.c  
ATL_F77wrap_dpttpsv.o    : ../f77wrap/ATL_F77wrap_tpsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_tpsv.c  
ATL_F77wrap_dpttrsv.o    : ../f77wrap/ATL_F77wrap_trsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_trsv.c  
#
ATL_F77wrap_cptgbmv.o    : ../f77wrap/ATL_F77wrap_gbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_gbmv.c  
ATL_F77wrap_cptgemv.o    : ../f77wrap/ATL_F77wrap_gemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_gemv.c  
ATL_F77wrap_cptgerc.o    : ../f77wrap/ATL_F77wrap_gerc.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_gerc.c  
ATL_F77wrap_cptgeru.o    : ../f77wrap/ATL_F77wrap_geru.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_geru.c  
ATL_F77wrap_cpthbmv.o    : ../f77wrap/ATL_F77wrap_hbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_hbmv.c  
ATL_F77wrap_cpthpmv.o    : ../f77wrap/ATL_F77wrap_hpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_hpmv.c  
ATL_F77wrap_cpthemv.o    : ../f77wrap/ATL_F77wrap_hemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_hemv.c  
ATL_F77wrap_cpthpr.o     : ../f77wrap/ATL_F77wrap_hpr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_hpr.c   
ATL_F77wrap_cpther.o     : ../f77wrap/ATL_F77wrap_her.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_her.c   
ATL_F77wrap_cpthpr2.o    : ../f77wrap/ATL_F77wrap_hpr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_hpr2.c  
ATL_F77wrap_cpther2.o    : ../f77wrap/ATL_F77wrap_her2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_her2.c  
ATL_F77wrap_cpttbmv.o    : ../f77wrap/ATL_F77wrap_tbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_tbmv.c  
ATL_F77wrap_cpttpmv.o    : ../f77wrap/ATL_F77wrap_tpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_tpmv.c  
ATL_F77wrap_cpttrmv.o    : ../f77wrap/ATL_F77wrap_trmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_trmv.c  
ATL_F77wrap_cpttbsv.o    : ../f77wrap/ATL_F77wrap_tbsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_tbsv.c  
ATL_F77wrap_cpttpsv.o    : ../f77wrap/ATL_F77wrap_tpsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_tpsv.c  
ATL_F77wrap_cpttrsv.o    : ../f77wrap/ATL_F77wrap_trsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_trsv.c  
#
ATL_F77wrap_zptgbmv.o    : ../f77wrap/ATL_F77wrap_gbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_gbmv.c  
ATL_F77wrap_zptgemv.o    : ../f77wrap/ATL_F77wrap_gemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_gemv.c  
ATL_F77wrap_zptgerc.o    : ../f77wrap/ATL_F77wrap_gerc.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_gerc.c  
ATL_F77wrap_zptgeru.o    : ../f77wrap/ATL_F77wrap_geru.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_geru.c  
ATL_F77wrap_zpthbmv.o    : ../f77wrap/ATL_F77wrap_hbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_hbmv.c  
ATL_F77wrap_zpthpmv.o    : ../f77wrap/ATL_F77wrap_hpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_hpmv.c  
ATL_F77wrap_zpthemv.o    : ../f77wrap/ATL_F77wrap_hemv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_hemv.c  
ATL_F77wrap_zpthpr.o     : ../f77wrap/ATL_F77wrap_hpr.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_hpr.c   
ATL_F77wrap_zpther.o     : ../f77wrap/ATL_F77wrap_her.c    $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_her.c   
ATL_F77wrap_zpthpr2.o    : ../f77wrap/ATL_F77wrap_hpr2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_hpr2.c  
ATL_F77wrap_zpther2.o    : ../f77wrap/ATL_F77wrap_her2.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_her2.c  
ATL_F77wrap_zpttbmv.o    : ../f77wrap/ATL_F77wrap_tbmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_tbmv.c  
ATL_F77wrap_zpttpmv.o    : ../f77wrap/ATL_F77wrap_tpmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_tpmv.c  
ATL_F77wrap_zpttrmv.o    : ../f77wrap/ATL_F77wrap_trmv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_trmv.c  
ATL_F77wrap_zpttbsv.o    : ../f77wrap/ATL_F77wrap_tbsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_tbsv.c  
ATL_F77wrap_zpttpsv.o    : ../f77wrap/ATL_F77wrap_tpsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_tpsv.c  
ATL_F77wrap_zpttrsv.o    : ../f77wrap/ATL_F77wrap_trsv.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_trsv.c  
#
ATL_F77wrap_sgemm.o      : ../f77wrap/ATL_F77wrap_gemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_gemm.c  
ATL_F77wrap_ssymm.o      : ../f77wrap/ATL_F77wrap_symm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_symm.c  
ATL_F77wrap_ssyrk.o      : ../f77wrap/ATL_F77wrap_syrk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_syrk.c  
ATL_F77wrap_ssyr2k.o     : ../f77wrap/ATL_F77wrap_syr2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_syr2k.c 
ATL_F77wrap_strmm.o      : ../f77wrap/ATL_F77wrap_trmm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_trmm.c  
ATL_F77wrap_strsm.o      : ../f77wrap/ATL_F77wrap_trsm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSREAL ../f77wrap/ATL_F77wrap_trsm.c  
#
ATL_F77wrap_dgemm.o      : ../f77wrap/ATL_F77wrap_gemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_gemm.c  
ATL_F77wrap_dsymm.o      : ../f77wrap/ATL_F77wrap_symm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_symm.c  
ATL_F77wrap_dsyrk.o      : ../f77wrap/ATL_F77wrap_syrk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_syrk.c  
ATL_F77wrap_dsyr2k.o     : ../f77wrap/ATL_F77wrap_syr2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_syr2k.c 
ATL_F77wrap_dtrmm.o      : ../f77wrap/ATL_F77wrap_trmm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_trmm.c  
ATL_F77wrap_dtrsm.o      : ../f77wrap/ATL_F77wrap_trsm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDREAL ../f77wrap/ATL_F77wrap_trsm.c  
#
ATL_F77wrap_cgemm.o      : ../f77wrap/ATL_F77wrap_gemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_gemm.c  
ATL_F77wrap_chemm.o      : ../f77wrap/ATL_F77wrap_hemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_hemm.c  
ATL_F77wrap_cherk.o      : ../f77wrap/ATL_F77wrap_herk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_herk.c  
ATL_F77wrap_cher2k.o     : ../f77wrap/ATL_F77wrap_her2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_her2k.c 
ATL_F77wrap_csymm.o      : ../f77wrap/ATL_F77wrap_symm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_symm.c  
ATL_F77wrap_csyrk.o      : ../f77wrap/ATL_F77wrap_syrk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_syrk.c  
ATL_F77wrap_csyr2k.o     : ../f77wrap/ATL_F77wrap_syr2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_syr2k.c 
ATL_F77wrap_ctrmm.o      : ../f77wrap/ATL_F77wrap_trmm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_trmm.c  
ATL_F77wrap_ctrsm.o      : ../f77wrap/ATL_F77wrap_trsm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DSCPLX ../f77wrap/ATL_F77wrap_trsm.c  
#
ATL_F77wrap_zgemm.o      : ../f77wrap/ATL_F77wrap_gemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_gemm.c  
ATL_F77wrap_zhemm.o      : ../f77wrap/ATL_F77wrap_hemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_hemm.c  
ATL_F77wrap_zherk.o      : ../f77wrap/ATL_F77wrap_herk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_herk.c  
ATL_F77wrap_zher2k.o     : ../f77wrap/ATL_F77wrap_her2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_her2k.c 
ATL_F77wrap_zsymm.o      : ../f77wrap/ATL_F77wrap_symm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_symm.c  
ATL_F77wrap_zsyrk.o      : ../f77wrap/ATL_F77wrap_syrk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_syrk.c  
ATL_F77wrap_zsyr2k.o     : ../f77wrap/ATL_F77wrap_syr2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_syr2k.c 
ATL_F77wrap_ztrmm.o      : ../f77wrap/ATL_F77wrap_trmm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_trmm.c  
ATL_F77wrap_ztrsm.o      : ../f77wrap/ATL_F77wrap_trsm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DDCPLX ../f77wrap/ATL_F77wrap_trsm.c  
#
ATL_F77wrap_sptgemm.o    : ../f77wrap/ATL_F77wrap_gemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_gemm.c  
ATL_F77wrap_sptsymm.o    : ../f77wrap/ATL_F77wrap_symm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_symm.c  
ATL_F77wrap_sptsyrk.o    : ../f77wrap/ATL_F77wrap_syrk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_syrk.c  
ATL_F77wrap_sptsyr2k.o   : ../f77wrap/ATL_F77wrap_syr2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_syr2k.c 
ATL_F77wrap_spttrmm.o    : ../f77wrap/ATL_F77wrap_trmm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_trmm.c  
ATL_F77wrap_spttrsm.o    : ../f77wrap/ATL_F77wrap_trsm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSREAL ../f77wrap/ATL_F77wrap_trsm.c  
#
ATL_F77wrap_dptgemm.o    : ../f77wrap/ATL_F77wrap_gemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_gemm.c  
ATL_F77wrap_dptsymm.o    : ../f77wrap/ATL_F77wrap_symm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_symm.c  
ATL_F77wrap_dptsyrk.o    : ../f77wrap/ATL_F77wrap_syrk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_syrk.c  
ATL_F77wrap_dptsyr2k.o   : ../f77wrap/ATL_F77wrap_syr2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_syr2k.c 
ATL_F77wrap_dpttrmm.o    : ../f77wrap/ATL_F77wrap_trmm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_trmm.c  
ATL_F77wrap_dpttrsm.o    : ../f77wrap/ATL_F77wrap_trsm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDREAL ../f77wrap/ATL_F77wrap_trsm.c  
#
ATL_F77wrap_cptgemm.o    : ../f77wrap/ATL_F77wrap_gemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_gemm.c  
ATL_F77wrap_cpthemm.o    : ../f77wrap/ATL_F77wrap_hemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_hemm.c  
ATL_F77wrap_cptherk.o    : ../f77wrap/ATL_F77wrap_herk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_herk.c  
ATL_F77wrap_cpther2k.o   : ../f77wrap/ATL_F77wrap_her2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_her2k.c 
ATL_F77wrap_cptsymm.o    : ../f77wrap/ATL_F77wrap_symm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_symm.c  
ATL_F77wrap_cptsyrk.o    : ../f77wrap/ATL_F77wrap_syrk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_syrk.c  
ATL_F77wrap_cptsyr2k.o   : ../f77wrap/ATL_F77wrap_syr2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_syr2k.c 
ATL_F77wrap_cpttrmm.o    : ../f77wrap/ATL_F77wrap_trmm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_trmm.c  
ATL_F77wrap_cpttrsm.o    : ../f77wrap/ATL_F77wrap_trsm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DSCPLX ../f77wrap/ATL_F77wrap_trsm.c  
#
ATL_F77wrap_zptgemm.o    : ../f77wrap/ATL_F77wrap_gemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_gemm.c  
ATL_F77wrap_zpthemm.o    : ../f77wrap/ATL_F77wrap_hemm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_hemm.c  
ATL_F77wrap_zptherk.o    : ../f77wrap/ATL_F77wrap_herk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_herk.c  
ATL_F77wrap_zpther2k.o   : ../f77wrap/ATL_F77wrap_her2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_her2k.c 
ATL_F77wrap_zptsymm.o    : ../f77wrap/ATL_F77wrap_symm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_symm.c  
ATL_F77wrap_zptsyrk.o    : ../f77wrap/ATL_F77wrap_syrk.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_syrk.c  
ATL_F77wrap_zptsyr2k.o   : ../f77wrap/ATL_F77wrap_syr2k.c  $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_syr2k.c 
ATL_F77wrap_zpttrmm.o    : ../f77wrap/ATL_F77wrap_trmm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_trmm.c  
ATL_F77wrap_zpttrsm.o    : ../f77wrap/ATL_F77wrap_trsm.c   $(INCdep)
	$(CC) -o $@ -c $(CCFLAGS) -DATL_USEPTHREADS -DDCPLX ../f77wrap/ATL_F77wrap_trsm.c  
#
fatlas_sptset.o : ../f77wrap/fatlas_set.c
	$(CC) $(CCFLAGS) -DSREAL -DATL_USEPTHREADS -o $@ -c \
           ../f77wrap/fatlas_set.c
fatlas_sptaxpby.o : ../f77wrap/fatlas_axpby.c
	$(CC) $(CCFLAGS) -DSREAL -DATL_USEPTHREADS -o $@ -c \
           ../f77wrap/fatlas_axpby.c
fatlas_dptset.o : ../f77wrap/fatlas_set.c
	$(CC) $(CCFLAGS) -DDREAL -DATL_USEPTHREADS -o $@ -c \
           ../f77wrap/fatlas_set.c
fatlas_dptaxpby.o : ../f77wrap/fatlas_axpby.c
	$(CC) $(CCFLAGS) -DDREAL -DATL_USEPTHREADS -o $@ -c \
           ../f77wrap/fatlas_axpby.c
fatlas_cptset.o : ../f77wrap/fatlas_set.c
	$(CC) $(CCFLAGS) -DSCPLX -DATL_USEPTHREADS -o $@ -c \
           ../f77wrap/fatlas_set.c
fatlas_cptaxpby.o : ../f77wrap/fatlas_axpby.c
	$(CC) $(CCFLAGS) -DSCPLX -DATL_USEPTHREADS -o $@ -c \
           ../f77wrap/fatlas_axpby.c
fatlas_zptset.o : ../f77wrap/fatlas_set.c
	$(CC) $(CCFLAGS) -DDCPLX -DATL_USEPTHREADS -o $@ -c \
           ../f77wrap/fatlas_set.c
fatlas_zptaxpby.o : ../f77wrap/fatlas_axpby.c
	$(CC) $(CCFLAGS) -DDCPLX -DATL_USEPTHREADS -o $@ -c \
           ../f77wrap/fatlas_axpby.c
fatlas_sset.o : ../f77wrap/fatlas_set.c
	$(CC) $(CCFLAGS) -DSREAL -o $@ -c \
           ../f77wrap/fatlas_set.c
fatlas_saxpby.o : ../f77wrap/fatlas_axpby.c
	$(CC) $(CCFLAGS) -DSREAL -o $@ -c \
           ../f77wrap/fatlas_axpby.c
fatlas_dset.o : ../f77wrap/fatlas_set.c
	$(CC) $(CCFLAGS) -DDREAL -o $@ -c \
           ../f77wrap/fatlas_set.c
fatlas_daxpby.o : ../f77wrap/fatlas_axpby.c
	$(CC) $(CCFLAGS) -DDREAL -o $@ -c \
           ../f77wrap/fatlas_axpby.c
fatlas_cset.o : ../f77wrap/fatlas_set.c
	$(CC) $(CCFLAGS) -DSCPLX -o $@ -c \
           ../f77wrap/fatlas_set.c
fatlas_caxpby.o : ../f77wrap/fatlas_axpby.c
	$(CC) $(CCFLAGS) -DSCPLX -o $@ -c \
           ../f77wrap/fatlas_axpby.c
fatlas_zset.o : ../f77wrap/fatlas_set.c
	$(CC) $(CCFLAGS) -DDCPLX -o $@ -c \
           ../f77wrap/fatlas_set.c
fatlas_zaxpby.o : ../f77wrap/fatlas_axpby.c
	$(CC) $(CCFLAGS) -DDCPLX -o $@ -c \
           ../f77wrap/fatlas_axpby.c
########################################################################
#
sclean           :
	rm -f $(slvl1obj) $(ATL_slvl1obj)
	rm -f $(slvl2obj) $(ATL_slvl2obj)
	rm -f $(slvl3obj) $(ATL_slvl3obj)
	rm -f $(aobj) s*.grd
dclean           :
	rm -f $(dlvl1obj) $(ATL_dlvl1obj)
	rm -f $(dlvl2obj) $(ATL_dlvl2obj)
	rm -f $(dlvl3obj) $(ATL_dlvl3obj)
	rm -f $(aobj) d*.grd
cclean           :
	rm -f $(clvl1obj) $(ATL_clvl1obj)
	rm -f $(clvl2obj) $(ATL_clvl2obj)
	rm -f $(clvl3obj) $(ATL_clvl3obj)
	rm -f $(aobj) c*.grd
zclean           :
	rm -f $(zlvl1obj) $(ATL_zlvl1obj)
	rm -f $(zlvl2obj) $(ATL_zlvl2obj)
	rm -f $(zlvl3obj) $(ATL_zlvl3obj)
	rm -f $(aobj) z*.grd
#
sptclean         :
	rm -f $(slvl1obj) $(ATL_sptlvl1obj)
	rm -f $(slvl2obj) $(ATL_sptlvl2obj)
	rm -f $(slvl3obj) $(ATL_sptlvl3obj)
	rm -f $(aobj) s*.grd
dptclean         :
	rm -f $(dlvl1obj) $(ATL_dptlvl1obj)
	rm -f $(dlvl2obj) $(ATL_dptlvl2obj)
	rm -f $(dlvl3obj) $(ATL_dptlvl3obj)
	rm -f $(aobj) d*.grd
cptclean         :
	rm -f $(clvl1obj) $(ATL_cptlvl1obj)
	rm -f $(clvl2obj) $(ATL_cptlvl2obj)
	rm -f $(clvl3obj) $(ATL_cptlvl3obj)
	rm -f $(aobj) c*.grd
zptclean         :
	rm -f $(zlvl1obj) $(ATL_zptlvl1obj)
	rm -f $(zlvl2obj) $(ATL_zptlvl2obj)
	rm -f $(zlvl3obj) $(ATL_zptlvl3obj)
	rm -f $(aobj) z*.grd
#
ptclean          :
	rm -f *.o *.grd
#
clean            :
	rm -f *.o *.grd
#
########################################################################
