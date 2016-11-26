/*
 * BLGRPCMD.H
 */

#define GRP_OP_IMP	0x00
#define GRP_OP_AND	0x01
#define GRP_OP_OR	0x02
#define GRP_OP_EOR	0x03
#define GRP_OP_NOT	0x04
#define GRP_OP_TIMP	0x08
#define GRP_OP_TAND	0x09
#define GRP_OP_TOR	0x0A
#define GRP_OP_TEOR	0x0B
#define GRP_OP_TNOT	0x0C

#define bl_grp_cmd_wait() bl_vdp_cmd_wait()
void bl_grp_put_pixel(uint16_t x, uint16_t y, uint8_t c, uint8_t op);
uint8_t bl_grp_get_pixel(uint16_t x, uint16_t y);
void bl_grp_hcopy_v2v(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t dx, uint16_t dy);
void bl_grp_hcopy_v2v_p(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t dx, uint16_t dy, uint8_t src);
void bl_grp_lcopy_v2v(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t dx, uint16_t dy, uint8_t op);
void bl_grp_lcopy_v2v_p(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t dx, uint16_t dy, uint8_t src, uint8_t op);
void bl_grp_line(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op);
void bl_grp_box(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op);
void bl_grp_boxfill(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op);
void bl_grp_boxfill_h(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c);
void bl_grp_circle(uint16_t cx, uint16_t cy, uint16_t radius, uint8_t c, uint8_t op);


