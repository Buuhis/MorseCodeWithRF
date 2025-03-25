#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "NRF24.h"
#include <cstring>
#include <cstdio>

#define LED_DOT 15   // LED for dot signal
#define LED_DASH 25  // LED for dash signal
#define RF24_CSN_PIN 17 
#define RF24_CE_PIN  6

// Function to blink LEDs as an end signal
void blink_end_signal() {
    for (int i = 0; i < 3; i++) {
        gpio_put(LED_DOT, 1);
        gpio_put(LED_DASH, 1);
        sleep_ms(200);
        gpio_put(LED_DOT, 0);
        gpio_put(LED_DASH, 0);
        sleep_ms(200);
    }
}

int main() {
    stdio_init_all();

    // Initialize LED pins
    gpio_init(LED_DOT);
    gpio_set_dir(LED_DOT, GPIO_OUT);
    gpio_init(LED_DASH);
    gpio_set_dir(LED_DASH, GPIO_OUT);    

    // Initialize NRF24 module
    NRF24 nrf(spi0, RF24_CSN_PIN, RF24_CE_PIN);
    nrf.config();
    nrf.modeRX(); // Set module to receive mode

    char buffer[32];
    while (1) {
        if (nrf.newMessage()) { // Check for new message
            nrf.receiveMessage(buffer); // Receive message from Pico TX
            printf("Received: %s\n", buffer);
            for (int i = 0; buffer[i] != '\0'; i++) {
                if (buffer[i] == '.') {
                    gpio_put(LED_DOT, 1);
                    sleep_ms(200);
                    gpio_put(LED_DOT, 0);
                } 
                else if (buffer[i] == '-') {
                    gpio_put(LED_DASH, 1);
                    sleep_ms(600);
                    gpio_put(LED_DASH, 0);
                }
                sleep_ms(200); // Delay between Morse elements
            }
            blink_end_signal(); // Signal end of message
        }
        sleep_ms(10); // Reduce CPU load
    }    return 0;
}
