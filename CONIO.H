#ifndef _CONIO_H_
#define _CONIO_H_

/*
 * Low-level console I/O functions
 */

#include <stddef.h>

extern char	getch(void);
extern char	getche(void);
extern void	putch(int);
extern void	ungetch(int);
extern int	kbhit(void);
extern char *	cgets(char *);
extern void	cputs(char *);

#endif /* _CONIO_H_ */
