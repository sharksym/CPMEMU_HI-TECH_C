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

#ifdef BL_DISABLE

#asm

;-------------------------------------------------------------------------------
; Startup code for MSX-DOS2 Only
;
	psect	text,global,pure
	psect	data,global
	psect	bss,global

	psect	text
	defs	100h		;Base of CP/M's TPA

	global	start, _main, _exit, __Hbss, __Lbss, __argc_, startup, wrelop

	jp	start		;By Tatsu
				;On MS-DOS, return to MS-DOS
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
	push	hl
	call	_exit
	jp	0

notdos2:
	ld	de,notdos2_mes
	ld	c,9
	call	0005H
	jp	0

notdos2_mes:
	defm	'ERROR: MSX-DOS2 required.'
	defb	0dH,0aH,'$'

	psect	data
nularg:	defb	0

#endasm

#else	/* BL_DISABLE */

/*#define DEBUG_INFO*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <sys.h>
#include <msxio.h>
#include <msxbdos.h>
#include <cpumode.h>
#include <blstd.h>
#include <blstdint.h>
#include <blstdmem.h>
#include <blstdslt.h>

/* Bank Call Routine */
#include <bankcall.h>

#define	ENASLT	00024h
#define	RAMAD1	0f342h
#define	RAMAD2	0f343h

#define	CALSLT	0001Ch
#define	EXPTBL	0FCC1h
#define EXTBIO	0FFCAh




uint8_t SegmentTotal = 0;
uint8_t SegmentFree = 0;
uint16_t MapperTableAddr = 0;

uint16_t MapperALL_SEG = 0;
uint16_t MapperFRE_SEG = 0;
uint16_t MapperRD_SEG = 0;
uint16_t MapperWR_SEG = 0;
uint16_t MapperCAL_SEG = 0;
uint16_t MapperCALLS = 0;
uint16_t MapperPUT_PH = 0;
uint16_t MapperGET_PH = 0;
uint16_t MapperPUT_P0 = 0;
uint16_t MapperGET_P0 = 0;
uint16_t MapperPUT_P1 = 0;
uint16_t MapperGET_P1 = 0;
uint16_t MapperPUT_P2 = 0;
uint16_t MapperGET_P2 = 0;
uint16_t MapperPUT_P3 = 0;
uint16_t MapperGET_P3 = 0;

uint16_t Bank0SegNo = 0;


uint8_t MapperInit(void);
uint8_t MapperAllocUser(void);
uint8_t MapperAllocSys(void);
void MapperFree(uint8_t SegNo);

uint8_t MapperGetPage0(void);
uint8_t MapperGetPage1(void);
uint8_t MapperGetPage2(void);
void MapperPutPage0(uint8_t SegNo);
void MapperPutPage1(uint8_t SegNo);
void MapperPutPage2(uint8_t SegNo);


void BankCallInit(void);
void ISRInit(void);
void ISRDeinit(void);


/*

Next, the program sets the device number of the extended BIOS in register D,
the function number in register E, and required parameters in other registers,
and then calls "EXTBIO" at FFCAh in page-3.
In this case, the stack pointer must be in page-3.
If there is the extended BIOS for the specified device number, the contents of
 the registers AF, BC and HL are modified according to the function;
 otherwise, they are preserved. Register DE is always preserved.
Note that in any cases the contents of the alternative registers
 (AF', BC', DE' and HL') and the index registers (IX and IY) are corrupted.

The functions available in the mapper support extended BIOS are:


* Get mapper variable table

	Parameter:	A = 0
			D = 4 (device number of mapper support)
			E = 1
	Result:		A = slot address of primary mapper
			DE = reserved
			HL = start address of mapper variable table
* Get mapper support routine address

	Parameter:	A = 0
			D = 4
			E = 2
	Result:		A = total number of memory mapper segments
			B = slot number of primary mapper
			C = number of free segments of primary mapper
			DE = reserved
			HL = start address of jump table

address			function
+0		Slot address of the mapper slot.
+1		Total number of 16k RAM segments. 1...255 (8...255 for the primary)
+2		Number of free 16k RAM segments.
+3		Number of 16k RAM segments allocated to the system
+4		Number of 16k RAM segments allocated to the user.
+5...+7		Unused. Always zero.
+8...		Entries for other mapper slots. If there is none, +8 will be zero.
A program uses the mapper support code by calling various subroutines.
These are accessed through a jump table which is located in the MSX-DOS
system area.

The contents of the jump table are as follows:

address	entry name	function
+0H	ALL_SEG     Allocate a 16k segment.
+3H	FRE_SEG     Free a 16k segment.
+6H	RD_SEG      Read byte from address A:HL to A.
+9H	WR_SEG      Write byte from E to address A:HL.
+CH	CAL_SEG     Inter-segment call. Address in IYh:IX
+FH	CALLS       Inter-segment call. Address in line after the call instruction.
+12H	PUT_PH      Put segment into page (HL).
+15H	GET_PH      Get current segment for page (HL)
+18H	PUT_P0      Put segment into page 0.
+1BH	GET_P0      Get current segment for page 0.
+1EH	PUT_P1      Put segment into page 1.
+21H	GET_P1      Get current segment for page 1.
+24H	PUT_P2      Put segment into page 2.
+27H	GET_P2      Get current segment for page 2.
+2AH	PUT_P3      Not supported since page-3 must never be changed.
+2DH	GET_P3      Get current segment for page 3.

*/



#asm

;-------------------------------------------------------------------------------
; Startup code for MSX-DOS2 Only
;
	psect	text,global,pure
	psect	data,global
	psect	bss,global

	psect	text
	defs	100h		;Base of CP/M's TPA

	global	start, _main_loader, _exit, __Hbss, __Lbss, __argc_, startup, wrelop

	jp	start		;By Tatsu
				;On MS-DOS, return to MS-DOS
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
	call	_main_loader
	push	hl
	call	_exit
	jp	0

notdos2:
	ld	de,notdos2_mes
	ld	c,9
	call	0005H
	jp	0

notdos2_mes:
	defm	'ERROR: MSX-DOS2 required.'
	defb	0dH,0aH,'$'

	psect	data
nularg:	defb	0

#endasm


static int8_t bl_tsr_mode = 0;
#ifdef BL_TSR
static int8_t bl_tsr_file_exist = 0;
#endif

static int16_t SegCnt = 0;
static int16_t free_seg_no = 0;
static uint8_t cFileHandle = 0;

#ifdef BL_TSR
static int8_t pTsrName[64];
static int8_t pTsrEnvName[16];
static char *pTsrEnv = NULL;
#endif
static char *pOvlName = NULL;
static int16_t OvlLen = 0;
static long OvlSize = 0;

static uint8_t Page2Old = 0;

/* Memory Segment Information */
struct bl_mem_seg_t {
	short BankMax;
	uint8_t BankTbl[128];
};

#ifndef BL_1BANK
static struct bl_mem_seg_t tMemSeg;
#endif
struct bl_irq_t {
	uint8_t *stat;
	uint16_t *addr;
	uint8_t *bank;
};

static struct bl_irq_t *pIRQ_start = NULL;


/* for BLSTDLIB */
static uint16_t mem_gap = 0;
static int8_t *pDummy = NULL;

static int16_t ret_val = 0;
int main(int argc, char *argv[]);
long _fsize(int fd);				/* Get File Size */

int main_loader(int argc, char *argv[])
{
#ifdef BL_TSR
	static int8_t name_cnt, name_pos;
#endif

#ifdef R800ONLY
	if (get_msx_version() == MSXTR) {
		set_cpu_mode_tr(CPU_TR_R800_DRAM);
	} else {
		puts("ERROR: MSXturboR required.");
		return 0;
	}
#endif

	pIRQ_start = (struct bl_irq_t *)(BankIRQ_addr + 1);

	/* Move Bank_Call Routine to 08000H */
	memcpy((int8_t *)0x8000, BankCallBin, sizeof(BankCallBin));

	free_seg_no = MapperInit();
#ifdef DEBUG_INFO
	printf("Free seg no = %d\n", free_seg_no);
#endif

	pOvlName = getenv("PROGRAM");			/* Get Full Path of PROGRAM */
	OvlLen = strlen(pOvlName);
	pOvlName[OvlLen - 3] = 'O';
	pOvlName[OvlLen - 2] = 'V';
	pOvlName[OvlLen - 1] = 'L';

#ifdef BL_TSR
	strcpy(pTsrName, pOvlName);
	pTsrName[OvlLen - 3] = 'T';
	pTsrName[OvlLen - 2] = 'S';
	pTsrName[OvlLen - 1] = 'R';

	for (name_cnt = 0; pTsrName[name_cnt] != 0; name_cnt++) {
		if (pTsrName[name_cnt] == '\\')
			name_pos = name_cnt + 1;
	}
	strcpy(pTsrEnvName, &pTsrName[name_pos]);
	name_pos = strlen(pTsrEnvName);
	pTsrEnvName[name_pos - 4] = '_';
	pTsrEnv = getenv(pTsrEnvName);
	cFileHandle = 0xFF;
	if (pTsrEnv) {
		if ((pTsrEnv[0] == 'O') && (pTsrEnv[1] == 'N'))	/* XXX_TSR = ON? */
			cFileHandle = open(pTsrName, O_RDONLY);	/* Open TSR File */

		free(pTsrEnv);
		if (cFileHandle != 0xFF) {
			bl_tsr_mode = 1;
			bl_tsr_file_exist = 1;
			read(cFileHandle, &tMemSeg, sizeof(tMemSeg));	/* Get segment data */
			close(cFileHandle);
		}
	}
#endif

#ifndef BL_1BANK
	if (!bl_tsr_mode) {
		cFileHandle = open(pOvlName, O_RDONLY);		/* Open OVL File */
		if (cFileHandle == 0xFF) {			/* Not Found? */
			printf("\nERROR: [%s] Not found\n", pOvlName);
			return 0;
		}

		OvlSize = _fsize(cFileHandle);
		tMemSeg.BankMax = (short)(OvlSize >> 15);
		close(cFileHandle);

		if (tMemSeg.BankMax * 2 > free_seg_no) {
			puts("\nERROR: Not enough memory");
			return 0;
		}

#ifdef DEBUG_INFO
		printf("Loading: %s\n", pOvlName);
#endif
	}
#endif

	/* Initialize Bank Caller */
	Bank0SegNo = MapperGetPage1();			/* Page1 Segment No. */
	Bank0SegNo <<= 8;
	Bank0SegNo &= 0xFF00;
	Bank0SegNo |= MapperGetPage0();			/* Page0 Segment No. */
	BankCallInit();

	/* Set Bank0 Table */
	*(BankIndex_addr + 0x00) = MapperGetPage0();
	*(BankIndex_addr + 0x01) = MapperGetPage1();

#ifdef BL_1BANK
	SegCnt = 0;
	cFileHandle = 0xFF;
	OvlSize = 0;
	Page2Old = MapperGetPage2();
#else
	if (bl_tsr_mode) {
		memcpy((BankIndex_addr + 0x02), tMemSeg.BankTbl, sizeof(tMemSeg.BankTbl));
		Page2Old = MapperGetPage2();

		for (SegCnt = 0; SegCnt < tMemSeg.BankMax * 2; SegCnt += 2) {
			MapperPutPage2(tMemSeg.BankTbl[SegCnt]);	/* Set segment */
			memcpy((uint8_t *)0x8000, (uint8_t *)0x0000, 0x0100);
		}
		MapperPutPage2(Page2Old);		/* Set original segment */
	} else {
		/* Load Banked code via Page2 */
		cFileHandle = open(pOvlName, O_RDONLY);	/* re-open */
		Page2Old = MapperGetPage2();

#ifdef DEBUG_INFO
#ifdef BL_TSR
		printf("Allocate sys seg:");
#else
		printf("Allocate usr seg:");
#endif
#endif
		for (SegCnt = 0; SegCnt < tMemSeg.BankMax * 2; SegCnt++, free_seg_no--) {
#ifdef BL_TSR
			tMemSeg.BankTbl[SegCnt] = MapperAllocSys();	/* Allocate system segment */
#else
			tMemSeg.BankTbl[SegCnt] = MapperAllocUser();	/* Allocate user segment */
#endif
#ifdef DEBUG_INFO
			printf(" %02X", tMemSeg.BankTbl[SegCnt]);
#endif
			MapperPutPage2(tMemSeg.BankTbl[SegCnt]);    	/* Set segment */
			read(cFileHandle, (uint8_t *)0x8000, 0x4000);
			if (!(SegCnt & 0x01)) {		/* Page0 area?, Copy 0x0000~0x00FF */
				memcpy((uint8_t *)0x8000, (uint8_t *)0x0000, 0x0100);
			}

			MapperPutPage2(Page2Old);			/* Set original segment */
			*(BankIndex_addr + 0x02 + SegCnt) = tMemSeg.BankTbl[SegCnt];
		}
		close(cFileHandle);
#ifdef DEBUG_INFO
		puts("");
#endif
	}
#endif

	/* Set malloc heap to over 0x9400 */
	pDummy = (int8_t *)malloc(1);
	mem_gap = 0x9400 - (uint16_t)pDummy;
	free(pDummy);
	pDummy = (int8_t *)malloc(mem_gap);

	/* Install ISR */
	ISRInit();

#ifdef DEBUG_INFO
	printf("Free seg no = %d\n", free_seg_no);
#endif

	/* Execute main() function */
	ret_val = main(argc, argv);

	/* Restore Original ISR */
	ISRDeinit();

	free(pDummy);
	free(pOvlName);

#ifdef BL_TSR
	if (bl_tsr_mode && !bl_tsr_file_exist) {	/* TSR File not exist? */
		cFileHandle = creat(pTsrName);		/* Create TSR File */
		if (cFileHandle == 0xFF) {			/* Error? */
			bl_tsr_mode = 0;
		} else {
			/* Leave system memory, save segment information */
			write(cFileHandle, &tMemSeg, sizeof(tMemSeg));
			close(cFileHandle);
		}
	}

	if (!bl_tsr_mode) {
		/* Free all segment */
		for (SegCnt = 0; SegCnt < tMemSeg.BankMax * 2; SegCnt++) {
			MapperFree(tMemSeg.BankTbl[SegCnt]);
		}
	}
#else
#ifdef DEBUG_INFO
	puts("Free segment");
#endif

#ifndef BL_1BANK
	/* Free all segment */
	for (SegCnt = 0; SegCnt < tMemSeg.BankMax * 2; SegCnt++, free_seg_no++) {
		MapperFree(tMemSeg.BankTbl[SegCnt]);
	}
#endif

#endif

#ifdef DEBUG_INFO
	printf("Free seg no = %d\n", free_seg_no);
#endif

	return ret_val;
}

void bl_enable_bios_timi(void)
{
	*BankIRQ_addr |= 0x01;
}

void bl_disable_bios_timi(void)
{
	*BankIRQ_addr &= ~0x01;
}

void bl_enable_mouse_poll(uint8_t port)
{
	*BankIRQ_addr |= 0x02;
	*((uint8_t *)0x82F3) = port;
}

void bl_disable_mouse_poll(void)
{
	*BankIRQ_addr &= ~0x02;
}

/*int16_t bl_request_irq_(uint8_t irq, void (*handler)(void), uint8_t bank)*/
int16_t bl_request_irq_(uint8_t irq, uint16_t handler, uint8_t bank)
{
	struct bl_irq_t *pIRQ = pIRQ_start + irq;

	if (*(pIRQ->stat) & 0x80) {
		return -1;			/* ISR already exist! */
	}

	*(pIRQ->addr) = handler;		/* 1 */
	*(pIRQ->bank) = bank << 1;		/* 2 */
	*(pIRQ->stat) = 0x80;			/* 3 */

	return 0;				/* OK */
}

int16_t bl_free_irq(uint8_t irq)
{
	struct bl_irq_t *pIRQ = pIRQ_start + irq;

	if (*(pIRQ->stat) & 0x80) {
		*(pIRQ->stat) = 0x00;
		return 0;
	}

	return -1;				/* ISR is not exist! */
}

void bl_enable_irq(uint8_t irq)
{
	struct bl_irq_t *pIRQ = pIRQ_start + irq;

	*(pIRQ->stat) |= 0x01;
}

void bl_disable_irq(uint8_t irq)
{
	struct bl_irq_t *pIRQ = pIRQ_start + irq;

	*(pIRQ->stat) &= ~0x01;
}

void bl_tsr_on(void)
{
	if (!bl_tsr_mode) {
#ifdef BL_TSR
		bl_tsr_mode = 1;
		setenv(pTsrEnvName, "ON");
#else
		bl_tsr_mode = 0;
#endif
	}
}

void bl_tsr_off(void)
{
	if (bl_tsr_mode) {
#ifdef BL_TSR
		setenv(pTsrEnvName, "");
#endif
		bl_tsr_mode = 0;
	}
}

int8_t bl_is_tsr_on(void)
{
	return bl_tsr_mode;
}

struct bl_lmem_t *bl_lmem_alloc(uint32_t size)
{
	struct bl_lmem_t *lmem;
	uint8_t page_no, n;

	size += 0x3FFF;			/* 16KB - 1 */
	size >>= 14;			/* N page */
	page_no = (uint8_t)size;

	if (!page_no || (page_no > free_seg_no)) {
		return NULL;
	}

	lmem = (struct bl_lmem_t *)malloc(sizeof(struct bl_lmem_t));
	if (lmem == NULL) {		/* not enough heap */
		return NULL;
	}

	free_seg_no -= page_no;
	lmem->page_max = page_no;
	for (n = 0; n < page_no; n++) {
		lmem->page_tbl[n] = MapperAllocUser();	/* Allocate user segment */
	}
/*	printf("Free seg no = %d\n", free_seg_no);*/

	return lmem;
}

void bl_lmem_free(struct bl_lmem_t *ptr)
{
	uint16_t n;

	if (ptr) {
		free_seg_no += ptr->page_max;
		for (n = 0; n < ptr->page_max; n++) {
			MapperFree(ptr->page_tbl[n]);
		}
		free(ptr);

/*		printf("Free seg no = %d\n", free_seg_no);*/
	}
}

void bl_lmem_copy_to(struct bl_lmem_t *dest, uint32_t addr32, uint8_t *src, uint16_t size)
{
/*	uint8_t page1_seg_old = (uint8_t)(Bank0SegNo >> 8);*/
	uint8_t page_no;
	uint16_t offset;

#asm
	DI
#endasm

	page_no = (uint8_t)(addr32 >> 14);
	offset = (uint16_t)(addr32 & 0x3FFF);
	offset |= 0x4000;

	MapperPutPage1_DI(dest->page_tbl[page_no]);
	while (size--) {
		*((uint8_t *)offset++) = *src++;
		if ((offset == 0x8000) && (size > 1)) {	/* End of mapped area? */
			offset = 0x4000;
			page_no++;	/* for next mem page */
			MapperPutPage1_DI(dest->page_tbl[page_no]);
		}
	}
/*	MapperPutPage1(page1_seg_old);*/
}

void bl_lmem_copy_from(uint8_t *dest, struct bl_lmem_t *src, uint32_t addr32, uint16_t size)
{
/*	uint8_t page1_seg_old = (uint8_t)(Bank0SegNo >> 8);*/
	uint8_t page_no;
	uint16_t offset;

#asm
	DI
#endasm

	page_no = (uint8_t)(addr32 >> 14);
	offset = (uint16_t)(addr32 & 0x3FFF);
	offset |= 0x4000;

	MapperPutPage1_DI(src->page_tbl[page_no]);
	while (size--) {
		*dest++ = *((uint8_t *)offset++);
		if ((offset == 0x8000) && (size > 1)) {	/* End of mapped area? */
			offset = 0x4000;
			page_no++;	/* for next mem page */
			MapperPutPage1_DI(src->page_tbl[page_no]);
		}
	}
/*	MapperPutPage1(page1_seg_old);*/
}

#asm
;-------------------------------------------------------------------------------
; Initialize Banking Helper Routine
;
;void BankCallInit(void);
		GLOBAL _BankCallInit

_BankCallInit:
		LD HL,(_MapperPUT_P0)
		LD DE,(_Bank0SegNo)
		JP BankInit_entry

;-------------------------------------------------------------------------------
; Interrupt Service Routine
;
;void ISRInit(void);
;void ISRDeinit(void);
		GLOBAL _ISRInit
		GLOBAL _ISRDeinit

_ISRInit:	JP ISRInit_entry
_ISRDeinit:	JP ISRDeinit_entry

;-------------------------------------------------------------------------------
; Initialize Mapper Routine
;
;uint8_t MapperInit(void)
		GLOBAL _MapperInit

_MapperInit:
		PUSH IY
		PUSH IX

		LD A,000H
		LD DE,0402H			; parameter
		CALL EXTBIO
		LD (_SegmentTotal),A		; Total number of segment
		LD A,C
		LD (_SegmentFree),A		; Free segment number
		LD (_MapperTableAddr),HL	; Base address

		LD DE,00003H
		LD (_MapperALL_SEG),HL
		ADD HL,DE
		LD (_MapperFRE_SEG),HL
		ADD HL,DE
		LD (_MapperRD_SEG),HL
		ADD HL,DE
		LD (_MapperWR_SEG),HL
		ADD HL,DE
		LD (_MapperCAL_SEG),HL
		ADD HL,DE
		LD (_MapperCALLS),HL
		ADD HL,DE
		LD (_MapperPUT_PH),HL
		ADD HL,DE
		LD (_MapperGET_PH),HL
		ADD HL,DE
		LD (_MapperPUT_P0),HL
		ADD HL,DE
		LD (_MapperGET_P0),HL
		ADD HL,DE
		LD (_MapperPUT_P1),HL
		ADD HL,DE
		LD (_MapperGET_P1),HL
		ADD HL,DE
		LD (_MapperPUT_P2),HL
		ADD HL,DE
		LD (_MapperGET_P2),HL
		ADD HL,DE
		LD (_MapperPUT_P3),HL
		ADD HL,DE
		LD (_MapperGET_P3),HL

		POP IX
		POP IY

		LD L,C				; return value (Free segment number)

		RET

;-------------------------------------------------------------------------------
; Allocate & Free Mapper Page
;
;uint8_t MapperAllocUser(void)
;uint8_t MapperAllocSys(void)
;void MapperFree(uint8_t SegNo)
		GLOBAL _MapperAllocUser
		GLOBAL _MapperAllocSys
		GLOBAL _MapperFree

_MapperAllocUser:
		LD A,000H			; for user segment
		LD B,000H			; for primary mapper
		JR _MapperAlloc

_MapperAllocSys:
		LD A,001H			; for system segment
		LD B,000H			; for primary mapper

_MapperAlloc:
		PUSH IY
		PUSH IX

		LD HL,_MapperAlloc_ret
		PUSH HL				; RET addr

		LD HL,(_MapperALL_SEG)
		JP (HL)

_MapperAlloc_ret:
		JR NC,_MapperAlloc_ok
		LD A,0FFH			; FF means error

_MapperAlloc_ok:
		POP IX
		POP IY

		LD L,A				; return value (allocated segment number)

		EI
		RET

_MapperFree:
		POP BC				; Return Addr
		POP DE				; E = SegNo
		PUSH DE
		PUSH BC

		PUSH IY
		PUSH IX

		LD A,E				; SegNo
		LD B,000H

		LD HL,_MapperFree_ret		; RET addr
		PUSH HL
		LD HL,(_MapperFRE_SEG)
		JP (HL)

_MapperFree_ret:
		POP IX
		POP IY

		EI
		RET

;-------------------------------------------------------------------------------
; Get Mapper Page
;
;uint8_t MapperGetPage0(void)
;uint8_t MapperGetPage1(void)
;uint8_t MapperGetPage2(void)
		GLOBAL _MapperGetPage0
		GLOBAL _MapperGetPage1
		GLOBAL _MapperGetPage2

_MapperGetPage0:
		PUSH IY
		PUSH IX

		LD HL,_MapperGetPage_ret	; RET addr
		PUSH HL
		LD HL,(_MapperGET_P0)
		JP (HL)

_MapperGetPage1:
		PUSH IY
		PUSH IX

		LD HL,_MapperGetPage_ret	; RET addr
		PUSH HL
		LD HL,(_MapperGET_P1)
		JP (HL)

_MapperGetPage2:
		PUSH IY
		PUSH IX

		LD HL,_MapperGetPage_ret	; RET addr
		PUSH HL
		LD HL,(_MapperGET_P2)
		JP (HL)

_MapperGetPage_ret:
		POP IX
		POP IY

		LD H,0
		LD L,A				; Segment No

		EI
		RET

;-------------------------------------------------------------------------------
;void MapperPutPage0(uint8_t SegNo)
;void MapperPutPage1(uint8_t SegNo)
;void MapperPutPage2(uint8_t SegNo)
		GLOBAL _MapperPutPage0
		GLOBAL _MapperPutPage1
		GLOBAL _MapperPutPage2

_MapperPutPage0:
		LD HL,(_MapperPUT_P0)
		JP _MapperPutPageN

_MapperPutPage1:
		LD HL,(_MapperPUT_P1)
		JP _MapperPutPageN

_MapperPutPage2:
		LD HL,(_MapperPUT_P2)
;		JP _MapperPutPageN

_MapperPutPageN:
		POP BC				; Return Addr
		POP DE				; E = SegNo
		PUSH DE
		PUSH BC

		PUSH IY
		PUSH IX

		LD A,E				; SegNo
		LD B,000H

		LD DE,_MapperPutPage_ret	; RET addr
		PUSH DE
		JP (HL)				; CALL MapperPut

_MapperPutPage_ret:
		POP IX
		POP IY

		EI
		RET

;-----------------------------------------------------------------
;void MapperPutPage1_DI(uint8_t SegNo)

_MapperPutPage1_DI:
		DI
		LD HL,(_MapperPUT_P1)
		POP BC				; Return Addr
		POP DE				; E = SegNo
		PUSH DE
		PUSH BC

		PUSH IY
		PUSH IX

		LD A,E				; SegNo
		LD B,000H

		LD DE,_MapperPutPageDI_ret	; RET addr
		PUSH DE
		JP (HL)				; CALL MapperPut

_MapperPutPageDI_ret:
		POP IX
		POP IY

		RET

;-------------------------------------------------------------------------------
; Shared Heap Management
;
;void *	bl_calloc(uint16_t, uint16_t)
;void	bl_free(void *)
;void *	bl_malloc(uint16_t)
;void *	bl_realloc(void *, uint16_t)
		GLOBAL _bl_calloc,_bl_free,_bl_malloc,_bl_realloc
		GLOBAL _calloc,_free,_malloc,_realloc

_bl_calloc:	JP _calloc
_bl_free:	JP _free
_bl_malloc:	JP _malloc
_bl_realloc:	JP _realloc

#endasm

#endif	/* BL_DISABLE */

;
