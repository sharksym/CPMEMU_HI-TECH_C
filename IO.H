#ifndef	_IO_H_
#define	_IO_H_

/* header file for MSX-DOS I/O routines for Z-80 C */

/* get basic definitions */
#define	MAXFILE		8		/* max number of files open */

#ifdef DOS2ONLY
/* FCB controls not required on MSX-DOS2 */
#else
/* for CP/M & MSX-DOS */
#define	SECSIZE		128		/* no. of bytes per sector */

struct	fcb
{
	/* 13 Bytes */
	unsigned char	dr;		/* drive code */
	char		name[8];	/* file name */
	char		ft[3];		/* file type */
	unsigned char	ex;		/* file extent */

	/* 19 Bytes */
	char		fil;		/* not used */
	unsigned int	recsiz;		/* record size */
	long		filesize;
	unsigned short	fcbdate;
	unsigned short	fcbtime;
	char		dm[8];		/* CP/M disk map */

	/* 10 Bytes */
	char		nr;			/* next record to read or write */
	unsigned char	ranrec[3];	/* CP/M random record number (24 bit no. ) */
	long		rwp;		/* read/write pointer in bytes */
	unsigned char	use;		/* use flag */
	unsigned char	uid;		/* user id belonging to this file */
};
extern struct	fcb	_fcb[MAXFILE];

struct fcb *	getfcb(void);
unsigned char	getuid(void);
void		setuid(int);
unsigned char	setfcb(struct fcb *, char *);
#endif /* DOS2ONLY */

char		bdos(int, ...);
short		bdoshl(int, ...);	/* bdos call returning value in hl */

/* for MSX-DOS */
/* bdos call returning value in a */
unsigned char msx_bdos(unsigned int,...);
/* bdos call returning value in hl */
unsigned short msx_bdosh(unsigned int,...);

/* for MSX-DOS2 */
struct msxfib
{
	unsigned char	ff;
	unsigned char	name[13];
	unsigned char	attr;
	unsigned int	time;
	unsigned int	date;
	unsigned int	cl;
	long 		size;
	unsigned char	drive;
	unsigned char	internal[38];
};

struct	fileinfo
{
	char		name[13];	/* File name with extension */
	unsigned char	drive;		/* Logical drive A=1, B=2, ...Default=0 */
#ifdef DOS2ONLY
	struct msxfib	tfib;
#else
	struct fcb	tfcb;
	struct msxfib	tfib;
#endif
};

int findfirst(char *path, unsigned int attr, struct fileinfo *pfi);
int findnext(struct fileinfo *pfi);
unsigned setfileattr(char *path, unsigned int attr);

/* flag values in fcb.use */
#define	U_READ	1		/* file open for reading */
#define	U_WRITE	2		/* file open for writing */
#define	U_RDWR	3		/* open for read and write */
#define	U_CON	4		/* device is console */
#define	U_RDR	5		/* device is reader */
#define	U_PUN	6		/* device is punch */
#define	U_LST	7		/* list device */

/* special character values */
#define	CPMETX	032		/* ctrl-Z, CP/M end of file for text */
#define	CPMRBT	003		/* ctrl-C, reboot CPM */

#define	MPM	0x100		/* bit to test for MP/M */
#define	CCPM	0x400		/* bit to test for CCP/M */

#define	ISMPM()	(bdoshl(CPMVERS)&MPM)	/* macro to test for MPM */

/* what to do after you hit return */
#define	EXIT	(*(int (*)())0)	/* where to go to reboot CP/M */

/* BDOS calls */
#define	CPMRCON	1		/* read console */
#define	CPMWCON	2		/* write console */
#define	CPMRRDR	3		/* read reader */
#define	CPMWPUN	4		/* write punch */
#define	CPMWLST	5		/* write list */
#define	CPMDCIO	6		/* direct console I/O */
#define	CPMGIOB	7		/* get I/O byte */
#define	CPMSIOB	8		/* set I/O byte */
#define	CPMRCOB	10		/* read console buffered */
#define	CPMICON	11		/* interrogate console ready */
#define	CPMVERS	12		/* return version number */
#define	CPMRDS	13		/* reset disk system */
#define	CPMLGIN	14		/* log in and select disk */
#define	CPMOPN	15		/* open file */
#define	CPMCLS	16		/* close file */
#define	CPMFFST	17		/* find first */
#define	CPMFNXT	18		/* find next */
#define	CPMDEL	19		/* delete file */
#define	CPMREAD	20		/* read next record */
#define	CPMWRIT	21		/* write next record */
#define	CPMMAKE	22		/* create file */
#define	CPMREN	23		/* rename file */
#define	CPMILOG	24		/* get bit map of logged in disks */
#define	CPMIDRV	25		/* interrogate drive number */
#define	CPMSDMA	26		/* set DMA address for i/o */
#define	CPMSUID	32		/* set/get user id */
#define	CPMRRAN	33		/* read random record */
#define	CPMWRAN	34		/* write random record */
#define	CPMCFS	35		/* compute file size */

/*
 * Declarations for Unix style low-level I/O functions.
 */
int open(char *, int);
int close(int);
int creat(char *,...);		/* Creat for writing only */
int dup(int);
long lseek(int, long, int);
int read(int, void *, unsigned short);
int unlink(char *);
#define remove(f) unlink(f)
int write(int, void *, unsigned short);
int isatty(int);
int chmod(char *, int);
int rename(char *,char *);
int move(char *,char *);
int delete(char *);
int mkdir(char *);
void setattr(char *, unsigned char);
unsigned char getattr(char *);

#define	O_RDONLY	0x00	/* Open for reading only */
#define	O_WRONLY	0x01	/* Open for writing only */
#define	O_RDWR		0x02	/* Open for reading and writing */

/* Additional Function for MSX only */
#define DOSVER (*(char *)0xf313)	/* MSX: 0=DOS1 */

#endif /* _IO_H_ */
