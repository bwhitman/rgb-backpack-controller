#include <stdio.h>
#include <avr/io.h>

#define CS  2
#define MOSI    3
#define MISO    4
#define SCK     5
#define RX_IN   0
#define TX_OUT  1

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

void ioinit(void);      
uint8_t uart_getchar(void);

// My lovely palette. 15 colors. RRRGGGBB
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
#define CRC     0xF0 // should never be

// CRC first so that you can't just send 0 as a valid frame
// Also allows anything 0x or x0 as reserved chars
// so far we have two reserved chars -- 0 (don't do anything) and 0x20 (start at beginning)
unsigned char colors[] = {
    CRC, RED, DKRED, ORANGE, 
    YELLOW, GREEN, DKGREEN, TEAL, 
    LTBLUE, BLUE, DKBLUE, PURPLE, 
    PINK, WHITE, GRAY, BLACK 
};

char frame_data[64];    

int main (void) {   
    uint8_t c, pos = 0;
    ioinit(); 

    while(1){
        c = uart_getchar();
        if (c!=0) {
            if (c == 32) {
                // start of frame, reset pos to 0
                pos = 0;
            } else {
                // a byte gives us two dots
                frame_data[pos++] = colors[c >> 4];
                frame_data[pos++] = colors[c & 0x0f];
                if (pos > 63) pos = 0;          
            }
        }
        // Now write to the LED backpack over SPI.
        cbi(PORTB, CS); 
        for(int LED=0; LED<64; LED++) { 
            SPDR = frame_data[LED];
            while(!(SPSR & 1<<SPIF));
        }
        sbi(PORTB, CS);     //Setting the CS line high will terminate the communication sequence.
    }
    return 0;
}


void ioinit(void) {
    //Setup the AVR I/O
    DDRB = (1<<CS)|(1<<MOSI)|(1<<SCK);
    DDRD = (1<<TX_OUT);

    //SPI Bus setup
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);    //Enable SPI HW, Master Mode, divide clock by 16

    // Set up the UART for 9600
    #define BAUD 9600
    #include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    UCSR0A &= ~(1 << U2X0);
}

uint8_t uart_getchar(void) {
    // Try to get a byte from the uart. Return 0 if there is none.
    if(UCSR0A & (1<<RXC0))  return(UDR0);
    else return 0;
}
