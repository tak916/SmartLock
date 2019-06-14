# SmartLock
SmartLock based on AT89C52 and periph  
Final Assignment of Shanghai Second Polytechnic University Embedded System Design Course  
Please open the schematic with Proteus and source code with Keil C51  
DO NOT violate MIT license when cloning this repo.  
# Periphs included
* MCU
> AT89C52
* EEPROM 
> AT24C02
* Decoder
> 74HC595
* LCD Display
> LCD1602
* Others
> Matrix Keyboard  
RESPACK-8  
Sounder  
MOSFET  
LED-RED  
Relay  
# Current Functionalities:
* Able to unlock by preset password and reset 8-digit password
* Can store password into EEPROM integrated on the PCB
* UI will be locked for 15 min if password is entered wrongly for 3 times
* Able to warn neighbours with integrated sounder.
# Known Issues/Tips:
* Keyboard may be laggy
* Default password: 11111111
# Contact Me
* If you encounter any bugs feel free to code review this project
* Email:974629667@qq.com
