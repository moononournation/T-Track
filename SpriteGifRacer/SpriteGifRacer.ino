/*******************************************************************************
 * Sprite GIF Demo
 * This is a sprite demo using a static GIF as a master image
 * Image Source:
 * https://craftpix.net/freebies/free-racing-game-kit/
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload GIF file
 *   FFat (ESP32):
 *     upload FFat (FatFS) data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   LittleFS (ESP32 / ESP8266 / Pico):
 *     upload LittleFS data with ESP8266 LittleFS Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *     ESP8266: https://github.com/earlephilhower/arduino-esp8266littlefs-plugin
 *     Pico: https://github.com/earlephilhower/arduino-pico-littlefs-plugin.git
 *   SPIFFS (ESP32):
 *     upload SPIFFS data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   SD:
 *     Most Arduino system built-in support SD file system.
 *     Wio Terminal require extra dependant Libraries:
 *     - Seeed_Arduino_FS: https://github.com/Seeed-Studio/Seeed_Arduino_FS.git
 *     - Seeed_Arduino_SFUD: https://github.com/Seeed-Studio/Seeed_Arduino_SFUD.git
 ******************************************************************************/
#define GIF_FILENAME "/racer294.gif"

/*******************************************************************************
 * Start of Arduino_GFX setting
 *
 * Arduino_GFX try to find the settings depends on selected board in Arduino IDE
 * Or you can define the display dev kit not in the board list
 * Defalult pin list for non display dev kit:
 * Arduino Nano, Micro and more: CS:  9, DC:  8, RST:  7, BL:  6, SCK: 13, MOSI: 11, MISO: 12
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22, SCK: 18, MOSI: 23, MISO: nil
 * ESP32-C3 various dev board  : CS:  7, DC:  2, RST:  1, BL:  3, SCK:  4, MOSI:  6, MISO: nil
 * ESP32-S2 various dev board  : CS: 34, DC: 38, RST: 33, BL: 21, SCK: 36, MOSI: 35, MISO: nil
 * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48, SCK: 36, MOSI: 35, MISO: nil
 * ESP8266 various dev board   : CS: 15, DC:  4, RST:  2, BL:  5, SCK: 14, MOSI: 13, MISO: 12
 * Raspberry Pi Pico dev board : CS: 17, DC: 27, RST: 26, BL: 28, SCK: 18, MOSI: 19, MISO: 16
 * RTL8720 BW16 old patch core : CS: 18, DC: 17, RST:  2, BL: 23, SCK: 19, MOSI: 21, MISO: 20
 * RTL8720_BW16 Official core  : CS:  9, DC:  8, RST:  6, BL:  3, SCK: 10, MOSI: 12, MISO: 11
 * RTL8722 dev board           : CS: 18, DC: 17, RST: 22, BL: 23, SCK: 13, MOSI: 11, MISO: 12
 * RTL8722_mini dev board      : CS: 12, DC: 14, RST: 15, BL: 13, SCK: 11, MOSI:  9, MISO: 10
 * Seeeduino XIAO dev board    : CS:  3, DC:  2, RST:  1, BL:  0, SCK:  8, MOSI: 10, MISO:  9
 * Teensy 4.1 dev board        : CS: 39, DC: 41, RST: 40, BL: 22, SCK: 13, MOSI: 11, MISO: 12
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#define GFX_DEV_DEVICE LILYGO_T_TRACK
#define GFX_EXTRA_PRE_INIT()           \
  {                                    \
    pinMode(4 /* POWER */, OUTPUT);    \
    digitalWrite(4 /* POWER */, HIGH); \
  }
Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(7 /* DC */, 9 /* CS */, 5 /* SCK */, 6 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_GFX *gfx = new Arduino_JD9613(bus, 8 /* RST */);
Arduino_Canvas_Indexed *canvasGfx = new Arduino_Canvas_Indexed(126 /* width */, 294 /* height */, gfx, 0, 0, 0);
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#include "TTRACK_PINS.h"
#include "trackball.h"

#include <LittleFS.h>

#include "GifClass.h"
static GifClass gifClass;

uint8_t *spriteMaster;
bool spriteInitiated = false;

#include "IndexedSprite.h"
IndexedSprite *road;
IndexedSprite *car1;
IndexedSprite *car1l;
IndexedSprite *car1r;
IndexedSprite *car2;
IndexedSprite *car2l;
IndexedSprite *car2r;
IndexedSprite *car3;
IndexedSprite *car3l;
IndexedSprite *car3r;

int speed = 2;
int carType = 1;
int car_x = 26;
int turning = 0;
int frame = 0;
int fpsSnapShot = 0;
unsigned long nextSnap = 0;

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("Arduino_GFX GIF Sprite example");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!canvasGfx->begin(80000000))
  {
    Serial.println("canvasGfx->begin() failed!");
  }
  canvasGfx->fillScreen(BLACK);
  canvasGfx->flush();
  canvasGfx->setDirectUseColorIndex(true);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  init_trackball(1);

  if (!LittleFS.begin())
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
    exit(0);
  }

  File gifFile = LittleFS.open(GIF_FILENAME, "r");
  if (!gifFile || gifFile.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
  }
  else
  {
    // read GIF file header
    gd_GIF *gif = gifClass.gd_open_gif(&gifFile);
    if (!gif)
    {
      Serial.println(F("gd_open_gif() failed!"));
    }
    else
    {
      spriteMaster = (uint8_t *)malloc(gif->width * gif->height / 2);
      if (!spriteMaster)
      {
        Serial.println(F("spriteMaster malloc failed!"));
      }
      else
      {
        int32_t res = gifClass.gd_get_frame(gif, spriteMaster);

        if (res > 0)
        {
          // inital palette
          uint16_t *palette = canvasGfx->getColorIndex();
          memcpy(palette, gif->palette->colors, gif->palette->len * 2);

          // IndexedSprite(x, y, *bitmap, *palette, w, h, x_skip, h_loop, v_loop, frames, speed_divider, chroma_key)
          road = new IndexedSprite(0, 0, spriteMaster + 3, palette, 126, 294, 168, false, true, 1, 1);

          car1 = new IndexedSprite(26, 180, spriteMaster + (177 * 294) + 206, palette, 29, 62, 294 - 29, false, false, 1, 1, gif->gce.tindex);
          car1l = new IndexedSprite(26, 180, spriteMaster + (178 * 294) + 155, palette, 34, 62, 294 - 34, false, false, 1, 1, gif->gce.tindex);
          car1r = new IndexedSprite(26, 180, spriteMaster + (178 * 294) + 252, palette, 34, 62, 294 - 34, false, false, 1, 1, gif->gce.tindex);

          car2 = new IndexedSprite(26, 180, spriteMaster + (78 * 294) + 203, palette, 35, 88, 294 - 35, false, false, 1, 1, gif->gce.tindex);
          car2l = new IndexedSprite(26, 180, spriteMaster + (78 * 294) + 148, palette, 48, 89, 294 - 48, false, false, 1, 1, gif->gce.tindex);
          car2r = new IndexedSprite(26, 180, spriteMaster + (78 * 294) + 246, palette, 46, 69, 294 - 46, false, false, 1, 1, gif->gce.tindex);

          car3 = new IndexedSprite(26, 180, spriteMaster + (2 * 294) + 205, palette, 31, 69, 294 - 31, false, false, 1, 1, gif->gce.tindex);
          car3l = new IndexedSprite(26, 180, spriteMaster + (2 * 294) + 153, palette, 38, 69, 294 - 38, false, false, 1, 1, gif->gce.tindex);
          car3r = new IndexedSprite(26, 180, spriteMaster + (2 * 294) + 251, palette, 37, 69, 294 - 37, false, false, 1, 1, gif->gce.tindex);

          spriteInitiated = true;
        }

        gifClass.gd_close_gif(gif);
      }
    }
  }

  canvasGfx->fillScreen(0xfd);
  canvasGfx->setTextColor(0xfd, 0x00);
  canvasGfx->setTextSize(2);
}

bool otherFrame = false;
void testingLoop(void)
{
  if (spriteInitiated)
  {
    road->v_scroll(speed);
    road->draw(canvasGfx);

    if (turning < 0)
    {
      turning++;
      if (carType == 2)
      {
        car2l->set_x(car_x);
        car2l->draw(canvasGfx);
      }
      else if (carType == 3)
      {
        car3l->set_x(car_x);
        car3l->draw(canvasGfx);
      }
      else
      {
        car1l->set_x(car_x);
        car1l->draw(canvasGfx);
      }
    }
    else if (turning > 0)
    {
      turning--;
      if (carType == 2)
      {
        car2r->set_x(car_x);
        car2r->draw(canvasGfx);
      }
      else if (carType == 3)
      {
        car3r->set_x(car_x);
        car3r->draw(canvasGfx);
      }
      else
      {
        car1r->set_x(car_x);
        car1r->draw(canvasGfx);
      }
    }
    else
    {
      if (carType == 2)
      {
        car2->set_x(car_x);
        car2->draw(canvasGfx);
      }
      else if (carType == 3)
      {
        car3->set_x(car_x);
        car3->draw(canvasGfx);
      }
      else
      {
        car1->set_x(car_x);
        car1->draw(canvasGfx);
      }
    }
  }
}

void loop()
{
  // handle speed
  if (trackball_up_count)
  {
    speed++;
    trackball_up_count = 0;
  }
  if (trackball_down_count)
  {
    speed--;
    trackball_down_count = 0;
  }
  if (speed > 10)
  {
    speed = 10;
  }
  if (speed < 1)
  {
    speed = 1;
  }

  // handle turning
  turning -= trackball_left_count * 6;
  turning += trackball_right_count * 6;
  trackball_left_count = 0;
  trackball_right_count = 0;
  if (turning < 0)
  {
    car_x -= speed;
  }
  else if (turning > 0)
  {
    car_x += speed;
  }
  if (car_x > 68)
  {
    car_x = 68;
  }
  else if (car_x < 24)
  {
    car_x = 24;
  }

  // handle click
  if (trackball_click_count)
  {
    if (carType == 1)
    {
      carType = 2;
    }
    else if (carType == 2)
    {
      carType = 3;
    }
    else
    {
      carType = 1;
    }
    trackball_click_count = 0;
  }

  testingLoop();

  canvasGfx->setCursor(32, 8);
  canvasGfx->print(fpsSnapShot);

  canvasGfx->flush();

  // calculate FPS
  frame++;
  if (millis() > nextSnap)
  {
    fpsSnapShot = frame;
    frame = 0;
    nextSnap = millis() + 1000;
  }
}
