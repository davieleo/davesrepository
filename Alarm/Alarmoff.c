#include<stdio.h>
#include<iostream>
#include<fcntl.h>
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
   
   buf[14] = buf[14]^DecTobcd(4); // Alarm1 and 2 set back to 0 from a both on state. turn off alarm 1 only use 
                                  // ^(XOR) DecTobcd(1), turn off alarm 2 only use (2), turn off both and not have
                                  //square wave on INTCN, (3)
   buf[15] = buf[15]&DecTobcd(0); //Set A1F and A2F to 0

   if(write(file, buf, BUFFER_SIZE)!=BUFFER_SIZE){
      perror("Alarm1 and Alarm2 off\n");
      return 1;
   } 
   else{
   cout << "Alarm has been turned off.\n" << endl;
   }
   close(file);
   return 0;
}
