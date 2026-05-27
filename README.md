# High Noon Duel 🌵
<img width="1021" height="512" alt="Snímek obrazovky 2026-05-28 011050" src="https://github.com/user-attachments/assets/e718333e-5b5b-4603-86ce-3c7278960817" />

A lightweight, 1985 arcade-style western duel game written in pure C using the Win32 API / GDI. Designed to run natively on Windows with zero external frameworks or dependencies (no SDL, SFML, or DirectX).

## Features
* **Zero Dependencies:** Compiles into a lightweight binary using standard Windows libraries (`user32.lib`, `gdi32.lib`).
* **Game Loop Architecture:** Managed state machine handling decoupled logic updates and software rendering.
* **Double Buffering:** Prevents screen flickering by rendering scenes to an in-memory DC before blitting to the window.
* **Retro Visuals:** Real-time CRT scanline shader effect simulated via GDI pen operations.
* **Dynamic AI:** Automated scaling of enemy reaction times and projectile velocity based on player score.

## Game Controls
* `SPACE` — Shoot
* `UP Arrow` — Jump (dodge low bullets)
* `DOWN Arrow` — Duck (dodge high bullets)
* `ENTER` — Start Game / Navigate Menu

## Compilation
To build the project via MSVC (Visual Studio Developer Command Prompt or VS Code task):

```bash
cl.exe main.c /link /SUBSYSTEM:WINDOWS user32.lib gdi32.lib
