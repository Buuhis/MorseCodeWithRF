#include "SH1106.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdio.h>

// 5x7 font supporting A-Z
static const uint8_t font5x7[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, // Space
    0x3E, 0x05, 0x05, 0x05, 0x3E, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x7F, 0x41, 0x41, 0x41, 0x3E, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x7F, 0x09, 0x09, 0x09, 0x01, // F
    0x3E, 0x41, 0x41, 0x49, 0x3A, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x20, 0x40, 0x41, 0x3F, 0x01, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O
    0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, // R
    0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x3F, 0x40, 0x38, 0x40, 0x3F, // W
    0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x07, 0x08, 0x70, 0x08, 0x07, // Y
    0x61, 0x51, 0x49, 0x45, 0x43  // Z
};

SH1106::SH1106(i2c_inst_t *i2c, uint8_t sda_pin, uint8_t scl_pin) {
    this->i2c = i2c;
    this->sda_pin = sda_pin;
    this->scl_pin = scl_pin;
}

void SH1106::writeCommand(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(i2c, SH1106_I2C_ADDRESS, buf, 2, false);
}

void SH1106::writeData(uint8_t *data, uint16_t len) {
    uint8_t buf[len + 1];
    buf[0] = 0x40;
    memcpy(buf + 1, data, len);
    i2c_write_blocking(i2c, SH1106_I2C_ADDRESS, buf, len + 1, false);
}

void SH1106::setCursor(uint8_t x, uint8_t page) {
    writeCommand(0xB0 + page);
    writeCommand(0x00 + (x & 0x0F));
    writeCommand(0x10 + (x >> 4));
}

void SH1106::init() {
    i2c_init(i2c, 100000);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    sleep_ms(100);
    writeCommand(0xAE); // Display OFF
    writeCommand(0xD5); writeCommand(0x80); // Set clock divide ratio
    writeCommand(0xA8); writeCommand(0x3F); // Set multiplex ratio (64)
    writeCommand(0xD3); writeCommand(0x00); // Set display offset
    writeCommand(0x40); // Set start line
    writeCommand(0xA1); // Segment remap
    writeCommand(0xC8); // COM scan direction
    writeCommand(0xDA); writeCommand(0x12); // Set COM pins
    writeCommand(0x81); writeCommand(0x7F); // Set contrast
    writeCommand(0xA4); // Entire display ON
    writeCommand(0xA6); // Normal display mode
    writeCommand(0xD9); writeCommand(0x22); // Set pre-charge period
    writeCommand(0xDB); writeCommand(0x20); // Set VCOMH level
    writeCommand(0x8D); writeCommand(0x14); // Enable charge pump
    writeCommand(0xAF); // Display ON
    clear();
    display();
}

void SH1106::clear() {
    memset(buffer, 0, sizeof(buffer));
}

void SH1106::display() {
    for (uint8_t page = 0; page < 8; page++) {
        setCursor(2, page); // Offset 2 columns for SH1106
        writeData(&buffer[page * SH1106_WIDTH], SH1106_WIDTH);
    }
}

void SH1106::drawText(int16_t x, int16_t y, const char *text) {
    int16_t current_x = x;
    uint8_t current_page = y / 8;
    uint8_t offset = y % 8;

    while (*text) {
        // If exceeding width (128 pixels), move to next line
        if (current_x + 6 > SH1106_WIDTH) {
            current_x = x; // Reset to initial x position
            current_page++; // Move to the next line
            if (current_page >= 8) return; // Exit if exceeding 8 pages
        }

        uint8_t c = *text - 'A'; // Map 'A' to index 0-25
        if (*text == ' ') c = 0; // Space
        else if (c > 25) c = 0;  // Unsupported characters use space

        uint16_t pos = current_page * SH1106_WIDTH + current_x;
        for (uint8_t i = 0; i < 5; i++) {
            if (pos + i < sizeof(buffer)) {
                buffer[pos + i] |= (font5x7[(c + 1) * 5 + i] << offset);
                if (offset > 0 && current_page < 7) {
                    buffer[pos + i + SH1106_WIDTH] |= (font5x7[(c + 1) * 5 + i] >> (8 - offset));
                }
            }
        }
        current_x += 6; // 5 columns for a character + 1 column spacing
        text++;
    }
}
