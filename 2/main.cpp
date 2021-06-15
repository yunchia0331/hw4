#include"mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);

BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx

BBCar car(pin5, pin6, servo_ticker);
Thread t;

// RPC control

void linedet(Arguments *in, Reply *out);
RPCFunction Linedet(&linedet, "linedet");

void linedet(Arguments *in, Reply *out){
   printf("YA");
   int x1 = in->getArg<double>();
   int y1 = in->getArg<double>();
   int x2 = in->getArg<double>();
   int y2 = in->getArg<double>();

   double x = x1-x2;
   double y = y1-y2;
   double m = y/x;
   printf("x=%f y= %f m= %f", x, y, m);
   if(m<0){
      car.turn(30, -0.8);
      printf("leftturn");
      ThisThread::sleep_for(5s);
   }else if(m>0){
      car.turn(30, 0.8);
      printf("rt");
      ThisThread::sleep_for(5s);
   }else if(m=0){
      car.goStraight(30);
      ThisThread::sleep_for(5s);
   }
   
}

int main(){
   uart.set_baud(9600);
   printf("HI");
   char buf[256], outbuf[256];
   FILE *devin = fdopen(&uart, "r");
   FILE *devout = fdopen(&uart, "w");
   while (1) {
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         char recv = fgetc(devin);
         if(recv == '\n') {
            printf("\r\n");
            break;
         }
         buf[i] = fputc(recv, devout);
      }
      RPC::call(buf, outbuf);
      printf("%s\r\n", outbuf);
   }
}