# Step 5 – Integration & End-to-End Testing (CER)

## Overview
All modules were integrated into the full system and validated using end-to-end tests on hardware.  
Evidence files are stored in `/evidence/` and linked below.

Integrated modules (high level):
- `PLL.c` (bus clock configuration ~25 MHz)
- `SysTick.c` (hardware timer ISR + callback task selection)
- `Keys_PC4_7.c` (4 piano keys using GPIO interrupts on PC4–PC7)
- `SW1_PF4.c` (SW1=PF4 mode toggle, SW2=PF0 octave/song selection)
- `S1_AudioSquare.c` (square-wave output on PA2)
- `S2_AudioDAC.c` (6-bit R-2R DAC on PB0–PB5 with 64-sample sine table)
- `Audio.c` (system selection + autoplay loop logic)
- `Songs.c/.h` (Tone_Tab + song data tables)
- `main.c` (full system integration + interrupt handlers)

---

# Claim–Evidence–Reasoning (CER)

---

## Claim 1 – Piano press/hold behavior (Both Systems)

**Claim:**  
In Piano mode, pressing and holding one of the four keys produces continuous audio output, and releasing the key immediately stops the sound.

**Evidence:**  
- `/evidence/Step5_C1_PianoHoldRelease_S1.mp4`  
- `/evidence/Step5_C1_PianoHoldRelease_S2.mp4`

**Reasoning:**  
Port C generates interrupts on both edges (press and release). On press, `StartTone()` enables SysTick audio generation; on release, `StopTone()` disables it immediately. The videos show uninterrupted tone while held and immediate silence on release in both systems.

---

## Claim 2 – Octave cycles correctly and updates active note

**Claim:**  
In Piano mode, pressing SW2 cycles through Lower → Middle → Upper octaves in round-robin order, and any currently held note updates immediately.

**Evidence:**  
- `/evidence/OctaveCarryoverS1.mp4`  
- `/evidence/OctaveCarryoverS2.mp4`

**Reasoning:**  
The PF0 interrupt updates `g_octave = (g_octave + 1) % 3`. If a key is held, the tone index is recalculated and restarted with the new octave. The evidence shows pitch changes and correct wrap-around behavior.

---

## Claim 3 – Auto-play operates correctly

**Claim:**  
In Auto-play mode, the system automatically plays the selected song without user input.

**Evidence:**  
- `/evidence/AutoPlayS1.mp4`  
- `/evidence/AutoPlayS2.mp4`

**Reasoning:**  
`Audio_AutoPlayLoop()` iterates through the song table and plays each note using the SysTick interrupt-driven audio system. The videos show full song playback occurring automatically in both systems.

---

## Claim 4 – Song switching restarts immediately

**Claim:**  
While in Auto-play mode, pressing SW2 changes the selected song and immediately restarts playback from the beginning.

**Evidence:**  
- `/evidence/S1Controls.mp4`  
- `/evidence/S2Controls (1).mp4`

**Reasoning:**  
SW2 increments `g_song` and sets `g_forceExit = 1`. This forces `Audio_AutoPlayLoop()` to exit and restart with the newly selected song index. The videos demonstrate immediate restart behavior mid-song.

---

## Claim 5 – Auto-play repeats indefinitely

**Claim:**  
In Auto-play mode, the selected song repeats indefinitely until the mode or song selection changes.

**Evidence:**  
- `/evidence/AutoPlayS1.mp4`  
- `/evidence/AutoPlayS2.mp4`

**Reasoning:**  
`Audio_AutoPlayLoop()` runs inside a `while(g_mode == MODE_AUTOPLAY)` loop. After completing a song and an inter-repeat delay, it restarts playback automatically. The videos show the song reaching the end and restarting without user input.

---

## Claim 6 – Correct waveform generation for each system

**Claim:**  
System 1 generates a square wave on PA2, and System 2 generates a sampled sine wave using a 6-bit R-2R DAC and a 64-sample lookup table.

**Evidence:**  
- `/evidence/S1_logic_analyzer_PA2.png`  
- `/evidence/S2_logic_analyzer_DAC.png`

**Reasoning:**  
In System 1, the SysTick ISR toggles PA2 to generate a square wave.  
In System 2, the SysTick ISR outputs successive values from a 64-sample sine table to PB0–PB5.  
Logic analyzer screenshots confirm square-wave digital output for S1 and stepped sinusoidal DAC output for S2.

---

## Claim 7 – Control behavior identical across System 1 and System 2

**Claim:**  
System 1 and System 2 implement identical control behavior for piano keys, SW1 mode toggle, and SW2 octave/song selection, differing only in audio waveform generation.

**Evidence:**  
- `/evidence/S1Controls.mp4`  
- `/evidence/S2Controls (1).mp4`

**Reasoning:**  
Both systems share the same interrupt handlers (`GPIOPortC_Handler`, `GPIOPortF_Handler`) and global mode logic (`g_mode`, `g_octave`, `g_song`). The only difference is the selected SysTick callback (`S1_Task` vs `S2_Task`). The evidence shows identical responses to key presses, octave cycling, mode toggling, and song changes.

---

# Test Matrix

| Requirement | Test Method | Evidence |
|------------|------------|----------|
| Piano press/hold plays continuously; release stops immediately | End-to-end hardware demo | `Step5_C1_PianoHoldRelease_S1.mp4`, `Step5_C1_PianoHoldRelease_S2.mp4` |
| Octave cycles Lower/Middle/Upper and updates held note | Hardware demo | `OctaveCarryoverS1.mp4`, `OctaveCarryoverS2.mp4` |
| Auto-play plays automatically | Hardware demo | `AutoPlayS1.mp4`, `AutoPlayS2.mp4` |
| Auto-play song restart on SW2 | Hardware demo | `S1Controls.mp4`, `S2Controls (1).mp4` |
| Auto-play repeats indefinitely | Hardware demo | `AutoPlayS1.mp4`, `AutoPlayS2.mp4` |
| System 1 square-wave generation | Logic analyzer measurement | `S1_logic_analyzer_PA2.png` |
| System 2 sine-wave DAC generation | Logic analyzer measurement | `S2_logic_analyzer_DAC.png` |
| Identical control behavior across systems | Comparative demo | `S1Controls.mp4`, `S2Controls (1).mp4` |

---

## Rejected AI Suggestions

**Suggestion 1:** Replace the SysTick-based audio timing with Timer0A for improved precision.

**Reason Rejected:**  
SysTick already satisfies the hardware timer interrupt requirement and is fully integrated into both audio systems via SysTick_SetTask(). Refactoring to another timer during final integration would introduce unnecessary risk and invalidate verified system behavior. Hardware testing confirmed stable frequency generation and responsive controls using the SysTick implementation.

---

**Suggestion 2:** Add automated frequency measurement using a capture timer to numerically verify note accuracy instead of relying on logic analyzer screenshots.

**Reason Rejected:**  
While automated frequency capture would provide numerical verification, it would significantly increase implementation complexity and add another timer module to the system late in integration. The logic analyzer already confirmed correct waveform period and frequency for representative notes across octaves. Given time constraints and existing verified evidence, additional instrumentation was unnecessary.

---

**Suggestion 3:** Add software debouncing using timer-based filtering instead of the current simple delay-based debounce.

**Reason Rejected:**  
A more advanced debounce algorithm (state machine or timer-based filtering) would improve robustness, but current hardware testing showed reliable button response without false triggers. Since the system met functional requirements and passed end-to-end testing, additional debounce complexity was not required for this project scope.


## Logs Updated
- Decision log updated to document final integrated architecture and retained SysTick design.
- AI-use log updated with accepted and rejected suggestions verified through hardware testing.
