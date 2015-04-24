SHELL = /bin/sh
CC = gcc
NM = -o
OJ = -c

config: xconfig
	./xconfig
	rm -f ./xconfig

DoNothing:
searchconf: 
	$(CC) -DForceSearch -o xconfig config.c
	./xconfig
	rm -f ./xconfig

xconfig : config.c
	$(CC) -o xconfig config.c

xatlas_tee : bin/atlas_tee.c
	$(CC) -o xatlas_tee bin/atlas_tee.c

error_report:
	$(MAKE) -f Make.top error_report arch=$(arch)

install :
	$(MAKE) -f Make.top install arch=$(arch)

dup0 :
	$(MAKE) -f Make.top dup0 arch=$(arch) arch0=$(arch0)
dup :
	sed -e 's/ARCH = $(arch0)/ARCH = $(arch)/' Make.$(arch0) > Make.$(arch)
	$(MAKE) -f Make.top dup arch=$(arch) arch0=$(arch0)
startup  :
	$(MAKE) -f Make.top startup arch=$(arch)
refresh  :
	$(MAKE) -f Make.top refresh arch=$(arch)
clean    :
	$(MAKE) -f Make.top clean   arch=$(arch)
kill     :
	$(MAKE) -f Make.top kill    arch=$(arch)
killall  :
	$(MAKE) -f Make.top killall arch=$(arch)
sanity_test:
	$(MAKE) -f Make.top sanity_test arch=$(arch)
ptsanity_test:
	$(MAKE) -f Make.top ptsanity_test arch=$(arch)

