This program, designed for the ATtiny85 microcontroller, is a simple and reliable MIDI-to-CV converter tailored for monophonic applications.
It transforms MIDI signals into analog control voltages (CV) and digital gate signals, making it ideal for integrating MIDI devices with analog synthesizers or other CV-based hardware.

The program reads MIDI input at the standard baud rate of 31,250 and converts note data into a 500 kHz PWM signal, which can easily be filtered using a low-pass filter to produce a continuous control voltage.

The gate output reflects note activity by turning ON when a note is played and OFF when all notes are released, unless the sustain pedal is engaged.

To provide visual feedback, an LED indicator lights up briefly with each received MIDI message.
The program also manages a buffer for up to 8 active notes, ensuring that even in overlapping situations, only the most recent note is used to set the control voltage.

Additional features include support for MIDI Control Change messages such as sustain (CC 64), which keeps the gate active after note release, and an "All Notes Off" (panic) command (CC 123) to reset the system by clearing the buffer and turning off the gate.
The program operates on a single configurable MIDI channel and implements MIDI Running Status for efficient message handling.

Below you will find an electronic schematic and some examples that can be used as a guide to integrate this MIDI-to-CV converter into your project.

![MIDI-TO-CV-ATTINY85](https://github.com/user-attachments/assets/a47dfad4-9dd5-4faa-a4a1-b70527d5d8be)

![OPTIONAL](https://github.com/user-attachments/assets/c29ad219-e8a6-4bae-8427-53af5e63b977)

Wiring to upload the code to the ATtiny85 usign Arduino Nano/Uno:

![FLASHING SETUP](https://github.com/user-attachments/assets/a28d8837-e0ff-41c2-a266-3c3756dc86c3)

Arduino IDE 2.x.x Setup:

-Add this to the additional boards in Preferences:
http://drazzy.com/package_drazzy.com_index.json

-intall "ATTinyCore" in boards manager
-set Board to "ATiny25/45/85 (No bootloader)"
-set Port to your arduino programmer
-set Chip to "ATtiny85"
-set Clock Source to "8 Mhz (internal)"
-set millis()/micros() to "Enabled"
-set Timer 1 Clock to "64Mhz"
-set programmer to "Arduino as ISP"
-burn the bootloader ONLY THE FIRST TIME
-upload the code using the Upload button

