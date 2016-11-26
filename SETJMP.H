typedef	int	jmp_buf[4];

extern	int	setjmp(jmp_buf);
extern void	longjmp(jmp_buf, int);
