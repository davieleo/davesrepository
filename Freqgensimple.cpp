
#include<iostream>
#include<fstream>
#include<string>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<wiringPiI2C.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#define BUFFER_SIZE 19 

using namespace std;
char temp;
char buf[BUFFER_SIZE];
char DecTobcd(char d) {return ((d/10*16) + (d%10)); }


int main(int argc, char* argv[]){
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
   if(read(file, buf, BUFFER_SIZE)!=BUFFER_SIZE){
      perror("Failed to read in the buffer\n");
      return 1;
   }


  
      cout << "Setting SQW." << endl;
      temp = buf[14]&DecTobcd(3); //save the alarm bits status in temp
      buf[14] = buf[14]>>5; //clear the first 5 bits to make sure the if INTCN is 0 if it was a 1
                            //and RS1 and RS2 are cleared also
      buf[14] = buf[14]<<5; //Move right 2 so RS2 adn RS1 are bit0 and bit1
      buf[14] = buf[14]|DecTobcd(8); //set RS2 "0" and RS1 "1" for 1.024kHz
      buf[14] = buf[14]|temp; //Alarm bits set back
   
   buf[1] = DecTobcd(1);
   if(write(file, buf, BUFFER_SIZE)!=1){
      perror("Failed to write to the register\n");
      return 1;
   }

   close(file);
   cout << "Finished the makeLED Program" << endl;
   return 0;

}
