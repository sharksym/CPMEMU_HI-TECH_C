#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>

#define	BUFSIZ		512
#define	_NFILE		8

#ifndef FILE
extern	struct	_iobuf {
	char *		_ptr;
	int		_cnt;
	char *		_base;
	unsigned char	_flag;
	char		_file;
} _iob[_NFILE];
#endif	FILE

#define	_IOREAD		01
#define	_IOWRT		02
#define	_IORW		03
#define	_IONBF		04
#define	_IOMYBUF	010
#define	_IOEOF		020
#define	_IOERR		040
#define	_IOSTRG		0100
#define	_IOBINARY	0200

#define	FILE		struct _iobuf
#define	EOF		(-1)

#define	stdin		(&_iob[0])
#define	stdout		(&_iob[1])
#define	stderr		(&_iob[2])
#define	getchar()	getc(stdin)
#define	putchar(x)	putc(x,stdout)

/* getc() and putc() must be functions for CP/M to allow the special
 * handling of '\r', '\n' and '\032'. The same for MSDOS except that
 * it at least knows the length of a file.
 */

#define	getc(p)		fgetc(p)
#define	putc(x,p)	fputc(x,p)
#define	feof(p)		(((p)->_flag&_IOEOF)!=0)
#define	ferror(p)	(((p)->_flag&_IOERR)!=0)
#define	fileno(p)	((unsigned char)p->_file)
#define	clrerr(p)	p->_flag &= ~_IOERR
#define	clreof(p)	p->_flag &= ~_IOEOF

#define	L_tmpnam	34	/* max length of temporary names */

#define SEEK_SET	0x00	/* Beginning of the file */
#define SEEK_CUR	0x01	/* Current position of the file pointer */
#define SEEK_END	0x02	/* End of file */

extern int	fclose(FILE *);
extern int	fflush(FILE *);
extern int	fgetc(FILE *);
extern int	ungetc(int, FILE *);
extern int	fputc(int, FILE *);
extern int	getw(FILE *);
extern int	putw(int, FILE *);
extern char *	gets(char *);
extern int	puts(char *);
extern int	fputs(char *, FILE *);
extern int	fread(void *, unsigned, unsigned, FILE *);
extern int	fwrite(void *, unsigned, unsigned, FILE *);
extern int	fseek(FILE *, long, int);
extern int	rewind(FILE *);
extern int	setbuf(FILE *, char *);
extern int	printf(char *, ...);
extern int	fprintf(FILE *, char *, ...);
extern int	sprintf(char *, char *, ...);
extern int	scanf(char *, ...);
extern int	fscanf(FILE *, char *, ...);
extern int	sscanf(char *, char *, ...);
extern int	remove(char *);
extern FILE *	fopen(char *, char *);
extern FILE *	freopen(char *, char *, FILE *);
extern FILE *	fdopen(int, char *);
extern long	ftell(FILE *);
extern char *	fgets(char *, int, FILE *);
extern char *	_bufallo(void);

#endif /* _STDIO_H_ */
