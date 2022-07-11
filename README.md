# Arduino_Universal_IR_Remote
A library for Arduino compatible boards capable of recording and reproducing any (or most) IR remote commands. My intention is to create a library capable of working/commanding AC units as most arduino IR libraries are not able to/offer limited features in that regard.


How the project came to be:
I want to automate the AC system that I have at my current place. It uses an IR remote, that I thought I can emulate with an arduino (or an ESP later on) and IR LED. 
My intent was to use an IR receiver module to record the commands for the AC and reproduce them later at my discretion.
Unfortunately, none of the available libraries work as inteded. The only exceptions are a few that are able to record and reproduce the raw signal instead of decoding it.
After some investigations I found out the main problems that prevent the libraries from working:
 - AC units can use custom protocols to comunicate their settings.
 - AC units use very long sequences that require too much memory. 

While TV remotes send simple codes to the TV to decode, AC remotes send a sequence that contain all the desired configurations (temperature, mode, fan speed etc). The encoding of this information can be different from unit to unit. Some of the libraries that I tested can decode a few AC protocols, but none worked for my unit. The only option was to store the raw values.
The problem wih storing raw values is that AC units use long sequences to send their configurations. After testing a few AC systems I've seen signals that have 100-200 impulses and signals that have ~500 impulses or even more. This means that you would be limited to storing just a few codes. (2-4 depending on their length and the amount of memory available).

There for I needed a library that:
  - could be able to receive/send the raw values. This way it doesn't matter what encoding it uses or what kind of device it is.
  - can compress the data in order to save on space in memory.
  - is simple in design and usage, as to be easier to adapt for different project and usages. 

The scope of the project:
A simple library/class capable of:
 - receiving IR commands
 - sending IR commands
 - compressing/decompressing the commands
 - is compatible with as many boards as possible


The current status:
The library consists of a class in the header file. 
The file still contains lines used for debugging and there are still some improvements that I wish to implement, but the class is capable of receiving and sending IR commands and is usable in its current state.

The ino file contains mostly code for testing/debugging the functionality of the class. I'm going to provide an example later on.

The library uses simple for and while loops to send/receive the data. I opted to use these solution as it is the most easy to understand and offers the best compatibility across boards. (Using timer interrupts on my ESP8266 was not possible).

Future plans:
 - support for using the interrupt pin for receiving data.
 - support using a timer interrupt for generating the output signal. (I intended for this feature from the start, but my ESP8266 resets when I try using the timer interrupt for short durations of time. I don't know if this behavior is specific for ESP8266s or due to my esp being defective. I didn't find much information on this searching online)
 - support for setting the output frequency. Right now the outptut is fixed at 38kHz
 - support for using an outside frequency generator.
 - 

