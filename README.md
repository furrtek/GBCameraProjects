# GBCamcorder
Lo-Fi portable video recorder using a GameBoy Camera cartridge.

More details at http://furrtek.free.fr/?a=gbcc

This initially was a 10-days rush project for the RGC 2015 (French retrogaming meeting).
It involved designing a custom PCB and a 3D-printable enclosure, writing the firmware, and assembling by hand.

It was finished 3 days late because of SD card and audio issues.

# Features
* ~14fps 128*112px video and 8192Hz audio recording on µSD(HC).
* LCD viewfinder.
* Auto-exposure with out-of-range alerts.
* Playback of recorded files (TODO).
* USB rechargeable LiPo battery.

# The idea
The 1998 Nintendo GameBoy Camera was an official accessory for the GameBoy which could shoot and save up to 40, 128*112px pictures in 4 shades of grey.

Numerous projects use the GameBoy Camera's CMOS sensor, but according to Google none use the complete cartridge.
The main idea is to preserve and show the cartridge as much as possible and interface to it directly, taking the GameBoy's place.

# Design
Knowing Atmel's 8-bit AVR MCUs since years, it was obvious that they wouldn't be powerful enough to handle all the tasks.
I chose NXP's LPC1343 ARM Cortex-M3 MCU for it's speed (72MHz max), price ($3), availability (Farnell), solderability (QFP), RAM size (8kB), and ease of development (free tools, USB firmware upload).

As time was the main constraint, I ordered Adafruit's 2.2" 320*240 LCD + µSD card slot, and amplified microphone breakout boards.

The GameBoy cartridge connector was purchased on eBay as a DS Lite GBA slot replacement part, I had to file the edges down to make original GB carts fit.

All the other components came from Farnell. The 3D-printed polyamide case was ordered from i-materialise (Belgium).

The 5V step-up for the GB Camera is done by a MCP1640.
The battery charging is done by a MCP73831.
The 3.3V regulator is a 1117-3.3, fed with 5V.

Electronics design was done in Labcenter's Proteus suite (will move to Eagle).
Case was done in Solidworks. Firmware was done in LPCXpresso.

# File format
For now (V1), audio and video are recorded raw in the same file and need to be converted to a suitable video format on a computer with a special tool. Directly recording uncompressed frames to AVI can be done (TODO).

The file starts with 4 bytes: "GBCC".

Vx (0x56 xx) indicates the start of image data, and how many times the previous image has to be replicated (skips). One image is 3584 bytes, in GB format.

Ax (0x41 xx) indicates the start of one or multiple (xx) audio frames. Each are 512 unsigned bytes (8192Hz).

# Timings

See http://furrtek.free.fr/?a=gbcc

# Problems encountered
* No 5V: Inverted a pair of pins on the MCP1640, nothing fried. Fixed with cuts and wires.
* No USB power: MCP73831 bypass didn't work as expected. Fixed with cuts, wires and diodes.
* Not getting MCU to go in programming mode: Should have used a reset generator. Fixed by changing 2 resistor values.
* Couldn't talk to GB Camera on battery power: 3.3V LDO gave 2.7V (1V dropout on 3.7V battery), not high enough for GB Cam to consider as logic "1". Wired LDO to be fed from 5V step-up instead of battery.
* Could create new file on SD card, but not write to it: This sucked. Wasn't waiting for the SD card to go back to idle after a sector write (see below).
* Extreme audio noise: Bad capacitor placement.
* LCD not initializing: It was, just didn't look like it because of wrong refresh rate.
* "Random" stutters during recording. Misread the SD documentation and found out that cards can have up to 250ms write times depending on some internal wear-levelling process. Needed to buffer multiple frames, not enough RAM. Huge design flaw on my side...

# The SD card issue
Also because of time, I used ChaN's FATFS library to handle files. The SD interface code was slapped together from various sources and with the help of the SD specifications.

Initilisation worked, mounting worked, opening a file worked, reading a file worked, creating a new file worked, writing to the file didn't work.

The f_write function always returned FR_DISK_ERR, which really has no meaning.

Genuine 4GB Sandisk SD was freshly formatted to FAT32.
No write protection on µSD, and FATFS was modified to not use a WP pin.
Power supply was good, no drops on the 3.3V rail.

Very annoying since the file creation worked (physical write to tha card's flash worked !)
Tracking down the cause of the reported FATFS error revealed that the card was answering correctly and wasn't reporting any  errors. FATFS wasn't happy because the card only executed the first write command (to the FAT, the file creation).

Hours of running in circles got me to notice that the SD card reported the sector was buffered correctly, but NOT written to flash.

After CMD24 and 0101b response, always wait for 0xFF afterwards (flash write done) !
