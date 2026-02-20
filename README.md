> 🎓 **Academic Project**: Developed as the final project for the *Computer Programming* course at the **Silesian University of Technology** (Politechnika Śląska).

# Xonix Arcade Game 🕹️

A classic retro arcade game clone written purely in **C++** using the **SFML** (Simple and Fast Multimedia Library). The project demonstrates strong object-oriented programming principles, memory management, and real-time game loop architecture.

## 🌟 Key Features
* **Classic Gameplay:** Navigate the playing field to capture territory while avoiding bouncing enemies.
* **Clean Architecture:** The codebase strictly separates business logic (`logika.cpp`) from rendering and presentation (`widok.cpp`).
* **Grid-Based Collision Detection:** Implements a robust 2D tile grid system to handle movement bounds, capturing areas, and enemy collisions.
* **Multimedia Integration:** Full support for custom sprites, fonts, and sound effects managed through SFML interfaces.

## 🛠️ Tech Stack
* **Language:** C++ 
* **Graphics & Audio API:** SFML 2.x
* **Architecture:** Object-Oriented Programming (OOP), Custom Game Loop
* **IDE:** Visual Studio

## 📸 Screenshots
*(Zrób 2 zrzuty ekranu - jeden z samej gry, a drugi z ekranu Game Over lub momentu zdobywania pola)*
* `![Gameplay](Project1/images/screen1.png)` - **Figure 1: Gameplay.** Capturing territory while avoiding enemies.
* `![Game Over](Project1/images/screen2.png)` - **Figure 2: Game State.** Handling win/loss conditions and rendering custom fonts.

## 🚀 How to Play
You don't need to build the project to play! 
1. Go to the [Releases](../../releases) tab on the right side of this repository.
2. Download the `XonixGame_Release.zip` file.
3. Extract the archive and run the `.exe` file (ensure all `.dll` files and asset folders are kept in the same directory).

## 💻 How to Build (For Developers)
1. Clone this repository.
2. Open the `.sln` file in Visual Studio.
3. The repository includes the necessary SFML headers and precompiled `.lib`/`.dll` files in the `External/` and `Project1/` directories.
4. Set the build configuration to **Release** (x86/x64 depending on the linked libraries) and click "Local Windows Debugger".
