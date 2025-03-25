#ifndef __NRF24_H_
#define __NRF24_H_
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define RF24_CE_PIN 6
#define RF24_CSN_PIN 17
#define RF24_SCK_PIN 18
#define RF24_MOSI_PIN 19
#define RF24_MISO_PIN 16
#define RF24_IRQ_PIN 21

class NRF24 {
private:
    spi_inst_t *port; // SPI port
    uint16_t csn; // Chip Select Not (CSN) pin
    uint16_t ce; // Chip Enable (CE) pin

    void csnLow() { gpio_put(csn, 0); } // Set CSN low
    void csnHigh() { gpio_put(csn, 1); } // Set CSN high
    void ceLow() { gpio_put(ce, 0); } // Set CE low
    void ceHigh() { gpio_put(ce, 1); } // Set CE high

public:
    NRF24(spi_inst_t *port, uint16_t csn, uint16_t ce);
    ~NRF24();

    uint8_t readReg(uint8_t reg); // Read register
    void writeReg(uint8_t reg, uint8_t data); // Write single byte to register
    void writeReg(uint8_t reg, uint8_t *data, uint8_t size); // Write multiple bytes to register

    void config(); // Configure nRF24 module
    void modeTX(); // Set module to transmit mode
    void modeRX(); // Set module to receive mode
    void sendMessage(char *msg); // Send message
    void receiveMessage(char *msg); // Receive message
    uint8_t newMessage(); // Check if a new message is available
    void setChannel(uint8_t ch); // Set communication channel
    void setRXName(char *name); // Set receiver name
    void setTXName(char *name); // Set transmitter name
};

#endif
