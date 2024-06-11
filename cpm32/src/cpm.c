/***********************************************************************/
/*           CP/M-80 V2.2 emulator for WIN32 v0.4                      */
/*                        cpm.exe                                      */
/*         Copyright (C) 2004-2012 by Keiji Murakami                   */
/***********************************************************************/
/*  2005/09 FCB rc 8bits->7bits (for PL/I-80 Linker)              K.M  */
/*  2005/12 Ctrl-C trap                                           K.M  */
/*  2007/01 delete CR on stdout                                   K.M  */
/*  2008/12 modify FCB reuse method (for TURBO PASCAL)            K.M  */
/*  2012/02 screen clear char change \0 to ' ' (for wineconsole)  K.M  */
/*  2012/02 BDOS 36 (set random record) bug-fix (for UNARC)       K.M  */
/*  2012/03 DEL code, debug message bug-fix                       N.F  */
/*  2012/03 adapt to VC++                                    N.F, K.M  */
/*  2012/03 add option -C (args to uppercase) (for ZLINK)    N.F, K.M  */
/*  2012/03 FCB terminate on [=:;<>]                         N.F, K.M  */
/*  2012/03 Fix screen control function                      N.F, K.M  */
/***********************************************************************/

/* for BORAND C++ */
#ifdef __BORLANDC__
#pragma resource "cpm.res"
#endif

/* for VC++ 2008/2010 */
#ifdef _MSC_VER
#pragma warning(disable:4996) 
#endif

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include "em180.h"

#define TRUE  1
#define FALSE 0

#define MAXFCB 64

int debug_flag;
#define DEBUGOUT if (debug_flag) fprintf

/* ============ Configuration variable ============ */
byte kbwait;		/* wait on console status check */
int pause_flag;		/* pause before exit */
int retcode_flag;	/* exit code type: 0..none 1:HI-TECH C 2:BDSC */
int no_auto_assign;	/* disable auto drive assign */
int uppercase_flag;	/* force to uppercase */

enum { RC_HITECH = 1, RC_BDSC};

/* ============ I/O callback ============ */
int io_input( int add) {
    fprintf( stderr, "ERROR: i/o read port:%04x\n", add);
    exit( -1);
    return 0;
}
void io_output( int add, int data) {
    fprintf( stderr, "ERROR: i/o write port:%04x <- %02x\n", add, data);
    exit( -1);
}

#define EMST_BDOS EMST_FUNC
#define EMST_BIOS_BASE (EMST_FUNC+1)

#define RETEM    0xed,0xfd,
#define CALLN(v) 0xed,0xed,(v),
#define JP(x)    0xc3,(byte)(x),(byte)((x)>>8),
#define RET	 0xc9,
#define HALT	 0x76

byte bdos_code[] = {
	CALLN( 0)
	RET
};

#define BDOS_ORG 0xfe00
#define BIOS_ORG 0xff00

#define DMY_DPB   (BDOS_ORG + 16)
#define DMY_DPB_SFT (DMY_DPB + 2)
#define DMY_DPB_MSK (DMY_DPB + 3)
#define DMY_DPB_MAX (DMY_DPB + 5)
#define DMY_ALLOC (BDOS_ORG + 32)
#define MAXBLK (128*8)

#define setword( p, x) { (p) = (byte)(x); (p) = (byte)((x)>>8); }

/* ================== CP/M BDOS emulation ================== */

#define MAXDRV 16

byte cpm_usr_no = 0;
byte cpm_disk_no = 'B'-'A';
word cpm_dma_addr = 0x80;
word cpm_version = 0x22;

word cpm_disk_vct = 0;
char *cpm_drive[ MAXDRV];

int abort_submit;

FILE *punfp, *rdrfp, *lstfp;

char filename[ 1024];
char filename2[ 1024];

#define CPMPATH "CPMPATH"

char *auto_drive_assign( char **arg)
{
    DWORD att;
    char *p;
    static char argbuf[ 256];
    static char *argp = argbuf;
    static int n = 2;

    if ( !no_auto_assign && ( p = strrchr( *arg, '\\')) != NULL) {
	*p = '\0';
	att = GetFileAttributes( *arg);
	*p = '\\';
	if ( *arg == p || 
	    ( att != (DWORD)-1 && ( att & FILE_ATTRIBUTE_DIRECTORY))) {
	    cpm_drive[ n] = *arg;
	    *arg = argp;
	    argp += sprintf( argp, "%c:%.124s", n++ + 'A', ++p) + 1;
	    *p = '\0';
	}
    }
    return *arg;
}

void setup_disk_vct( void)
{
    int i;
    char envname[ 8];

    cpm_disk_vct = 3;
    for ( i = 2; i < MAXDRV; i++) {
	if ( cpm_drive[ i]) continue;
	sprintf( envname, "CPM%c", i + 'A');
	if (( cpm_drive[ i] = getenv( envname)) != NULL) {
	    cpm_disk_vct |= (word)( 1 << i);
	}
    }
}

int load_program( char *pfile)
{
    FILE *fp;
    char drv[ _MAX_DRIVE], dir[ _MAX_DIR], fname[ _MAX_FNAME], ext[ _MAX_EXT];

    _splitpath( pfile, drv, dir, fname, ext);
    if ( drv[ 0] == '\0' && dir[ 0] == '\0') {
	if ( ext[ 0] == '\0') {
	    _makepath( filename2, drv, dir, fname, "cpm");
	    _searchenv( filename2, CPMPATH, filename);
	    if ( filename[ 0] == '\0') {
		_makepath( filename2, drv, dir, fname, "com");
		_searchenv( filename2, CPMPATH, filename);
	    }
	} else {
	    _searchenv( pfile, CPMPATH, filename);
	}
	if ( filename[ 0] == '\0') return FALSE;
	if (( fp = fopen( filename, "rb")) == NULL) return FALSE;
	_splitpath( filename, drv, dir, fname, ext);
   } else if ( ext[ 0] == '\0'){
	_makepath( filename, drv, dir, fname, "cpm");
	if (( fp = fopen( filename, "rb")) == NULL) {
	    _makepath( filename, drv, dir, fname, "com");
	    if (( fp = fopen( filename, "rb")) == NULL) return FALSE;
	}
	_splitpath( filename, drv, dir, fname, ext);
    } else {
	if (( fp = fopen( pfile, "rb")) == NULL) return FALSE;
    }

    fread( mem + 0x100, 1, BDOS_ORG-0x100, fp);
    fclose( fp);

    if ( stricmp( ext, ".com") == 0) cpm_version = 0x122;
    cpm_drive[ 0] = (char *)malloc( strlen( drv) + strlen( dir) + 1);
    _makepath( cpm_drive[ 0], drv, dir, NULL, NULL);
    cpm_drive[ 1] = "";

    return TRUE;
}

void mkFCB( byte *p, char *s)
{
    int i, c;

    *p = 0;
    if ( s[ 1] == ':') {
        if (( c = *s) >= 'a') c &= 0x5f;
        *p = (byte)( c - 'A' + 1); s += 2;
    }
    p++;
    for ( i = 0; i < 8; i++) {
        c = ( *s && *s != '.') ? *s++ : ' ';
//	if ( c >= 'a' && c <= 'z') c &= 0x5f;
        *p++ = (byte)c;
    }
    while ( *s && *s != '.') s++;
    if ( *s) s++;
    for ( i = 0; i < 3; i++) {
        c = ( *s) ? *s++ : ' ';
//	if ( c >= 'a' && c <= 'z') c &= 0x5f;
        *p++ = (byte)c;
    }
}

char *mk_filename( char *s, byte *fcb)
{
    int i, j;
    char *dir;

    i = fcb[ 0];
    if ( i == '?' || i == 0) i = cpm_disk_no;
    else i--;
    if ( i >= MAXDRV) return NULL;
    if ( cpm_drive[ i] == NULL) {
	if ( cpm_disk_vct == 0) setup_disk_vct();
	if ( cpm_drive[ i] == NULL) return NULL;
    }
    strcpy( s, cpm_drive[ i]);
    s += i = strlen( s);
    if ( i && s[ -1] != '\\' && s[ -1] != '/' && s[ -1] != ':') *s++ = '/';
    dir = s;

    for ( j = 8; j >= 1 && fcb[ j] == '?'; j--);
    j++;
    for ( i = 1; i < j; i++) {
	if ( fcb[ i] == ' ') break;
	*s++ = fcb[ i];
    }
    if ( i == j && i <= 8) *s++ = '*';

    *s++ = '.';

    for ( j = 11; j >= 9 && fcb[ j] == '?'; j--);
    j++;
    for ( i = 9; i < j; i++) {
	if ( fcb[ i] == ' ') break;
	*s++ = fcb[ i];
    }
    if ( i == j && i <= 11) *s++ = '*';
    *s = '\0';
    return dir;
}

struct {
    FILE *fp;
    unsigned pos;
    byte *addr;
    byte wr, mod, dmy1, dmy2;
} fcbs[ MAXFCB];

struct FCB {
    byte dr, f[ 8], t[ 3];
    byte ex, s1, s2, rc, d[ 16];
    byte cr, r0, r1, r2;
};

HANDLE hFindFile = INVALID_HANDLE_VALUE;

byte cpm_findnext( void)
{
    WIN32_FIND_DATA aFindData;
    char *p;
    struct FCB *fcb = (struct FCB *)(mem + cpm_dma_addr);

    if ( hFindFile == INVALID_HANDLE_VALUE) return 0xff;

    while ( FindNextFile( hFindFile, &aFindData)) {
	p = aFindData.cAlternateFileName;
	if ( !*p) p = aFindData.cFileName;
	if (( aFindData.dwFileAttributes & 
		(FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN)) == 0) {
	    DWORD l = aFindData.nFileSizeLow;
	    if ( aFindData.nFileSizeHigh || l > 0x7fff80) l = 0x7fff80;
	    l = (l + 0x7f) >> 7;
	    fcb->ex = (byte)(l >> 7);
	    fcb->rc = (byte)(l & 0x7f);
	    mkFCB( mem + cpm_dma_addr, p);
	    return 0;
	}
    }
    FindClose( hFindFile); 
    hFindFile = INVALID_HANDLE_VALUE;
    return 0xff;
}

byte cpm_findfirst( byte *fcbaddr)
{
    WIN32_FIND_DATA aFindData;
    char *p;
    struct FCB *fcb = (struct FCB *)(mem + cpm_dma_addr);
    DWORD l;

    if ( hFindFile != INVALID_HANDLE_VALUE) FindClose( hFindFile);

    if ( !mk_filename( filename, fcbaddr)) return 0xff;

    hFindFile = FindFirstFile( filename, &aFindData);
    if ( hFindFile == INVALID_HANDLE_VALUE) return 0xff;

    p = aFindData.cAlternateFileName;
    if ( !*p) p = aFindData.cFileName;
    if ( aFindData.dwFileAttributes & 
	 (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN)) {
	return cpm_findnext();
    }

    l = aFindData.nFileSizeLow;
    if ( aFindData.nFileSizeHigh || l > 0x7fff80) l = 0x7fff80;
    l = (l + 0x7f) >> 7;
    fcb->ex = (byte)(l >> 7);
    fcb->rc = (byte)(l & 0x7f);
    mkFCB( mem + cpm_dma_addr, p);
    return 0;
}

#define CFRW_SEQ 0
#define CFRW_RND 1
#define CFRW_RD 0
#define CFRW_WR 1
#define CF_OPEN   0
#define CF_WROPEN 1
#define CF_CREATE 2

byte cpm_file_open( byte *fcbaddr, int cr)
{
    int i;
    int d = 0;
    struct FCB *fcb = (struct FCB *)fcbaddr;

    if (( i = fcb->d[ 0]) > 0 && --i < MAXFCB && 
      fcbs[ i].addr == fcbaddr && fcbs[ i].fp) {
DEBUGOUT( stderr, "REOPEN %d - ", i);
      fclose( fcbs[ i].fp); fcbs[ i].fp = NULL;
    } else { // 2008.12 FCBをクリアするアプリに対応
      for ( i = 0; i < MAXFCB && fcbs[ i].fp; i++) {
        if ( fcbs[ i].addr == fcbaddr) {
           fclose( fcbs[ i].fp); fcbs[ i].fp = NULL; break;
        }
      }
    }
    if ( i >= MAXFCB || !mk_filename( filename, fcbaddr)) return 0xff;

    if ( cr == CF_CREATE) {
	if ( access( filename, 0) == 0) return 0xff;
	if (( fcbs[ i].fp = fopen( filename, "w+b")) == NULL) return 0xff;
	fcbs[ i].mod = fcbs[ i].wr = TRUE;
    } else {
	if (( fcbs[ i].fp = fopen( filename, cr ? "r+b" : "rb")) == NULL)
		return 0xff;
	fcbs[ i].mod = fcbs[ i].wr = FALSE;
	fseek( fcbs[ i].fp, 0, SEEK_END);
	d = ftell( fcbs[ i].fp);
	fseek( fcbs[ i].fp, 0, SEEK_SET);
	d = ( d + 0x7f) >> 7;
    }
    fcbs[ i].pos = 0;
    fcbs[ i].addr = fcbaddr;

    memset( fcbaddr + 12, 0, 33-12);
    fcb->d[0] = (byte)(i + 1);
    fcb->d[1] = (byte)d;
    fcb->d[2] = (byte)(d >> 8);
    fcb->d[3] = (byte)(d >> 16);
    fcb->rc = (byte)(( d >= 127) ? 127 : d);
    return 0;
}

byte cpm_file_close( byte *fcbaddr)
{
    int i;
    struct FCB *fcb = (struct FCB *)fcbaddr;

	if (( i = fcb->d[ 0]) > 0 && --i < MAXFCB) {
DEBUGOUT( stderr, "CLOSE %d - ", i);
	    if ( fcbs[ i].fp) fclose( fcbs[ i].fp);
	    fcbs[ i].fp = 0;
	    fcb->d[ 0] = 0;
	    return 0;
	}
	if ( i == 0) return 0;
	return 0xff;
}

void cpm_change_filesize( byte *fcbaddr)
{
    int i, sz;
    struct FCB *fcb = (struct FCB *)fcbaddr;

    sz = (fcb->d[ 3] << 16) + (fcb->d[2] << 8) + fcb->d[1];
    if (( sz >> 7) == fcb->ex && (sz & 0x7f) > fcb->rc &&
	 mk_filename( filename, fcbaddr) &&
	 ( i = open( filename, O_RDWR)) != -1) {
		chsize( i, (( fcb->ex << 7) + fcb->rc) << 7);
		close( i);
DEBUGOUT( stderr, "Change file size : %s\n", filename);
    }
}


byte cpm_file_rw( byte *fcbaddr, byte wr, int rnd)
{
    int i, n, p, max;
    struct FCB *fcb = (struct FCB *)fcbaddr;


    if ( rnd) {
        if ( fcb->r2) return 6;
        p = fcb->r0 + ( fcb->r1 << 8);
        fcb->cr = (byte)(p & 0x7f); fcb->ex = (byte)(p >> 7);
    } else {
        p = (fcb->cr & 0x7f) + (fcb->ex << 7);
    }
    max = fcb->d[1] + (fcb->d[2] << 8) + (fcb->d[3] << 16);

DEBUGOUT( stderr, "BDOS: %s%s file '%11.11s'. @%d", 
      rnd ? "random ": "", wr ? "write" : "read", fcbaddr + 1, p);

    if (( i = fcb->d[ 0]) == 0 || --i >= MAXFCB || fcbs[ i].fp == NULL) {
        if ( !wr && p >= max) return 1;
        if ( cpm_file_open( fcbaddr, wr ? CF_WROPEN : CF_OPEN)) return 0xff;
        fcb->cr = (byte)(p & 0x7f); fcb->ex = (byte)(p >> 7); /* 2012.02 bugfix 8->7 */
        i = fcb->d[ 0] - 1;
    } else if ( !fcbs[ i].mod && wr) {
        fclose( fcbs[ i].fp);
        if ( !mk_filename( filename, fcbaddr) ||
             ( fcbs[ i].fp = fopen( filename, "r+b")) == NULL) {
            cpm_file_close( fcbaddr);
            return 0xff;
        }
        fcbs[ i].pos = 0;
    }

    if ((unsigned)(n = p << 7) != fcbs[ i].pos || fcbs[ i].wr != wr) {
        if (( !wr && p > max) ||
             fseek( fcbs[ i].fp, n, SEEK_SET)) {
            cpm_file_close( fcbaddr);
            return 1;
        }
        fcbs[ i].pos = n;
    }

    if ( wr) {
        fcbs[ i].mod = fcbs[ i].wr = TRUE;
        if ( fwrite( mem + cpm_dma_addr, 128, 1, fcbs[ i].fp) != 1)
             return 1;
        n = 128;
    } else {
        fcbs[ i].wr = FALSE;
        if (( n = fread( mem + cpm_dma_addr, 1, 128, fcbs[ i].fp)) <= 0) {
            if ( !fcbs[ i].mod) cpm_file_close( fcbaddr);
            return 1;
        }
        if ( n < 128) memset( mem + cpm_dma_addr + n, 0x1a, 128 - n);
    }
    fcbs[ i].pos += n;

    ++p;
    if ( !rnd) { fcb->cr = (byte)(p & 0x7f); fcb->ex = (byte)(p >> 7); }
    if ( p >= max && !fcbs[ i].mod) cpm_file_close( fcbaddr);
    if ( p > max) {
        fcb->d[ 1] = (byte)p;
        fcb->d[ 2] = (byte)(p >> 8);
        fcb->d[ 3] = (byte)(p >> 16);
        fcb->rc = (byte)(p & 0x7f);
    }

    return 0;
}

byte cpm_file_size( byte *fcbaddr)
{
    struct FCB *fcb = (struct FCB *)fcbaddr;
    DWORD d;

    if ( cpm_file_open( fcbaddr, CF_OPEN)) return 0xff;
    d = fcb->d[1] + (fcb->d[2] << 8) + (fcb->d[3] << 16);
    fcb->r0 = (byte)d;
    fcb->r1 = (byte)(d >> 8); 
    fcb->r2 = (byte)(d >> 16);
    cpm_file_close( fcbaddr);

    return 0;
}

byte cpm_file_delete( byte *fcbaddr)
{
    struct FCB *fcb = (struct FCB *)fcbaddr;
    int i;
    HANDLE hFindFile;
    WIN32_FIND_DATA aFindData;
    char *fname;
    byte st = 0xff;

    if ( memcmp( fcbaddr, "\1$$$     SUB", 1+8+3) == 0) abort_submit = TRUE;

    if (( i = fcb->d[ 0]) > 0 && --i < MAXFCB && 
		fcbs[ i].addr == fcbaddr && fcbs[ i].fp) {
DEBUGOUT( stderr, "REUSE %d - ", i);
	fclose( fcbs[ i].fp); fcbs[ i].fp = NULL;
    }

    if (( fname = mk_filename( filename, fcbaddr)) == NULL) return 0xff;
    hFindFile = FindFirstFile( filename, &aFindData);
    if ( hFindFile == INVALID_HANDLE_VALUE) return 0xff;
    do {
	char *p = aFindData.cAlternateFileName;
	if ( !*p) p = aFindData.cFileName;
	if ( *p != '.') {
	    strcpy( fname, p);
	    if ( remove( filename) == 0) st = 0;
	}
    } while( FindNextFile( hFindFile, &aFindData));
    FindClose( hFindFile);

    return st;
}


byte cpm_rename( byte *src, byte *dst)
{
    if ( !mk_filename( filename, src)) return 0xff;
    if ( !mk_filename( filename2, dst)) return 0xff;
    return (byte)rename( filename, filename2);
}

byte cpm_set_rndrec( byte *fcbaddr)
{
    struct FCB *fcb = (struct FCB *)fcbaddr;
    unsigned d = (fcb->cr & 0x7f) + (fcb->ex << 7); /* 2012.02 bugfix 8->7 */

    fcb->r0 = (byte)d;
    fcb->r1 = (byte)(d >> 8); 
    fcb->r2 = (byte)(d >> 16);

    return 0;
}

void frameup_dpb_alloc( void)
{
    DWORD spc, bps, free, total;
    DWORD *alloc = (DWORD *)(mem + DMY_ALLOC);
    byte sft;
    int i;

    GetDiskFreeSpace( NULL, &spc, &bps, &free, &total);
    spc *= bps;
    spc >>= 9; sft = 2;
    /* 2008.12 for large disk space (> 2TBytes) */
    while ( spc && (spc & 1) == 0) { spc >>= 1; sft++; }
    free *= spc;
    while ( free > MAXBLK) { free >>= 1; sft++; }
    mem[ DMY_DPB_SFT] = sft;
    *(word *)(mem + DMY_DPB_MSK) = (word)((1 << sft) - 1);

    i = 0;
    while ( free >= 32) {
        free -= 32; alloc[ i++] = 0;
    }
    alloc[ i++] = ~0 << free;
    while ( i < MAXBLK/32) alloc[ i++] = ~0;
}

byte cpm_gettime( byte *buf, time_t t)
{
    int d;
    static time_t time780101;

    if ( time780101 == 0) {
 	struct tm tm;
	memset( &tm, 0, sizeof tm);
	tm.tm_year = 78;
	tm.tm_mon =   0;
	tm.tm_mday =  1;
	time780101 = mktime( &tm);
    }

    t -= time780101;
    d = t % 60; t /= 60;
    buf[ 4] = (byte)((d % 10) + ((d / 10) << 4));
    d = t % 60; t /= 60;
    buf[ 3] = (byte)((d % 10) + ((d / 10) << 4));
    d = t % 24; t /= 24;
    buf[ 2] = (byte)((d % 10) + ((d / 10) << 4));
    d = t + 1;
    buf[ 0] = (byte)d;
    buf[ 1] = (byte)(d >> 8);

    return buf[ 4];
}

byte cpm_disk_reset( void)
{
    HANDLE hFindFile;
    WIN32_FIND_DATA aFindData;

    cpm_disk_no = 0;
    cpm_dma_addr = 0x80;

    sprintf( filename, "%s$*.*", cpm_drive[ cpm_disk_no]);
    hFindFile = FindFirstFile( filename, &aFindData);
    if ( hFindFile == INVALID_HANDLE_VALUE) return 0;
    FindClose( hFindFile);
    return 0xff;
}

/* ================== CP/M LST,RDR,PUN emulation ================== */

void cpm_lst_out( byte c)	/* LST: out */
{
    static int err;

    if ( lstfp == NULL && !err) {
	char *dev = getenv( "CPMLST");
	err = ( lstfp = fopen( dev ? dev : "PRN:", "wb")) == NULL;
    }
    if ( lstfp) putc( c, lstfp);
}

byte cpm_rdr_in( void)		/* RDR: in */
{
    static int err;
    char *dev;
    int c, ct = 16;

    if ( rdrfp == NULL && !err) {
	if (( dev = getenv( "CPMRDR")) != NULL) {
	    err = ( rdrfp = fopen( dev, "r+b")) == NULL;
	} else if ( punfp) {
	    rdrfp = punfp;
	} else {
	    dev = getenv( "CPMAUX");
	    err = ( rdrfp = fopen( dev ? dev : "AUX:", "r+b")) == NULL;
	}
    }
    if ( rdrfp && ( c = getc( rdrfp)) != EOF) return (byte)c;
    if ( --ct == 0) exit( 0);
    return '\x1a';
}

void cpm_pun_out( byte c)	/* PUN: out */
{
    static int err;
    char *dev;

    if ( punfp == NULL && !err) {
	if (( dev = getenv( "CPMPUN")) != NULL) {
	    err = ( punfp = fopen( dev, "r+b")) == NULL;
	} else if ( rdrfp) {
	    punfp = rdrfp;
	} else {
	    dev = getenv( "CPMAUX");
	    err = ( punfp = fopen( dev ? dev : "AUX:", "r+b")) == NULL;
	}
    }
    if ( punfp) putc( c, punfp);
}

/* ================== CP/M consol emulation ================== */

HANDLE hConOut = INVALID_HANDLE_VALUE;
HANDLE hStdIn = INVALID_HANDLE_VALUE;
int conout;	/* 標準出力はコンソール？ */
int conin;	/* 標準入力はコンソール？ */
int eofcount = 16;
COORD cpm_cur;

void w32_putch( byte c)
{
    DWORD n;

    WriteConsole( hConOut, &c, 1, &n, NULL);
}

void w32_gotoxy( int x, int y)
{
    COORD cur;
    if ( x > 0) x--;
    if ( y > 0) y--;
    cur.X = (short)x; cur.Y = (short)y;
    SetConsoleCursorPosition( hConOut, cur);
}

void w32_gotodxy( int dx, int dy)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo( hConOut, &csbi);
    dx += csbi.dwCursorPosition.X;
    dy += csbi.dwCursorPosition.Y;
    if ( dx >= csbi.dwSize.X) dx = csbi.dwSize.X - 1;
    else if ( dx < 0) dx = 0;
    if ( dy >= csbi.dwSize.Y) dy = csbi.dwSize.Y - 1;
    else if ( dy < 0) dy = 0;
    csbi.dwCursorPosition.X = (short)dx;
    csbi.dwCursorPosition.Y = (short)dy;
    SetConsoleCursorPosition( hConOut, csbi.dwCursorPosition);
}

void w32_savexy( void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo( hConOut, &csbi);
    cpm_cur = csbi.dwCursorPosition;
}

void w32_restorexy( void)
{
    SetConsoleCursorPosition( hConOut, cpm_cur);
}

void w32_cls( int arg)
{
    DWORD n, size;
    WORD attr;
    COORD cur = {0,0};
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo( hConOut, &csbi);
    attr = csbi.wAttributes;

    // 2012.03 (swap arg=0,1) include CUR by N.Fujita
    if ( arg == 0) {
    	cur = csbi.dwCursorPosition;		/* CUR to end of screen */
	size = csbi.dwSize.X * (csbi.dwSize.Y - csbi.dwCursorPosition.Y)
		- csbi.dwCursorPosition.X;
    } else if ( arg == 1) {
	size = csbi.dwSize.X * csbi.dwCursorPosition.Y 
		+ csbi.dwCursorPosition.X + 1; 	/* screen top to CUR */
    } else {
	size = csbi.dwSize.X * csbi.dwSize.Y;	/* ALL of SCREEN */
    }

    FillConsoleOutputCharacter( hConOut, ' ', size, cur, &n); /* NUL->SP */
    FillConsoleOutputAttribute( hConOut, attr, size, cur, &n);
    if ( arg == 2) SetConsoleCursorPosition( hConOut, cur);
}

void w32_clrln( int arg)
{
    DWORD n, size;
    WORD attr;
    COORD cur;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo( hConOut, &csbi);
    cur = csbi.dwCursorPosition;
    attr = csbi.wAttributes;
    if ( arg == 0) {		/* CUR to EOL */
	size = csbi.dwSize.X - csbi.dwCursorPosition.X;
    } else if ( arg == 1) {	/* TOL to CUR */
	size = csbi.dwCursorPosition.X + 1;  // 2012.03 include CUR
	cur.X = 0;		/* ALL LINE */
    } else {
	size = csbi.dwSize.X;
	cur.X = 0;
    }

    FillConsoleOutputCharacter( hConOut, ' ', size, cur, &n); /* NUL->SP */
    FillConsoleOutputAttribute( hConOut, attr, size, cur, &n);
}

void w32_scroll( int len)
{
    COORD dst;
    SMALL_RECT src;
    CHAR_INFO chinfo;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo( hConOut, &csbi);
    src.Left = 0; 
    src.Top = (short)(csbi.dwCursorPosition.Y + (( len >= 0) ? len : 0));
    src.Right = (short)(csbi.dwSize.X - 1);
    src.Bottom = (short)(csbi.dwSize.Y - 1);
    dst.X = 0;
    dst.Y = (short)(csbi.dwCursorPosition.Y - ((len >= 0) ? 0 : len));
    chinfo.Char.AsciiChar = ' ';
    chinfo.Attributes = csbi.wAttributes;

    ScrollConsoleScreenBuffer( hConOut, &src, NULL, dst, &chinfo);
}

void w32_up( void)
{
    COORD dst;
    SMALL_RECT src;
    CHAR_INFO chinfo;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo( hConOut, &csbi);

    if ( csbi.dwCursorPosition.Y) {
	csbi.dwCursorPosition.Y--;
	SetConsoleCursorPosition( hConOut, csbi.dwCursorPosition);
    } else {
	src.Left = 0;
	src.Top = 0;
	src.Right = (short)(csbi.dwSize.X - 1);
	src.Bottom = (short)(csbi.dwSize.Y - 2);
	dst.X = 0;
	dst.Y = 1;
	chinfo.Char.AsciiChar = ' ';/* NUL->SP */
	chinfo.Attributes = csbi.wAttributes;
	ScrollConsoleScreenBuffer( hConOut, &src, NULL, dst, &chinfo);
    }
}

void w32_down( void)
{
    COORD dst;
    SMALL_RECT src;
    CHAR_INFO chinfo;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo( hConOut, &csbi);
    if ( ++csbi.dwCursorPosition.Y < csbi.dwSize.Y) {
	SetConsoleCursorPosition( hConOut, csbi.dwCursorPosition);
    } else {
	src.Left = 0;
	src.Top = 1;
	src.Right = (short)(csbi.dwSize.X - 1);
	src.Bottom = (short)(csbi.dwSize.Y - 1);
	dst.X = 0;
	dst.Y = 0;
	chinfo.Char.AsciiChar = ' ';/* NUL->SP */
	chinfo.Attributes = csbi.wAttributes;
	ScrollConsoleScreenBuffer( hConOut, &src, NULL, dst, &chinfo);
    }
}

void w32_left( void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo( hConOut, &csbi);
    if ( ++csbi.dwCursorPosition.X < csbi.dwSize.X) {
	SetConsoleCursorPosition( hConOut, csbi.dwCursorPosition);
    } else {
	csbi.dwCursorPosition.X = 0;
	SetConsoleCursorPosition( hConOut, csbi.dwCursorPosition);
	w32_down();
    }
}

WORD w32_attr( void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo( hConOut, &csbi);
    return csbi.wAttributes;
}


enum { ST_START, ST_NOP, ST_CHAR, ST_ESC, ST_EQ, ST_EQ2, ST_ANSI};

byte color_table[] = { 0, 4, 2, 6, 1, 5, 3, 7};
byte color_table2[] = { 0, 4, 1, 5, 2, 6, 3, 7};

BOOL _stdcall console_event_hander( DWORD type)
{
//    DWORD n;

    if ( type != CTRL_C_EVENT) return FALSE;
    reg.x.pc = 0;
//    WriteConsole( hConOut, "^C\r\n", 4, &n, NULL);
    return TRUE;
}

void cpm_conio_setup( void)
{
    DWORD md;
    HANDLE hnd = GetStdHandle( STD_OUTPUT_HANDLE);

    if ( hnd != INVALID_HANDLE_VALUE &&
    	 GetConsoleMode( hnd, &md)) {
	hConOut = hnd;
	conout = TRUE;
    }

    hStdIn = GetStdHandle( STD_INPUT_HANDLE);
    if ( hStdIn != INVALID_HANDLE_VALUE &&
	 GetConsoleMode( hStdIn, &md)) {
	conin = TRUE;
    }

     SetConsoleCtrlHandler( console_event_hander, TRUE);
}

byte cpm_const( void)
{
    DWORD t;
    static DWORD t0, ct;

    if ( conin  ? kbhit() 
		: WaitForSingleObject( hStdIn, 0) != WAIT_TIMEOUT) {
	return 0xff;
    }
    if ( kbwait) {
	t = GetTickCount();
	if ( t != t0) ct = (( t - t0) * 4) >> kbwait;
	if ( ct == 0) Sleep( 1 << (kbwait - 1));
	else --ct;
	t0 = t;
    }
    return 0;
}

/* 2012.03 replace VC++'s broken getch */
#ifdef _MSC_VER
static int getch( void)
{
    static unsigned keybuf = 0;
    static unsigned char ctltbl[] = { 
      0x84-0x49, 0x76-0x51, 0x75-0x4f, 0x77-0x47, // PgUP, PgDOWN, End, Home,
      0x73-0x4b, 0x8d-0x48, 0x74-0x4d, 0x91-0x50, // LEFT, UP, RIGHT, DOWN,
      0,         0,         0,    0,
      0x92-0x52, 0x93-0x53, 0,    0,     // INS, DEL
    };
    int c = 0x1a;
    DWORD l;
    DWORD mode;
    INPUT_RECORD evt;
//  HANDLE hStdIn = GetStdHandle( STD_INPUT_HANDLE);

    if ( keybuf) { 
        c = (unsigned char)keybuf; 
        keybuf >>= 8; 
        return c;
    }

    GetConsoleMode( hStdIn, &mode);
    SetConsoleMode( hStdIn, 0);
    while ( ReadConsoleInput( hStdIn, &evt, 1, &l)) {
		KEY_EVENT_RECORD *key = &evt.Event.KeyEvent;
		if ( evt.EventType == KEY_EVENT && key->bKeyDown) {
			int k = key->wVirtualKeyCode;
			int s = key->wVirtualScanCode;
			DWORD st = key->dwControlKeyState;
			c = (BYTE)key->uChar.AsciiChar;
			if ( c) break;
			if ( k >= 0x21 && k <= 0x2e) {  // extend keys
				if ( st & (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED)) 
				    s += 0x50;
				else if ( st & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) 
				    s += ctltbl[ k - 0x21];
				keybuf = s;
				break;
			}
			if ( k >= 0x70 && k <= 0x7b) {  // F1-F12
				int b = ( k < 0x7a) ? 0x19 : 2;
				int f = ( k < 0x7a) ? 10 : 2;
				if ( k >= 0x7a) s += 0x85-0x57;
				if ( st & (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED)) s += b + f + f;
				else if ( st & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) s += b + f;
				else if ( st & SHIFT_PRESSED) s += b;
				keybuf = s;
				break;
			}
		}
	
    }
    SetConsoleMode( hStdIn, mode);

	return c;
}
#endif

byte cpm_getch( void)
{
    int c;

    if ( conin) {
	if (( c = getch()) == 0) {
	    switch ( getch()) {
	    case 72: c = 'E' - 0x40; break; /* UP */
	    case 80: c = 'X' - 0x40; break; /* DOWN */
	    case 75: c = 'S' - 0x40; break; /* LEFT */
	    case 77: c = 'D' - 0x40; break; /* RIGHT */
	    case 82: c = 'V' - 0x40; break; /* INS */
	    case 83: c = '\x7f';     break; /* DEL */ /* 2012.03 bug-fix by N.Fujita */
	    case 73: c = 'R' - 0x40; break; /* PgUP */
	    case 81: c = 'C' - 0x40; break; /* PgDOWN */
	    case 141: c = 'W' - 0x40; break; /* ^UP */
	    case 145: c = 'Z' - 0x40; break; /* ^DOWN */
	    case 115: c = 'A' - 0x40; break; /* ^LEFT */
	    case 116: c = 'F' - 0x40; break; /* ^RIGHT */
	    }
	}
    } else if (( c = getchar()) == EOF) {
	if ( --eofcount == 0) exit( 0);
	c = '\x1a';
    }
    return (byte)c;
}

byte cpm_getche( void)
{
    int c;
    DWORD n;

    c = cpm_getch();
    if ( conin) {
	if ( c == '\x03') {
		GenerateConsoleCtrlEvent( CTRL_C_EVENT, 0);
	}
	if ( hConOut == INVALID_HANDLE_VALUE) {
	    hConOut = CreateFile( "CONOUT$", GENERIC_READ | GENERIC_WRITE,
	    			  0, NULL, OPEN_EXISTING, 0, NULL);
	}
	WriteConsole( hConOut, &c, 1, &n, NULL);
    }
    return (byte)c;
}

byte cpm_gets( byte *buf)
{
    int i;

    if ( fgets((char *)buf + 2, buf[ 0], stdin) == NULL) {
	if ( --eofcount == 0) exit( 0);
	strcpy((char *)buf + 2, "\x1a\n");
    }
    i = strlen((char *)buf + 2);
    if (i > 0) i--;
    buf[ 2 + i] = '\r'; buf[ 1] = (byte)i;
    if ( conout && conin) w32_up();
    return 0;
}

void cpm_putch( int c)
{
    static byte esc_stat, arg_n;
    static byte args[ 8];

    switch ( esc_stat) {
    case ST_NOP: if ( c != '\r') putchar((char)c); return;
    case ST_START:
	if ( !conout) {
	    esc_stat = ST_NOP;
	    putchar(( byte)c);
	    return;
	}
	esc_stat = ST_CHAR;
    case ST_CHAR:
    	switch ( c) {
	case '\x0b': w32_up(); break;
	case '\x0c': w32_left(); break;
	case '\x1a': w32_cls( 2); break;
	case '\x1b': esc_stat = ST_ESC; break;
	case '\x1e': w32_gotoxy(1,1); break;
	default: w32_putch((byte)c); break;
	}
	return;
    case ST_ESC:
	switch ( c) {
	case '*': w32_cls( 2); break;
	case '=': esc_stat = ST_EQ; return;
	case '(': SetConsoleTextAttribute( hConOut, w32_attr() | 8); break;
	case ')': SetConsoleTextAttribute( hConOut, w32_attr() & ~8); break;
	case 't':
	case 'T': w32_clrln( 0); break;
	case 'y':
	case 'Y': w32_cls( 0); break;   // 2012.03 add 
	case 'D': w32_down(); break;
	case 'E': w32_scroll( -1); break;
	case 'M': w32_up(); break;
	case 'R': w32_scroll( 1); break;
	case '[': arg_n = 0; args[ 0] = args[ 1] = 0; 
		  esc_stat = ST_ANSI; return;
	default: if (debug_flag) printf( "ESC%c", c); break;
	}
	break;
    case ST_EQ:
	args[ 0] = (byte)(c - ' ' + 1);
	esc_stat = ST_EQ2;
	return;
    case ST_EQ2:
	args[ 1] = (byte)(c - ' ' + 1);
	w32_gotoxy( args[ 1], args[ 0]);
	break;
    case ST_ANSI:
	if ( c >= '0' && c <= '9') {
	    args[ arg_n] = (byte)(args[ arg_n] * 10 + c - '0');
	    return;
	} else if ( c == ';') {
	    if ( ++arg_n >= sizeof args) --arg_n;
	    args[ arg_n] = 0;
	    return;
	} else if ( c == 'H' || c == 'f') {
	    w32_gotoxy( args[ 1], args[ 0]);
	} else if ( c == 'A') {
	    if ( args[ 0] == 0) args[ 0]++;
	    w32_gotodxy( 0, -args[ 0]);
	} else if ( c == 'B') {
	    if ( args[ 0] == 0) args[ 0]++;
	    w32_gotodxy( 0, args[ 0]);
	} else if ( c == 'C') {
	    if ( args[ 0] == 0) args[ 0]++;
	    w32_gotodxy( args[ 0], 0);  // 2012.03 ESC[C <=> ESC[D
	} else if ( c == 'D') {
	    if ( args[ 0] == 0) args[ 0]++;
	    w32_gotodxy( -args[ 0], 0); // 2012.03 ESC[C <=> ESC[D
	} else if ( c == 'J') {
	    w32_cls( args[ 0]);
	} else if ( c == 'K') {
	    w32_clrln( args[ 0]);
	} else if ( c == 'M') {
	    if ( args[ 0] == 0) args[ 0]++; // 2012.03 ESC[M = ESC[1M
	    w32_scroll( args[0]);
	} else if ( c == 'L') {
	    if ( args[ 0] == 0) args[ 0]++; // 2012.03 ESC[L = ESC[1L
	    w32_scroll( -args[0]);
	} else if ( c == 'u') {
	    w32_restorexy();
	} else if ( c == 's') {
	    w32_savexy();
	} else if ( c == 'm') {
	    int i;
	    word f = 7, b = 0, a = 0, t;
	    for ( i = 0; i <= arg_n; i++) {
		c = args[ i];
		if ( c >= 30 && c <= 37) {
		    f = color_table[ c - 30];
		} else if ( c >= 40 && c <= 47) {
		    b = color_table[ c - 40];
		} else if ( c >= 16 && c <= 23) {
		    f = color_table2[ c - 16];
		} else if ( c == 1) {
		    a |= FOREGROUND_INTENSITY;
		} else if ( c == 7) {
		    a |= COMMON_LVB_REVERSE_VIDEO;
		} else if ( c == 4) {
		    a |= COMMON_LVB_UNDERSCORE;
		}
	    }
	    /* bugbug! not work REVERSE_VIDEO at 2K,XP console */
	    f |= a & FOREGROUND_INTENSITY;
	    if ( a & COMMON_LVB_REVERSE_VIDEO) { t = f; f = b; b = t;}
	    a &= COMMON_LVB_UNDERSCORE;
	    SetConsoleTextAttribute( hConOut, f | (b << 4) | a);
	} else {
	    if (debug_flag) printf( "ESC[n%c", c);
	}
	break;
    default:
	break;
    }
    esc_stat = ST_CHAR;
}

void help( void)
{
    fprintf( stderr, "CPM.EXE -- CP/M-80 program EXEcutor for Win32 V0.4\n"
		"Copyright (C) 2004-2012 by K.Murakami\n"
		"  Usage: CPM [-hxapdC][-w[0-9]] command arg1 arg2 ...\n"
		"	-h .. return HI-TECH C exit code\n"
		"	-x .. return ERROR if A:$$$.SUB deleted\n"
		"	-a .. select A: (program directry)\n"
		"	-p .. pause before exit\n"
		"	-d .. disable auto drive assign\n"
		"	-C .. args to uppercase\n"
		"	-w[0-9] .. wait on console status check (9:max)\n"
    );
    exit( 1);
}

int main( int argc, char *argv[])
{
    int st, i, p, q;
    char *arg1, *arg2;

    for ( i = 1; i < argc; i++) {
	char *s = argv[ i];
	if ( *s != '-') break;
	if ( *++s == '-') { i++; break;}
	while ( *s) {
	    switch ( *s++) {
	    case 'D': debug_flag = TRUE; break;
	    case 'w': kbwait = ( *s >= '0' && *s <= '9') ?
	    				 (*s++ - '0' + 1) : 1; 
	    	break;
	    case 'a': cpm_disk_no = 'A' - 'A'; break;
	    case 'p': pause_flag = TRUE; break;
	    case 'h': retcode_flag = RC_HITECH; break;
	    case 'x': retcode_flag = RC_BDSC; break;
	    case 'd': no_auto_assign = TRUE; break;
	    case 'C': uppercase_flag = TRUE; break;
	    default: help();
	    }
	}
    }
    if ( i >= argc) help();

    if ( !load_program( argv[ i])) {
        fprintf( stderr, "ERROR: program not found.\n");
        return -1;
    }

    /* setup 0 page */
    p = 0;
    mem[ p++] = 0xC3; setword( mem[ p++], BIOS_ORG + 3);	/* JP WBOOT */
    p++;
    mem[ p++] = cpm_disk_no;
    mem[ p++] = 0xC3; setword( mem[ p++], BDOS_ORG);		/* JP BDOS */

    /* 2012.03 force to uppercase option */
    if ( uppercase_flag) {
        int j;
        for ( j = i + 1; j < argc; j++) CharUpper( argv[ j]);
    }

                                   /* setup FCB1,FCB2   */
    arg1 = (i+1 < argc) ? auto_drive_assign( &argv[ i+1]) : "";
    arg2 = (i+2 < argc) ? auto_drive_assign( &argv[ i+2]) : "";

    /* 2012.03 terminate on "=:;<>" */
    p = ( *arg1 && arg1[1] == ':') ? 2 : 0;
    for (; (q = arg1[ p]) != '\0' && !strchr("=:;<>", q); p++);
    if ( q) {
        arg1[ p] = '\0';
        mkFCB( mem + 0x5c, arg1);
        mkFCB( mem + 0x6c, "");
        arg1[ p] = q;
    } else {
        mkFCB( mem + 0x5c, arg1);
        mkFCB( mem + 0x6c, arg2);
    }

    p = 0x81;					   /* setup command buffer */
    for ( i++; i < argc; i++) {
        mem[ p++] = ' ';
        for ( q = 0; p < 0x100-1 && argv[ i][ q]; q++) mem[ p++] = argv[ i][ q];
        if ( p >= 0x100-1) { p = 0x100-1; break;}
    }
    mem[ p] = '\0';
    mem[ 0x80] = p - 0x81;


    /* setup BDOS code */
    p = BDOS_ORG;
    mem[ p++] = 0xED; mem[ p++] = 0xED; mem[ p++] = 0;		/* CALLN 0 */
    mem[ p] = 0xC9;						/* RET */

    mem[ DMY_DPB] = 64;
    mem[ DMY_DPB_SFT] = 4;
    mem[ DMY_DPB_MSK] = (1 << 4) - 1;
    mem[ DMY_DPB_MAX] = (byte) MAXBLK;
    mem[ DMY_DPB_MAX+1] = (byte)(MAXBLK >> 8);

    /* setup BIOS code */
    p = BIOS_ORG;
    q = p + 3*17;
    for ( i = 0; i < 17; i++) {
	mem[ p++] = 0xC3; setword( mem[ p++], q);	     /*   JP l */
	mem[ q++] = 0xED; mem[ q++] = 0xED; 
				mem[ q++] = (byte)(1+i);     /*l: CALLN 1+i */
	mem[ q++] = 0xC9;				     /*   RET */
    }

    /* emuration start */
    cpm_conio_setup();
    memset( guard, HALT, sizeof guard);
    reg.b.r = (byte)time( NULL);
    reg.x.sp = BDOS_ORG - 2;
    reg.x.pc = 0x100;
    while (( st = em180()) != EMST_STOP) {
	if ( st == EMST_BDOS) {				/* -- BDOS CALL -- */
	    reg.x.hl = 0;
	    switch ( reg.b.c) {
	    case  0: return 0;				/* boot */
	    case  1:					/* con in */
		reg.b.l = cpm_getche();
		break;
	    case  2: 		  			/* con out */
		cpm_putch( reg.b.e);
		break;
	    case  3: 		  			/* RDR: in */
		reg.b.l = cpm_rdr_in();
		break;
	    case  4: 		  			/* PUN: out */
		cpm_pun_out( reg.b.e);
		break;
	    case  5: 		  			/* LST: out */
		cpm_lst_out( reg.b.e);
		break;
	    case  6:			 		/* direct i/o */
		if ( reg.b.e == 0xff) {
		    reg.b.l = cpm_const() ? cpm_getch() : 0 ;
		} else {
		    cpm_putch( reg.b.e);
		}
		break;
	    case  9: 					/* string out */
		for ( i = reg.x.de; mem[ i] != '$'; i++) cpm_putch( mem[ i]);
		break;
	    case 10: 					/* string in */
		cpm_gets( mem + reg.x.de);
                break;
	    case 11: 					/* const */
                reg.b.l = cpm_const();
                break;
	    case 12:					/* version */
                reg.x.hl = cpm_version;
                break;
	    case 13:					/* disk reset */
		reg.b.l = cpm_disk_reset();
DEBUGOUT( stderr, "BDOS: disk reset -> %02x\n", reg.b.l);
                break;
	    case 14:					/* select disk */
DEBUGOUT( stderr, "BDOS: select disk %c:\n", reg.b.e + 'A');
		if ( cpm_disk_vct == 0) setup_disk_vct();
		if ( reg.b.e >= MAXDRV || 
			cpm_drive[ reg.b.e] == NULL) {
		    reg.b.l = 0xff;
		    break;
		}
                cpm_disk_no = reg.b.e;
                break;
	    case 15:					/* file open */
		reg.b.l = cpm_file_open( mem + reg.x.de, CF_OPEN);
DEBUGOUT( stderr, "BDOS: open file %c:'%11.11s'.%d -> %02x\n", 
	 mem[ reg.x.de] + '@', mem + reg.x.de + 1, mem[ reg.x.de + 16] - 1, reg.b.l);
		break;
	    case 16:					/* file close */
		reg.b.l = cpm_file_close( mem + reg.x.de);
DEBUGOUT( stderr, "BDOS: close file '%11.11s'. -> %02x\n",
	 mem + reg.x.de + 1, reg.b.l);
		cpm_change_filesize( mem + reg.x.de);
		break;
	    case 17:					/* file find first */
		reg.b.l = cpm_findfirst( mem + reg.x.de);
		break;
	    case 18:					/* file find next */
		reg.b.l = cpm_findnext();
		break;
	    case 19:					/* file delete */
		reg.b.l = cpm_file_delete( mem + reg.x.de);
DEBUGOUT( stderr, "BDOS: delete file %c:'%11.11s' ->%02x.\n",
	 mem[ reg.x.de] + '@', mem + reg.x.de + 1, reg.b.l);
		break;
	    case 20:					/* sequential read */
		reg.b.l = cpm_file_rw( mem + reg.x.de, CFRW_RD, CFRW_SEQ);
DEBUGOUT( stderr, "-> %02x\n", reg.b.l);
		break;
	    case 21:					/* sequential write */
		reg.b.l = cpm_file_rw( mem + reg.x.de, CFRW_WR, CFRW_SEQ);
DEBUGOUT( stderr, "-> %02x\n", reg.b.l);
		break;
	    case 22:					/* file create */
		reg.b.l = cpm_file_open( mem + reg.x.de, CF_CREATE);
DEBUGOUT( stderr, "BDOS: create file %c:'%11.11s'.-> %02x\n", 
	 mem[ reg.x.de] + '@', mem + reg.x.de + 1, reg.b.l);
		break;
	    case 23:					/* file rename */
		reg.b.l = cpm_rename( mem + reg.x.de, mem + reg.x.de + 16);
		break;
	    case 24:					  /* get disk vct */
		if ( cpm_disk_vct == 0) setup_disk_vct();
		reg.x.hl = cpm_disk_vct;
		break;
	    case 25:					  /* get disk no */
		reg.b.l = cpm_disk_no;
		break;
	    case 26:					  /* set dma addr */
		cpm_dma_addr = reg.x.de;
		break;
	    case 27:					  /* get alloc tbl */
DEBUGOUT( stderr, "BDOS: get alloc tbl\n");
		reg.x.hl = DMY_ALLOC;
		break;
	    case 31:					  /* get disk prm */
DEBUGOUT( stderr, "BDOS: get disk prm\n");
		frameup_dpb_alloc();
		reg.x.hl = DMY_DPB;
		break;
	    case 32:                                      /* set/get uid */
                if ( reg.b.e == 0xff) reg.x.hl = cpm_usr_no;
                else cpm_usr_no = reg.b.e;
                break;
	    case 33:					/* random read */
		reg.b.l = cpm_file_rw( mem + reg.x.de, CFRW_RD, CFRW_RND);
DEBUGOUT( stderr, "-> %02x\n", reg.b.l);
		break;
	    case 34:					/* random write */
		reg.b.l = cpm_file_rw( mem + reg.x.de, CFRW_WR, CFRW_RND);
DEBUGOUT( stderr, "-> %02x\n", reg.b.l);
		break;
	    case 35:					/* file size */
		reg.b.l = cpm_file_size( mem + reg.x.de);
DEBUGOUT( stderr, "BDOS:get file size '%11.11s' -> %02x.\n", 
	 mem + reg.x.de + 1, reg.b.l);
		break;
	    case 36:
DEBUGOUT( stderr, "BDOS: set random record '%11.11s'.\n", mem + reg.x.de + 1);
		reg.b.l = cpm_set_rndrec( mem + reg.x.de);
		break;
	    case 37:					/* drive reset */
DEBUGOUT( stderr, "BDOS: drive reset %04x \n", reg.x.de); /* 2012.03 bug-fix by N.Fujita */
		break;
	    case 40:					/* random write w0 */
		reg.b.l = cpm_file_rw( mem + reg.x.de, CFRW_WR, CFRW_RND);
DEBUGOUT( stderr, " with 0 -> %02x\n", reg.b.l);
		break;
	    case 102:				/* get file time (CP/M3.0) */
		if ( mk_filename( filename, mem + reg.x.de)) {
		    struct stat st;
		    stat( filename, &st);
		    cpm_gettime( mem + reg.x.de + 0x18, st.st_atime);
		    cpm_gettime( mem + reg.x.de + 0x1c, st.st_mtime);
		} else {
		    reg.b.l = 0xff;
		}
		break;
	    case 105:				/* get time (CP/M3.0) */
		reg.b.l = cpm_gettime( mem + reg.x.de, time( NULL));
		break;
	    default:
		fprintf( stderr, "Unsupported BDOS call: %d\n", reg.b.c);
                break;
	    }
	    reg.b.a = reg.b.l; reg.b.b = reg.b.h;
	} else if ( st < EMST_BDOS) {			/* -- HALT -- */
	    fprintf( stderr, "ERROR: Halted. %02x\n", st);
	    return -1;
	} else if ( st < EMST_UNKOWN) {			/* -- BIOS CALL -- */
	    switch ( st -= EMST_BIOS_BASE) {
	    case  0: 					  /* cold boot */
	    case  1:					  /* warm boot */
		if ( pause_flag) while ( cpm_getch() >= ' ');
		return  retcode_flag == RC_HITECH ? *(word *)(mem + 0x80) :
			retcode_flag == RC_BDSC && abort_submit ? 2 : 0;
	    case  2:		/* const */
                reg.b.a = cpm_const();
                break;
	    case  3:		/* conin */
		reg.b.a = cpm_getch();
                break;
	    case  4:		/* conout */
		cpm_putch( reg.b.c);
		break;
	    case  5: 		/* list */
		cpm_lst_out( reg.b.c);
		break;
	    case  6: 		/* punch */
		cpm_pun_out( reg.b.e);
		break;
	    case  7: 		/* reader */
		reg.b.a = cpm_rdr_in();
		break;
	    default:
		fprintf( stderr, "ERROR: Unsupported BIOS call: %d\n", st);
		return -1;
	    }
	} else {					/* UNKOWN op-code */
	    int n = ( st >= 0x10000) ? 6 : ( st >= 0x100) ? 4 : 2;
	    fprintf( stderr, "ERROR: Illegal op-code executed. %*x\n", n, st);
	    return -1;
	}
    }
    return 0;
}
