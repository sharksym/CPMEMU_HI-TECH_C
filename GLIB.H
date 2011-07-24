/*
 * ASCII (MSX) C COMPATIBLE GRAPHICS LIBRARY for HI-TECH C
 *
 */

#ifndef uint
#define uint unsigned int
#endif
#ifndef uchar
#define uchar unsigned char
#endif

#define c_dpage 	(*(uchar *)0xfaf5)	/* visible page */
#define c_apage         (*(uchar *)0xfaf6)      /* active page */
#define c_fore          (*(uchar *)0xf3e9)      /* color foreground */
#define c_back          (*(uchar *)0xf3ea)      /* color background */
#define c_bord          (*(uchar *)0xf3eb)      /* color border */
#define c_lastx         (*(uint*)0xfcb7)        /* csrx */
#define c_lasty         (*(uint*)0xfcb9)        /* csry */
#define c_screen        (*(uchar *)0xfcaf)      /* screen nr. */
#define c_sprite        (*(uchar *)0xf3e0 & 3)  /* sprites */
#define c_xmax          (gtxmax())              /* max.graph x */
#define c_ymax          (gtymax())              /* max.graph y */

#define OP_PSET    (uchar)0
#define OP_AND     (uchar)1
#define OP_OR      (uchar)2
#define OP_XOR     (uchar)3
#define OP_PRESET  (uchar)4
#define OP_TPSET   (uchar)8
#define OP_TAND    (uchar)9
#define OP_TOR     (uchar)10
#define OP_TXOR    (uchar)11
#define OP_TPRESET (uchar)12

void    screen(uchar);          /* set msx screen mode */
void    ginit(void);            /* get VDP io-port (0x98) */
void    interlace(uchar);       /* set interlace mode */
uint    gtxmax(void);           /* max.x on graphical screen */
uint    gtymax(void);           /* max.y on graphical screen */
void    setrd(uint);            /* set up the VDP for read from address */
uchar   invdp(void);            /* get byte from current VDP address */
void    setwrt(uint);           /* set up VDP for write to address */
void    outvdp(uchar);          /* send byte to current VDP address */
uchar   vpeek(uint);            /* vpeek address */
void    vpoke(uint,uchar);      /* vpoke(adr,value) */
void    filvrm(uint,uint,uchar);  /* fill(address,length,databyte) */
void    ldirmv(char *,uint,uint); /* vram to ram (dest,src,len) */
void    ldirvm(uint,char *,uint); /* ram to vram (dest,src,len) */
void    wrtvdp(uchar,uchar);      /* write vdp(register,value) */
uchar   rdvdp(uchar);             /* read vdp(register) */
uchar   rdvsts(uchar);            /* read VDP statusregister(register) */
void    color(uchar,uchar,uchar); /* color(fore,back,border) */
void    iniplt(void);           /* init palette */
void    rstplt(void);           /* reset palette */
uint    getplt(uchar);          /* get palette(color)=l=RB,h=G */
void    setplt(uchar,uint);     /* set palette(color,GRBdata) */
void    pset(uint,uint,uchar,uchar); /* pset(x,y,color,logop) */
void    line(uint,uint,uint,uint,uchar,uchar);    /* line(x1,y1,x2,y2,color,logop) */
void    boxline(uint,uint,uint,uint,uchar,uchar); /* line(x1,y1,x2,y2,color,logop) */
void    boxfill(uint,uint,uint,uint,uchar,uchar); /* line(x1,y1,x2,y2,color,logop) */
void    circle(uint,uint,uint,uchar,int,int,uint);
                /* circle(x,y,r,color,s_angle,e_angle,aspect) */
void    paint(uint,uint,uchar,uchar);   /* paint(x,y,color,b_color) */
uchar   point(uint,uint);               /* color=point(x,y) */
void    inispr(uchar);                  /* set sprite(size) */
uint    calpat(uchar);                  /* calculate sprite pattern */
uint    calatr(uchar);                  /* calculate sprite attribute */
void    sprite(uchar,char *);           /* set sprite (num,data*) */
void    colspr(uchar,char *);           /* sprite color (num,color*) */
void    putspr(uchar,uint,uint,uchar,uchar); /* putsprite(num,x,y,color,pattern */
void    cpyv2v(uint,uint,uint,uint,uchar,uint,uint,uchar,uchar);
                /* copy vram to vram (sx1,sy1,sx2,sy2,sp,dx,dy,dp,logop) */
void    cpyv2m(uint,uint,uint,uint,uchar,char *);
                /* copy vram to ram (sx1,sy1,sx2,sy2,sp,destadres) */
void    cpym2v(char *,uchar,uint,uint,uchar,uchar);
                /* copy ram to vram(srcadres,direction,dx,dy,dp,logop) */
void    totext(void);           /* set text mode */
void    grpprt(char,char);      /* print char on graphic screen */
void    knjprt(uint,char,char); /* print kanji (char (uint),logop,mode) */
void    glocate(uint,uint);     /* graphic locate x,y */
void    setpg(uchar,uchar);     /* set page(display,active) */
uint    vramsize(void);         /* get vram size (16,64,128) */

