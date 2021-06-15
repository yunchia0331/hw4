#include "mbed.h"
#include "mbed_rpc.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);

BufferedSerial xbee(D1, D0);
BufferedSerial pc(USBTX, USBRX);

void Control(Arguments *in, Reply *out);
RPCFunction rpcControl(&Control, "Control");

int main() {

   pc.set_baud(9600);

   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
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
   }
}
// Make sure the method takes in Arguments and Reply objects.
void Control (Arguments *in, Reply *out)   {
printf("IN");
      // Have code here to call another RPC function to wake up specific led or close it.
//      char buffer[200], outbuf[256];
//      char strings[20];
   // In this scenario, when using RPC delimit the two arguments with a space.
   int d1 = in->getArg<int>();
   int d2 = in->getArg<int>();
   char head = in->getArg<char>();

      // int led = x;
      // int on = y;   
   // please contruct you own calibration table with each servo
   double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table0[] = {-10.445, -9.812, -9.647, -9.408, -5.900, 0.000, 5.900, 10.843, 11.880, 11.401, 12.199};
   double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table1[] = {-10.445, -9.812, -9.647, -9.408, -5.900, 0.000, 5.900, 10.843, 11.880, 11.401, 12.199};

   // first and fourth argument : length of table
   car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);
   float t1= (d1/30.0)*1000;
   float t2=(d2/5.0)*1000;
// go back d2cm
   if(head =='W'){
      printf("W");
      car.goStraight(-10);
      ThisThread::sleep_for(5s);

      car.goStraightCalib(-5);
      ThisThread::sleep_for(t2);
      car.stop();
      ThisThread::sleep_for(1s);      

      car.turn(-50, -1);
      ThisThread::sleep_for(2s);
      car.stop();
      ThisThread::sleep_for(1s);

      car.goStraightCalib(-30);
      ThisThread::sleep_for(t1);
      car.stop();
     
   } else if(head =='E'){
      printf("E");
      car.goStraightCalib(-30);
      ThisThread::sleep_for(t2);
      car.stop();
      ThisThread::sleep_for(1s);      

      car.turn(-50, 1);
      ThisThread::sleep_for(2s);
      car.stop();
      ThisThread::sleep_for(1s);

      car.goStraightCalib(-30);
      ThisThread::sleep_for(t1);
      car.stop();
   }

}