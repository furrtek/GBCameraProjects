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

# File format
For now (V1), audio and video are recorded raw in the same file and need to be converted to a suitable video format on a computer with a special tool. Directly recording uncompressed frames to AVI can be done (TODO).

The file starts with 4 bytes: "GBCC".

Vx (0x56 xx) indicates the start of image data, and how many times the previous image has to be replicated (skips). One image is 3584 bytes, in GB format.

Ax (0x41 xx) indicates the start of one or multiple (xx) audio frames. Each are 512 unsigned bytes (8192Hz).
