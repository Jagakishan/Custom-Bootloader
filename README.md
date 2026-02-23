# Dual-Slot Bootloader with Watchdog-Based Rollback (STM32)

A bare-metal bootloader for STM32 microcontrollers implementing **dual application slots**, **CRC-based validation**, **watchdog-driven automatic rollback** and **firmware update(not 100% OTA yet)** for new app updates.

The project files only involve the bootloader's main file. the linker scripts of application where memory layout is decided and image headers are pushed into app's start address under the sections column.
Application's main files are just dummy codes for debugging, but could be anything if you're serious about using it. 

---

## 🚀 Features

- **Dual application slots**
  - Two independent firmware images stored in flash
  - Each app occupies **24 KB**
  - The bootloader occupies **16KB**
  - First application starts at `0x08004000`
  - Second application starts at `0x0800A000`

- **Image header–based validation**
  - Magic number
  - Version
  - Image size
  - CRC32 checksum
  - State flags (pending / confirmed / rejected)

- **CRC32 integrity check**
  - Ensures application image is not corrupted
  - CRC validated before every jump
  - Done by post-build commands, where it calls upon a python code which recieves the binary of the file and computes size and CRC32.
  - The python code automatically patches the computed size and CRC in image header of the application.

- **Watchdog-based rollback mechanism**
  - By default firmware boots in **pending** state
  - Application must confirm itself after successful startup
  - This confirmation is purely internal to bootloader. The image header of the application is not modified.
  - If the app crashes or hangs → watchdog reset
  - Bootloader detects watchdog reset and **rolls back automatically**

- **Fail-safe boot logic**
  - Bootloader always boots a **known-good** image
  - Rejected images are permanently skipped

- **Bare-metal implementation**
  - No RTOS
  - No HAL dependency in boot logic
  - Direct register-level control for critical paths

- **Manual Firmware update**
  - Automated flashing of new app image.
  - But, binary has to be pasted in the project folder manually.
  - This binary will be transmitted over Wi-fi in next version of project. Or it will be uploaded as a seperate project in the same repository. 

---

## 🧠 Boot Flow Overview

RESET

 ↓
 
Bootloader Vector table

 ↓
 
Bootloader Reset_Handler

 ↓
 
Validate apps

 ↓
 
Select app

 ↓
 
Read MSP + Reset_Handler

 ↓
 
Disable IRQs

 ↓
 
Set VTOR = APP_BASE

 ↓
 
Set MSP = app_msp
 
 ↓
 
Jump to app reset handler
 
 ↓
 
Application Vector table
 
 ↓
 
Application Reset_Handler
 
 ↓
 
main()


---

## 📦 Flash Layout

Flash Start: 0x08000000

Bootloader |  16KB  | 0x08000000-0x08003FFF

App Slot1  | ~24 KB | 0x08004000-0x08009FFF (Header+Image)

App Slot1  | ~24 KB | 0x0800A000-0x0800EFFF (Header+Image)

---

## 🧾 Image Header Format

128 bytes header with 108 bytes reserved for future extension and also VTOR needs 128 byte aligned start address for normal execution and ISR jumping.

| Field        | Description                          |
|-------------|--------------------------------------|
| Magic       | Identifies valid application image   |
| Version     | Used to select newer firmware        |
| Size        | Application image size               |
| CRC32       | Integrity verification               |
| State Flag  | Pending / Confirmed / Rejected       |

---

## ❌ Technologies that I didn't implement

These were intentionally excluded to keep the system minimal and focused:

- OTA transport (Wi-Fi) - Will be added anytime soon.
- Firmware encryption
- Flash wear leveling

---

## Author
Jagakishan S.K


