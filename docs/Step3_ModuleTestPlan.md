# Step 3 – Module-by-Module Test Plan

## Project
CECS 447 – Project 1: Digital Piano

## Purpose
This document defines the independent test plan for each software module in the
Digital Piano project.  
Each module is tested in isolation using `ModuleTest.c` prior to full system
integration.  
Step 3 focuses on verifying module functionality and basic system flows for both
systems (S1 and S2). Requirements not yet implemented remain **IN PROGRESS** but
are still traced to planned implementation locations.

---

## Module Overview

| Module Name | Description |
|------------|-------------|
| SysTick | Shared SysTick timer (callback-based) used by S1 and S2 |
| LED | Port F LEDs: PF2 Blue (note playing), PF3 Green (octave change for S1) |
| Keys_PC4_7 | Piano keys on PC4–PC7 (negative logic), GPIO interrupts |
| SW1_PF4 | Octave switch on PF4 (negative logic), GPIO interrupt |
| S1_AudioSquare | S1 audio: square wave output on PA2 using SysTick callback |
| S2_AudioDAC | S2 audio: 6-bit DAC output on PB0–PB5 using SysTick callback + wavetable |
| ModuleTest | Step 3 harness to run module tests + S1/S2 flow tests via build `#define` |

---

## Test Environment
- Hardware: TM4C123 LaunchPad
- Clock: **16 MHz** (Step 3 harness)
- Tools: Keil uVision, Visual Studio Code
- Observation: LEDs, Logic Analyzer, speaker/DAC output

---

## Module Test Cases

### Module: SysTick
| Test ID | Input | Expected Output |
|-------|-------|-----------------|
| TMR-01 | Set reload + start | Periodic callbacks occur (observable via LED toggle) |
| TMR-02 | Stop timer | No callbacks occur |
| TMR-03 | Reload < 2 | Reload clamps safely (no crash/lockup) |

Edge cases:
- Timer disabled → no callbacks
- Invalid reload → safe handling (clamp)

Owner: __TEAM9______  
Reviewer: ________  
Acceptance: Callback rate within ±5% expected (logic analyzer / LED timing)

Evidence:
- `evidence/S1_logic_analyzer_PA2.png` (shows SysTick-driven toggling)
- `evidence/S2_logic_analyzer_DAC.png` (shows SysTick-driven sample stepping)

---

### Module: Keys_PC4_7
| Test ID | Input | Expected Output |
|-------|-------|-----------------|
| KEY-01 | Press PC4 | Key 0 detected |
| KEY-02 | Press PC5 | Key 1 detected |
| KEY-03 | Release key | Key becomes “none” and tone stops in S1 piano |

Edge cases:
- Button bounce → single press/release handled
- No input → no interrupts

Owner: __TEAM9______  
Reviewer: ________  
Acceptance: Correct key detection and release behavior

Evidence:
- `evidence/S1_flow_video.mp4`

---

### Module: SW1_PF4
| Test ID | Input | Expected Output |
|-------|-------|-----------------|
| SW1-01 | Press SW1 | Octave increments and wraps 0→1→2→0 |
| SW1-02 | Press while key held | Note restarts in new octave |

Edge cases:
- Switch bounce → single octave change per press

Owner: __TEAM9______  
Reviewer: ________  
Acceptance: One octave change per press; correct wrap

Evidence:
- `evidence/S1_flow_video.mp4`

---

### Module: LED
| Test ID | Condition | Expected Output |
|-------|-----------|-----------------|
| LED-01 | S1 note playing | PF2 Blue ON |
| LED-02 | S1 octave change | PF3 Green blinks once |
| LED-03 | S2 note playing | PF2 Blue ON |

Owner: ___TEAM9_____  
Reviewer: ________  
Acceptance: LEDs match “playing” and “octave change” states

Evidence:
- `evidence/S1_flow_video.mp4`
- `evidence/S2_flow_video.mp4`

---

### Module: S1_AudioSquare
| Test ID | Input | Expected Output |
|-------|-------|-----------------|
| S1-01 | Start reload | Square wave toggles PA2 |
| S1-02 | Stop | PA2 held low |

Owner: ____TEAM9____  
Reviewer: ________  
Acceptance: PA2 toggling visible + audible tone

Evidence:
- `evidence/S1_logic_analyzer_PA2.png`
- `evidence/S1_flow_video.mp4`

---

### Module: S2_AudioDAC
| Test ID | Input | Expected Output |
|-------|-------|-----------------|
| S2-01 | Start Hz | DAC output changes on PB0–PB5 (wavetable samples) |
| S2-02 | Stop | DAC output becomes stable |

Owner: __TEAM9______  
Reviewer: ________  
Acceptance: Distinct tones; visible DAC activity on logic analyzer

Evidence:
- `evidence/S2_logic_analyzer_DAC.png`
- `evidence/S2_flow_video.mp4`

---

## System Flow Tests (Step 3 Harness)

### System 1 Flow (S1)
| Test ID | Build Selection | Expected Output |
|-------|------------------|-----------------|
| TC-SYS-001 | `TEST_SYS_S1_FLOW` | Autoplay demo plays notes C,D,E,F across 3 octaves, then piano mode enables key interrupts; PF2 indicates playing; PF3 blinks on octave change |

Evidence:
- `evidence/S1_flow_video.mp4`
- `evidence/S1_logic_analyzer_PA2.png`

---

### System 2 Flow (S2)
| Test ID | Build Selection | Expected Output |
|-------|------------------|-----------------|
| TC-SYS-001 | `TEST_SYS_S2_FLOW` | Plays C,D,E,F across 3 octaves using DAC; PF2 ON while note plays |

Evidence:
- `evidence/S2_flow_video.mp4`
- `evidence/S2_logic_analyzer_DAC.png`

---

## Evidence
All evidence is stored in the repository folder: `/evidence`

Expected artifacts for Step 3:
- Logic analyzer screenshots:
  - `evidence/S1_logic_analyzer_PA2.png`
  - `evidence/S2_logic_analyzer_DAC.png`
- System flow videos:
  - `evidence/S1_flow_video.mp4`
  - `evidence/S2_flow_video.mp4`

---
