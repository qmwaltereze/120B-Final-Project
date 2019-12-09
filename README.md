# 120Fall-Final-Project

Custom Project Report

Description
Using the Atmega1284 to demonstrate a simple locking system designed to allow the user to lock and unlock an application, in this case a door. During certain intervals of the day, the locking system will be disabled and the code system will be enabled allowing for a code to be input to unlock or lock the door. At anytime the user may input a new code that they can use to lock and unlock the door. 

User Guide
When power is given to the system, the system starts by displaying the current state of the door, prompting the user to input a code, displaying time, and displaying the number of reboots the system has undergone on the LCD.
The system always starts locked and at 12:00 AM
From 6 PM to 6 AM the transmitter microcontroller relays to the receiving microcontroller to illuminate a light.
When the light goes out at 6 AM the user will have a buffer of one hour before they can input a code. When the light comes on at 6 PM the user will have a buffer of three hours before the lock system is enabled.
At 7 AM the user will be permitted to input a 3 digit code. From 7 AM to 9 PM, the user will be permitted to unlock and lock the door.
The user may choose to input the code currently programmed into the system or change code the code and program a new code into the system themselves.
The code can be changed at any time of the day.
Pressing a button indicates that the user wishes to change the code. Input a 3 digit code. Pressing the button again indicates that the new code has been set.
Note: Once the button is pressed, user must enter a new code. Pressing the button before the third digit has been set does nothing. Pressing multiple buttons at the same time does nothing. Inputting the same number twice in a row does nothing.The status of the door cannot be changed until a new code has been set.
Inputting the correct code will either unlock the door if it was locked, displaying “open” on the LCD, or lock the door if it was unlocked, displaying “Enter code”. Inputting the wrong code does nothing and the system remains in its current state.
Note: Button presses for the code may need to held from a couple seconds before the system recognizes the press.
Once the time reaches 9 PM the lock system is enabled.
If the door is locked at this time, the user can no longer unlock the door until morning. If the user was in the middle of inputting a code, that code is void and does nothing.
If the door is unlocked at this time, the user may still enter a code. Once the new code has been entered and the door is locked, the user can no longer enter a code until the morning.
Toggling power on and off will start the system back at 12:00 AM in a locked state. The latest version of the code programmed to the system is saved. The number of days since the last time the system was rebooted is displayed on the LCD.

Youtube Video
https://www.youtube.com/watch?v=zane53QcCTc

Images/Drawings
https://drive.google.com/drive/u/2/folders/1ITm-Hdwm4AKZrwpbhh4yGHmUWkcOEANb

IO Files
bit.h: 
Sets bit on a PORT by taking in a char, the pin number and the binary value and returning the new value of the PORT
timer.h: 
Sets the period of the system, in the case, the GCD
eeprom.h: 
Save data to byte at specified memory location
usart.h: 
Enabling communication between two atmegas. Utilizing usart_send and usrat_recievce.
io.c: 
LCD 4 pin configuration implementation, sending the higher then lower nibbles
https://drive.google.com/drive/u/2/folders/149FDrVOq2Y2sdJRQLE_GRjMXdqDYndZS

Included in main:
buildcharacter.h: 
Capacity to hold 8 characters to memory, writing in individual pixels to form a character
keypad.h: 
Implementation of keypad and GetKEyPAdKey functionality
scheduler.h: 
Find the GCD and implements a task struct for the state machines

Sources used
Atmega1284 datasheet
http://ww1.microchip.com/downloads/en/devicedoc/atmel-42718-atmega1284_datasheet.pdf

LCD configuration (modified to better fit the functionality of the LCD 8 pin configuration give in class) https://www.electronicwings.com/avr-atmega/interfacing-lcd-16x2-in-4-bit-mode-with-atmega-16-32-

Usart example (has nothing to do with my project but it helped my understand usart a bit better)
https://www.instructables.com/id/Heart-Monitor/

Eeprom (good reference for figuring out eeprom use and implementation)
https://www.youtube.com/watch?v=LPaOVwjKRqs
Eeprom.h http://ww1.microchip.com/downloads/en/devicedoc/atmel-42718-atmega1284_datasheet.pdf

Custom Characters
http://www.handsonembedded.com/lcd16x2-hd44780-tutorial-5/
http://saeedsolutions.blogspot.com/2012/12/how-to-display-custom-characters-on-lcd.html

Keypad Code Implementation (I had a lot of trouble getting this to work at first, referenced this custom project for help. Aslo where I found a reference for the custom characters!)
https://github.com/rosalba78/EE_120B_Custom_Lab_Security_System
