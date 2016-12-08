# REMOTE RFID READER
using Serial-AMA0 get device ID
then output to a file.
# HOW TO USE
## Raspberry Config
        sudo raspi-config //enable Serial console
        sudo vi /boot/cmdline.txt // THEN remove "console=ttyAMA0,115200"
## make file
        make
## auto startup as Deamon
vi "sudo /home/pi/Documents/RFID-remote/a.out &" to `/etc/rc.local`
## File Output
path is `/home/pi/Documents/remoteID`