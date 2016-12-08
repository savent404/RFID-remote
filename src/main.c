#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
int output2file(char* device_data);
int main(void) {
    char buffer[20];
    char *pt = buffer;
    int fd = serialOpen("/dev/ttyAMA0", 9600);
    if (fd < 0) {
        fprintf(stderr, "Open serial error\n");
        return -1;
    }

    while (1) {
        if (serialDataAvail(fd) <= 0)
        continue;

        //get char
        *pt = serialGetchar(fd);

        //output to file
        if (*pt == 0x0A && *(pt - 1) == 0x0D) {
            //cut string:"\r\n"
            *(pt-3) = '\0';
            output2file(buffer);
            memset(buffer, 0, sizeof(buffer));
            pt = buffer;
            continue;
        }
        pt += 1;
    }
}

int output2file(char * pt) {
    int cnt = 10;
    time_t t;
    char buf[100] = "";
    int fd = open("/home/pi/Documents/remoteID", O_WRONLY|O_CREAT|O_APPEND);
    if (fd < 0)
        return -1;
    if (*pt++ != 0x30||*pt++ != 0x32)
        return -2;

    // check ok, then write to file
    pt = pt + 2;
    time(&t);
    strcpy(buf, pt);
    strcat(buf, "\tLogin at\t");
    strcat(buf, asctime(localtime(&t)));
    strcat(buf, "\n");
    write(fd, buf, strlen(buf));
    close(fd);
    return 0;
}
