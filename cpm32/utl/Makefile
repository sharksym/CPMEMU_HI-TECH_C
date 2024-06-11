# Makefile for TCCP.COM and 4GCLCOK.COM    by K.Murakami

.SUFFIXES:	.c .obj .as .lib

LIBR	= cpm -h libr
AS	= cpm -h zas
CC	= cpm -h c
LINK	= cpm -h link
OBJHEX	= cpm -h objtohex
ENHUFF	= cpm -h enhuff
CFLAGS	= -O -x $(DEFS)
ASFLAGS	= -j

.c.obj:
	$(CC) $(CFLAGS) -c $*.c

.as.obj: 
	$(AS) $(ASFLAGS) -L$*.lst $*.as

all: rccp.com 4Gclock.com info0p.com

rccp.com: loader.obj rccp.obj
	$(LINK) -l -ploader=100H,text,bss -Drccp.sym loader.obj rccp.obj
	$(OBJHEX) -R -B100H l.obj rccp.com
	DEL l.obj

tccp0.com: rccp.obj
	$(LINK) -Z -Ptext=100H -C100H -Dtccp0.sym -Orccp0.com rccp.obj

4Gclock.com: 4Gclock.obj
	$(LINK) -Z -Ptext=100H -C100H -O4Gclock.com 4Gclock.obj

info0p.com: info0p.c
	$(CC) $(CFLAGS) $*.c


clean:
	del /F *.obj *.LST *.SYM *.COM
