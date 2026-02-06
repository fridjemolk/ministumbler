# ministumbler, a tiny ESP32 WiFi stumbling tool

This repo contains arduino code, helper python scripts, KiCad and Gerber Files, and hardware information about my ESP32 based WiFi stumbler, designed with low cost, minimal size, and easy of asssembly in mind. It is currently based around the Seeed XIAO/Adafruit QT Py ecosystem.

<img width="1606" height="1823" alt="photo_2026-02-06_13-54-47" src="https://github.com/user-attachments/assets/be31c994-0b6e-467f-973e-6ebb7ca41445" />

As of 02/2026, this project exists in 3 different hardware configurations, all of which are based on a GNSS Module, a MicroSD card slot, and a Xiao ESP32-S3:

#### v0.2: A custom PCB backplane supporting each of the modules, plus a [LiPo battery](https://thepihut.com/products/2000mah-3-7v-lipo-battery) charged by the Xiao ESP32-S3's built-in charge circuit, designed to fit into [this case from the Pi Hut](https://thepihut.com/products/small-plastic-project-enclosure-weatherproof-with-clear-top). The KiCad and Gerber files are [available here.](https://github.com/fridjemolk/ministumbler/tree/master/ministumbler_weatherproof_case) 
![photo_2026-01-19_00-50-52](https://github.com/user-attachments/assets/4a180b09-6744-44c3-ac56-591ac72c86cb)

#### v0.1: A custom flex PCB backplane, supporting each of the modules, designed for stealthy use in the pocket of a bag or inside clothing. The KiCad and Gerber files are [available here.](https://github.com/fridjemolk/ministumbler/tree/master/ministumbler_kicad)
<img width="1434" height="547" alt="IMG_6390" src="https://github.com/user-attachments/assets/90a6a6ae-ce02-425a-a5d4-51295b315baf" />


#### v0: A soldered-together stack of all three modules into a very compact rough cube shape. This was the initial prototype and is a little awkward to assemble as it requires deliberately not soldering certain pins for each module. 
![photo_2024-10-22_11-17-53](https://github.com/user-attachments/assets/c75c636c-1c94-47f4-addd-2b2b55e19a51)

All three hardware versions currently [use the same firmware](https://github.com/fridjemolk/ministumbler/blob/master/Ministumbler_WiGLE/Ministumbler_WiGLE.ino), with the same pin configurations, requireing the TX jumper pad to be **CUT** on the SD module and the A1 jumper pad **SOLDERED**. This firmware builds off the firmware created by [lozaning](https://github.com/lozaning) for their [Wifydra Project](https://github.com/lozaning/The_Wifydra), so credit to them for all their hard work in supporting the WiGLE CSV format and handling SD card file creation and management. 

[As of 02/2026, this is the main firmware used for this project](https://github.com/fridjemolk/ministumbler/blob/master/Ministumbler_WiGLE/Ministumbler_WiGLE.ino)

### Required Hardware

#### In the current design version the following three boards are used:

- [Seeed XIAO ESP32S3.](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/) The S3 variant of the ESP32 has the added benefit of Bluetooth support, and of having a U.FL connector for an external antenna, opening up options for optimising reception.
- [L76K GNSS Module for SeeedStudio XIAO.](https://wiki.seeedstudio.com/get_start_l76k_gnss/) A low-cost, TinyGPS++ compatible GPS module that has proven easy to use and quick to lock. Includes an active GPS antenna, also connected via a U.Fl connector
- [Adafruit microSD Card BFF.](https://learn.adafruit.com/adafruit-microsd-card-bff) A basic microSD card breakout board from adafruit that directly connects an insterted microSD to the SPI pins on the ESP32S3. Since the ESP32S3 has an on-board 3.3v regulator and runs at a 3.3v logic level there is no need for any regulator or level shifter to interface with a microSD card.

The v0 hardware design leads to an issue with the GPS RESET pin being the same as the MOSI (D10) pin of the ESP32, which is used to communicate with the microSD card. For the **v0 configuration ONLY** it is therefore required to ensure that this pin from the ESP32 connects only to the SD board, as the GPS RESET pin is not needed. In fact, the GPS module only requires it's RX, TX, 5V, GND, and 3v3 pins to actually be connected to the ESP32. The 5V pin can also be omitted to disabled the green GPS PPS LED if required.

## Software

### This repo, as it currently stands, contains three pieces of software:

- A fully fledged WiGLE scanning script, that scans all wifi channels and creats a WiGLE format CSV that can be uploaded to the WiGLE website.
- [A deprecated, more basic Arduino sketch](https://github.com/fridjemolk/ministumbler/blob/master/GPS_WiFi_XIAO_Serial_CSV/GPS_WiFi_XIAO_Serial_CSV.ino) that interfaces with the GPS module via the ESP32 fork of TinyGPSPlus and uses the Arduino WiFi libray to scan WiFi networks. The sketch will repeatedly output a line of CSV data over USB Serial detailing the current GPS location, date, time, and a list of WiFi SSIDs. 
- [A simple python script](https://github.com/fridjemolk/ministumbler/blob/master/csv_write_test.py) that records the data received from the ESP32 over USB Serial and writes it to a CSV file. 

## KiCad Projects

To open these projects in KiCad, clone this repo or download one of the kicad directories and open the `.kicad_pro` file located inside each folder from KiCad's GUI. 



