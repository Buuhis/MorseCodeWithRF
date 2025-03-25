#include "NRF24.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string.h>
#include <cstdio>

NRF24::NRF24(spi_inst_t *port, uint16_t csn, uint16_t ce) {
    this->port = port;
    this->csn = csn;
    this->ce = ce;

    spi_init(this->port, 1000000); // 1MHz SPI
    gpio_set_function(RF24_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(RF24_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(RF24_MISO_PIN, GPIO_FUNC_SPI);

    printf("SPI initialized\n");

    gpio_init(csn);
    gpio_init(ce);
    gpio_set_dir(csn, 1);
    gpio_set_dir(ce, 1);

    ceLow();
    csnHigh();
}

NRF24::~NRF24() {}

uint8_t NRF24::readReg(uint8_t reg) {
    uint8_t result = 0;
    reg = (0b00011111 & reg); // Mask register address
    csnLow();
    spi_write_blocking(port, &reg, 1);
    spi_read_blocking(port, 0xff, &result, 1);
    csnHigh();
    printf("Read reg %d: %d\n", reg, result);
    return result;
}

void NRF24::writeReg(uint8_t reg, uint8_t data) {
    writeReg(reg, &data, 1);
}

void NRF24::writeReg(uint8_t reg, uint8_t *data, uint8_t size) {
    reg = 0b00100000 | (0b00011111 & reg); // Write command with register address
    csnLow();
    spi_write_blocking(port, &reg, 1);
    spi_write_blocking(port, data, size);
    csnHigh();
}

void NRF24::config() {
    csnHigh();
    ceLow();
    sleep_us(1500); // Wait 1.5ms (as per datasheet)
    writeReg(0, 0b00001010); // Config: PWR_UP=1, CRC enabled
    printf("Config reg: %d\n", readReg(0));
    sleep_us(1500); // Additional 1.5ms stabilization
    writeReg(1, 0); // Disable ACK
    writeReg(5, 60); // Set channel to 60
    writeReg(0x0a, (uint8_t*)"pico1", 5); // RX address
    writeReg(0x10, (uint8_t*)"pico1", 5); // TX address
    writeReg(0x11, 32); // Payload length 32 bytes
}

void NRF24::modeTX() {
    uint8_t reg = readReg(0);
    reg &= ~(1 << 0); // PRIM_RX = 0 (TX mode)
    writeReg(0, reg);
    ceLow();
    sleep_us(130); // Wait 130µs
    printf("TX mode: %d\n", readReg(0));
}

void NRF24::modeRX() {
    uint8_t reg = readReg(0);
    reg |= (1 << 0); // PRIM_RX = 1 (RX mode)
    writeReg(0, reg);
    ceHigh();
    sleep_us(130); // Wait 130µs
    printf("RX mode: %d\n", readReg(0));
}

void NRF24::sendMessage(char *msg) {
    uint8_t reg = 0b10100000; // Write payload command
    csnLow();
    spi_write_blocking(port, &reg, 1);
    spi_write_blocking(port, (uint8_t*)msg, 32);
    csnHigh();
    ceHigh();
    sleep_us(50); // Wait 10µs
    ceLow();
}

void NRF24::receiveMessage(char *msg) {
    uint8_t cmd = 0b01100001; // Read payload command
    csnLow();
    spi_write_blocking(port, &cmd, 1);
    spi_read_blocking(port, 0xff, (uint8_t*)msg, 32);
    csnHigh();
    printf("Received raw: %s\n", msg);
}

uint8_t NRF24::newMessage() {
    uint8_t fifo_status = readReg(0x17); // Read FIFO status register
    return !(0x00000001 & fifo_status); // Check if RX FIFO is empty
}

void NRF24::setChannel(uint8_t ch) {
    writeReg(5, ch); // Set RF channel
}

void NRF24::setRXName(char *name) {
    if (strlen(name) != 5) return; // Ensure name length is 5 bytes
    writeReg(0x0a, (uint8_t*)name, 5);
}

void NRF24::setTXName(char *name) {
    if (strlen(name) != 5) return; // Ensure name length is 5 bytes
    writeReg(0x10, (uint8_t*)name, 5);
}
