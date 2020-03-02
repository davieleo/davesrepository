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
   
 
   //seconds set
   buf[7] = 0x00; //start with clear register
   //bitwise or |
   int seconda = 33; //set alarm seconds
   int asec = seconda/10; //seconds div 10 for bit 4 to 6
   int bsec = seconda%10; //seconds remainder for bit 0 to 3
   buf[7] = buf[7] | DecTobcd(asec); //set bit 4 to 6 at 0 to 3
   buf[7] = (buf[7]<<4) | DecTobcd(bsec); //shift left 4 bits and set the bsec 0 to 3 bits
   
   //minutes set
   buf[8] = 0x00;
   int mina = 55; //set alarm seconds
   buf[8] = buf[8] | DecTobcd(mina/10); //set bit 4 to 6 at 0 to 3
   buf[8] = (buf[8]<<4) | DecTobcd(mina%10); //shift left 4 bits and set the bsec 0 to 3 bits
    
   //hour set
   buf[9] = 0x00;
   int houra = 12;
   buf[9] = buf[9] |DecTobcd(houra/10); //done without using variable
   buf[9] = buf[9]<<4 | DecTobcd(houra%10);

   //day set
   buf[10] = 0x00;
   int daya = 15;
   buf[10] = buf[10] | DecTobcd(daya/10);
   buf[10] = buf[10]<<4 | DecTobcd(daya%10);

   //minutes set alarm 2
   buf[11] = 0x00;
   int minb = 9;
   buf[11] = buf[11] | DecTobcd(minb/10);
   buf[11] = buf[11]<<4 | DecTobcd(minb%10);

   buf[12] = 0x00;
   int hourb = 15;
   buf[12] = buf[12] | DecTobcd(hourb/10);
   buf[12] = buf[12]<<4 | DecTobcd(hourb%10);

   buf[13] = 0x00;
   int dayb = 23;
   buf[13] = buf[13] | DecTobcd(dayb/10);
   buf[13] = buf[13]<<4 | DecTobcd(dayb%10);

   if(write(file, buf, BUFFER_SIZE)!=1){
      perror("Writing date and time\n");
      return 1;
   }
   close(file);
   return 0;
}
