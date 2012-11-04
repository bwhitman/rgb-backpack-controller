rgb-backpack-controller
=======================

Runs a 9600bps "server" on the [Sparkfun LED Matrix backpack](https://www.sparkfun.com/products/760) to control from a "real computer"

![LED Matrix](https://dlnmh9ip6v2uc.cloudfront.net/images/products/7/6/0/00760-04-L.jpg)

## Set up

* Get the [Sparkfun LED Matrix backpack](https://www.sparkfun.com/products/760)
* Get an atmel or arduino. I'm using the [BeagleBoard Trainer](http://elinux.org/BeagleBoard_Trainer) talking to a BeagleBoard xM
* Wire 5 pins from the microcontroller to the backpack. You need GND, MOSI, CS, SCLK, VCC.

### (If it's not an arduino already)

* Get some way to ISP program an Atmel if it's not an arduino already, like [one of these](https://www.sparkfun.com/products/8702)
* Put the ISP 6 pin cable of the programmer so that the ribbon cable goes away from the board
* Get this [arduino boot firmware](http://arduino.googlecode.com/svn/trunk/hardware/arduino/bootloaders/atmega/ATmegaBOOT_168_atmega328_pro_8MHz.hex)
* Program it to the board via your ISP using avrdude:

```bash
avrdude -V -F -c stk500v2 -p m328p -P /dev/cu.usbmodem26211 -b 57600 -e -u -U lock:w:0x3f:m -U efuse:w:0x05:m -U hfuse:w:0xda:m -U lfuse:w:0xff:m
avrdude -V -F -c stk500v2 -p m328p -P /dev/cu.usbmodem26211 -b 57600 -U flash:w:ATmegaBOOT_168_atmega328_pro_8MHz.hex -U lock:w:0x0f:m
```

## Controlling the matrix from anywhere

* Hook up a serial connection between your atmel/arduino and whatever you want to control the LED Matrix from (since I'm using a Beagleboard, the serial port is already connected to the trainer as /dev/ttyO1)
* Flash led_sender.hex anyway you know how. Since I put that firmware on my board above, I do this quickly after hitting reset:

```bash
avrdude -V -F -c stk500v1 -p m328p -P /dev/ttyO1 -b 57600 -U flash:w:led_sender.hex 
```

* (You can of course compile led_sender.c yourself if you want, i put the avr-gcc makefile in the repo too. But you shouldn't need to change it unless your pin numbers are different.)
* On the computer you want to control the LED Matrix from, change the serial port name in block_sender.c 
* Compile the block_sender.c:

```bash
gcc -o block_sender block_sender.c
```

* Run the block sender on the computer you want to control the LED Matrix from
* It should look like this (in real life it's less blown out)

![alt text](http://cl.ly/image/0Y0h3m0Z0s3U/Image%202012.11.04%2010:40:22%20AM.png)

* Now control your LED matrix at will. In block_sender you can see how it's done -- you fill up a char array with 64 bytes, each one a 4 bit number corresponding to the palette index. Then you call 

```c
draw_frame(fd, frame);
```

* Lower level if you want: the atmel listens for bytes on the UART at 9600. Every time it gets a byte, it checks if it is 32. If it is, it starts the drawing pointer at position 0. If it's not, it splits the byte up into two 4-bit numbers, each one a palette index. After drawing two dots it incremements the position automatically. So you should send 33 bytes per frame, starting with the number 32 and then 64 pixels, each 4 bits.

## FAQ

### Why didn't you just use SPI, isn't it a waste to have 2 microcontrollers converting stuff back and forth

Have you tried to use SPI on a computer before, so annoying

### Can I increase the baud rate?

Probably. The UART might get in the way of the SPI. I haven't played with that too much and 9600 is fine for what I need right now.

### Can there be more colors?

Yes -- my protocol kept it at 4 bits but you could change that. There's not too much variation in the colors and some of them flicker a lot, so I kept it safe. They are, in order (RED is 1, DKRED is 2, etc)

```c
// My lovely palette. 15 colors. RRRGGGBB
#define CRC     0xF0 // should never be
#define RED     0xE0
#define DKRED   0x40
#define ORANGE  0xE8
#define YELLOW  0xFC
#define GREEN   0x1C
#define DKGREEN 0x0c
#define TEAL    0x5E
#define LTBLUE  0x1F
#define BLUE    0x07
#define DKBLUE  0x02
#define PURPLE  0x46
#define PINK    0xE3
#define WHITE   0xFF
#define GRAY    0x4A
#define BLACK   0x00
``` 




