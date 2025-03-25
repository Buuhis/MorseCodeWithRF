#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <cstring>
#include <cstdio>
#include "NRF24.h"

#define BUTTON_DOT   22  // Dot button
#define BUTTON_DASH  13  // Dash button
#define BUTTON_SEND  14  // Send button
#define RF24_CSN_PIN 17 
#define RF24_CE_PIN  6

int main() {
    stdio_init_all();

    // Initialize buttons
    gpio_init(BUTTON_DOT);
    gpio_set_dir(BUTTON_DOT, GPIO_IN);
    gpio_pull_up(BUTTON_DOT);

    gpio_init(BUTTON_DASH);
    gpio_set_dir(BUTTON_DASH, GPIO_IN);
    gpio_pull_up(BUTTON_DASH);

    gpio_init(BUTTON_SEND);
    gpio_set_dir(BUTTON_SEND, GPIO_IN);
    gpio_pull_up(BUTTON_SEND);

    // Initialize nRF24 module
    NRF24 nrf(spi0, RF24_CSN_PIN, RF24_CE_PIN);
    nrf.config();
    nrf.modeTX(); // Set module to transmit mode

    char buffer[32] = {0};  // Buffer to store Morse code
    int index = 0;

    while (1) {
        if (!gpio_get(BUTTON_DOT)) { // Dot button pressed
            if (index < 31) {
                buffer[index++] = '.';
                printf("Added dot: %s\n", buffer);
            }
            sleep_ms(200); // Debounce
        }
        else if (!gpio_get(BUTTON_DASH)) { // Dash button pressed
            if (index < 31) {
                buffer[index++] = '-';
                printf("Added dash: %s\n", buffer);
            }
            sleep_ms(200);
        }
        else if (!gpio_get(BUTTON_SEND)) { // Send button pressed
            if (index > 0) {
                buffer[index] = '\0'; // Null-terminate string
                nrf.sendMessage(buffer); // Send via NRF24
                printf("Sent: %s\n", buffer);
                index = 0; // Reset buffer
            }
            sleep_ms(200);
        }
        sleep_ms(10); // Reduce CPU load    
    }
    return 0;
}
