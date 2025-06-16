#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

struct RGB {
    int r, g, b;
};

class ColorSensor {
public:
    void begin();
    RGB readColor();  // szín olvasása
};

#endif
