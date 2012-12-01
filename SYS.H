/*
 *	System-dependent functions.
 */

extern int	execl(char *, char *, char *, ...);
extern int	execv(char *, char **);
extern int	spawnl(char *, char *, char *, ...);
extern int	spawnv(char *, char **);
extern int	spawnle(char *, char *, char *, char *, ...);
extern int	spawnve(char *, char **, char *);

/*	Modified
extern int	getuid(void);
extern int	setuid(int);
*/

unsigned char	getuid(void);
void		setuid(int);

extern int	chdir(char *);
extern int	mkdir(char *);
extern int	rmdir(char *);
extern int	getcwd(int);
extern char **	_getargs(char *, char *);
extern int	_argc_;
extern int	inp(int);
extern void	outp(int, int);

