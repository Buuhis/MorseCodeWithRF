#ifndef __SH1106_H_
#define __SH1106_H_

#include "hardware/i2c.h"
#include <stdint.h>

#define SH1106_I2C_ADDRESS 0x3C  // Default I2C address of SH1106
#define SH1106_WIDTH 128
#define SH1106_HEIGHT 64

class SH1106 {
private:
    i2c_inst_t *i2c;
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint8_t buffer[SH1106_WIDTH * SH1106_HEIGHT / 8]; // Buffer size: 128x64 / 8 = 1024 bytes

    void writeCommand(uint8_t cmd);  // Send a command to the display
    void writeData(uint8_t *data, uint16_t len); // Send data to the display
    void setCursor(uint8_t x, uint8_t page); // Set cursor position

public:
    SH1106(i2c_inst_t *i2c, uint8_t sda_pin, uint8_t scl_pin);
    void init();     // Initialize the display
    void clear();    // Clear the display buffer
    void display();  // Update the display with the buffer content
    void drawText(int16_t x, int16_t y, const char *text); // Draw text on the display
};

#endif
