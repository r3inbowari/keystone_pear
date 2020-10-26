#ifndef __DIS_H
#define __DIS_H

#include <U8g2lib.h>

#define MENU_OFFSET 6
#define PICDEF int
#define MAX_ITEM_COUNT 4

#define PIC_CLEAR 0
#define PIC_CHARGE 1
#define PIC_WIFI 2
#define PIC_LTE 3
#define PIC_WENDU 4
#define PIC_UPLOAD 5
#define PIC_WIFI_SEARCH_1 6
#define PIC_WIFI_SEARCH_2 7
#define PIC_WIFI_SEARCH_3 8
#define PIC_USB 9
#define PIC_ACCESS_POINT 10

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
extern int apds_swiper_index;

int menu_addItem(PICDEF pic_index);
void menu_delItem(PICDEF pic_index);

#endif
