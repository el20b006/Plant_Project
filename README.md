# TempCheck

The Plant Project is a program that helps you track if your plant is happy with the current temperature.
The program is designed to be used for a peace lily. The specification of proper temperatures can be found in Menu>Description.

It consists of two parts:
One uses a temperature sensor to get data, the other visualises the data.


Program on the Microchip:
The program receives the data from a MCP9808 temperature sensor and converts it into readable temperatures.
For this the data must be converted into binary code, which can then be used to calculate a decimal temperature.
The program then sends the data via UART to the computer.
The programm is written in C using STM32CubeIDE.


Program on the Computer:
The program creates a GUI using gtk3.0, which displays the temperatures to the end user.
For improved usability the program also shows a graphical interpretation of the data.
The program is written in C++ and uses windows specific functions to access the microchip connected via USB.