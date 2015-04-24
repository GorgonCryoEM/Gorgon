
SHELL = /bin/sh
# include Make.inc
mydir = ${PWD}
# mydir = 'pwd'
atlrun = atlas_run
waitfile=config.tmp
ARCH=archdefault
CC = gcc
atlas_run :
	cd $(atldir) ; ./$(exe) $(args) > $(redir)
atlas_runX :
	ssh $(targ) 'cd $(atldir) ; ./$(exe) $(args) > $(redir)'

$(waitfile) :
	- @sleep 2 

IRunAixInfo: xaixarchinfo
	- rm -f config.tmp
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xaixarchinfo redir=config.tmp
	$(MAKE) config.tmp
	- cat config.tmp
IRunHpuxInfo: xhpuxarchinfo
	- rm -f config.tmp
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xhpuxarchinfo args="-n" \
                redir=config.tmp
	$(MAKE) config.tmp
	- cat config.tmp

xprobe_AltiVec : probe_AltiVec.c
	$(CC) -o $@ probe_AltiVec.c
IRun_AltiVec: xprobe_AltiVec
	- rm -f config.tmp
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_AltiVec redir=config.tmp
	$(MAKE) config.tmp
	- cat config.tmp
xprobe_SSE2 : probe_SSE2.c
	$(CC) -o $@ probe_SSE2.c
IRun_SSE2: xprobe_SSE2
	- rm -f config.tmp
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_SSE2 redir=config.tmp
	$(MAKE) config.tmp
	- cat config.tmp
xprobe_SSE1 : probe_SSE1.c
	$(CC) -o $@ probe_SSE1.c
IRun_SSE1: xprobe_SSE1
	- rm -f config.tmp
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_SSE1 redir=config.tmp
	$(MAKE) config.tmp
	- cat config.tmp
xprobe_3DNow2 : probe_3DNow2.c
	$(CC) -o $@ probe_3DNow2.c
IRun_3DNow2: xprobe_3DNow2
	- rm -f config.tmp
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_3DNow2 redir=config.tmp
	$(MAKE) config.tmp
	- cat config.tmp
xprobe_3DNow1 : probe_3DNow1.c
	$(CC) -o $@ probe_3DNow1.c
IRun_3DNow1: xprobe_3DNow1
	- rm -f config.tmp
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_3DNow1 redir=config.tmp
	$(MAKE) config.tmp
	- cat config.tmp

IRun_AS_OSX_PPC :
	gcc -o xprobe_as_osx_ppc probe_asm.c probe_as_osx_ppc.s
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_as_osx_ppc redir=config.tmp
	- cat config.tmp
IRun_GAS_LINUX_PPC :
	gcc -o xprobe_gas_linux_ppc probe_asm.c probe_gas_linux_ppc.s
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_gas_linux_ppc redir=config.tmp
	- cat config.tmp
IRun_GAS_SPARC :
	gcc -o xprobe_gas_sparc probe_asm.c probe_gas_sparc.s
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_gas_sparc redir=config.tmp
	- cat config.tmp
IRun_GAS_x8664 :
	gcc -o xprobe_gas_x8664 probe_asm.c probe_gas_x8664.s
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_gas_x8664 redir=config.tmp
	- cat config.tmp
IRun_GAS_x8632 :
	gcc -o xprobe_gas_x8632 probe_asm.c probe_gas_x8632.s
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xprobe_gas_x8632 redir=config.tmp
	- cat config.tmp

IRunName:
	- rm -f fname.o cname.o xfcnam config.tmp
	$(MAKE) fname.o cname.o F77=$(F77) CC=$(CC) \
                F77FLAGS="$(F77FLAGS)" CCFLAGS="$(CCFLAGS)"
	$(FLINK) $(FLFLAGS) -o ./xfcnam fname.o cname.o
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xfcnam redir=config.tmp
	$(MAKE) config.tmp
	- cat config.tmp
	- rm -f fname.o cname.o xfcnam

IRunInt:
	- rm -f ff2cint.o cf2cint.o xfcint
	$(MAKE) ff2cint.o cf2cint.o F77=$(F77) CC=$(CC) \
                F77FLAGS="$(F77FLAGS)" CCFLAGS="$(CCFLAGS)"
	$(FLINK) $(FLFLAGS) -o ./xfcint ff2cint.o cf2cint.o
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xfcint redir=config.tmp
	$(MAKE) config.tmp
	cat config.tmp
	- rm -f ff2cint.o cf2cint.o xfcint
	
IRunStr:
	- rm -f ff2cstr.o cf2cstr.o xfcstr
	$(MAKE) ff2cstr.o cf2cstr.o F77=$(F77) CC=$(CC) \
                F77FLAGS="$(F77FLAGS)" CCFLAGS="$(CCFLAGS)"
	$(FLINK) $(FLFLAGS) -o ./xfcstr ff2cstr.o cf2cstr.o
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xfcstr redir=config.tmp
	$(MAKE) config.tmp
	cat config.tmp
	- rm -f ff2cstr.o cf2cstr.o xfcstr

IRunSol:
	rm -f SolErr.o xsoltst
	$(CC) $(CCFLAGS) -o xsoltst SolErr.c
	$(MAKE) $(atlrun) atldir=$(mydir) exe=xsoltst redir=config.tmp
	$(MAKE) config.tmp
	- rm -f SolErr.o xsoltst
IGetSunVers:
	rm -f config.tmp
	- cc -V > config.tmp 2>&1
	$(MAKE) config.tmp

IBlasLink :
	$(F77) $(F77FLAGS) -o xtst ./blaslink.f $(BLASlib)
	- rm -f xtst blaslink.o

cTryComp :
	$(COMP) $(FLAGS) -c ./tst.c
	- rm -f tst.o
fTryComp :
	$(COMP) $(FLAGS) -c ./tst.f
	- rm -f tst.o
	
wincc :
	gcc -mno-cygwin -o $(wcname) '$(wcexe)' wincc.c
winf77 :
	gcc -mno-cygwin -o $(wcname) '$(wcexe)' winf77.c

winf77.exe : winf77.c defdf.h
	gcc -mno-cygwin -I./ -o winf77.exe winf77.c

winf77bob : ../bin/$(ARCH)/winf77.exe

../bin/$(ARCH)/winf77.exe : winf77.c defdf.h
	gcc -mno-cygwin -I./ -o ../bin/$(ARCH)/winf77.exe winf77.c

xhpuxarchinfo : archinfo_hpux.c
	cc -Ae -o $@ archinfo_hpux.c
xaixarchinfo : archinfo_aix.c
	xlc -o $@ archinfo_aix.c

.c.o :
	$(CC) -c $(CCFLAGS) $<
.f.o :
	$(F77) -c $(F77FLAGS) $<

