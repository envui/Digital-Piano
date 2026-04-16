# Step 2 — System Design

**Project:** CECS 447 Project 1 - Digital Piano Team 9

---

## Solution A — S1: Digital Output Square Wave (SysTick GPIO Toggle)

**High-level description:**  
System 1 generates audio by outputting a **square wave** on a GPIO pin connected to the speaker. The **SysTick interrupt** toggles the speaker GPIO pin at a rate set by the SysTick reload value, which determines the note frequency. Two modes are supported: **Piano Mode** (press-and-hold 1 of 4 keys to play continuously, stop on release) and **Auto-Play Mode** (repeat one of three songs, switch songs when selected).

---

## 1. Hardware Design

### GPIO Pin Assignment
- **PA2** — Speaker output (GPIO output, toggled in SysTick ISR)
- **PB0–PB3** — Piano keys K1–K4 (GPIO inputs, interrupts; press-and-hold)
- **PF4 (SW1 / left switch)** — Mode toggle (GPIO input, interrupt)
- **PF0 (SW2 / right switch)** — Octave select in Piano mode / Song select in Auto-Play mode (GPIO input, interrupt)
- **PF1–PF3** — RGB LED status/debug (GPIO outputs, optional)

---

### Peripherals Used
- **SysTick Timer (interrupt-driven)**  
  Generates periodic interrupts; the ISR toggles PA2 to create the square wave. Reload value sets note frequency.
- **GPIO Port A**  
  Digital output for speaker on PA2.
- **GPIO Ports B and F (interrupt-driven inputs)**  
  PB0–PB3 handle piano key press-and-hold, PF4/PF0 handle mode and octave/song selection.
- **System Clock**  
  Configured such that **system clock > 16 MHz and < 80 MHz**; SysTick timing uses the bus clock.

---

### External Components
- **4 push buttons** (piano keys) wired to PB0–PB3 (with pull-ups/pull-downs as required by wiring logic)
- **Speaker + audio amplifier** connected to PA2 (per project hardware list)
- Jumpers / breadboard wiring as required

---

## 2. Software Architecture

### Modules and Interfaces (API)

- **`SysTick.c / SysTick.h`**
  - `void SysTick_Init(void);` — Configure SysTick clock source + interrupt enable and set priority
  - `void SysTick_start(void);` — Enable SysTick
  - `void SysTick_stop(void);` — Disable SysTick (silence output)
  - `void SysTick_Set_Current_Note(uint32_t n_value);` — Load SysTick reload for the current note
  - `void SysTick_Handler(void);` — ISR toggles PA2 to generate square wave

- **`Speaker.c / Speaker.h`**
  - `void Speaker_Init(void);` — Configure PA2 as digital output for speaker

- **`Input.c / Input.h`**
  - `void Input_Init(void);` — Configure PB0–PB3, PF4, PF0 as inputs and enable GPIO interrupts
  - `uint8_t Input_GetHeldKey(void);` — Return currently held key (0–3 or NONE)
  - `uint8_t Input_GetModeEvent(void);` — Latched SW1 event
  - `uint8_t Input_GetSelectEvent(void);` — Latched SW2 event
  - `void GPIOPortB_Handler(void);` — Updates held-key state (press/release)
  - `void GPIOPortF_Handler(void);` — Posts mode/select events

- **`Music.c / Music.h`**
  - `const uint32_t Tone_Tab[];` — SysTick reload table for note frequencies (scaled for clock)
  - `uint32_t NoteReload(uint8_t note, uint8_t octave);` — Returns reload value for selected note/octave
  - `void Song_Reset(uint8_t song_id);` — Restart a song at beginning
  - `uint8_t Song_NextNote(uint8_t song_id, uint8_t *tone_index, uint8_t *dur);` — Returns next note and duration

- **`Control.c / Control.h`**
  - `void Control_Init(void);` — Initialize default mode/state
  - `void Control_Tick(void);` — Main loop state machine controlling Piano vs Auto-Play

- **`main.c`**
  - `int main(void);` — Initializes modules, enables interrupts, runs control loop

---

### Data Flow (ASCII Flow Chart)

```text
+-------------------+        GPIO IRQ         +----------------------+
|  PB0–PB3 Keys      | --------------------->  | GPIOPortB_Handler     |
|  (piano buttons)   |                         | (update held-key)     |
+-------------------+                         +-----------+----------+
                                                        |
                                                        | held-key state
                                                        v
                                              +---------+----------+
                                              |   Control_Tick     |
                                              | (mode + note ctrl) |
                                              +----+-----------+---+
                                                   |           |
                         mode/select event          |           | note request
                                                   v           v
+-------------------+        GPIO IRQ         +-----+-----+  +------------------+
| PF4 (SW1) / PF0    | ---------------------> | GPIOPortF |  | Music.c          |
| (mode/select)      |                        | _Handler  |  | NoteReload /     |
+-------------------+                        | (latch    |  | Song_NextNote    |
                                             | events)   |  +--------+---------+
                                             +-----+-----+           |
                                                   |                 | reload value / rest
                                                   +-----------------v------------------+
                                                                     |
                                                                     v
                                                        +------------+-------------+
                                                        | SysTick_Set_Current_Note |
                                                        | SysTick_start/stop       |
                                                        +------------+-------------+
                                                                     |
                                                                     v
                                                        +------------+-------------+
                                                        | SysTick_Handler (ISR)    |
                                                        | toggle PA2 (square wave) |
                                                        +------------+-------------+
                                                                     |
                                                                     v
                                                        +--------------------------+
                                                        | PA2 -> Speaker/Amp       |
                                                        +--------------------------+
```
##Call Graph

```text
main
├─ Speaker_Init()
├─ SysTick_Init()
├─ Input_Init()                    (PB0–PB3, PF4, PF0 interrupts)
├─ Control_Init()
├─ EnableInterrupts()
└─ while(1)
   └─ Control_Tick()               (handles Piano vs Auto-play sequencing)

GPIOPortF_Handler (SW1/SW2)
└─ latch mode/select event(s) -> consumed by Control_Tick()

GPIOPortB_Handler (Keys)
└─ update held-key state -> consumed by Control_Tick()

SysTick_Handler (audio)
└─ toggle PA2 (square wave output)
```

---
## 3. Timing and Accuracy Considerations
- **Square wave generation:** SysTick uses the bus clock; each SysTick interrupt toggles PA2. One full waveform period requires two toggles, so output frequency depends on the reload value.
- **Reload values:** note frequencies are produced by loading SysTick reload values from `Tone_Tab[]` (or computed with `NoteReload()`), consistent with the example implementation.
- **Clock constraint:** system clock configured **> 16 MHz and < 80 MHz**; reload values must match the active clock.
- **Responsiveness:** piano keys and switches use GPIO interrupts so press/release and mode selection are handled immediately while SysTick continues generating audio.

---

## 4. Advantages and Tradeoffs

### Advantages
- Meets System 1 requirement: **digital square wave** output to speaker
- Simple audio generation: single pin toggle in **SysTick ISR**
- Low external hardware complexity for S1

### Tradeoffs
- Square wave timbre (harmonics) vs sine wave DAC system
- Higher interrupt rate at higher notes (more CPU ISR activity)
- Requires correct scaling of reload values if clock differs from the example

---

## 5. Intended Use
This is the **primary design for System 1 (S1)** and will be implemented as the digital square-wave solution to compare against the System 2 DAC sine-wave design.


## Solution B — S2: Analog Output Sine Wave (6-bit R/2R DAC + SysTick Sample ISR)

**High-level description:**  
System 2 generates audio by outputting a **sampled sine wave** through a **6-bit R/2R ladder DAC** connected to an audio amplifier/speaker. The **SysTick interrupt** runs at a fixed **audio sample rate** and advances through a **64-sample sine wavetable**, writing one 6-bit value to the DAC each interrupt. The output tone frequency is controlled by setting the SysTick reload such that:

- `f_sample = 64 * f_note`  
- SysTick ISR outputs the next sample each interrupt.

Two modes are supported: **Piano Mode** (press-and-hold 1 of 4 keys to play continuously, stop on release) and **Auto-Play Mode** (repeat one of three songs, switch songs when selected and restart immediately).

---

## 1. Hardware Design

### GPIO Pin Assignment
- **PD0–PD5** — 6-bit DAC output (GPIO outputs; PD0=LSB … PD5=MSB)  
  Connected to the **6-bit R/2R ladder**; DAC analog output goes to the amplifier input.
- **PB0–PB3** — Piano keys K1–K4 (GPIO inputs, interrupts; press-and-hold)
- **PF4 (SW1 / left switch)** — Mode toggle (GPIO input, interrupt)
- **PF0 (SW2 / right switch)** — Octave select in Piano mode / Song select in Auto-Play mode (GPIO input, interrupt)
- **PF1–PF3** — RGB LED status/debug (GPIO outputs, optional)

> Note: DAC pins are placed on Port D to avoid conflicts with PB0–PB3 key inputs and keep wiring clean.

---

### Peripherals Used
- **SysTick Timer (interrupt-driven, waveform sampling)**  
  Generates periodic interrupts at `f_sample`. The ISR outputs the next sample from a 64-entry sine table to the 6-bit DAC. Reload value controls sample rate and therefore the note frequency.
- **(Recommended) GPTM Hardware Timer (interrupt-driven, tempo / note durations)**  
  A slower timer (e.g., Timer1A at 1 ms or 10 ms) can be used to manage note durations for Auto-Play without blocking delays.
- **GPIO Port D (DAC outputs)**  
  6 digital outputs drive the R/2R ladder DAC (PD0–PD5).
- **GPIO Ports B and F (interrupt-driven inputs)**  
  PB0–PB3 handle piano key press-and-hold; PF4/PF0 handle mode and octave/song selection.
- **System Clock**  
  Configured such that **system clock > 16 MHz and < 80 MHz**; SysTick timing uses the bus clock, so reload values must match the active clock.

---

### External Components
- **4 push buttons** (piano keys) wired to PB0–PB3 (with pull-ups/pull-downs as required by wiring logic)
- **6-bit R/2R ladder DAC** built from resistors, driven by PD0–PD5
- **Speaker + audio amplifier** connected to DAC analog output (per project hardware list)
- Jumpers / breadboard wiring as required

---

## 2. Software Architecture

### Modules and Interfaces (API)

- **`SysTickAudio.c / SysTickAudio.h`**  (SysTick wavetable engine)
  - `void SysTickAudio_Init(void);` — Configure SysTick clock source + interrupt enable and set priority
  - `void SysTickAudio_Start(void);` — Enable SysTick (begin audio output)
  - `void SysTickAudio_Stop(void);` — Disable SysTick (silence output)
  - `void SysTickAudio_SetNoteFreq(uint32_t f_note_hz);` — Set `f_sample = 64*f_note_hz` and update SysTick reload
  - `void SysTick_Handler(void);` — ISR outputs next sample to DAC (64-sample table indexing)

- **`DAC6.c / DAC6.h`**
  - `void DAC6_Init(void);` — Configure PD0–PD5 as digital outputs
  - `void DAC6_Out(uint8_t code6);` — Output 6-bit value (0–63) to PD0–PD5

- **`Input.c / Input.h`**
  - `void Input_Init(void);` — Configure PB0–PB3, PF4, PF0 as inputs and enable GPIO interrupts
  - `uint8_t Input_GetHeldKey(void);` — Return currently held key (0–3 or NONE)
  - `uint8_t Input_GetModeEvent(void);` — Latched SW1 event
  - `uint8_t Input_GetSelectEvent(void);` — Latched SW2 event
  - `void GPIOPortB_Handler(void);` — Updates held-key state (press/release)
  - `void GPIOPortF_Handler(void);` — Posts mode/select events

- **`Music.c / Music.h`**
  - `uint32_t NoteFreqHz(uint8_t note_id, uint8_t octave_id);` — Returns note frequency in Hz for selected note/octave
  - `void Song_Reset(uint8_t song_id);` — Restart a song at beginning
  - `uint8_t Song_NextNote(uint8_t song_id, uint8_t *note_id, uint8_t *dur);` — Returns next note + duration unit

- **`Control.c / Control.h`**
  - `void Control_Init(void);` — Initialize default mode/state (Piano mode, Lower octave)
  - `void Control_Tick(void);` — Main loop state machine controlling Piano vs Auto-Play
  - `void TempoTick_Handler(void);` — (Recommended) called by a slow timer ISR to advance song durations

- **`main.c`**
  - `int main(void);` — Initializes modules, enables interrupts, runs control loop

---

### Data Flow (ASCII Flow Chart)

```text
+-------------------+        GPIO IRQ         +----------------------+
|  PB0–PB3 Keys      | --------------------->  | GPIOPortB_Handler     |
|  (piano buttons)   |                         | (update held-key)     |
+-------------------+                         +-----------+----------+
                                                        |
                                                        | held-key state
                                                        v
                                              +---------+----------+
                                              |   Control_Tick     |
                                              | (mode + note ctrl) |
                                              +----+-----------+---+
                                                   |           |
                         mode/select event         |           | f_note (Hz)
                                                   v           v
+-------------------+        GPIO IRQ         +-----+-----+  +------------------+
| PF4 (SW1) / PF0    | ---------------------> | GPIOPortF |  | Music.c          |
| (mode/select)      |                        | _Handler  |  | NoteFreqHz /     |
+-------------------+                         | (latch    |  | Song_NextNote    |
                                              | events)   |  +--------+---------+
                                              +-----+-----+          |
                                                   |                 | f_note (Hz)
                                                   +-----------------v------------------+
                                                                     |
                                                                     v
                                                        +------------+-------------+
                                                        | SysTickAudio_SetNoteFreq |
                                                        | SysTickAudio_Start/Stop  |
                                                        +------------+-------------+
                                                                     |
                                                                     | SysTick IRQ @ f_sample
                                                                     v
                                                        +------------+-------------+
                                                        | SysTick_Handler (ISR)    |
                                                        | idx=(idx+1)%64           |
                                                        | DAC6_Out(Sine64[idx])    |
                                                        +------------+-------------+
                                                                     |
                                                                     v
                                                        +--------------------------+
                                                        | R/2R DAC -> Amp -> Speaker|
                                                        +--------------------------+
```
##Call Graph

```text
main
├─ DAC6_Init()
├─ SysTickAudio_Init()            (SysTick periodic interrupt for samples)
├─ Input_Init()                   (PB0–PB3, PF4, PF0 interrupts)
├─ Control_Init()
├─ EnableInterrupts()
└─ while(1)
   └─ Control_Tick()              (handles Piano vs Auto-play sequencing)

GPIOPortF_Handler (SW1/SW2)
└─ latch mode/select event(s) -> consumed by Control_Tick()

GPIOPortB_Handler (Keys)
└─ update held-key state -> consumed by Control_Tick()

SysTick_Handler (audio sample ISR)
└─ DAC6_Out(Sine64[idx])          (64-sample sine wavetable output)

```
## 3. Timing and Accuracy Considerations

- **Waveform generation:** SysTick interrupts at the **audio sample rate**. Each interrupt outputs **one** 6-bit DAC sample from a **64-sample** sine wavetable.
- **Frequency control (64-sample table):** To generate a target note frequency `f_note`, the system sets the sample rate:
  - `f_sample = 64 * f_note`
  Since SysTick triggers once per sample, the SysTick interrupt rate equals `f_sample`.
- **SysTick reload calculation:** With SysTick using the bus clock `f_bus`,
  - `SysTick_Reload = (f_bus / f_sample) - 1`
  Reload values must be recomputed if `f_bus` changes.
- **Clock constraint:** The system clock is configured to satisfy **> 16 MHz and < 80 MHz**. SysTick timing and reload values must match the active clock to keep pitch accurate.
- **ISR execution time:** The SysTick ISR must remain short (increment index, output DAC value) to prevent jitter and audible distortion. Avoid heavy logic inside the ISR.
- **Responsiveness:** Piano keys and switches use **GPIO interrupts**, so press/release and mode selection are handled immediately while SysTick continues generating audio.
- **Auto-Play timing:** Note durations should be advanced without blocking audio. Preferred approach is a separate **tempo timebase** (e.g., a slow timer tick or non-blocking counter) while SysTick continues the audio sampling uninterrupted.

 ## 4. Advantages and Tradeoffs

### Advantages
- Meets System 2 requirement: **analog sine wave** output via **6-bit R/2R DAC** and a **64-sample wavetable**
- Better audio quality (fewer harsh harmonics) compared to a square wave output
- Simple, proven structure based on the provided example: **SysTick ISR + wavetable + DAC output**
- Keeps user input responsive using **GPIO interrupts** while audio continues uninterrupted

### Tradeoffs
- More external hardware complexity: requires building and wiring a **6-bit R/2R resistor ladder**
- Higher interrupt rate than square wave at the same pitch (64 samples per waveform period), increasing CPU ISR load
- Requires correct scaling of SysTick reload values to the active clock or pitch will be incorrect
- DAC bit order and resistor accuracy affect waveform quality (wrong wiring can distort audio)

---

## 5. Intended Use
This is the **primary design for System 2 (S2)** and will be implemented as the analog sine-wave DAC solution to compare against the System 1 digital square-wave solution (S1). It provides the required **6-bit DAC + 64-sample sine wavetable** implementation using **SysTick interrupts**, consistent with the example project structure.
