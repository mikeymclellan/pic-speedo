# PIC-Speedo

An embedded PIC-based speedometer project that uses a hall effect sensor and stepper motors to drive a classic Datsun 240z speedometer. 

## Overview

The PIC-Speedo project uses a Honeywell 1GT101DC Hall effect sensor to detect axle rotations and drives drives two stepper motors via LD239 drivers. One stepper motor for the speedo needle, and the other for the odometer. The processor I used was the PIC16F628A.

Prototype board: 

<img src="https://s3-ap-southeast-2.amazonaws.com/forum-media/photobucket/IMAG0283-1.jpg" />

This is the stepper motor used for the speedometer:

<img src="https://s3-ap-southeast-2.amazonaws.com/forum-media/photobucket/ScreenShot2013-11-14at85322PM.png" />

The hall effect sensor is trigger by the bolts on the diff flange:

<img src="https://s3-ap-southeast-2.amazonaws.com/forum-media/photobucket/IMG_2655_zps529df59d1.jpg" />

Testing the odometer:

<img src="https://s3-ap-southeast-2.amazonaws.com/forum-media/photobucket/IMG_20130802_203251.jpg" />
