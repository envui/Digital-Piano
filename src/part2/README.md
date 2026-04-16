# src/ Folder Layout (CECS447 Project 1)

This `src/` folder contains all source code and required low-level files for **both embedded systems** in Project 1:

- **Part 1: Music Box**
- **Part 2: Digital Piano**

## Contents

### `common/`
Shared source code used by **both** Part 1 and Part 2.

Includes:
- Common drivers/utilities (e.g., clock, GPIO helpers, debounce, timing utilities)
- **`ModuleTest.c`**: shared module test harness used to test individual modules before integration

### `part1/`
Source code specific to **Part 1 (Music Box)** only.

Typical content:
- Music box state machine (on/off, song selection, reset behavior)
- Tone/square-wave generation using timer interrupts
- Part 1 application logic

### `part2/`
Source code specific to **Part 2 (Digital Piano)** only.

Typical content:
- Digital piano state machines (piano mode / auto-play mode)
- Octave selection logic
- DAC + wavetable tone generation using timer interrupts
- Part 2 application logic

### `startup.s`
Startup/boot code for the TM4C123 (vector table, `Reset_Handler`, interrupt vectors).  
This file is required by the Keil projects and should not be modified unless instructed.

### `tm4c123gh6pm.h`
TM4C123 device register definition header used for direct register access.

## Notes for Keil uVision Projects
- The Keil project for **Part 1** should include:
  - `common/` + `part1/` + `startup.s` + `tm4c123gh6pm.h`
- The Keil project for **Part 2** should include:
  - `common/` + `part2/` + `startup.s` + `tm4c123gh6pm.h`

If you add new `.c` files, remember to **add them to the Keil project** (Project → Add Existing Files to Group).
