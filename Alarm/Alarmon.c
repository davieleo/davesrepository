#include<stdio.h>
#include<fcntl.h>
#include<iostream>
#include<unistd.h>  //needed this as compile giving implicit delcaration errors without
#include<wiringPiI2C.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#define BUFFER_SIZE 19      //0x00 to 0x12
using namespace std;

// the time is in the registers in encoded decimal form
int bcdToDec(char b) { return (b/16)*10 + (b%16); }
char DecTobcd(char d) {return ((d/10*16) + (d%10)); }

int main(){
   int file;
   printf("Starting the DS3231 test application\n");
   if((file=open("/dev/i2c-1", O_RDWR)) < 0){
      perror("failed to open the bus\n");
      return 1;
   }
   if(ioctl(file, I2C_SLAVE, 0x68) < 0){
      perror("Failed to connect to the sensor\n");
      return 1;
   }
   char writeBuffer[1] = {0x00};
   if(write(file, writeBuffer, 1)!=1){
      perror("Failed to reset the read address\n");
      return 1;
   }
   char buf[BUFFER_SIZE];
   if(read(file, buf, BUFFER_SIZE)!=BUFFER_SIZE){
      perror("Failed to read in the buffer\n");
      return 1;
   }
   
   buf[0] = DecTobcd(1);
   
   buf[14] = buf[14]|DecTobcd(5); // Alarm1 set. INTCN and A1IE set to 1
   buf[14] = buf[14]|DecTobcd(6); // Alarm2 set. INTCN and A2IE set to 1

   if(write(file, buf, BUFFER_SIZE)!=BUFFER_SIZE){
      perror("Error turning the alarm on\n");
      return 1;
   }
   else{
   cout << "Alarm has been turned on.\n";
   }
   close(file);
   return 0;
}
