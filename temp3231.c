#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>  //needed this as compile giving implicit delcaration errors without
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#define BUFFER_SIZE 19      //0x00 to 0x12

// the time is in the registers in encoded decimal form
int bcdToDec(char b) { return (b/16)*10 + (b%16); }

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
   int tempbuf = bcdToDec(buf[17]);
   if (buf[17]>0x7f){       //if the MSBit is high, it is a negative number. a number minus 2 times the number is the number minused
      tempbuf = bcdToDec(buf[17]) - (bcdToDec(buf[17]) + bcdToDec(buf[17]));
      
   }
 
   printf("The RTC Temperature is %02d.%02d\n",
           tempbuf, bcdToDec(buf[18]));
   close(file);
   return 0;
}