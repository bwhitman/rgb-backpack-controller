
#include <stdio.h>    
#include <fcntl.h>    
#include <termios.h> 

// space between bytes. on my box was not stable at 500us
#define SLEEP_US 1000

int serialport_init(const char* serialport, speed_t brate);
void draw_frame(int fd, char*frame);

int main(int argc, char *argv[]) {

    // Obviously set up for your environment. 
    int fd = serialport_init("/dev/ttyO1", B9600);

    // 64 entries, each one a palette index.
    char frame[] = {
        1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,
        9 ,10,11,12,13,14,15,15,
        1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,
        9 ,10,11,12,13,14,15,15,
        1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,
        9 ,10,11,12,13,14,15,15,
        1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,
        9 ,10,11,12,13,14,15,15,
    };
    draw_frame(fd, frame);
}

// Take a 64-char long array of colors and convert into a buf for the LED board & write it to fd
void draw_frame(int fd, char*frame) {
    int i=0;
    char buf = 32;
    // Write the start frame
    write(fd, &buf, 1);
    usleep(SLEEP_US);
    // Now write the colors
    for(i=0;i<64;i=i+2) {
        buf = (frame[i] << 4) | frame[i+1];
        write(fd, &buf, 1);
        usleep(SLEEP_US);
    }
}


int serialport_init(const char* serialport, speed_t brate) {
    struct termios toptions;
    int fd;
    fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)  {
        perror("init_serialport: Unable to open port ");
        return -1;
    }
    
    if (tcgetattr(fd, &toptions) < 0) {
        perror("init_serialport: Couldn't get term attributes");
        return -1;
    }
    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;
    
    if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
        perror("init_serialport: Couldn't set term attributes");
        return -1;
    }

    return fd;
}