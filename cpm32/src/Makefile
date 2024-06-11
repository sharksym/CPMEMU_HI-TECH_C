# Makefile for CPM.EXE

# --- for BCC32 ---
CC  = bcc32
ASM = bcc32
RC  = brcc32
CFLAGS = -O1 -w -WC
AFLAGS = -O1 -w -c
RFLAGS = 
LIBS   = noeh32.lib
# ------------------

# --- for VC++ 2008 ---
#CC  = cl
#ASM = ml
#RC  = rc
#CFLAGS = -O1 -W3 -D_CRT_SECURE_NO_WARNINGS
#AFLAGS = /c
#RFLAGS = /l411
#LIBS   = cpm.res user32.lib
# ------------------

PERL = jperl

OBJS = cpm.obj em180.obj

.c.obj:
	$(CC) $(CFLAGS) -c $<

.asm.obj:
	$(ASM) $(AFLAGS) $<

cpm.exe: $(OBJS) cpm.res
	$(CC) $(CFALGS) $(OBJS) $(LIBS)

cpm.res: cpm.rc
	$(RC) $(RFLAGS) cpm.rc

clean:
	del /F cpm.obj em180.obj cpm.res cpm.tds

$(OBJS): em180.h

em180.asm: z80op.txt mkem180.pl
	$(PERL) mkem180.pl z80op.txt >em180.asm

z80op.txt: mkz80op.pl
	$(PERL) mkz80op.pl >z80op.txt

