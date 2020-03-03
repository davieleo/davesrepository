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
   
   int secs = 45;
   buf[7] = DecTobcd(secs/10);
   buf[7] = buf[7]<<4|DecTobcd(secs%10);
   buf[8] = DecTobcd(3);
   buf[9] = DecTobcd(1);
   buf[10] = DecTobcd(2);
   buf[11] = DecTobcd(3);

   if(write(file, buf, BUFFER_SIZE)!=1){
      perror("Writing date and time\n");
      return 1;
   }
   close(file);
   return 0;
}
