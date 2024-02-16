/* Standard Banking Library (BLSTD.H)*/

typedef char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

/* bl_bank is defined at each bank (Number is set 0,1,2,3... ) */
extern int16_t bl_bank;

/* callback for 'Not enough memory'
   Adjust BankMax. */
short bl_cb_adjust_BankMax(short free_bank);

/* exit() */
void bl_exit(int n);
#define exit(n)		bl_exit(n)

/* Abort Handler */
void bl_abort_on(void);
void bl_abort_off(void);

/* Get OVL info */
void bl_get_ovl_info(char *name, short *bank_max);

/* Get BANK info */
#define bl_get_bank_info()	bl_get_seg_info(bl_bank)
uint16_t bl_get_seg_info(short bank);

/* Overlay BANK with specified OVL bank */
void bl_overlay_seg(short dest_bank, short ovl_bank);

/* Set TSR mode */
void bl_tsr_on(void);
void bl_tsr_off(void);
int8_t bl_is_tsr_on(void);

#define BL_RANDOM_MAX	0x10000
#define BL_RANDOM_MASK	0xFFFF
uint16_t bl_random(void);

#ifdef BL_DISABLE
#define	bl_free		free
#define	bl_malloc	malloc
void free(void *);
void *malloc(uint16_t);
#else	/* Shared heap */
void *bl_calloc(uint16_t num, uint16_t size);
void bl_free(void *ptr);
void *bl_malloc(uint16_t size);
void *bl_realloc(void *ptr, uint16_t size);
void *bl_get_memtop(void);
#endif

#ifdef BL_ROM
extern uint8_t bl_ram_slotid;
extern uint8_t bl_rom_slotid;

void bl_rom_read_page3(uint16_t rom_addr, uint16_t dest_addr, uint16_t bytes);
#endif
