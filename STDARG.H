#ifndef _STDARG_H_
#define _STDARG_H_

/* Macros for accessing variable arguments */

typedef void *	va_list[1];

#define	va_start(ap, parmn)	*ap = (char *)&parmn + sizeof parmn
#define	va_arg(ap, type)	(*(*(type **)ap)++)
#define	va_end(ap)

#endif /* _STDARG_H_ */

