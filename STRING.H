#ifndef _STRING_H_
#define _STRING_H_

/* String functions */

extern void *	memcpy(void *, void *, size_t);
extern void *	memmove(void *, void *, size_t);
extern void *	memset(void *, int, size_t);
extern char *	strcat(char *, char *);
extern char *	strncat(char *, char *, size_t);
extern char *	strcpy(char *, char *);
extern char *	strncpy(char *, char *, size_t);
extern char *	strdup(char *);
extern int	memcmp(void *, void *, size_t);
extern int	strcmp(char *, char *);
extern int	strncmp(char *, char *, size_t);
extern int	strcasecmp(char *, char *);
#define	stricmp	strcasecmp
extern int	strncasecmp(char *, char *, size_t);
#define	strnicmp	strncasecmp
extern void *	memchr(void *, int, size_t);
extern size_t	strlen(char *);
extern char *	strchr(char *, int);
extern char *	index(char *, int);
extern char *	strrchr(char *, int);
extern char *	rindex(char *, int);
extern char *	strstr(char*, char*);
extern char *	strtok(char *, char *);
extern char *	strcasestr(char*, char*);
#define	stristr	strcasestr
extern char *	strnstr(char*, char*, size_t);
extern char *	strncasestr(char*, char*, size_t);
#define	strnistr	strncasestr

#endif /* _STRING_H_ */
