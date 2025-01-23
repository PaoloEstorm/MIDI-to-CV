// Code based on the "ATtiny85 MIDI to CV" project by Kevin
// https://emalliab.wordpress.com/2019/03/02/attiny85-midi-to-cv/

// Authors: Paolo Estorm, Kevin

// Version 3.50

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.


// Add this to the additional boards in Preferences:
// http://drazzy.com/package_drazzy.com_index.json

// intall "ATTinyCore" in boards manager
// set Board to "ATiny25/45/85 (No bootloader)"
// set Port to your arduino programmer
// set Chip to "ATtiny85"
// set Clock Source to "8 Mhz (internal)"
// set millis()/micros() to "Enabled"
// set Timer 1 Clock to "64Mhz"
// set programmer to "Arduino as ISP"
// burn the bootloader ONLY THE FIRST TIME
// upload using the Upload button

#include <SoftwareSerial.h>

#define MIDICH 1  // Set this to the MIDI channel to listen to (1 to 16)

#define MIDIRX 0   // PB0 (Pin 5) MIDI in
#define GATE 2     // PB2 (Pin 7) Gate
#define PITCHCV 1  // PB1 (Pin 6) Pitch CV
#define MIDILED 3  // PB3 (Pin 2) MIDI Indicator

SoftwareSerial midiSerial(MIDIRX, -1);

uint8_t MIDIRunningStatus = 0;
uint8_t MIDINote = 0;
uint8_t MIDILevel = 0;
bool SameChannel = false;

const uint8_t NoteBuffersSize = 8;
int8_t NoteBuffer[NoteBuffersSize];
int8_t NumKeyPressed = 0;

bool Sustain = false;
bool BlinkMidiLed = false;

void setup() {

  midiSerial.begin(31250);  // MIDI Baud rate

  pinMode(GATE, OUTPUT);
  pinMode(PITCHCV, OUTPUT);
  pinMode(MIDILED, OUTPUT);  // Enable midi led output pin

  TCCR1 = _BV(PWM1A) | _BV(COM1A1) | _BV(CS10);
  GTCCR = 0;
  OCR1C = 127;
  OCR1A = 0;  // Initial Pitch CV = 0

  digitalWrite(MIDILED, LOW);
  GateOff();
}

void SetTimerPWM(uint8_t note) {
  OCR1A = note;
}

void GateOn() {
  digitalWrite(GATE, HIGH);
}

void GateOff() {
  digitalWrite(GATE, LOW);
}

void loop() {

  if (midiSerial.available()) {
    doMIDI(midiSerial.read());
  }

  LedOnTimer();
}

void doMIDI(uint8_t midibyte) {

  BlinkMidiLed = true;

  if ((midibyte >= 0x80) && (midibyte <= 0xEF)) {  //if it's a status message
    //
    // MIDI Voice category message
    //
    // Start handling the RunningStatus

    if ((midibyte & 0x0F) == (MIDICH - 1)) {
      // Store, but remove channel information now we know its for us
      MIDIRunningStatus = midibyte & 0xF0;
      SameChannel = true;
      MIDINote = 0;
      MIDILevel = 0;
    } else {
      SameChannel = false;
      // Not on our channel, so ignore
    }
  }

  else if (midibyte <= 0x7F && SameChannel) {

    if (MIDIRunningStatus == 0x80) {  // NoteOff
      // First find the note
      if (MIDINote == 0) {
        MIDINote = midibyte;
      } else {
        // If we already have a note, assume its the level
        MIDILevel = midibyte;
        HandleNoteOff(MIDINote);
        MIDINote = 0;
        MIDILevel = 0;
      }
    }

    else if (MIDIRunningStatus == 0x90) { // NoteOn
      if (MIDINote == 0) {
        MIDINote = midibyte;
      }

      else {
        // If we already have a note, assume its the level
        MIDILevel = midibyte;

        // Now we have a note/velocity pair, act on it
        if (MIDILevel == 0) {
          HandleNoteOff(MIDINote);
        }

        else {
          HandleNoteOn(MIDINote);
        }
        MIDINote = 0;
        MIDILevel = 0;
      }
    }

    else if (MIDIRunningStatus == 0xB0) {  // CC
      // First find the note
      if (MIDINote == 0) {
        MIDINote = midibyte;
      }

      else {
        // If we already have a note, assume its the level
        MIDILevel = midibyte;

        // Now we have a note/velocity pair, act on it
        HandleCC(MIDINote, MIDILevel);
        MIDINote = 0;
        MIDILevel = 0;
      }
    }
  }
}

void LedOnTimer() {

  static unsigned long SampleTime = 0;  // Time at which the timer was set
  static bool TimerState = false;       // Is the timer still running?

  // Start the timer
  if (BlinkMidiLed) {
    BlinkMidiLed = false;
    TimerState = true;
    SampleTime = millis();
    digitalWrite(MIDILED, HIGH);
  }

  // If the timer is running, check if it has expired
  if (TimerState && millis() - SampleTime > 100) {
    TimerState = false;
    digitalWrite(MIDILED, LOW);
  }
}

void ResetSynth() {

  NumKeyPressed = 0;
  GateOff();
  Sustain = false;

  for (uint8_t i = 0; i < NoteBuffersSize; i++) {  // shift the -1s to the left of the "activeNotes" array
    NoteBuffer[i] = -1;
  }
}

void HandleCC(uint8_t cc, uint8_t value) {

  if (cc == 64) {
    if (value > 0) Sustain = true;
    else {
      Sustain = false;
      if (NumKeyPressed <= 0) GateOff();
    }
  }

  if (cc == 123) ResetSynth();
}

void HandleNoteOn(uint8_t note) {

  NumKeyPressed++;
  if (NumKeyPressed > NoteBuffersSize) NumKeyPressed = NoteBuffersSize;
  if (NumKeyPressed > 0) NoteBuffer[NumKeyPressed - 1] = note;
  SetTimerPWM(note);
  GateOn();
}

void HandleNoteOff(uint8_t note) {

  for (uint8_t j = 0; j < NoteBuffersSize; j++) {
    for (uint8_t i = 0; i < NoteBuffersSize; i++) {
      if (NoteBuffer[i] == note) {
        NoteBuffer[i] = -1;
        SortArray();
        NumKeyPressed--;
        if (NumKeyPressed < 0) NumKeyPressed = 0;
      }
    }
  }

  if (NumKeyPressed > 0) SetTimerPWM(NoteBuffer[NumKeyPressed - 1]);
  else {
    if (!Sustain) GateOff();
  }
}

void SortArray() {  // sort "NoteBuffer" array

  for (uint8_t i = 0; i < NoteBuffersSize; i++) {  // shift the -1s to the left of the "activeNotes" array
    if (NoteBuffer[i] == -1) {
      NoteBuffer[i] = NoteBuffer[i + 1];
      NoteBuffer[i + 1] = -1;
    }
  }
}