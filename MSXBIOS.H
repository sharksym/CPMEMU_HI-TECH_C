#ifndef _MSXBIOS_H_
#define _MSXBIOS_H_

/* header file for HI-TECH C msxbios library */

#ifndef uchar
#define	uchar	unsigned char
#endif
#ifndef uint
#define	uint	unsigned int
#endif

struct REGS {
	uchar	f;	/* f-flag */
	uchar	a;	/* accu */
	int	bc;	/* BC */
	int	de;	/* DE */
	int	hl; };	/* HL */

#define	fnkstr(n)	((char *)0xf86f+16*(n))
#define msx2()		(*(uchar*)0xfaf8)

void	calbio(uint,char *);	/* call bios(routine,*regs) */
void	calbas(uint,char *);	/* call basic(routine,*regs) */
void	calsub(uint,char *);	/* call subrom(routine,*regs) */
void	calslt(uchar,uint,char *); /* callslot(slot,address,*regs) */
uchar	rdslt(uchar,uint);	   /* read (slot,address) */
void	wrslt(uchar,uint,uchar);   /* write (slot,address,data */
void	callx(uint,char *);	   /* call (routine,*regs) */

void	inifnk();	/* initialize function keys */
void	disscr();	/* disable screen */
void	enascr();	/* enable screen */
void	gicini();	/* init psg */
void	sound(uchar,uchar);	/* sound(register,value */
uchar	rdpsg(uchar);	/* read psg (register) */
uchar	chsns();	/* checks kbhit(), 0 if not else 1 */
char	chget();	/* disbled, use getch() in conio.h */
void	chput(char);	/* print char */
uchar	lptout(char);	/* print char, -1 if break, 1 if busy, 0 if OK */
uchar	lptstt();	/* printer status, returns 1 if printer ready */
char	*pinlin();	/* get line */
char	*inlin();	/* get line */
uchar	breakx();	/* check break, return 1 if CTRL-STOP pressed */
void	beep(); 	/* sound beep */
void	cls();		/* clear screen */
void	locate(uchar,uchar);	/* locate(x,y) on text screen */
void	erafnk();	/* erase function keys */
void	dspfnk();	/* display function keys */
uchar	gtstck(uchar);	/* get stick(ID) status */
uchar	gttrig(uchar);	/* get trig(ID) status */
uchar	gtpad(uchar);	/* get touch pad (ID) status */
uchar	gtpdl(uchar);	/* get paddle (ID) status */
void	chgsnd(uchar);	/* sound on/off (0=off) */
uchar	snsmat(uchar);	/* read keyboard row */
void	kilbuf();	/* kill keyboard buffer */
int	rnd(uint);	/* get random number (range) */
void	di();		/* disable interrupts */
void	ei();		/* enable interrupts */

#endif /* _MSXBIOS_H_ */
