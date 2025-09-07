#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_TSC2007.h>

// Touch kalibráció
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define TS_MIN_PRESSURE 200

Adafruit_TSC2007 ts;

// ILI9341 lábak ESP32-höz
#define TFT_CS   5
#define TFT_DC   6
#define TFT_RST  7   // opcionális
#define TFT_MOSI 11
#define TFT_CLK  12
#define TFT_MISO 13

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_MISO);

#define I2C_SDA 8
#define I2C_SCL 9


// színes dobozok
#define BOXSIZE 52
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void) {
  Serial.begin(115200);

  // I²C init (touch)
  Wire.begin(I2C_SDA, I2C_SCL);

  tft.begin();

  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");

  tft.fillScreen(ILI9341_BLACK);

  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);

  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
  currentcolor = ILI9341_RED;
}

void loop() {
  uint16_t x, y, z1, z2;
  if (ts.read_touch(&x, &y, &z1, &z2) && (z1 > TS_MIN_PRESSURE)) {

    x = map(x, TS_MINX, TS_MAXX, 0, tft.width());
    y = map(y, TS_MINY, TS_MAXY, 0, tft.height());

    if (y < BOXSIZE) {
      oldcolor = currentcolor;

      if (x < BOXSIZE) currentcolor = ILI9341_RED;
      else if (x < BOXSIZE*2) currentcolor = ILI9341_YELLOW;
      else if (x < BOXSIZE*3) currentcolor = ILI9341_GREEN;
      else if (x < BOXSIZE*4) currentcolor = ILI9341_CYAN;
      else if (x < BOXSIZE*5) currentcolor = ILI9341_BLUE;
      else if (x < BOXSIZE*6) currentcolor = ILI9341_MAGENTA;

      if (oldcolor != currentcolor) {
        tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
        tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
        tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
        tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
        tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
        tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
        tft.drawRect((currentcolor/0x10000)*BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
      }
    }
    if (((y-PENRADIUS) > BOXSIZE) && ((y+PENRADIUS) < tft.height())) {
      tft.fillCircle(x, y, PENRADIUS, currentcolor);
    }
  }
}
