#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "NRF24.h"
#include <cstring>
#include <cstdio>

#define BUTTON_PIN 22
#define LED_PIN 25

int main() {
    stdio_init_all();

    // Initialize LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialize nRF24 module
    NRF24 nrf(spi0, RF24_CSN_PIN, RF24_CE_PIN);
    nrf.config();
    nrf.modeRX(); // Set module to receive mode

    char buffer[32];
    while (1) {
        if (nrf.newMessage()) {
            nrf.receiveMessage(buffer);
            printf("Received: %s\n", buffer);
            if (strcmp(buffer, "LED_ON") == 0) {
                gpio_put(LED_PIN, 1); // Turn LED on
            } else if (strcmp(buffer, "LED_OFF") == 0) {
                gpio_put(LED_PIN, 0); // Turn LED off
            }
        }
        sleep_ms(10); // Reduce CPU load
    }
    return 0;
}