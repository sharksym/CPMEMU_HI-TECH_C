#ifndef	_MSXBDOS_H_
#define	_MSXBDOS_H_

#define	BDOS_CPM	0x00
#define	BDOS_MSXDOS1	0x01
#define	BDOS_MSXDOS2	0x02
#define	BDOS_AUTO	0xFF


extern unsigned short GetBdosVersion(void);

extern unsigned char gcBdosMode;
extern unsigned char SetBdosMode(unsigned char cMode);
extern unsigned char GetBdosMode(void);

struct disk_rominfo_t {
	unsigned char drive_max;
	unsigned char slot_no;
};
struct disk_info_t {
	unsigned char drive_max;		/* total drives */
	unsigned char master_slot_no;		/* master diskrom */
	struct disk_rominfo_t diskrom[4];	/* total 4 interfaces */
};
extern void disk_get_info(struct disk_info_t *info);


extern void cursor(char onoff);
extern void sleep(unsigned int seconds);

extern void vdp_di(void);
extern void vdp_ei(void);


#endif /* __MSXBDOS_H_ */
