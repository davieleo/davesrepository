#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>  //needed this as compile giving implicit delcaration errors without
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#define BUFFER_SIZE 19 //0x00 to 0x13
// the time is in the registers in encoded decimal form
int bcdToDec(char b) { return (b/16)*10 + (b%16); }
int A1secs, A1mins, A1hours, A1day, A2mins, A2hours, A2day;
bool A1M1,A1M2,A1M3,A1M4,A2M1,A2M2,A2M3,A1DOW,A2DOW;
char bufAShift,bufDShift;


int main(){
   int file;
   printf("Starting the DS3231 Alarm application\n");
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
   if (buf[0x07]>0x7f){A1M1=true;}else{A1M1=false;} //the following lines are operations to check the 
   if (buf[0x08]>0x7f){A1M2=true;}else{A1M2=false;} //states of the Alarm registers and assign the 
   if (buf[0x09]>0x7f){A1M3=true;}else{A1M3=false;} //different bits to alarm bits that corrospond to
   if (buf[0x0A]>0x7f){A1M4=true;}else{A1M4=false;} //alarm identifiers on the data sheet. If the data in the buffer 
   if (buf[0x0B]>0x7f){A2M1=true;}else{A2M1=false;} //is greater than or equal to 128, bit 8 must be high.
   if (buf[0x0C]>0x7f){A2M2=true;}else{A2M2=false;} //these are the registers to say how often the alarm 
   if (buf[0x0D]>0xf7){A2M3=true;}else{A2M3=false;} //goes off
   bufAShift = buf[0x0A]<<1; //check the DY of alarm 1
   bufDShift = buf[0x0D]<<1; //check the DY of alarm 2
   if (bufAShift>0x7f){A1DOW=true;}else{A1DOW=false;} //Shift the register 1 bit to the left and if the data is greater
   if (bufDShift>0x7f){A2DOW=true;}else{A2DOW=false;} //than 128, bit 7 must be high. DOW false means its date of month
   A1secs=bcdToDec(buf[0x07] & 0x7F); //read in data from register 07 for seconds and and with 7F to get the for all secs
   A1mins=bcdToDec(buf[0x08] & 0x7F); //read in data from register 07 for seconds and and with 7F to get the for all mins
   A1hours=bcdToDec(buf[0x09] & 0x1F);//read in data from register 07 for seconds and and with 1F to get the for all hours
   A1day=bcdToDec(buf[0x0A] & 0x3F); //read in data from register 07 for seconds and and with 3F to get the for all days
   A2mins=bcdToDec(buf[0x0B] & 0x7F); //read in data from register 07 for seconds and and with 7F to get the for all mins
   A2hours=bcdToDec(buf[0x0C] & 0x1F);//read in data from register 07 for seconds and and with 7F to get the for all hours
   A2day=bcdToDec(buf[0x0D] & 0x3F); //read in data from register 07 for seconds and and with 7F to get the for all days
   
   if (A1DOW){              //2 if statements to output if alarm1 is set to day of week or month
   printf("The current alarm 1 is set for %02d second of %02d min of %02d hour of the %02d day of the week.\n",A1secs,
   A1mins, A1hours, A1day);
   }
   else{
   printf("The current alarm 1 is set for %02d second of %02d min of %02d hour of the %02d day of the month.\n",A1secs,
   A1mins, A1hours, A1day);
   }
    
   // Next set if if statements are to state if the alarm is going off every Sec, min, hour, day of week or month. 
   // Alarm mode is in the data sheet for DS3231
   if (A1M1 && A1M2 && A1M3 && A1M4){
   printf("Alarm 1 will go off once per second until it is cleared.\n\n");
   }
   else if (!A1M1 && A1M2 && A1M3 && A1M4){
   printf("Alarm 1 will go off once per minute until it is cleared.\n\n");
   }
   else if (!A1M1 && !A1M2 && A1M3 && A1M4){
   printf("Alarm 1 will go off once per hour until it is cleared.\n\n");
   }
   else if (!A1M1 && !A1M2 && !A1M3 && A1M4){
   printf("Alarm 1 will go off once per day until it is cleared.\n\n");
   }
   else if (!A1M1 && !A1M2 && !A1M3 && !A1M4 && !A1DOW){
   printf("Alarm 1 will go off once per month until it is cleared.\n\n");
   }
   else if (!A1M1 && !A1M2 && !A1M3 && !A1M4 && A1DOW){
   printf("Alarm 1 will go off once per week until it is cleared.\n\n");
   }
   else{
   perror("There is a problem with the Alarm 1 status register.\n");
   return 1;
   }
   
   // the next if else statement is to state if alarm 2 is set for day of the week or month
   if (A2DOW){
   printf("The current alarm 2 is set for %02d min of %02d hour of the %02d day of the week.\n",
   A2mins, A2hours, A2day);
   }
   else{
   printf("The current alarm 2 is set for %02d min of %02d hour of the %02d day of the month.\n",
   A2mins, A2hours, A2day);
   }

   //The next set of if statements are to state how often the alarm 2 goes off. Alarm mode is in the data sheet for DS3231
   if (A2M1 && A2M2 && A2M3){
   printf("Alarm 2 will go off once per minute until it is cleared.\n");
   }
   else if (!A2M1 && A2M2 && A2M3){
   printf("Alarm 2 will go off once per hour until it is cleared.\n");
   }
   else if (!A2M1 && !A2M2 && A2M3){
   printf("Alarm 2 will go off once per day until it is cleared.\n");
   }
   else if (!A2M1 && !A2M2 && !A2M3 && !A2DOW){
   printf("Alarm 2 will go off once per month until it is cleared.\n");
   }
   else if (!A2M1 && !A2M2 && !A2M3 && A2DOW){
   printf("Alarm 2 will go off once per week until it is cleared.\n");
   }
   else{
   perror("There is a problem with the Alarm 2 status register.\n");
   return 1;
   }
}