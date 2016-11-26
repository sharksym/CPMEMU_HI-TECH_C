#ifndef _MATH_H_
#define _MATH_H_

#define M_PI		3.1415926535	/* pi */
#define M_PI_2		1.5707963267	/* pi/2 */
#define M_PI_4		0.7853981633	/* pi/4 */
#define M_1_PI		0.3183098861	/* 1/pi */
#define M_2_PI		0.6366197723	/* 2/pi */

extern double fabs(double), floor(double), ceil(double), modf(double, double *);
extern double sqrt(double), atof(char *);
extern double sin(double), cos(double), tan(double);
extern double asin(double), acos(double), atan(double), atan2(double, double);
extern double frexp(double, int *), ldexp(double, int);
extern double log(double), log10(double), pow(double, double), exp(double);
extern double sinh(double), cosh(double), tanh(double);

#endif _MATH_H_
