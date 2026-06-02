# High Noon Duel 🌵 v1.2
<img width="1051" height="513" alt="Snímek obrazovky 2026-06-02 215029" src="https://github.com/user-attachments/assets/259f750a-2096-494e-acd4-5d80381cf477" />


A lightweight, 1985 arcade-style western duel game written in pure C using the Win32 API / GDI. Designed to run natively on Windows with zero external frameworks or dependencies (no SDL, SFML, or DirectX).
<img width="1009" height="512" alt="Snímek obrazovky 2026-06-02 215012" src="https://github.com/user-attachments/assets/706ccd63-f422-4647-8966-dd451d150ca2" />

## 🕹️ Features
* **Zero Dependencies:** Compiles into a lightweight binary using standard native Windows libraries (`user32.lib`, `gdi32.lib`).
* **Game Loop Architecture:** Managed state machine handling decoupled logic updates and software rendering.
* **Double Buffering:** Prevents screen flickering by rendering scenes to an in-memory Device Context (DC) before blitting (`BitBlt`) to the active window.
* **Dynamic FX & Particle Engine:** Real-time generation of weather and combat effects:
  * **Atmospheric Dust Storm:** An interactive particle system running over the entire viewport (sky and sand) to simulate a harsh desert environment.
  * **Muzzle Flashes:** Dynamic GDI-rendered light bursts at the gun barrel upon firing.
  * **Impact Sparks & Blood FX:** Velocity-based particle bursts triggering exactly at the hitboxes on successful impacts.
* **Tactical Arsenal (Ammo Switching):** On-the-fly weapon mode selection modifying projectile physics and damage output.
* **Dynamic AI & Progression:** Automated scaling of enemy reaction times, ducking behaviors, and bullet velocity based on your current score.
* **Retro Visuals:** Real-time CRT scanline shader emulation simulated via optimized horizontal GDI pen operations.

## 🎛️ Game Controls
* `SPACE` — Shoot
* `UP Arrow` — Jump (dodge low bullets)
* `DOWN Arrow` — Duck (dodge high bullets)
* `1` — Switch to **Normal Ammo** (Standard speed and damage)
* `2` — Switch to **Fast Ammo** (High projectile velocity)
* `3` — Switch to **Heavy Ammo** (Slow projectile velocity, **x2 Damage**)
* `ENTER` — Start Game / Restart Session / Navigate Menu

## 🛠️ Weapon System Specs
| Ammo Type | Projectile Speed | Damage | Best Used For |
| :--- | :--- | :--- | :--- |
| **1 - Normal** | `15.0f` | 1 HP | Balanced dueling |
| **2 - Fast** | `25.0f` | 1 HP | Countering rapid enemy shots |
| **3 - Heavy** | `8.0f` | 2 HP | Punishing static targets / Guard breaking |

## 💻 Compilation & Build
To build the project via MSVC (Visual Studio Developer Command Prompt or VS Code task):

```bash
cl.exe main.c /link /SUBSYSTEM:WINDOWS user32.lib gdi32.lib
