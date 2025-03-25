# Morse Code Communication with RF24 on Raspberry Pi Pico

## Project Overview
The ultimate goal of this project is to enable Morse code communication between two Raspberry Pi Pico boards using the RF24 module.

## Initial Testing
Before implementing Morse code communication, we will verify basic communication between the two microcontrollers:
- **Transmitter (picoTx):** A button will be connected to **GP22**.
- **Receiver (picoRx):** An LED will be connected to **GP25**.
- When the button on **picoTx** is pressed, the LED on **picoRx** will toggle its state.

This test ensures that the RF24 module is functioning correctly before proceeding with Morse code transmission.

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


