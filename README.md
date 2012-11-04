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
* With the avr-gcc toolchain somewhere, run make on this repo.
* You'll have a file called led_sender.hex, flash it anyway you know how. Since I put that firmware on there above, I do this quickly after hitting reset:

```bash
avrdude -V -F -c stk500v1 -p m328p -P /dev/ttyO1 -b 57600 -U flash:w:led_sender.hex 
```

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

## FAQ

### Why didn't you just use SPI, isn't it a waste to have 2 microcontrollers converting stuff back and forth

Have you tried to use SPI on a computer before, so annoying

### Can I increase the baud rate?

Yeah, probably

### Can there be more colors?

Yes -- my protocol kept it at 4 bits but you could change that. There's not too much variation in the colors and some of them flicker a lot, so I kept it safe.






