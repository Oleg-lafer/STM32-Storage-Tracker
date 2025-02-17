Smart Storage Weight & RFID Verification System

📌 Project Overview

This project is a Smart Storage System that integrates weight sensors (HX711 load cells) and RFID for inventory tracking. The system detects when an item is taken from storage by monitoring weight changes and verifies the action using RFID scanning at the exit. If the detected weight change does not match the expected RFID tags, an alert is triggered.


---

⚙️ System Workflow

1. Continuous Weight Monitoring:

The system constantly checks the storage unit’s weight using an HX711 load cell sensor.

If a weight change is detected, the system saves the new value and signals the RFID scanner to activate.



2. RFID Scanning at Exit:

When a person exits with an item, they must pass through an RFID scanner.

The system collects the number of scanned RFID tags (each representing an item).



3. Verification Process:

The system calculates the expected weight change using the number of scanned RFID tags.

If the total weight removed matches the number of scanned tags, the transaction is valid.

If the values do not match, an error is logged.



4. Logging Results:

The STM32 prints messages to a console indicating whether the transaction was successful or if an error was detected.





---

🛠️ Hardware Requirements


---

📜 Code Overview

The software is written for an STM32 microcontroller using FreeRTOS.

🔹 Tasks in the System

WeightTask: Continuously reads weight from HX711 and detects changes.

RFIDTask: Scans RFID tags when a weight change is detected.

VerificationTask: Matches weight changes with the number of RFID tags scanned and logs results.



---

📂 File Structure

📁 SmartStorageRFID
│── 📄 main.c                  # Main program
│── 📄 hx711.h / hx711.c       # HX711 driver
│── 📄 rfid.h / rfid.c         # RFID module driver
│── 📄 FreeRTOSConfig.h        # FreeRTOS configuration
│── 📄 README.md               # Project documentation
│── 📄 Makefile                # Build script


---

📝 How to Use

1️⃣ Setup the Hardware

Connect HX711 Load Cell to STM32 (Data + Clock pins).

Connect RFID RC522 Module (SPI interface).

Power the STM32 and connect to a PC (for debugging output).


2️⃣ Flash the Firmware

Compile and upload the STM32 firmware using STM32CubeIDE or another compiler.

Start the system and monitor serial output for logs.


3️⃣ Observe Behavior

Place an item on the weight scale and remove it.

Scan the item’s RFID tag at the exit.

If the weight change matches the scanned items, the system prints:

Transaction OK - Weight and RFID match.

If there is a discrepancy, the system prints:

Error Detected - Mismatch between weight and RFID tags!



---

🎯 Future Improvements

✅ LCD Display – Show real-time status.
✅ Wireless Connectivity – Send logs to a web dashboard.
✅ Multiple Weight Sensors – Improve accuracy for different items.


---

📌 Conclusion

This project provides a simple yet effective method for tracking inventory using weight sensors and RFID. The STM32 handles multiple tasks using FreeRTOS, making it a great learning experience for embedded systems development.


---

🤝 Contributions

Feel free to fork, improve, and contribute! 🚀

