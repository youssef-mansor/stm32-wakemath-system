# WakeMath

WakeMath is an embedded systems project built on the **STM32 Nucleo-L432KC**.  
It detects prolonged inactivity using a **PIR motion sensor**, triggers a **buzzer alarm**,  
and requires the user to solve a **math challenge** on a **Qapass LCD** using the  
**Digilent Pmod KYPD** keypad before the alarm will stop.

## Features
- **PIR-based inactivity detection**
- **Buzzer alarm** triggered after inactivity timeout
- **LCD math challenge** displayed on a Qapass character LCD
- **User input via Digilent Pmod KYPD**
- **Alarm deactivates only when the correct answer is entered**

## Hardware Used
- STM32 Nucleo L432KC
- HC-SR501 PIR Motion Sensor
- Passive or Active Buzzer
- Qapass 1602 LCD (I2C)
- Digilent Pmod KYPD (4×4 matrix keypad)
- Jumper wires & breadboard

## Planned Functionality
- Motion detection & inactivity timer  
- Math problem generator  
- Keypad input handler  
- LCD display interface  
- Alarm logic state machine  

## Status
**Motion Detection & inactivity timer:** The PIR sensor is functional but the inactivity timer is yet to be implemented
**Math Problem Generator:** Fully functional
**Keypad input handler:** Fully functional
**LCD display interface:** Fully functional
**Alarm logic state machine:** Unimplemented

## License
MIT


