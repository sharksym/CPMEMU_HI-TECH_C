#ifndef _STAT_H_
#define _STAT_H_

struct stat
{
	short	st_mode;	/* flags */
	long	st_atime;	/* access time */
	long	st_mtime;	/* modification time */
	long	st_size;	/* file size in bytes */
};

extern int	stat(char *, struct stat *);

#ifdef DOS2ONLY
/* Flag bits in st_mode */

#define S_IFMT		0x00BF	/* file type mask */
#define S_IFRDO		0x0001	/* read-only file */
#define S_IFHID		0x0002	/* hidden file */
#define S_IFSYS		0x0004	/* system file */
#define S_IFVOL		0x0008	/* volume name */
#define S_IFDIR		0x0010	/* directory */
#define S_IFARC		0x0020	/* archive */
#define S_IFDEV		0x0080	/* device file */
#define S_IPERM		0xFF00	/* file permission mask */
#define S_IREAD		0x0100	/* can be read */
#define S_IWRIT		0x0200	/* can be written */
#define S_IEXEC		0x0400	/* can be executed */

/* MSX-DOS2 fileinfo block attr
   Bit 0 - READ ONLY
   Bit 1 - HIDDEN FILE
   Bit 2 - SYSTEM FILE
   Bit 3 - VOLUME NAME
   Bit 4 - DIRECTORY
   Bit 5 - ARCHIVE BIT
   Bit 6 - Reserved (always 0)
   Bit 7 - DEVICE BIT
*/

#else
/* Flag bits in st_mode */

#define S_IFMT		0x600	/* type bits */
#define S_IFDIR		0x400	/* is a directory */
#define S_IFREG		0x200	/* is a regular file */
#define	S_IREAD		0400	/* file can be read */
#define	S_IWRITE	0200	/* file can be written */
#define	S_IEXEC		0100	/* file can be executed */
#define	S_HIDDEN	0x1000	/* file is hidden */
#define	S_SYSTEM	0x2000	/* file is marked system */
#define	S_ARCHIVE	0x4000	/* file has been written to */
#endif /* DOS2ONLY */

#endif /* _STAT_H_ */

