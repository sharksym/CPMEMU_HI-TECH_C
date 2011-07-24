/* Standard Banking Library (BLSTD.H)*/

typedef char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

/* bl_bank is defined at each bank (Number is set 0,1,2,3... ) */
extern int16_t bl_bank;

/* Set TSR mode */
void bl_tsr_on(void);

#define BL_RANDOM_MAX	1023
uint16_t bl_random(void);

/* Shared heap */
void *bl_calloc(size_t num, size_t size);
void bl_free(void *ptr);
void *bl_malloc(size_t size);
void *bl_realloc(void *ptr, size_t size);
