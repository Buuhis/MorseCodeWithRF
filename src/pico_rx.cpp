#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "NRF24.h"
#include "SH1106.h"
#include <cstring>
#include <cstdio>

#define BUZZER_PIN 3    // Buzzer connected to GP3
#define RF24_CSN_PIN 17 
#define RF24_CE_PIN  6
#define OLED_SDA_PIN 10 // OLED SDA at GP10
#define OLED_SCL_PIN 11 // OLED SCL at GP11

// Morse code lookup table (A-Z)
static const struct {
    const char *morse;
    char letter;
} morse_table[] = {
    {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'}, 
    {".", 'E'}, {"..-.", 'F'}, {"--.", 'G'}, {"....", 'H'}, 
    {"..", 'I'}, {".---", 'J'}, {"-.-", 'K'}, {".-..", 'L'}, 
    {"--", 'M'}, {"-.", 'N'}, {"---", 'O'}, {".--.", 'P'}, 
    {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'}, 
    {"..-", 'U'}, {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'}, 
    {"-.--", 'Y'}, {"--..", 'Z'}
};
#define MORSE_TABLE_SIZE (sizeof(morse_table) / sizeof(morse_table[0]))

// Decode Morse code to an alphabet character
char decode_morse(const char *morse) {
    for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
        if (strcmp(morse, morse_table[i].morse) == 0) {
            return morse_table[i].letter;
        }
    }
    return '?'; // Return '?' if not found
}

int main() {
    stdio_init_all();

    // Initialize buzzer
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);

    // Initialize NRF24 module
    NRF24 nrf(spi0, RF24_CSN_PIN, RF24_CE_PIN);
    nrf.config();
    nrf.modeRX();

    // Initialize OLED SH1106
    SH1106 oled(i2c1, OLED_SDA_PIN, OLED_SCL_PIN);
    oled.init();

    char buffer[32] = {0};  // Buffer to store received Morse code
    char decoded[32] = {0}; // Buffer to store decoded text

    while (1) {
        if (nrf.newMessage()) {
            // Receive new message
            nrf.receiveMessage(buffer);
            printf("Received: %s\n", buffer);

            // Check for clear command
            if (strcmp(buffer, "CLEAR") == 0) {
                memset(decoded, 0, sizeof(decoded)); // Clear decoded text
                oled.clear();
                oled.display();
                printf("Screen cleared\n");
            }
            else {
                // Play received Morse code using the buzzer
                for (int i = 0; buffer[i] != '\0'; i++) {
                    if (buffer[i] == '.') {
                        gpio_put(BUZZER_PIN, 1);
                        sleep_ms(200);
                        gpio_put(BUZZER_PIN, 0);
                    } else if (buffer[i] == '-') {
                        gpio_put(BUZZER_PIN, 1);
                        sleep_ms(600);
                        gpio_put(BUZZER_PIN, 0);
                    }
                    sleep_ms(200);
                }

                // Decode Morse code and append to decoded string
                char temp[6] = {0};
                int temp_idx = 0;
                int decoded_idx = strlen(decoded);

                for (int i = 0; buffer[i] != '\0' && decoded_idx < 31; i++) {
                    if (buffer[i] == ' ') {
                        if (temp_idx > 0) {
                            temp[temp_idx] = '\0';
                            decoded[decoded_idx++] = decode_morse(temp);
                            temp_idx = 0;
                            memset(temp, 0, sizeof(temp));
                        }
                    } else if (buffer[i] == '.' || buffer[i] == '-') {
                        if (temp_idx < 5) {
                            temp[temp_idx++] = buffer[i];
                        }
                    }
                }
                if (temp_idx > 0 && decoded_idx < 31) {
                    temp[temp_idx] = '\0';
                    decoded[decoded_idx++] = decode_morse(temp);
                }
                decoded[decoded_idx] = '\0';
                printf("Decoded: %s\n", decoded);

                // Display decoded text on OLED
                oled.clear();
                oled.drawText(0, 16, decoded); // Start from x=0, y=16
                oled.display();
            }
        }

        sleep_ms(50); // Reduce CPU usage
    }
    return 0;
}

