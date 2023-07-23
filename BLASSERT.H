#ifndef _BL_ASSERT_H_
#define _BL_ASSERT_H_

/*
 * Assertion - use liberally for debugging. Defining NDEBUG
 * turns assertions off.
 * assert(exp) where exp is non-zero does nothing, while
 * assert(exp) where exp evaluates to zero aborts the program
 * with a message like
 *
 * Assertion failed: prog.c line 123: "exp"
 *
 */

#ifndef	NDEBUG
void _fassert(int, char *, char *);
void bl_exit(int n);
#define	assert(exp)	if(!(exp)) {_fassert(__LINE__, __BL_FILE__, "exp");bl_exit(0);}
#else
#define	assert(exp)
#endif

#endif /* _BL_ASSERT_H_ */
