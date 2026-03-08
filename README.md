# Writing Tamagotchi 🦖⌨️

A physical **Tamagotchi-style writing companion** built with an
ESP32-S3.

Tama is a tiny dinosaur that **survives on your words**. Feed it at
least **250 words per day** or it will slowly get sick... and eventually
die.

The project combines:

-   Embedded software
-   Pixel animation
-   Habit tracking
-   Physical hardware (buttons, screen, battery)
-   3D printed enclosure

The goal is to create a **delightful physical motivator for writing
habits**.

------------------------------------------------------------------------

# What It Does

Tama lives inside the device and reacts to your writing progress.

## Writing Rules

-   Feed Tama **250+ words per day**
-   Miss **3 days → Tama gets sick**
-   Miss **6 days → Tama dies**

## Lifecycle

  Stage   Trigger
  ------- -------------------
  Egg     Start state
  Hatch   1,000 words
  Adult   20% of total goal
  Sick    3 missed days
  Dead    6 missed days

## Device Controls

The Tamagotchi is controlled with **three buttons**:

  Button   Action
  -------- ----------------
  Left     Decrease value
  Middle   Reset
  Right    Increase value

The buttons are used to:

-   log writing progress
-   navigate menus
-   trigger interactions

## Visual Feedback

Tama expresses its state through **pixel animations**:

-   idle
-   reading
-   writing
-   celebrating
-   sick
-   dead

Animations are rendered on the LCD using **LVGL**.

------------------------------------------------------------------------

# Hardware

This project uses an **ESP32-S3 AMOLED development board**.

## Main Components

-   ESP32-S3 Touch AMOLED 1.8 display
-   3 × tactile buttons
-   LiPo battery
-   speaker (optional)
-   3D printed case

## GPIO Layout

Buttons are connected to GPIO pins and ground.

Example wiring:

Button → GPIO\
Button → GND

Each button press is detected as a digital input event.

------------------------------------------------------------------------

# Project Architecture

    writing-tamagotchi
    │
    ├── main
    │   ├── main.c
    │   ├── assets
    │   │   ├── images
    │   │   └── animations
    │   │
    │   └── audio
    │
    ├── components
    │   └── esp_lcd_touch_ft3168
    │
    ├── partitions.csv
    └── CMakeLists.txt

## Core Systems

### 1. UI Layer

Implemented using **LVGL**

Responsibilities:

-   render Tamagotchi background
-   display animations
-   show icons and menus

### 2. Animation System

Animations are stored as **C image arrays** generated from PNG files.

assets/animations/

Each state has its own animation set:

prehatch/\
hatched/\
idle/\
writing/\
celebrate/\
sick/\
dead/

Animation frames are displayed sequentially using an FPS controller.

### 3. Tamagotchi State Machine

The behaviour logic manages:

-   lifecycle transitions
-   health tracking
-   writing milestones
-   animation switching

Simplified state flow:

Egg → Hatched → Idle/Writing → Sick → Dead

### 4. Input System

Button presses trigger events such as:

-   increment word counter
-   decrement
-   reset

Inputs are read from **GPIO pins**.

### 5. Asset Pipeline

Animations are created as images and converted using the **LVGL Image
Converter**.

PNG → LVGL converter → C pixel array

These are compiled directly into the firmware.

------------------------------------------------------------------------

# Software Requirements

You will need:

-   ESP-IDF v5+
-   Python 3
-   CMake
-   ESP-IDF VSCode Extension (recommended)

------------------------------------------------------------------------

# Installation

Clone the repository:

    git clone https://github.com/yourname/writing-tamagotchi
    cd writing-tamagotchi

Install ESP-IDF dependencies:

    idf.py install

Configure the project:

    idf.py menuconfig

Select the correct **ESP32-S3 board configuration**.

------------------------------------------------------------------------

# Build

Compile the firmware:

    idf.py build

------------------------------------------------------------------------

# Flash to Device

Connect the ESP32-S3 via USB.

Flash and monitor:

    idf.py -p /dev/ttyUSB0 flash monitor

On macOS the port may be:

    /dev/tty.usbmodem*

------------------------------------------------------------------------

# Running

Once flashed, the device will boot automatically.

The display will show:

1.  Tamagotchi background
2.  Current animation
3.  Writing progress interface

Use the buttons to simulate writing progress.

------------------------------------------------------------------------

# Development Workflow

Edit code → Build → Flash → Test → Repeat

------------------------------------------------------------------------

# Creating New Animations

1.  Draw animation frames
2.  Export as PNG images
3.  Convert with LVGL image converter
4.  Add generated `.c` files to:

assets/animations/

5.  Register animation in the animation system.

------------------------------------------------------------------------

# Future Improvements

Potential upgrades:

-   WiFi syncing with writing apps
-   persistent habit storage
-   multiple pets
-   streak tracking
-   sound effects
-   USB charging port in case design

------------------------------------------------------------------------

# License

MIT License

------------------------------------------------------------------------

# Credits

Inspired by the original Tamagotchi virtual pets.
