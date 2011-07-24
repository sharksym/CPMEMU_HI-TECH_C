#ifndef	__MSXBDOS.H
#define	__MSXBDOS.H

#define	BDOS_CPM		0x00
#define	BDOS_MSXDOS1	0x01
#define	BDOS_MSXDOS2	0x02
#define	BDOS_AUTO		0xFF


extern unsigned short GetBdosVersion (void);

extern unsigned char gcBdosMode;
extern unsigned char SetBdosMode (unsigned char cMode);
extern unsigned char GetBdosMode (void);


extern void cursor (char onoff);
extern void sleep (unsigned int seconds);

extern void vdp_di (void);
extern void vdp_ei (void);


#endif /* __MSXBDOS.H */


