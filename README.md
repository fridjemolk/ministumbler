# ministumbler, a tiny ESP32 WiFi stumbling tool

This repo contains arduino code, helper python scripts, and hardware information about my ESP32 based WiFi stumbler, designed with low cost, minimal size, and easy of asssembly in mind. It is currently based around the Seeed XIAO/Adafruit QT Py ecosystem. 

##Required Hardware

###The minimum requirments for a WiFi stumbler are:
- A way to scan for WiFi networks
- A way to record GPS position 
- A way to keep track of time accurately
- A way to record all of the above data
- A computer to connect all of the above components and process their data

###In this design, these requirements are met as simply as possible, as follows:
- An ESP32 to be our computer, and to handle WiFi scanning.
- A GPS module, for GPS positioning, as well as an accurate source of the current time.
- A microSD card.

###In the current design version, the Seeed XIAO/Adafruit QT Py ecosystem of boards are used for their small size, snap-together pin header based design, wide availability, and low cost. Currently, the following three boards are used:

- [Seeed XIAO ESP32S3.](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/) The S3 variant of the ESP32 has the added benefit of Bluetooth support, and of having a U.FL connector for an external antenna, opening up options for optimising reception.
- [L76K GNSS Module for SeeedStudio XIAO.](https://wiki.seeedstudio.com/get_start_l76k_gnss/) A low-cost, TinyGPS++ compatible GPS module that has proven easy to use and quick to lock. Includes an active GPS antenna, also connected via a U.Fl connector
- [Adafruit microSD Card BFF.](https://learn.adafruit.com/adafruit-microsd-card-bff) A basic microSD card breakout board from adafruit that directly connects an insterted microSD to the SPI pins on the ESP32S3. Since the ESP32S3 has an on-board 3.3v regulator and runs at a 3.3v logic level there is no need for any regulator or level shifter to interface with a microSD card.

Due to confusion with the pinouts in the documentation of the above boards, the first attempt at assembling this hardware design led to an issue with the GPS RESET pin being the same as the MOSI (D10) pin of the ESP32, which was used to communicate with the microSD card. Due to this discrepancy not being discovered until after soldered assembly, and removal of the pin and replacement with a bodge wire failing due to the D10 pin burning off the board (my bad lol), the first tests of this hardware were conducted by instead streaming the WiFi, GPS, and time date out to a laptop over USB serial (see below). 

##Software

###This repo, as it currently stands, contains two pieces of software:

- A basic Arduino sketch that interfaces with the GPS module via the ESP32 fork of TinyGPSPlus and uses the Arduino WiFi libray to scan WiFi networks. The sketch will repeatedly output a line of CSV data over USB Serial detailing the current GPS location, date, time, and a list of WiFi SSIDs. 
- Asimple python script that records the data received from the ESP32 over USB Serial and writes it to a CSV file. 

Once the hardware issues detailed above are resolved, an Arduino sketch will be available that will write the CSV file locally on the ESP32 onto a microSD.



