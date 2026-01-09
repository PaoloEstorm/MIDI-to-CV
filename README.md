This program, designed for the ATtiny85 microcontroller, is a simple and reliable MIDI-to-CV converter specific for monophonic applications.

It translates MIDI notes messages into an analog control voltage (CV) and a gate signal, making it useful for integrating MIDI devices with analog synthesizers or other CV-based hardware.

The program reads MIDI input at the standard baud rate of 31,250 and converts note data into a 500 kHz PWM signal, which can easily be filtered using a low-pass filter to produce a continuous control voltage.

The gate output reflects note activity by turning ON when a note is played and OFF when all notes are released, unless the sustain pedal is engaged.

To provide visual feedback, an LED indicator lights up briefly with each received MIDI message.
The program also manages a buffer for up to 8 active notes, ensuring that even in overlapping situations, only the most recent note is used to set the control voltage.

Additional features include support for MIDI Control Change messages such as sustain (CC 64), which keeps the gate active after note release, and an "All Notes Off" (panic) command (CC 123) to reset the system by clearing the buffer and turning off the gate.

The program operates on a single configurable MIDI channel and implements MIDI Running Status for efficient message handling.

Below you can find an electronic schematic and some examples that can be used as a guide to integrate this MIDI-to-CV converter into your project.

Upload wiring and setup usign Arduino Nano/Uno:

![Wiring](https://github.com/user-attachments/assets/4f31524b-d1b5-49b2-9cd0-6c37baad57df)

Arduino IDE 2.x.x Setup:

-Add this to the additional boards in Preferences:
http://drazzy.com/package_drazzy.com_index.json

-Install "ATTinyCore" in boards manager

-Set Board to "ATiny25/45/85 (No bootloader)"

-Set Port to your arduino programmer

-Set Chip to "ATtiny85"

-Set Clock Source to "8 Mhz (internal)"

-Set millis()/micros() to "Enabled"

-Set Timer 1 Clock to "64Mhz"

-Set programmer to "Arduino as ISP"

-Burn the bootloader ONLY THE FIRST TIME

-Upload the code using the Upload button

Schematics:

<img width="1617" height="2297" alt="Schematic_MIDI-TO-CV-ATTINY85_2026-01-09_Pagina_1" src="https://github.com/user-attachments/assets/c2bcc47b-113c-4a00-8f24-6fd6369b0251" />
<img width="1617" height="2299" alt="Schematic_MIDI-TO-CV-ATTINY85_2026-01-09_Pagina_2" src="https://github.com/user-attachments/assets/e32db71b-93f8-45af-9c0d-44448fbd7713" />

