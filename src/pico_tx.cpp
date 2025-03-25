#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <cstring>
#include <cstdio>
#include "NRF24.h"

#define BUTTON_PIN 22
#define LED_PIN 25

int main() {
    stdio_init_all();

    // Initialize button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN); // Pull-up to ensure the button works (pressed = 0)

    // Initialize nRF24 module
    NRF24 nrf(spi0, RF24_CSN_PIN, RF24_CE_PIN);
    nrf.config();
    nrf.modeTX(); // Set module to transmit mode

    char buffer[32];
    while (1) {
        if (!gpio_get(BUTTON_PIN)) { // Button pressed (LOW due to pull-up)
            strcpy(buffer, "LED_ON");
            nrf.sendMessage(buffer);
            printf("Sent: LED_ON\n");
        } else { // Button not pressed
            strcpy(buffer, "LED_OFF");
            nrf.sendMessage(buffer);
            printf("Sent: LED_OFF\n");
        }
        sleep_ms(100); // Debounce and reduce CPU load
    }
    return 0;
}
