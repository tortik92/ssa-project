# Pad

This subproject is part of the larger **SoundLeap** project, which aims to create smart sports equipment for visually impaired users. The **Smart Sport Pad** is a device equipped with weight sensors and a piezo buzzer, designed to provide audio feedback and interact with a main module via ESP-NOW.

## Features

- **Weight Detection**: The pad uses an HX711 weight sensor to detect when a player steps on it.
- **Audio Feedback**: A piezo buzzer emits sounds to guide or notify the user.
- **Wireless Communication**: The pad communicates with a main module (see `/compiler`) using the ESP-NOW protocol.
- **Command Handling**: The pad can process various commands, such as playing sounds or notifying when a player is on the pad.

## Hardware Requirements

- **Microcontroller**: ESP8266 (e.g., D1 Mini)
- **Weight Sensor**: HX711 load cell
- **Piezo Buzzer**
- **Power equipment**: rechargeable battery, battery charging module, power switch 
- **Other Components**: wires

## Software Overview

The main logic for the pad is implemented in [`src/main.cpp`](src/main.cpp). Key functionalities include:

- **Message Queue**: A queue system to handle incoming commands and manage their execution.
- **Commands**:
  - `play8Sounds`: Plays up to 8 sounds with specified frequencies and durations.
  - `play16Sounds`: Plays up to 16 sounds in two batches of 8.
  - `sayWhenPlayerOnPad`: Notifies the main module when a player steps on the pad.
  - `cancelPlaySound` and `cancelSayPlayerOnPad`: Cancels ongoing operations.
- **ESP-NOW Communication**: Handles sending and receiving messages between the pad and the main module.

## Setup Instructions

1. **Install PlatformIO**: Ensure you have PlatformIO installed in your development environment (e.g., Visual Studio Code).
2. **Open the Project**:
   - Clone the whole project with `git clone https://github.com/tortik92/ssa-project.git` or with the `Clone Repository` button in your IDE
   - If you use Visual Studio Code, go to `File -> Open Folder` and select the `/pad` folder to start development. 
3. **Upload the Code**:
   - Connect your ESP8266 board to your computer.
   - Use PlatformIO to build and upload the code to the board.
4. **Hardware Connections**:
   - Connect the HX711 load cell to the specified pins (`D5` and `D6`).
   - Connect the piezo buzzer to pin `D2`.

## Usage

1. Power on the pad.
2. The pad will initialize and play a starting melody to indicate readiness.
3. Commands can be sent from the main module to control the pad's behavior:
   - Play sounds.
   - Notify when a player steps on the pad.
   - Cancel ongoing operations.

## Communication Protocol

The pad communicates with the main module using ESP-NOW. Messages are structured as follows:

- **SendMsg**: Used to send responses to the main module.
- **RecvMsg**: Used to receive commands from the main module.

Refer to the `OnDataRecv` and `OnDataSent` callbacks in [`src/main.cpp`](src/main.cpp) for implementation details.

## Future Improvements

- Add support for more complex sound patterns and high-quality sounds (with speakers).
- Implement additional feedback mechanisms, such as LED strips or vibration.
- improve user and object detection (we tried to use microwave radar technology, which was very inaccurate)

## License

See the main `README.md` for more details.