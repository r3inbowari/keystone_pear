#include "display.h"
#include "apds.h"
#include "bme.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// apds的轮播索引, 由mesh的控制服务接管并进行自动页面切换
int apds_swiper_index = 0;
// bme的轮播索引, 由mesh的控制服务接管并进行自动页面切换
int bme_swiper_index = 0;

#define SLEEP_PIN 0 // button maping

OneButton sleep_btn = OneButton(
                        SLEEP_PIN,  // Input pin for the button
                        true,       // Button is active high
                        true        // Disable internal pull-up resistor
                      );

bool oled_sleep = false;

void init_display() {
  u8g2.begin();
  u8g2.enableUTF8Print();

  //  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  //  u8g2.setFontDirection(0);
  u8g2.setFont(u8g2_font_t0_11_tf);
  u8g2.setFontDirection(0);

  // open sleep listen
  sleep_btn.attachClick(handleSleep);

  sleep_btn.attachDoubleClick([]() {
    Serial.println("[BTN] Update");
    onUpdateByUser();
  });
}

void clearDraw() {
  u8g2.clearBuffer();
  u8g2.drawFrame(0, 0, 128, 20);
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
  // upload
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x18, 0x00, 0x14, 0x60, 0x02, 0x90, 0x01, 0x08, 0x18, 0x00, 0x00, 0x80, 0x19, 0x00, 0x00, 0x98, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // WIFI1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // WIFI2
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xe0, 0x07, 0x20, 0x04, 0x80, 0x01, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // WIFI3
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x30, 0x0c, 0xd8, 0x1b, 0xe0, 0x07, 0x20, 0x04, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // USB
  0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x10, 0x08, 0x50, 0x0a, 0x10, 0x08, 0x10, 0x08, 0xf8, 0x1f, 0x0c, 0x20, 0x0c, 0x20, 0x0c, 0x20, 0x0c, 0x20, 0x04, 0x20, 0x0c, 0x20, 0x00, 0x00, 0x00, 0x00,
  // AP
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0c, 0x30, 0x24, 0x24, 0x34, 0x2c, 0x96, 0x69, 0x96, 0x69, 0x34, 0x2c, 0x04, 0x20, 0x0c, 0x30, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
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

// 时间重绘
void menu_timePrint(int hh, int mm) {
  int menu_offset = (4 - 1) * 20 + MENU_OFFSET;
  u8g2.setCursor(90, 14);
  String h, m;
  if (hh < 10) {
    h = "0" + String(hh);
  } else {
    h = String(hh);
  }
  if (mm < 10) {
    m = "0" + String(mm);
  } else {
    m = String(mm);
  }
  u8g2.print(h + ":" + m);
  u8g2.sendBuffer();
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
  // menuItemsCount < 6
  for (int i = 0; i < MAX_ITEM_COUNT; i++) {
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
  // menuItemsCount < 7
  if (menuItemsCount < MAX_ITEM_COUNT + 1) {
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
  u8g2.setDrawColor(0);
  u8g2.drawBox(1, 20, 126, 20);
  u8g2.setDrawColor(1);
  switch (bme_swiper_index) {
    case BME_TAG::TA:
      {
        u8g2.setCursor(5, 38);
        u8g2.print("TA " + String(tem - 12) + String("°C"));
        u8g2.setCursor(70, 38);
        u8g2.print(alt + String("M"));
        u8g2.sendBuffer();
      }
      break;
    case BME_TAG::HP:
      {
        u8g2.setCursor(5, 38);
        u8g2.print("HP " + String(hum) + String("%"));
        u8g2.setCursor(66, 38);
        u8g2.print(pre / 1000 + String("KPa"));
        u8g2.sendBuffer();
      }
      break;
    case BME_TAG::CV:
      {
        u8g2.setCursor(5, 38);
        u8g2.print("CV " + String(co2) + String("ppm"));
        u8g2.setCursor(70, 38);
        u8g2.print(String("lv.") + tvoc);
        u8g2.sendBuffer();
      }
      break;
  }
}

void content_apds9960() {
  u8g2.setDrawColor(0);
  u8g2.drawBox(1, 45, 126, 18);
  u8g2.setDrawColor(1);
  switch (apds_swiper_index) {
    case APDS_TAG::Clear:
      {
        u8g2.setCursor(5, 56);
        u8g2.print("Clear Light " + String(ambient_light) + String("Lx"));
        u8g2.sendBuffer();
      }
      break;
    case APDS_TAG::Red:
      {
        u8g2.setCursor(5, 56);
        u8g2.print("Red Light " + String(red_light) + String("Lx"));
        u8g2.sendBuffer();
      }
      break;
    case APDS_TAG::Green:
      {
        u8g2.setCursor(5, 56);
        u8g2.print("Green Light " + String(green_light) + String("Lx"));
        u8g2.sendBuffer();
      }
      break;
    case APDS_TAG::Blue:
      {
        u8g2.setCursor(5, 56);
        u8g2.print("Blue Light " + String(blue_light) + String("Lx"));
        u8g2.sendBuffer();
      }
      break;
  }
}

void handleSleep() {
  if (oled_sleep) {
    u8g2.setPowerSave(0);
    Serial.println("[BTN] Quit PowerSave Mode");
  } else {
    u8g2.setPowerSave(1);
    Serial.println("[BTN] Enter PowerSave Mode");
  }
  oled_sleep = !oled_sleep;
}

void oled_update() {
  sleep_btn.tick();
}
