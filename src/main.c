#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

#define EXTRA_FIFO_NAME "/tmp/sig_fifo"
#define OUTPUT_FILE_PATH "/home/pi/Documents/remoteID"

int output2file(char* device_data);

static char buffer[20];
static char *pt = buffer;
static pthread_t thread1, thread2;
static int fd;
static int sig = 0;
static void *serial_get(void *);
static void *sig_get(void *);
static int res = 0;

int main(void) {
    fd = serialOpen("/dev/ttyAMA0", 9600);

    if (fd < 0) {
        //Output Error To file
        system("echo \"Open serial error\" >> /home/pi/Documents/SerialLog");
        system("echo date >> /home/pi/Documents/SerialLog");
        exit(-1);
    }

    /* Create a FIFO file buffer input type
     */
    if (access(EXTRA_FIFO_NAME, F_OK) == -1) {
        res = mkfifo(EXTRA_FIFO_NAME, 0720);
        if (res != 0) {
            system("echo \"Create FIFO error\" >> /home/pi/Documents/SerialLog");
            system("echo date >> /home/pi/Documents/SerialLog");
            exit(-1);
        }
    }

    if (pthread_create(&thread1, NULL, serial_get, NULL) < 0) {
        system("echo \"Create Thread serial_get Error\" >> /home/pi/Documents/SerialLog");
        system("echo date >> /home/pi/Documents/SerialLog");
        exit(-2);
    }
    if (pthread_create(&thread2, NULL, sig_get, NULL) < 0) {
        system("echo \"Create Thread sig_get Error\" >> /home/pi/Documents/SerialLog");
        system("echo date >> /home/pi/Documents/SerialLog");
        exit(-3);
    }
    while (1) {
        sleep(1);
    }
    exit(0);


}
static void *serial_get(void *arg) {
    while (1) {

        if (serialDataAvail(fd) <= 0)
        continue;

        //get char
        *pt = serialGetchar(fd);

        if (!sig)
        continue;

        //output to file
        if (*pt == 0x0A && *(pt - 1) == 0x0D) {
            //cut string:"\r\n"
            *(pt-3) = '\0';
            output2file(pt - 17);
            printf("L:%s\n", pt - 17);
            memset(buffer, 0, sizeof(buffer));
            pt = buffer;
            continue;
        }
        pt += 1;
    }
}
static void *sig_get(void *arg) {
    char buf[50];
    int in = open(EXTRA_FIFO_NAME, O_RDONLY);
    if (in < 0) {
        system("echo \"Open FIFO error\" >> /home/pi/Documents/SerialLog");
        system("echo date >> /home/pi/Documents/SerialLog");
        exit (-4);
    }
    while (1) {
        if (read(in, buf, 50) > 0) {
            printf("Get sig\n");
            sig = 1;
            sleep(10);
            printf("Out sig\n");
            sig = 0;
        }
    }
}
int output2file(char * pt) {
    int cnt = 10;
    time_t t;
    char buf[100] = "";
    int out = open(OUTPUT_FILE_PATH, O_WRONLY|O_CREAT|O_APPEND);
    if (out < 0){
        system("echo \"Open output File Error\" >> /home/pi/Documents/SerialLog");
        system("echo date >> /home/pi/Documents/SerialLog");
        return -1;
    }
    if (*pt++ != 0x30||*pt++ != 0x32) {
        system("echo \"Get Serial Para Error\" >> /home/pi/Documents/SerialLog");
        system("echo date >> /home/pi/Documents/SerialLog");
        return -2;
    }

    // check ok, then write to file
    pt = pt + 2;
    time(&t);
    strcpy(buf, pt);
    strcat(buf, "\tLogin at\t");
    strcat(buf, asctime(localtime(&t)));
    write(out, buf, strlen(buf));
    close(out);
    return 0;
}
