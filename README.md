# GBCamcorder
Lo-Fi portable video recorder using a GameBoy Camera cartridge.

This initially was a 10-days rush project for the RGC 2015 (French retrogaming meeting).
It involved designing a custom PCB and a 3D-printable enclosure, writing the firmware, and assembling by hand.

It was finished 3 days late because of SD card and audio issues.

# Features
* Steady 16fps 128*112px video and 8192Hz audio recording on µSD(HC).
* LCD viewfinder.
* Auto-exposure with out-of-range alerts.
* Playback of recorded files (TODO).
* USB rechargeable LiPo battery.

# The idea
The 1998 GameBoy Camera was an official accessory for the GameBoy which could shoot and save up to 40, 128*112px pictures in 4 shades of grey.

Numerous projects use the GameBoy Camera's CMOS sensor, but according to Google none use the complete cartridge.
The main idea is to preserve and show the cartridge as much as possible and interface to it directly, taking the GameBoy's place.

# Design
Knowing Atmel's 8-bit AVR MCUs since years, it was obvious that they wouldn't be powerful enough to handle all the tasks.
I chose NXP's LPC1343 ARM Cortex-M3 MCU for it's speed (72MHz max), price ($3), availability (Farnell), solderability (QFP), RAM size (8kB), and ease of development (free tools, USB firmware upload).

As time was the main constraint, I ordered Adafruit's 2.2" 320*240 LCD + µSD card slot, and amplified microphone breakout boards.

The GameBoy cartridge connector was purchased on eBay as a DS Lite GBA replacement part, I had to file the edges down to make original GB carts fit.

All the other components came from Farnell. The 3D-printed polyamide case was ordered from i-materialise (Belgian).

The 5V step-up for the GB Camera is done by a MCP1640.
The battery charging is done by a MCP73831.
The 3.3V regulator was a 1117-3.3, later changed to a MCP1755-33.

Electronics design was done in Labcenter's Proteus suite (will move to Eagle).
Case was done in Solidworks. Firmware was done in LPCXpresso (warning: Eclipse).

# File format
For now (V1), audio and video are recorded raw in the same file and needs to be converted to a suitable video format on a computer with a special tool. Directly recording uncompressed frames to AVI can be done (TODO).

One 3584 (0xE00) bytes image in GB format, with one 512 (0x200) bytes audio frame. 16 pairs per second (64kB/s).
18.2 hours to fill up a 4GB card.

# Timings

TODO.

# Problems encountered
* No 5V: Inverted pins on MCP1640, nothing fried. Fixed with cuts and wires.
* No USB power: MCP73831 bypass didn't work as expected. Fixed with cuts, wires and diodes.
* Not getting MCU to go in programming mode: Should have used a reset generator. Fixed by changing 2 resistor values.
* Couldn't talk to GB Camera on battery power: 3.3V LDO gave 2.7V (1V dropout on 3.7V battery), not high enough for GB Cam to consider as logic "1". Changed LDO to be even an even more LDO.
* Could create new file on SD card, but not write to it: This sucked. The code wasn't waiting for the SD card to go back to idle after a sector write (see below).
* Extreme audio noise: 18MHz SPI clock pin going to LCD board acted as an antenna. Fixed with ferrite bead and shielding (see below).
* LCD not initializing: It was, just didn't look like it because of wrong refresh rate.

# The SD card issue
Also because of time, I used ChaN's FATFS library to handle files. The SD (MMC) interface code was slapped together from various sources and with the help of the SD specifications.

Initilisation worked, mounting worked, opening a file worked, reading a file worked, creating a new file worked, writing to the file didn't work.

The f_write function always returned FR_DISK_ERR, which really has no meaning.

Genuine 4GB Sandisk SD was freshly formatted to FAT32.
No write protection on µSD and FATFS was modified to not use a WP pin.
Power supply was good, no drops on the 3.3V rail.

Very annoying since the file creation worked (physical write to tha card's flash worked !)
Tracking down the cause of the reported FATFS error revealed that the card was answering correctly and wasn't reporting any  errors. FATFS wasn't happy because the card only executed the first write command (to the FAT, the file creation).

Hours of running in circles got me to notice that SD card reported that the sector to write was buffered correctly, but NOT written to flash.

After CMD24 and 0101b response, always wait for 0xFF afterwards (flash write done) !

# Microphone noise issue
Very loud and unfilterable noise bursts in audio signal.
Matched the SD card transfer periods.

The 3.3V rail was still very clean. 10µ and 100n decoupling caps were close to the mic board.
The board layout did make the audio line cross several digital lines, but cutting it from the mic board showed that the noise was still present right at the board's Vout pin.

Guessed that it could only be amplified EMI from signals near the mic board (which is on top of non-scanned button lines only). Could only be the backlight PWM or SPI pins going to the LCD board.
Shut down SPI but kept the 100kHz PWM on, no noise anymore.

Cranked up PWM to 10MHz, slight continuous noise. Tried 10/10ms 10MHz PWM bursts and got the same very loud noise.
Narrowed it down to the SPI clock signal which runs between 15MHz (LCD) and 18MHZ (SD), the MOSI line which is 6mm away didn't cause any noise.

Fixed the issue by adding a ferrite bead around the pin going up to the LCD board and adding ghetto shielding around the mic board.
