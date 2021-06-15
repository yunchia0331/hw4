#include"mbed.h"
#include "bbcar.h"
#include "cstdlib"

BufferedSerial pc(USBTX,USBRX);
BufferedSerial uart(D1,D0);

DigitalOut led3(LED3);
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalInOut pin10(D10);

BBCar car(pin5, pin6, servo_ticker);

int main(){
    uart.set_baud(9600);
    parallax_ping  ping1(pin10);
        while(1) {
            printf("IN");
            if((float)ping1>35) {
                printf("ping>35");
                led3 = 1;
                if(uart.readable()){
                    char recv[1];
                    uart.read(recv, sizeof(recv));
                    pc.write(recv, sizeof(recv));
                    printf("rot= %c\n",recv[0]);
                    if (recv[0]=='3') {
                        printf("rightt");
                        car.goStraight(20);
                        ThisThread::sleep_for(1s);
                        car.turn(20, 0.7);  // turn right
                        ThisThread::sleep_for(1s);
                    }else if (recv[0]=='1') {
                        printf("leftturn");
                        car.goStraight(20);
                        ThisThread::sleep_for(1s);
                        car.turn(20, -0.7);  // turn left
                        ThisThread::sleep_for(1s);
                    } else if(recv[0]=='2'){
                        printf("straight!!");
                        car.goStraight(30);
                        ThisThread::sleep_for(2s);
                    }

                }
            } else {
                printf("ping<30");
                led3 = 0;
                car.stop();
                break;
            }
            ThisThread::sleep_for(100ms);
        }
}