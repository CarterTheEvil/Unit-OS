# 🖥️ Unit OS

Welcome to **Unit OS**! This is a hobbyist, custom-built monolithic terminal operating system. It features a **custom kernel**, its own **bootloader**, an independent **keyboard driver**, and a **VGA rendering engine**. 

Unlike massive distributions like Ubuntu or Debian, Unit OS is designed from the ground up to be lightweight, simple, and minimal, operating with a specific, curated set of commands.

👤 **Developer:** [@CarterTheEvil](https://github.com) (Solo Project)  
🌐 **Project Website:** [Unit OS Info Page](https://google.com)

---

## 🛠️ Architecture & System Structure

The operating system is built entirely from scratch using low-level systems programming:
* **Languages:** Written natively in **C (82.3%)** and **Assembly (10.2%)** with a custom Makefile compilation workflow.
* **Boot & Initialization:** Utilizes a custom assembly-based boot sequence (`boot_kernel.asm`) along with custom Interrupt Descriptor Table (IDT) configuration (`interrupts.c`).
* **Hardware Drivers:** Features built-in hardware management, including an independent keyboard driver with scancode handling (`keyboard.c`) and custom PIT timer integration (`timer.c`).
* **Graphics:** Implements a direct VGA color definition system (`rendering.c`) to control terminal display visuals.

---

## 📦 Getting Started

Because standard repository README files are limited to basic Markdown and text formatting, a rich instruction suite is built directly into an interactive file.

* **Option 1:** [Run `Readme.html` in an online HTML compiler](https://github.io)
* **Option 2:** Download `Readme.html` and open it locally in your favorite web browser.

---

## ⚠️ Important Release Notes

> 💡 **Recommended Version:** For the most stable experience and a verified boot cycle, please download and deploy the **`0.0.3` stable release**.

* **Current Version:** `0.0.4 (Unstable / Development)`
* **Project Status:** **Paused.** This repository is currently staying at version `0.0.4`. Regular updates are on hold unless I choose to dive back into the source code at a later date.

---

## 🐛 Bug Reports & Solo Support

Please note that this is a completely solo passion project. I do not have a secondary contributor, peer reviewer, or development team behind me.
* **Delayed Fixes:** If you run into core panics, compilation failures, or input bugs, I will have to investigate them entirely on my own. It may take some time to roll out a patch.
* Your patience, testing, and feedback are incredibly appreciated!


