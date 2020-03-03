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
   

   
   buf[0] = DecTobcd(1); //initial write
   
   //seconds set
   buf[7] = 0x00; //0x07 register
   //bitwise or |
   int seconda = 33; //set alarm 1 seconds
   int asec = seconda/10; //seconds div 10 for bit 4 to 6
   int bsec = seconda%10; //seconds remainder for bit 0 to 3
   buf[7] = buf[7] | DecTobcd(asec); //set bit 4 to 6 at 0 to 3
   buf[7] = (buf[7]<<4) | DecTobcd(bsec); //shift left 4 bits and set the bsec 0 to 3 bits
   buf[7] = buf[7] | 0x80; //A1M1 setting high to alarm once per second

   //minutes set
   buf[8] = 0x00; //0x08 register
   int mina = 55; //set mins for alarm 1
   buf[8] = buf[8] | DecTobcd(mina/10); //set bit 4 to 6 at 0 to 3
   buf[8] = (buf[8]<<4) | DecTobcd(mina%10); //shift left 4 bits and set the bsec 0 to 3 bits
   buf[8] = buf[8] | 0x80; //A1M2 setting high to alarm once per second 

   //hour set
   buf[9] = 0x00; //0x09 register
   int houra = 12; //hour set for alarm 1
   buf[9] = buf[9] |DecTobcd(houra/10); //done without using variable
   buf[9] = buf[9]<<4 | DecTobcd(houra%10);
   buf[9] = buf[9] | 0x80; //A1M3 setting high to alarm once per second
                           //All A1M* bits high alarms every second, 2,3,4 high for each minute when the seconds match
                           //3,4 high for once an hour when minutes and seconds match, just 4 high for once a day when hours, mins and seconds match
   buf[9] = buf[9] | 0x00; //setting 12/24 low to set alarm to 24 hour format, change to 0x40 if 
                           //12 hour is desired
   buf[9] = buf[9] | 0x00; //This bit low as 24 hour format is selected above and this is the 10s for 24 hour format
                           //when 12/24 is changed above set this to 0x00 for AM or 0x20 for PM

   //day set
   buf[10] = 0x00; //0x0A register
   int daya = 6; //day set for alarm 1 1-7 for day of week, or the day of the month
   buf[10] = buf[10] | DecTobcd(daya/10);
   buf[10] = buf[10]<<4 | DecTobcd(daya%10);
   buf[10] = buf[10] | 0x80; //A1M4 setting high to alarm once per second. see 0x09 register for setting this bit
   buf[10] = buf[10] | 0x40; //setting DY/DT high to set day of the week, change to 0x00 if 
                             //day of the month is desiredAll A1M* bits need to be low to alarm 
                             //on the date (day of the month). if high it will alarm on the day of the week
                             //when day, hours, and minutes match.
 
   //minutes set alarm 2
   buf[11] = 0x00;   //0x0B register
   int minb = 9; //minute set for alarm 2
   buf[11] = buf[11] | DecTobcd(minb/10);
   buf[11] = buf[11]<<4 | DecTobcd(minb%10);
   buf[11] = buf[11] | 0x00; //A2M2 setting low to alarm once per hour when mins match, set to 0x80 to set high
                           //All A2M* bits high alarms every minute when seconds are 00, 3,4 high for each hour when the minutes match
                           //just 4 high for once a day when hours and mins match. 
   
   buf[12] = 0x00; //0x0C register
   int hourb = 15; //hour set for alarm 2
   buf[12] = buf[12] | DecTobcd(hourb/10);
   buf[12] = buf[12]<<4 | DecTobcd(hourb%10);
   buf[12] = buf[12] | 0x80; //A2M3 setting high to alarm once per hour when mins match. 
   buf[12] = buf[12] | 0x00; //setting 12/24 low to set alarm to 24 hour format, change to 0x40 if 
                             //12 hour is desired
   buf[12] = buf[12] | 0x00; //This bit low as 24 hour format is selected above and this is the 10s for 24 hour format
                             //when 12/24 is changed above set this to 0x00 for AM or 0x20 for PM


   buf[13] = 0x00;  //0x0D register
   int dayb = 23; //day set for alarm 2
   buf[13] = buf[13] | DecTobcd(dayb/10);
   buf[13] = buf[13]<<4 | DecTobcd(dayb%10);
   buf[13] = buf[13] | 0x80; //A2M4 setting high to alarm once per hour when minutes match
   buf[13] = buf[13] | 0x00; //setting DY/DT low to set day of the month, change to 0x40 if 
                             //day of the week is desired. All A1M* bits need to be low to alarm 
                             //on the date (day of the month). if high it will alarm on the day of the week
                             //when day, hours, and minutes match.


   if(write(file, buf, BUFFER_SIZE)!=1){
      perror("Writing to the alarm registers\n");
      return 1;
   }
   close(file);
   return 0;
}
