# Morse Code Communication with RF24 on Raspberry Pi Pico

## Project Overview
The ultimate goal of this project is to enable Morse code communication between two Raspberry Pi Pico boards using the RF24 module.

## Project Development Progress
In this update, we will integrate `3 buttons` for sending, along with a `buzzer and an SH1106 OLED display` for outputting and displaying the content. 

**`To clear the screen, press BUTTON_SEND 3 times.`**

## Hardware Setup
### picoTx (Transmitter)
- **Buttons:**
  - GP22: Dot button
  - GP13: Dash button
  - GP14: Send button
- **NRF24 Module:**
  - CSN: GP17
  - CE: GP6

### picoRx (Receiver)
- **LEDs:**
  - GP15: LED for dot signal
  - GP25: LED for dash signal
- **NRF24 Module:**
  - CSN: GP17
  - CE: GP6
- **Buzzer:**
  - GP16: Buzzer output
- **SH1106 OLED Display:**
  - I2C Communication (SDA: GP4, SCL: GP5)

## How to Run and Test the Code
### Requirement
First, you need to have the Pico SDK installed: [Pico SDK GitHub](https://github.com/raspberrypi/pico-sdk).

### Build and Compile
1. Create a build directory and navigate into it:
   ```bash
   mkdir build && cd build
   ```
2. Generate the build files using CMake:
   ```bash
   cmake ..
   ```
3. Compile the code:
   ```bash
   make
   ```
