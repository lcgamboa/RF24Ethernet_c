CC=/home/gamboa/microcontroladores/arduino-1.6.12/hardware/tools/avr/bin/avr-gcc

INCLUDE= -I/home/gamboa/microcontroladores/arduino-1.6.12/hardware/arduino/avr/cores/arduino
INCLUDE+= -I/home/gamboa/Desktop/microcontroladores/arduino-1.6.12/hardware/arduino/avr/variants/standard/
INCLUDE+= -I../RF24_c -I../RF24Network_c -I../RF24Mesh_c -I.

FLAGS= -Os -DF_CPU=16000000UL -mmcu=atmega328p -Wall -DARDUINO

all:
	${CC} ${FLAGS} ${INCLUDE} -c  RF24Ethernet_c.c
	${CC} ${FLAGS} ${INCLUDE} -c  RF24Client_c.c
	${CC} ${FLAGS} ${INCLUDE} -c  RF24Server_c.c  
	${CC} ${FLAGS} ${INCLUDE} -c  RF24Udp_c.c
	${CC} ${FLAGS} ${INCLUDE} -c  Dns_c.c

clean:
	rm -Rf *.o
