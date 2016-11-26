#ifndef _FLOAT_H_
#define _FLOAT_H_

/* Characteristics of floating types */

#define	DBL_RADIX	2		/* radix of exponent for a double */
#define	DBL_ROUNDS	1		/* doubles round when converted to int */
#define	FLT_RADIX	2		/* radix of float exponent */
#define	FLT_ROUNDS	1		/* float also rounds to int */
#define	FLT_MANT_DIG	24		/* 24 bits in mantissa */
#define	DBL_MANT_DIG	24		/* ditto for double */
#define	DBL_MANT_DIG	24		/* ditto long double */
#define	FLT_EPSILON	-1.192093	/* smallest x, x+1.0 != 1.0 */
#define	DBL_EPSILON	-1.192093	/* smallest x, x+1.0 != 1.0 */
#define	FLT_DIG		6		/* decimal significant digs */
#define	DBL_DIG		6
#define	FLT_MIN_EXP	-62		/* min binary exponent */
#define	DBL_MIN_EXP	-62
#define	FLT_MIN		1.084202e-19	/* smallest floating number */
#define	DBL_MIN		1.084202e-19
#define	FLT_MIN_10_EXP	-18
#define	DBL_MIN_10_EXP	-18
#define	FLT_MAX_EXP	64		/* max binary exponent */
#define	DBL_MAX_EXP	64
#define	FLT_MAX		1.84467e19	/* max floating number */
#define	DBL_MAX		1.84467e19
#define	FLT_MAX_10_EXP	19		/* max decimal exponent */
#define	DBL_MAX_10_EXP	19

/* long double equates to double */

#define	LDBL_MANT_DIG	DBL_MANT_DIG
#define	LDBL_EPSILON	DBL_EPSILON
#define	LDBL_DIG	DBL_DIG
#define	LDBL_MIN_EXP	DBL_MIN_EXP
#define	LDBL_MIN	DBL_MIN
#define	LDBL_MIN_10_EXP	DBL_MIN_10_EXP
#define	LDBL_MAX_EXP	DBL_MAX_EXP
#define	LDBL_MAX	DBL_MAX
#define	LDBL_MAX_10_EXP	DBL_MAX_10_EXP

#endif /* _FLOAT_H_ */

