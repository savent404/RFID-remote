all:
	gcc -g src/main.c -lwiringPi -lpthread
clean:
	rm a.out
