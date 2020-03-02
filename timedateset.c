#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>  //needed this as compile giving implicit delcaration errors without
#include<wiringPiI2C.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#define BUFFER_SIZE 19      //0x00 to 0x12

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
   buf[1] = DecTobcd(49); //set the mins to 7
   buf[2] = DecTobcd(17); //set the hours 19
   buf[3] = DecTobcd(7); //setting it to saturday
   buf[4] = DecTobcd(29); //setting the date to 29th
   buf[5] = DecTobcd(2); //setting the month to Feb
   buf[6] = DecTobcd(20); //setting the year to 2020

   if(write(file, buf, BUFFER_SIZE)!=1){
      perror("Writing date and time\n");
      printf("The Time is set to %d:%d:%d\n", bcdToDec(buf[2]), bcdToDec(buf[1]), bcdToDec(buf[0]));
      printf("The Date is set to %d:%d:%d\n", bcdToDec(buf[4]), bcdToDec(buf[5]), bcdToDec(buf[6]));
      return 1;
   }
   close(file);
   return 0;
}
