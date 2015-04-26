#ifndef _STDLIB_H_
#define _STDLIB_H_

/* Standard utility functions */

#define	RAND_MAX	32767		/* max value returned by rand() */

extern double	atof(char *);
extern int	atoi(char *);
extern long	atol(char *);
extern int	rand(void);
extern void	srand(unsigned int);
extern void *	calloc(size_t, size_t);
extern void	free(void *);
extern void *	malloc(size_t);
extern void *	realloc(void *, size_t);
extern void	abort(void);
extern void	exit(int);
extern char *	getenv(char *);
extern int	setenv(char *, char *);
extern char *getdir(void);
extern int chgdir(char *path);
extern int	system(char *);
extern void	qsort(void *, size_t, size_t, int (*)(void *, void *));
extern int	abs(int);
extern long	labs(long);

#endif /* _STDLIB_H_ */
