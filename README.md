# CECS447 Project 1 — Digital Piano (Two-System Implementation)
This repository supports **Keil uVision (primary)** and **VS Code + arm-none-eabi (optional)** workflows.

## Project workflow (6 steps)
Use the GitHub Issues in this repo to track each step. Each step requires a small, checkable artifact.

1. **Requirements & constraints**
2. **System design (two solutions)**
3. **Module test plan + ModuleTest.c**
4. **Feature implementation plan + task assignment**
5. **Integration + full system test (CER Evidence Pack)**
6. **Demonstration + submission**

> **Process rule:** Do not start integration until module tests pass in `ModuleTest.c`.

---

## Repository Structure (What goes where)
- `src/common/` — shared modules used by both systems (includes `ModuleTest.c`)
- `src/part1/` — System P1 (square-wave) implementation
- `src/part2/` — System P2 (DAC + sine-wave) implementation
- `src/startup.s` — startup/vector table for TM4C123
- `src/tm4c123gh6pm.h` — device header (register definitions)
- `keil/` — Keil uVision project files (one per system)
- `docs/` — step deliverables (design, test plan, CER evidence pack, etc.)
- `evidence/` — screenshots/logs/video links referenced by docs
---
## Build / Run (Keil uVision)
This repo provides **two Keil projects**, one per system:
- **System P1 (Square wave):** open `keil/CECS447Project1P1.uvprojx`
- **System P2 (DAC + sine wave):** open `keil/CECS447Project1P2.uvprojx`

> If you add new `.c` files, you must add them to the Keil project (Project → Add Existing Files to Group).

## Testing
Use `src/common/ModuleTest.c` to test modules independently before integration (recommended). Store test evidence in `evidence/` and link it from your docs.

## Required deliverables by step (what to commit)
### Step 1 – Requirements & constraints
- `docs/Step1_RequirementsConstraints.md`

### Step 2 – System design (two solutions)
- `docs/Step2_SystemDesign.md`
- Include: pin map, peripherals, external components, data-flow, call graph, module list/APIs

### Step 3 – Module test plan + ModuleTest.c
- `docs/Step3_ModuleTestPlan.md`
- `/docs/Traceability.csv`
    Note: Traceability is living, not frozen. Whenever you have new design and development progress related to tasks we need to trace, update traceability table.``
    Fill in the "Implementation" and "Evidence" columns: map requirements → software modules / tasks
    Add derived requirements (e.g., “ISR must complete < 10 µs”)
    Link requirements to interfaces or peripherals
    Flag requirements that now need verification strategy changes
- `src/ModuleTest.c` (test harness)
- Evidence snippets in `evidence/` (terminal screenshots or measurement images)

### Step 4 – Feature plan & task assignment
- `docs/Step4_TaskPlan.md`
- GitHub Issues/PRs for each module/feature (owner + reviewer + acceptance criteria)

### Step 5 – Integration + full system test (CER Evidence Pack)
- `docs/Step5_CER_EvidencePack.md`
- Link each claim to evidence files in `evidence/`

### Step 6 – Demo + submission
- `docs/Step6_SubmissionChecklist.md`
- Final report/video link (as instructed by the course)

---

## GitHub workflow expectations (recommended)
- Use **Issues** for each step and for each feature/bug.
- Work in **branches** (e.g., `step2-design`, `step3-moduletest`, `step5-integration`).
- Submit **Pull Requests**; get at least **one peer review** before merging.
- Keep `main` stable; merge only when tests/evidence are ready.

---

## Toolchain notes
### Keil uVision (primary)
- If a Keil project is provided, place it in `keil/` and ensure it references source files in `../src/`.
- Do **not** commit build outputs: `Objects/`, `Listings/`, `.axf`, etc. (see `.gitignore`).

### VS Code + arm-none-eabi (optional)
- If you use GCC locally, keep build outputs in `build/` (ignored by git).
- See `vscode/` for optional stubs.

---

## Academic integrity + AI tools
AI tools may be used for:
- understanding requirements, brainstorming design alternatives, drafting diagrams,
- writing test plans, improving comments/documentation.

AI tools may **not** be used to paste a complete solution. Any AI-assisted content must be:
- logged (what prompt, what was used),
- verified by tests/evidence before integration.

---

