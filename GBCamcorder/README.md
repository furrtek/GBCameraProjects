# GBCamcorder
Lo-Fi portable video recorder using a GameBoy Camera cartridge.

More details at http://furrtek.free.fr/?a=gbcc

This initially was a 10-days rush project for the RGC 2015 (French retrogaming meeting).
It involved PCB and 3D-printable enclosure design, firmware writing in C, and assembly by hand.

It started to work 3 days late because of hair-pulling SD card and audio issues, which were caused by a misbehaving DC-DC converter.

# Features
* ~14fps 128*112px 4 colors video and 8192Hz audio recording on µSD(HC) card.
* Auto-exposure with out-of-range alerts.
* Dump the cartridge's saved pictures to the µSD card.
* Real time LCD viewfinder.
* No need to destroy cartridges.
* Playback of recorded files.
* USB rechargeable LiPo battery.

# The idea
The 1998 Nintendo GameBoy Camera was an official accessory for the GameBoy which could shoot and save up to 30, 128*112px pictures in 4 shades of grey.

Numerous projects use the GameBoy Camera's CMOS sensor, but according to Google none use the complete cartridge.
The main idea is to preserve the cartridge and interface to it directly, taking the GameBoy's place, and record shots fast enough to create videos.

# File format
For now, audio and video are recorded raw and interleaved in the files. They need to be converted to common video formats on a computer with a special tool. Photos and SRAM dumps are also in raw format but could be saved as .bmp files with a bit more work.

The file starts with 4 bytes: "GBCC". The next 4 bytes are the video frame count, the next 4 are the audio frame count, the next 4 are reserved for now, and then the interleaved data starts (offset 16).

Raw video and audio frames are stored with a 2 bytes header: 

Vx (0x56 xx) indicates the start of image data, and how many times the previous image has to be replicated (skips). One image is exactly 3584 bytes, in GB format.

Ax (0x41 xx) indicates the start of one or multiple (xx) audio frames. Each are 512 unsigned bytes and should be played back at 8192Hz.
