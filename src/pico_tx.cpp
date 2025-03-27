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

    gpio_init(BUTTON_DOT);
    gpio_set_dir(BUTTON_DOT, GPIO_IN);
    gpio_pull_up(BUTTON_DOT);

    gpio_init(BUTTON_DASH);
    gpio_set_dir(BUTTON_DASH, GPIO_IN);
    gpio_pull_up(BUTTON_DASH);

    gpio_init(BUTTON_SEND);
    gpio_set_dir(BUTTON_SEND, GPIO_IN);
    gpio_pull_up(BUTTON_SEND);

    NRF24 nrf(spi0, RF24_CSN_PIN, RF24_CE_PIN);
    nrf.config();
    nrf.modeTX();

    char buffer[32] = {0};
    int index = 0;
    int send_press_count = 0;
    uint32_t last_press_time = 0;

    while (1) {
        if (!gpio_get(BUTTON_DOT)) {
            if (index < 31) {
                buffer[index++] = '.';
                printf("Added dot: %s\n", buffer);
            }
            sleep_ms(200);
            send_press_count = 0; // Reset count when another button is pressed
        }
        else if (!gpio_get(BUTTON_DASH)) {
            if (index < 31) {
                buffer[index++] = '-';
                printf("Added dash: %s\n", buffer);
            }
            sleep_ms(200);
            send_press_count = 0; // Reset count when another button is pressed
        }
        else if (!gpio_get(BUTTON_SEND)) {
            uint32_t current_time = time_us_32() / 1000;
            if (current_time - last_press_time > 1000) { // Reset if more than 1 second
                send_press_count = 0;
            }
            if (current_time - last_press_time > 200) { // Debounce
                send_press_count++;
                last_press_time = current_time;

                if (send_press_count == 3) { // Clear buffer on triple press
                    strcpy(buffer, "CLEAR");
                    nrf.sendMessage(buffer);
                    printf("Sent: %s\n", buffer);
                    send_press_count = 0;
                    index = 0;
                    memset(buffer, 0, sizeof(buffer));
                }
                else if (index > 0) { // Send message if there is input
                    buffer[index] = '\0';
                    nrf.sendMessage(buffer);
                    printf("Sent: %s\n", buffer);
                    index = 0;
                    memset(buffer, 0, sizeof(buffer));
                }
            }
            sleep_ms(200);
        }
        sleep_ms(10);
    }
    return 0;
}


