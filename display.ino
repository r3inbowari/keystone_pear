#include "display.h"
#include "apds.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

#define MENU_OFFSET 6
#define PICDEF int

#define PIC_CLEAR 0
#define PIC_CHARGE 1
#define PIC_WIFI 2
#define PIC_LTE 3
#define PIC_WENDU 4

void init_display() {
  u8g2.begin();
  u8g2.enableUTF8Print();

  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.setFontDirection(0);

  u8g2.clearBuffer();
  u8g2.drawFrame(0, 0, 128, 20);
  u8g2.sendBuffer();
  delay(200);
  u8g2.drawFrame(0, 19, 128, 45);
  u8g2.sendBuffer();
}

void r3inb() {
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.setFontDirection(0);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 10);
  u8g2.print("#include <stdio>");
  u8g2.setCursor(0, 25);
  u8g2.print("void main() {");
  u8g2.setCursor(0, 40);
  u8g2.print("print(\"r3inb\")\;");
  u8g2.setCursor(0, 55);
  u8g2.print("}");
  u8g2.sendBuffer();

  delay(1000);
}





const unsigned char pics[][32] = {
  // clear
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // charge
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0xbe, 0x1f, 0xbe, 0x1f, 0xbe, 0x1f, 0x98, 0x7f, 0xe8, 0x7f, 0xe8, 0x1f, 0xe8, 0x1f, 0xe8, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // wifi
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0xf0, 0x0f, 0x1c, 0x38, 0xe4, 0x27, 0x36, 0x6c, 0xd8, 0x1b, 0xe8, 0x17, 0x20, 0x04, 0x80, 0x01, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // lte
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x24, 0x00, 0x24, 0x00, 0x24, 0x80, 0x25, 0xa0, 0x25, 0xa0, 0x25, 0xa4, 0x25, 0xa4, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // wendu
  0x00, 0x00, 0x80, 0x01, 0xc0, 0x03, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xa0, 0x05, 0xb0, 0x0d, 0xb0, 0x0d, 0x20, 0x04, 0xe0, 0x07, 0x80, 0x01, 0x00, 0x00,
};

int menuItems[20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int menuItemsCount = 0;

void printTrack() {
  for (int i = 0; i < 20; i ++) {
    Serial.print(menuItems[i]);
    Serial.print(" ");
  }
  Serial.println();
}

// 快速区域重绘
void menu_fastSinglePrint(PICDEF pic_index) {
  // 绘制起点偏移量
  int menu_offset = (menuItemsCount - 1) * 20 + MENU_OFFSET;
  u8g2.drawXBMP(menu_offset, 2, 16, 16, pics[pic_index]);
  u8g2.sendBuffer();
  delay(1);
}

// 起点重绘
void menu_redraw() {
  int item_offset = 0 + MENU_OFFSET;
  for (int i = 0; i < 6; i++) {
    if (menuItems[i] != -1) {
      u8g2.drawXBMP(item_offset, 2, 16, 16, pics[menuItems[i]]);
    } else {
      u8g2.drawXBMP(item_offset, 2, 16, 16, pics[0]);
    }
    item_offset += 20;
  }
  u8g2.sendBuffer();
}

int menu_addItem(PICDEF pic_index) {
  menuItems[menuItemsCount] = pic_index;
  menuItemsCount ++;
  if (menuItemsCount < 7) {
    menu_fastSinglePrint(pic_index);
  }
}

void menu_delItem(PICDEF pic_index) {
  int find_num = -1;
  for (int i = 0; i < 20; i++) {
    if (menuItems[i] == pic_index) {
      find_num = i;
      break;
    }
  }

  if (find_num != -1) {
    int cur = find_num + 1;
    while (cur < menuItemsCount) {
      menuItems[cur - 1] = menuItems[cur];
      cur ++;
    }
    menuItemsCount --;
    menuItems[menuItemsCount] = -1;
    menu_redraw();
  }
}

void content_bme280() {
  u8g2.setCursor(5, 38);
  u8g2.print("TH 12.3C 1223M");
  u8g2.sendBuffer();
}

void content_apds9960() {
  u8g2.setCursor(5, 56);
  u8g2.print(" C " + String(ambient_light));
  u8g2.sendBuffer();
}
