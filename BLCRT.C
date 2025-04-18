/**********************************************************************
 *
 * BLCRT: MSX-DOS2 Banked Application Loader
 *
 * 0100H~7FFFH: Banked Area
 * 8000H~8FFFH: Banking Helper
 * 9000H~93FFH: Internal shared buffer (1KBytes)
 * 9400H~     : Shared Heap
 *
 * 100602 - First version
 *
 *********************************************************************/

#ifdef BL_ROM

#if BL_ROM == 32
#asm
	psect	text
	defs	4000h		;offset for page1
#endasm
#endif

#asm
;-------------------------------------------------------------------------------
; Startup code for ROM loader
;
	psect	text
	defs	100h
	defm	'ROM '		;ID
	defw	start

	global	start, _main, __Hbss, __Lbss, _gcBdosMode

start:	ld	de,__Lbss	;Start of BSS segment
	or	a		;clear carry
	ld	hl,__Hbss
	sbc	hl,de		;size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			;clear memory

	jp	_main
#endasm
#else	/* BL_ROM */

#ifdef BL_DISABLE

#ifdef BL_DOS1

#asm
;-------------------------------------------------------------------------------
; Startup code for CP/M, MSX-DOS1, MSX-DOS2
;
	psect	text
	defs	100h		;Base of CP/M's TPA

	global	start, _main, _exit_hl, __Hbss, __Lbss, __argc_, startup
	global	_GetBdosVersion, _gcBdosMode

start:	ld	hl,(6)		;base address of fdos
	ld	sp,hl		;stack grows downwards
	ld	de,__Lbss	;Start of BSS segment
	or	a		;clear carry
	ld	hl,__Hbss
	sbc	hl,de		;size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			;clear memory
	ld	hl,nularg
	push	hl
	ld	hl,80h		;argument buffer
	ld	c,(hl)
	inc	hl
	ld	b,0
	add	hl,bc
	ld	(hl),0		;zero terminate it
	ld	hl,81h
	push	hl
	call	startup
	pop	bc		;unjunk stack
	pop	bc
	push	hl
	ld	hl,(__argc_)
	push	hl

	; By Yeongman
	; Check BDOS version
	call	_GetBdosVersion
	ld	a,h		; 0:CP/M, 1:MSX-DOS1, 2:MSX-DOS2
	ld	(_gcBdosMode),a

	call	_main
	jp	_exit_hl	; hl <- err no.

	psect	data
nularg:	defb	0

#endasm

#else	/* BL_DOS1 */

#asm
;-------------------------------------------------------------------------------
; Startup code for MSX-DOS2 Only
;
	psect	text
	defs	100h		;Base of CP/M's TPA

	global	start, _main, _exit_hl, __Hbss, __Lbss, __argc_, startup, wrelop

start:
	ld	hl,(6)		;base address of fdos
	ld	sp,hl		;stack grows downwards

	ld	c,6fH		;Check MSX-DOS Version By Tatsu
	call	0005H
	or	a
	jp	nz,notdos2
	ld	a,b
	cp	2
	jp	c,notdos2

	ld	de,__Lbss	;Start of BSS segment
	or	a		;clear carry
	ld	hl,__Hbss
	sbc	hl,de		;size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			;clear memory
	ld	hl,nularg
	push	hl
	ld	hl,80h		;argument buffer
	ld	c,(hl)
	inc	hl
	ld	b,0
	add	hl,bc
	ld	(hl),0		;zero terminate it
	ld	hl,81h
	push	hl
	call	startup
	pop	bc		;unjunk stack
	pop	bc
	push	hl
	ld	hl,(__argc_)
	push	hl
	call	_main
	jp	_exit_hl	;hl <- err no.

notdos2:
	ld	de,notdos2_mes
	ld	c,9
	call	0005H
	jp	0

notdos2_mes:
	defm	'ERROR: MSX-DOS2 required'
	defb	0dH,0aH,'$'

	psect	data
nularg:	defb	0

#endasm

#endif	/* BL_DOS1 */

#else	/* BL_DISABLE */

/* #define BL_DEBUG */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <msxbdos.h>
#include <cpumode.h>
#include <blstd.h>
#include <blstdint.h>
#include <blstdmem.h>
#include <blstdslt.h>

/* Bank Call Routine */
#include <bankcall.h>

#ifdef BL_DEBUG
#define bl_dbg_pr(A)		printf(A)
#define bl_dbg_pr_x(A, X)	printf(A, X)
#else
#define bl_dbg_pr(A)
#define bl_dbg_pr_x(A, X)
#endif

#asm

;-------------------------------------------------------------------------------
; Startup code for MSX-DOS2 Only
;
	psect	text
	defs	100h		;Base of CP/M's TPA

	global	start, _exit_hl, __Hbss, __Lbss, __argc_, startup, wrelop

start:
	ld	hl,(6)		;base address of fdos
	ld	sp,hl		;stack grows downwards

	ld	c,6fH		;Check MSX-DOS Version By Tatsu
	call	0005H
	or	a
	jp	nz,notdos2
	ld	a,b
	cp	2
	jp	c,notdos2

	di			;disable interrupt
	ld	de,__Lbss	;Start of BSS segment
	or	a		;clear carry
	ld	hl,__Hbss
	sbc	hl,de		;size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			;clear memory

	ld	hl,BankCallBin
	ld	de,BankCall_entry
	ld	bc,BankCall_size
	ldir			;transfer BankCall
	call	abort_init
	call	diskerr_init

	ld	hl,nularg
	push	hl
	ld	hl,80h		;argument buffer
	ld	c,(hl)
	inc	hl
	ld	b,0
	add	hl,bc
	ld	(hl),0		;zero terminate it
	ld	hl,81h
	push	hl
	call	startup
	pop	bc		;unjunk stack
	pop	bc
	push	hl
	ld	hl,(__argc_)
	push	hl
	ei			;enable interrupt

	call	_bl_main
	push	hl
	call	diskerr_deinit
	call	abort_deinit
	pop	hl
	jp	_exit_hl	;hl <- err no.

notdos2:
	ld	de,notdos2_mes
	ld	c,9
	call	0005H
	jp	0

notdos2_mes:
	defm	'ERROR: MSX-DOS2 required'
	defb	0dH,0aH,'$'

	psect	data
nularg:	defb	0
#endasm

extern char seg_env_buf[];
extern char assert_msg[];
#asm
_seg_env_buf	equ	BankCallBin		; Recycling
_assert_msg	equ	09000H
#endasm

static int16_t free_seg_no;
static uint8_t seg;

#ifndef BL_1BANK
struct bl_mem_seg_t {				/* Memory Segment Info. */
	short BankMax;
	uint8_t BankTbl[64];
};
static struct bl_mem_seg_t tMemSeg;
static int8_t bl_tsr_mode = 0;
static uint8_t Page2Old;
static short SegCnt;
static short SegOvl;
static char pOvlName[64];

#ifdef BL_TSR
static unsigned char mem_seg_size = sizeof(tMemSeg);
static char pTsrEnvName[14];			/* ENV name */
static char *pTsrEnv = NULL;			/* ENV data */
static int8_t bl_tsr_env_exist = 0;
void MakeTsrEnvName(void);
void put_seg_table(void);
void get_seg_table(void);
#endif
#endif

extern uint8_t bl_abort;

void	bl_init_himem(void);
long    _fsize(int fd);

uint8_t MapperInit(void);
uint8_t MapperAllocUser(void);
uint8_t MapperAllocSys(void);
void    MapperFree(uint8_t SegNo);

uint8_t MapperGetPage0(void);
uint8_t MapperGetPage1(void);
uint8_t MapperGetPage2(void);
void    MapperPutPage0(uint8_t SegNo);
void    MapperPutPage1(uint8_t SegNo);
void    MapperPutPage2(uint8_t SegNo);

void    MakeOvlName(void);
void    BankCallInit(void);
void    ISRInit(void);
void    ISRDeinit(void);

uint8_t	OvlOpen(void);
void	OvlClose(void);
short	OvlGetBankMax(void);
void	OvlLoad16kbPage2(uint8_t SegNo);

void    copy_256_p0_to_p2(void);
void    put_lmem_seg_table(struct bl_lmem_t *ptr);
void    get_lmem_seg_table(struct bl_lmem_t *ptr);

int     main(int argc, char *argv[]);		/* main() */

static int ret_val;
int bl_main(int argc, char *argv[])
{
	bl_init_himem();			/* Init himem */

#ifdef R800ONLY
	if (get_msx_version() == MSXTR) {
		set_cpu_mode_tr(CPU_TR_R800_DRAM);
	} else {
		puts("ERROR: MSXturboR required");
		return 0;
	}
#endif
	free_seg_no = MapperInit();
	bl_dbg_pr_x("[BL] Free seg = %d\n", free_seg_no);

#ifndef BL_1BANK
	MakeOvlName();				/* Get Full Path of *.OVL */

#ifdef BL_TSR
	MakeTsrEnvName();			/* Make Name of environment variable */
	pTsrEnv = getenv(pTsrEnvName);
	if (pTsrEnv && (pTsrEnv[0] == '*')) {
		bl_dbg_pr_x("[BL] Load memory info [%s] ...", pTsrEnvName);
		bl_tsr_mode = 1;
		bl_tsr_env_exist = 1;
		get_seg_table();		/* tMemSeg from pTsrEnv */
		bl_dbg_pr("Ok\n");
	}
#endif

	if (!bl_tsr_mode) {
		if (OvlOpen() == 0xFF) {			/* Open OVL, Not Found? */
			puts("ERROR: OVL not found");
			return 0;
		}

		tMemSeg.BankMax = OvlGetBankMax();
#ifdef BL_BMAX
		if (tMemSeg.BankMax > BL_BMAX)			/* Partial loading? */
			tMemSeg.BankMax = BL_BMAX;
#endif
		OvlClose();

		if (tMemSeg.BankMax * 2 > free_seg_no) {
			tMemSeg.BankMax = bl_cb_adjust_BankMax(free_seg_no / 2);
			if (tMemSeg.BankMax * 2 > free_seg_no) {
				puts("ERROR: Not enough memory");
				return 0;
			}
		}

		bl_dbg_pr_x("[BL] Loading: %s\n", pOvlName);
	}
#endif

	/* Initialize Bank Caller */
	BankCallInit();

#ifndef BL_1BANK
	if (bl_tsr_mode) {
#asm
		; memcpy(Bank_idx_addr + 0x02, tMemSeg.BankTbl, sizeof(tMemSeg.BankTbl));
		LD	HL, _tMemSeg + 2		; HL = tMemSeg.BankTbl
		LD	DE, BankIndex_entry + 2		; DE = Bank_idx_addr + 0x02
		LD	BC, 64				; BC = sizeof(tMemSeg.BankTbl)
		LDIR
#endasm
		Page2Old = MapperGetPage2();

		for (SegCnt = 0; SegCnt < tMemSeg.BankMax * 2; SegCnt += 2) {
			MapperPutPage2(tMemSeg.BankTbl[SegCnt]);	/* Set segment */
			copy_256_p0_to_p2();
		}
		MapperPutPage2(Page2Old);		/* Set original segment */
	} else {
		/* Load Banked code via Page2 */
		OvlOpen();				/* re-open */
		Page2Old = MapperGetPage2();

#ifdef BL_TSR
		bl_dbg_pr("[BL] Allocate sys seg:");
#else
		bl_dbg_pr("[BL] Allocate usr seg:");
#endif
		for (SegCnt = 0; SegCnt < tMemSeg.BankMax * 2; SegCnt++, free_seg_no--) {
#ifdef BL_TSR
			seg = MapperAllocSys();		/* Allocate system segment */
#else
			seg = MapperAllocUser();	/* Allocate user segment */
#endif
			bl_dbg_pr_x(" %02X", seg);

			tMemSeg.BankTbl[SegCnt] = seg;
			*(Bank_idx_addr + 0x02 + SegCnt) = seg;

			OvlLoad16kbPage2(seg);    	/* Set segment & Load 16KB */
			if (!(SegCnt & 0x01))		/* Page0 area?, Copy 0x0000~0x00FF */
				copy_256_p0_to_p2();

			MapperPutPage2(Page2Old);	/* Set original segment */
		}
		OvlClose();
		bl_dbg_pr("\n");
	}
#endif
	/* Disable OPLn Timer */
	bl_opl_timer_off();

	/* Install ISR */
	ISRInit();

	bl_dbg_pr_x("[BL] Free seg = %d\n", free_seg_no);

	/* Backup regs for bl_exit() */
#asm
	LD	(main_ix), IX
	LD	(main_iy), IY
	LD	(main_sp), SP
#endasm
	/* Enable Abort Handler */
	bl_abort_on();

	/* Execute main() function */
	bl_dbg_pr("[BL] main() start\n");
	ret_val = main(argc, argv);
#asm
main_ret:
#endasm
	/* Disable Abort Handler */
	bl_abort_off();

	bl_dbg_pr_x("[BL] main() done (ret = %d)\n", ret_val);

	/* Restore Original ISR */
	ISRDeinit();

	/* Assert message */
	if (*assert_msg)
		puts(assert_msg);

#ifndef BL_1BANK
#ifdef BL_TSR
	if (bl_tsr_mode) {
		if (!bl_tsr_env_exist) {		/* TSR ENV not exist? */
			bl_dbg_pr_x("[BL] Save memory info [%s] ...", pTsrEnvName);
			seg_env_buf[0] = mem_seg_size;
			put_seg_table();		/* tMemSeg to seg_env_buf */
			setenv(pTsrEnvName, seg_env_buf);
			bl_dbg_pr("Ok\n");
		}
	} else {
		if (bl_tsr_env_exist)
			setenv(pTsrEnvName, "");	/* Clear ENV */

		/* Free all segment */
		bl_dbg_pr("[BL] Release seg:");
		for (SegCnt = 0; SegCnt < tMemSeg.BankMax * 2; SegCnt++, free_seg_no++) {
			MapperFree(tMemSeg.BankTbl[SegCnt]);
			bl_dbg_pr_x(" %02X", tMemSeg.BankTbl[SegCnt]);
		}
		bl_dbg_pr("\n");
	}
#else
	/* Free all segment */
	bl_dbg_pr("[BL] Release seg:");
	for (SegCnt = 0; SegCnt < tMemSeg.BankMax * 2; SegCnt++, free_seg_no++) {
		MapperFree(tMemSeg.BankTbl[SegCnt]);
		bl_dbg_pr_x(" %02X", tMemSeg.BankTbl[SegCnt]);
	}
	bl_dbg_pr("\n");
#endif
#endif
	bl_dbg_pr_x("[BL] Free seg = %d\n", free_seg_no);

	return ret_val;
}

#asm
;-------------------------------------------------------------------------------
; Workmem (9000H ~ 93FFH), Comm (9400H ~ )
;
;void bl_init_himem(void);
	global	_bl_init_himem
	global	__Hcomm, _brk
	psect	text
_bl_init_himem:
		LD	HL, __Hcomm
		PUSH	HL			; for brk()
		LD	DE, 09001H
		XOR	A
		SBC	HL, DE
		LD	B, H
		LD	C, L
		LD	HL, 09000H
		LD	(HL), 0
		LDIR
		CALL	_brk
		POP	HL
		RET

;void bl_exit(int n);
	global	_bl_exit
	psect	text
_bl_exit:
		POP	HL
		POP	HL
		LD	(_ret_val), HL

		DEFB	0DDH, 021H		; LD IX, nn
main_ix:	DEFW	0

		DEFB	0FDH, 021H		; LD IY, nn
main_iy:	DEFW	0

		DEFB	031H			; LD SP, nn
main_sp:	DEFW	0

		JP	main_ret		; return to bl_main()

;void bl_abort_on(void);
;void bl_abort_off(void);
		global	_bl_abort_on, _bl_abort_off, _callbdos_ixiy
		psect	text
_bl_abort_on:
		LD	A, 1
		JR	set_bl_abort
_bl_abort_off:
		XOR	A
set_bl_abort:
		LD	(_bl_abort), A
		RET

abort_init:
		LD	HL, _bl_exit
		LD	(_ExitAddr), HL
		LD	DE, _AbortCallback
		JR	set_abort_cb
abort_deinit:
		LD	DE, 0
set_abort_cb:
		LD	C, 063H			; _DEFAB
		JP	_callbdos_ixiy

diskerr_init:
		LD	DE, _DiskECallback
		JR	set_diskerr_cb
diskerr_deinit:
		LD	DE, 0
set_diskerr_cb:
		LD	C, 064H			; _DEFER
		JP	_callbdos_ixiy

; bl_opl_timer_off(void);
		global	_bl_opl_timer_off
		psect	text
_bl_opl_timer_off:
		DI
		LD	DE, 00418H
		LD	BC, 080C0H
		CALL	dis_opl_timer		; OPL C0H
		CALL	dis_opl_timer		; OPL C2H
						; OPL C4H
dis_opl_timer:
		OUT	(C), D
		INC	C
		OUT	(C), E			; R04 = 18H (Disable T1, T2)
		NOP
		OUT	(C), B			; Reset IRQ Flag
		INC	C
		RET
#endasm

#ifndef BL_1BANK
static int fh_ovl;

uint8_t OvlOpen(void)
{
	fh_ovl = open(pOvlName, O_RDONLY);

	return (uint8_t)fh_ovl;
}

void OvlClose(void)
{
	close(fh_ovl);
}

short OvlGetBankMax(void)
{
	return (short)(_fsize(fh_ovl) >> 15);
}

void OvlLoad16kbPage2(uint8_t SegNo)
{
	MapperPutPage2(SegNo);
	read(fh_ovl, (uint8_t *)0x8000, 0x4000);
}

void bl_get_ovl_info(char *name, short *bank_max)
{
	strcpy(name, pOvlName);
	*bank_max = tMemSeg.BankMax;
}

uint16_t bl_get_seg_info(short bank)
{
	return *((uint16_t *)Bank_idx_addr + bank);
}

void bl_overlay_seg(short dest_bank, short ovl_bank)
{
	uint8_t seg0, seg1;

	ISRDeinit();

	SegOvl = (ovl_bank - 1) * 2;
	SegCnt = (dest_bank - 1) * 2;
	seg0 = tMemSeg.BankTbl[SegCnt];
	seg1 = tMemSeg.BankTbl[SegCnt + 1];

	lseek(OvlOpen(), (long)(ovl_bank - 1) * 0x8000, SEEK_SET);
	OvlLoad16kbPage2(seg0);
	copy_256_p0_to_p2();
	OvlLoad16kbPage2(seg1);
	MapperPutPage2(Page2Old);
	OvlClose();

	*(Bank_idx_addr + 0x02 + SegOvl) = seg0;
	*(Bank_idx_addr + 0x03 + SegOvl) = seg1;

	ISRInit();
}

void bl_tsr_on(void)
{
#ifdef BL_TSR
	bl_tsr_mode = 1;
#endif
}

void bl_tsr_off(void)
{
	bl_tsr_mode = 0;
}

int8_t bl_is_tsr_on(void)
{
	return bl_tsr_mode;
}
#endif

uint32_t bl_lmem_get_free(void)
{
	return (uint32_t)free_seg_no * 0x4000;
}

#asm
	global	_bl_lmem_alloc, _bl_lmem_alloc_sys
	psect	text

;struct bl_lmem_t *bl_lmem_alloc(uint32_t size)
_bl_lmem_alloc:
		XOR	A
		JR	bl_lmem_alloc_

;struct bl_lmem_t *bl_lmem_alloc_sys(uint32_t size)
_bl_lmem_alloc_sys:
		LD	A, 1
bl_lmem_alloc_:
		LD	(_lmem_sys_seg), A
		JP	_bl_lmem_alloc_do
#endasm

static uint8_t lmem_sys_seg;
struct bl_lmem_t *bl_lmem_alloc_do(uint32_t size)
{
	static struct bl_lmem_t *lmem;
	uint8_t page_no, n;

	bl_dbg_pr_x("[BL] lmem alloc size = %lu bytes\n", size);
	page_no = (uint8_t)((size + 0x3FFF) >> 14);	/* N page */
	if (!page_no || (page_no > free_seg_no)) {
		return NULL;
	}

	lmem = (struct bl_lmem_t *)malloc(sizeof(struct bl_lmem_t));
	if (lmem == NULL) {		/* not enough heap */
		return NULL;
	}

	free_seg_no -= page_no;
	lmem->page_max = page_no;
	lmem->sys_used = lmem_sys_seg;
	bl_dbg_pr("[BL] seg:");
	for (n = 0; n < page_no; n++) {
		seg = lmem_sys_seg ? MapperAllocSys() : MapperAllocUser();
		lmem->page_tbl[n] = seg;
		bl_dbg_pr_x(" %02X", seg);
	}
	bl_dbg_pr_x("\n[BL] Free seg = %d\n", free_seg_no);

	return lmem;
}

void bl_lmem_free(struct bl_lmem_t *ptr)
{
	uint8_t n;

	if (ptr) {
		free_seg_no += ptr->page_max;
		for (n = 0; n < ptr->page_max; n++) {
			MapperFree(ptr->page_tbl[n]);
		}
		free(ptr);

		bl_dbg_pr_x("[BL] Free seg = %d\n", free_seg_no);
	}
}

uint8_t bl_lmem_get_seg(struct bl_lmem_t *ptr, uint32_t addr32)
{
	return ptr->page_tbl[(uint8_t)(addr32 >> 14)];
}

void bl_lmem_export(struct bl_lmem_t *ptr, char *name)
{
	if (ptr->sys_used) {
		seg_env_buf[0] = ptr->page_max;
		put_lmem_seg_table(ptr);
		setenv(name, seg_env_buf);
	}
}

struct bl_lmem_t *bl_lmem_import(char *name)
{
	static struct bl_lmem_t *lmem;
	static char *env;

	lmem = (struct bl_lmem_t *)malloc(sizeof(struct bl_lmem_t));
	if (lmem == NULL) {		/* not enough heap */
		return NULL;
	}

	env = getenv(name);
	if (env[0] == '*') {
		strcpy(seg_env_buf, env);
		get_lmem_seg_table(lmem);
		lmem->page_max = env[1] - 0x20;
		lmem->sys_used = 1;
	} else {			/* env not found */
		free(lmem);
		return NULL;
	}

	return lmem;
}

void bl_lmem_copy_to(struct bl_lmem_t *dest, uint32_t addr32, uint8_t *src, uint16_t size)
{
	uint8_t lmem_page_no, page1_seg;
	uint16_t lmem_offset;
#asm
	DI
#endasm
	lmem_page_no = (uint8_t)(addr32 >> 14);
	lmem_offset = ((uint16_t)addr32 & 0x3FFF) | 0x4000;

	page1_seg = MapperGetPage1();
	MapperPutPage1(dest->page_tbl[lmem_page_no]);
	while (size--) {
		*((uint8_t *)lmem_offset++) = *src++;
		if ((lmem_offset == 0x8000) && (size > 1)) {	/* End of mapped area? */
			lmem_offset = 0x4000;
			lmem_page_no++;				/* for next mem page */
			MapperPutPage1(dest->page_tbl[lmem_page_no]);
		}
	}
	MapperPutPage1(page1_seg);
#asm
	EI
#endasm
}

void bl_lmem_copy_from(uint8_t *dest, struct bl_lmem_t *src, uint32_t addr32, uint16_t size)
{
	uint8_t lmem_page_no, page1_seg;
	uint16_t lmem_offset;
#asm
	DI
#endasm
	lmem_page_no = (uint8_t)(addr32 >> 14);
	lmem_offset = (((uint16_t)addr32) & 0x3FFF) | 0x4000;

	page1_seg = MapperGetPage1();
	MapperPutPage1(src->page_tbl[lmem_page_no]);
	while (size--) {
		*dest++ = *((uint8_t *)lmem_offset++);
		if ((lmem_offset == 0x8000) && (size > 1)) {	/* End of mapped area? */
			lmem_offset = 0x4000;
			lmem_page_no++;				/* for next mem page */
			MapperPutPage1(src->page_tbl[lmem_page_no]);
		}
	}
	MapperPutPage1(page1_seg);
#asm
	EI
#endasm
}

#ifndef BL_1BANK
#asm
;-------------------------------------------------------------------------------
; Fill pOvlName[]
;
;void MakeOvlName(void);

		psect	text
		global	_getenv, _strcpy_de_hl, _strlen_hl
_MakeOvlName:
		LD	HL, _str_program
		PUSH	HL
		CALL	_getenv			; HL <- full path
		POP	BC			; cleanup stack
		LD	DE, _pOvlName
		CALL	_strcpy_de_hl
		CALL	_strlen_hl		; HL <- length
		LD	DE, _pOvlName - 3
		ADD	HL, DE
		LD	(HL), 'O'
		INC	HL
		LD	(HL), 'V'
		INC	HL
		LD	(HL), 'L'
		RET

_str_program:	DEFM	'PROGRAM'
		DEFB	0
#endasm
#endif

#asm
;-------------------------------------------------------------------------------
; Initialize Banking Helper Routine
;
;void BankCallInit(void);

_BankCallInit:
		CALL _MapperGetPage0
		LD E,L				; Page0 segment
		CALL _MapperGetPage1
		LD D,L				; Page1 segment
		LD (BankIndex_entry),DE		; Set Bank0 table
_BankCallInit_P0a:
		LD HL, 0			; LD HL,(_MapperPUT_P0)
		JP BankInit_entry

;-------------------------------------------------------------------------------
; Initialize Mapper Routine
;
;uint8_t MapperInit(void)

_MapperInit:
		PUSH IX

		LD A,000H
		LD DE,0402H			; parameter
		CALL 0FFCAH			; EXTBIO
		LD (_MapperSegTotal),A		; Total number of segment
		LD A,C
		LD (_MapperSegFree),A		; Free segment number
		LD (_MapperTblAddr),HL		; Base address

		LD (_MapperAlloc_a + 1),HL	; ALL_SEG
		LD DE,3
		ADD HL,DE			; FRE_SEG
		LD (_MapperFree_a + 1),HL
		ADD HL,DE			; RD_SEG
		ADD HL,DE			; WR_SEG
		ADD HL,DE			; CAL_SEG
		ADD HL,DE			; CALLS
		ADD HL,DE			; PUT_PH
		ADD HL,DE			; GET_PH
		ADD HL,DE			; PUT_P0
		LD (_BankCallInit_P0a + 1),HL
		LD (_MapperPutPage0 + 1),HL
		ADD HL,DE			; GET_P0
		LD (_MapperGetPage0 + 1),HL
		ADD HL,DE			; PUT_P1
		LD (_MapperPutPage1 + 1),HL
		ADD HL,DE			; GET_P1
		LD (_MapperGetPage1 + 1),HL
		ADD HL,DE			; PUT_P2
		LD (_MapperPutPage2 + 1),HL
		ADD HL,DE			; GET_P2
		LD (_MapperGetPage2 + 1),HL
;		ADD HL,DE			; PUT_P3
;		ADD HL,DE			; GET_P3

		POP IX

		LD L,C				; return value (Free segment number)
		RET
_MapperSegTotal:
		DEFB 0
_MapperSegFree:
		DEFB 0
_MapperTblAddr:
		DEFW 0


;-------------------------------------------------------------------------------
; Allocate & Free Mapper Page
;
;uint8_t MapperAllocUser(void)
;uint8_t MapperAllocSys(void)
;void MapperFree(uint8_t SegNo)

_MapperAllocUser:
		LD A,000H			; for user segment
		LD B,000H			; for primary mapper
		JR _MapperAlloc

_MapperAllocSys:
		LD A,001H			; for system segment
		LD B,000H			; for primary mapper

_MapperAlloc:
_MapperAlloc_a:
		CALL 0				; CALL ALL_SEG
		LD H,0
		LD L,A				; return value (allocated segment number)
		RET NC
		LD L,0FFH			; return value (Error = FFh)
		RET

_MapperFree:
_MapperFree_hl:					; BLOPTIM fastcall
		LD A,L				; SegNo
		LD B,000H
_MapperFree_a:
		JP 0				; JP FRE_SEG

;-------------------------------------------------------------------------------
; Get Mapper Page
;
;uint8_t MapperGetPage0(void)
;uint8_t MapperGetPage1(void)
;uint8_t MapperGetPage2(void)

_MapperGetPage0:
		CALL 0				; CALL GET_P0
		LD H,0
		LD L,A
		RET

_MapperGetPage1:
		CALL 0				; CALL GET_P1
		LD H,0
		LD L,A
		RET

_MapperGetPage2:
		CALL 0				; CALL GET_P2
		LD H,0
		LD L,A
		RET

;-------------------------------------------------------------------------------
;void MapperPutPage0(uint8_t SegNo)
;void MapperPutPage1(uint8_t SegNo)
;void MapperPutPage2(uint8_t SegNo)

_MapperPutPage0:
_MapperPutPage0_hl:				; BLOPTIM fastcall
		LD DE,0				; (_MapperPUT_P0)
		JR _MapperPutPageN

_MapperPutPage1:
_MapperPutPage1_hl:				; BLOPTIM fastcall
		LD DE,0				; (_MapperPUT_P1)
		JR _MapperPutPageN

_MapperPutPage2:
_MapperPutPage2_hl:				; BLOPTIM fastcall
		LD DE,0				; (_MapperPUT_P2)
;		JR _MapperPutPageN

_MapperPutPageN:
		EX DE,HL			; E <- SegNo, HL <- PUT_Pn

		LD A,E				; SegNo
		JP (HL)				; JP PUT_Pn

;-------------------------------------------------------------------------------
; Shared Heap Management
;
;void *bl_calloc(uint16_t, uint16_t)
;void  bl_free(void *)
;void *bl_malloc(uint16_t)
;void *bl_realloc(void *, uint16_t)
;void *bl_get_memtop(void)
		GLOBAL _bl_calloc,_bl_free,_bl_malloc,_bl_realloc,_bl_get_memtop
		GLOBAL _calloc,_free,_malloc,_realloc,memtop

_bl_calloc:	JP _calloc
_bl_free:	JP _free
_bl_malloc:	JP _malloc
_bl_realloc:	JP _realloc
_bl_get_memtop:	LD HL,(memtop)
		RET
#endasm


#ifndef BL_1BANK
#asm
;-------------------------------------------------------------------------------
; Copy DOS system scratch 256bytes to 8000H
; Used: BC, DE, HL
;void copy_256_p0_to_p2(void)
_copy_256_p0_to_p2:
		LD HL,00000H
		LD DE,08000H
		LD BC,256
		LDIR
		RET
#endasm

#ifdef BL_TSR
#asm
;-------------------------------------------------------------------------------
; Fill pTsrEnvName[]
;
;void MakeTsrEnvName(void);

		psect	text
		GLOBAL	_strcpy_de_hl, _strlen_hl
_MakeTsrEnvName:
		LD	HL, _pOvlName
		LD	DE, _pOvlName
_MakeTsrEnvName_l:
		LD	A, (DE)
		AND	A
		JR	Z, _MakeTsrEnvName_0
		CP	'\'
		INC	DE
		JR	NZ, _MakeTsrEnvName_l
		LD	H, D
		LD	L, E			; HL <- filename only
		JR	_MakeTsrEnvName_l
_MakeTsrEnvName_0:
		LD	DE, _pTsrEnvName
		CALL	_strcpy_de_hl
		CALL	_strlen_hl		; HL <- length
		LD	DE, _pTsrEnvName - 4
		ADD	HL, DE
		LD	(HL), '_'
		INC	HL
		LD	(HL), 'T'
		INC	HL
		LD	(HL), 'S'
		INC	HL
		LD	(HL), 'R'
		RET

;-------------------------------------------------------------------------------
; Put memory segment information to env-string
;
;void put_seg_table(void)
_put_seg_table:
		LD HL,_tMemSeg
		LD DE,_seg_env_buf
		LD A,(HL)			; BankMax value
		INC A
		RLCA				; info *2 bytes
		JP _put_seg_main

;-------------------------------------------------------------------------------
; Get memory segment information from env-string
;
;void get_seg_table(void)
_get_seg_table:
		LD HL,(_pTsrEnv)
		LD DE,_tMemSeg
		LD A,(_mem_seg_size)
		JP _get_seg_main

#endasm
#endif	/* BL_TSR */

#endif	/* BL_1BANK */

#asm
;-------------------------------------------------------------------------------
; Put lmem segment information to env-string
;
;void put_lmem_seg_table(struct bl_lmem_t *ptr)
_put_lmem_seg_table_hl:				; BLOPTIM fastcall
		LD DE,_seg_env_buf
		LD A,(DE)			; segment count
		JP _put_seg_main

;-------------------------------------------------------------------------------
; Get lmem segment information from env-string
;
;void get_lmem_seg_table(struct bl_lmem_t *ptr)
_get_lmem_seg_table_hl:				; BLOPTIM fastcall
		EX DE,HL			; DE <- ptr
		LD HL,_seg_env_buf + 1		; Skip Head '*'
		LD A,(HL)
		SUB 020H			; segment count
		DEC HL				; HL = _seg_env_buf
		JP _get_seg_main

;-------------------------------------------------------------------------------
; _put_seg_main
; HL = segment table address
; DE = target environment string address
; A  = segment count
_put_seg_main:
		LD B,A				; Loop counter
		LD A,'*'			; Add Head '*'
		LD (DE),A
		INC DE
		LD A,B
		ADD A,020H			; count + 0x20
		LD (DE),A
		INC DE
_put_seg_loop:
		LD A,(HL)
		AND 00FH			; Low 4bits
		OR 040H
		LD (DE),A
		INC DE
		LD A,(HL)
		RRA
		RRA
		RRA
		RRA
		AND 00FH			; High 4bits
		OR 040H
		LD (DE),A
		INC DE
		INC HL
		DJNZ _put_seg_loop
		XOR A
		LD (DE),A			; Null-end string
		RET

;-------------------------------------------------------------------------------
; _get_seg_main
; HL = environment string address
; DE = target segment table address
; A  = segment count
_get_seg_main:
		LD B,A
		INC HL				; Skip Head '*'
		INC HL				; Skip (count + 0x20)
_get_seg_loop:
		LD A,(HL)
		INC HL
		AND A
		RET Z				; end?
		AND 00FH			; Low 4bits
		LD C,A
		LD A,(HL)
		INC HL
		RLA
		RLA
		RLA
		RLA
		AND 0F0H			; High 4bits
		OR C
		LD (DE),A
		INC DE
		DJNZ _get_seg_loop
		RET
#endasm

#endif	/* BL_DISABLE */

#endif	/* BL_ROM */
