# hw4
## 1
### XBEE傳RPC指令(含車子位置參數)，使車子停入格子
* main
> 吃 XBEE 傳進的RPC指令
```C++
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
```
* 進到 RPC function void Control (Arguments *in, Reply *out)   {
> 將數值存起
```C++
   int d1 = in->getArg<int>();
   int d2 = in->getArg<int>();
   char head = in->getArg<char>();
```
> contruct you own calibration table with each servo
```C++
   double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table0[] = {-10.445, -9.812, -9.647, -9.408, -5.900, 0.000, 5.900, 10.843, 11.880, 11.401, 12.199};
   double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table1[] = {-10.445, -9.812, -9.647, -9.408, -5.900, 0.000, 5.900, 10.843, 11.880, 11.401, 12.199};

   // first and fourth argument : length of table
   car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);
```
> t計算以此轉速要走d所需秒數(距離除時間)
```C++
   float t1= (d1/30.0)*1000;
   float t2=(d2/5.0)*1000;
```
> 若面向西方，先後退，朝後左轉彎，再後退
```C++
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
  ```
  > 若面向東方，先後退，朝後右轉彎，再後退
  ```C++
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
```
* car_control.py
> 輸RPC指令
```py
s.write("/Control/run 5 3 W \n".encode())
```
## 2
### 用openmv測直線，使車子沿直線走
* RPC function detect line 
```C++
void linedet(Arguments *in, Reply *out){
```
> 存openmv傳的數值
```C++
   int x1 = in->getArg<double>();
   int y1 = in->getArg<double>();
   int x2 = in->getArg<double>();
   int y2 = in->getArg<double>();
```
> 計算斜率
```C++
   double x = x1-x2;
   double y = y1-y2;
   double m = y/x;
```
> 沿著斜率走
```C++
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
```
*  int main(){
> 讀openmv傳的值，(會讀到RPC指令，和4參數)
```C++
   uart.set_baud(9600);
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
```
## 3
### 用PING測距離，用OPENMV測角度，使車子朝AprilTag 90度前進
* int main(){

```C++
    uart.set_baud(9600);
    parallax_ping  ping1(pin10);
 ```
 > 若距離AprilTag > 35公分，繼續朝AprilTag走
  ```C++ 
        while(1) {
            if((float)ping1>35) {
                printf("ping>35");
                led3 = 1;
   ```        
   > 讀openmv測的角度
```C++
                if(uart.readable()){
                    char recv[1];
                    uart.read(recv, sizeof(recv));
                    pc.write(recv, sizeof(recv));
                    printf("rot= %c\n",recv[0]);
```
> '3'表車子在AprilTag右方，'1'表車子在AprilTag左方，'2'表車子在AprilTag前方
```C++              
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
 ```
 > 若距離AprilTag<35公分即停下
  ```C++
            } else {
                printf("ping<30");
                led3 = 0;
                car.stop();
                break;
            }
            ThisThread::sleep_for(100ms);
        }
}
 ```
