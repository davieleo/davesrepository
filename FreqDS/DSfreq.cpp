/** Simple GPIO LED flashing program - written in C++ by Derek Molloy
*    simple functional struture for the Exploring Raspberry Pi book
*
*    This program uses GPIO4 and can be executed in the following ways:
*         makeLED setup
*         makeLED on
*         makeLED off
*         makeLED status (get the LED state)
*         makeLED close
*/

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
   if(argc!=2){
      cout << "Usage is makeLED and one of: " << endl;
      cout << "   setup, on, off, status, or close" << endl;
      cout << " e.g. makeLED on" << endl;
      return 2;
   }
   string cmd(argv[1]);
   cout << "Starting the Freq Gen program" << endl;
   
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

   if (cmd=="1Hz"){
  
      cout << "Setting SQW to 1Hz." << endl;
      temp = buf[14]&DecTobcd(3); //save the alarm bits status in temp
      buf[14] = buf[14]>>5; //clear the first 5 bits to make sure the if INTCN is 0 if it was a 1
                            //and RS1 and RS2 are cleared also
      buf[14] = buf[14]<<5; //Move right 2 so RS2 adn RS1 are bit0 and bit1
      buf[14] = buf[14]|temp; //Alarm bits set back
   }


   else if (cmd=="2kHz"){
  
      cout << "Setting SQW to 2Hz." << endl;
      temp = buf[14]&DecTobcd(3); //save the alarm bits status in temp
      buf[14] = buf[14]>>5; //clear the first 5 bits to make sure the if INTCN is 0 if it was a 1
                            //and RS1 and RS2 are cleared also
      buf[14] = buf[14]<<5; //Move right 2 so RS2 adn RS1 are bit0 and bit1
      buf[14] = buf[14]|DecTobcd(8); //set RS2 "0" and RS1 "1" for 1.024kHz
      buf[14] = buf[14]|temp; //Alarm bits set back
   }
   else if (cmd=="4kHz"){
  
      cout << "Setting SQW to 4kHz." << endl;
      temp = buf[14]&DecTobcd(3); //save the alarm bits status in temp
      buf[14] = buf[14]>>5; //clear the first 5 bits to make sure the if INTCN is 0 if it was a 1
                            //and RS1 and RS2 are cleared also
      buf[14] = buf[14]<<5; //Move right 2 so RS2 adn RS1 are bit0 and bit1
      buf[14] = buf[14]|DecTobcd(16); //set RS2 "1" and RS1 "0" for 1.024kHz
      buf[14] = buf[14]|temp; //Alarm bits set back
   }

   else if (cmd=="8kHz"){
  
      cout << "Setting SQW to 8kHz." << endl;
      temp = buf[14]&DecTobcd(3); //save the alarm bits status in temp
      buf[14] = buf[14]>>5; //clear the first 5 bits to make sure the if INTCN is 0 if it was a 1
                            //and RS1 and RS2 are cleared also
      buf[14] = buf[14]<<5; //Move right 2 so RS2 adn RS1 are bit0 and bit1
      buf[14] = buf[14]|0x18; //set RS2 "1" and RS1 "1" for 1.024kHz
      buf[14] = buf[14]|temp; //Alarm bits set back
   }
   else{
      cout << "Invalid command." << endl;
   }
   
   buf[0] = DecTobcd(1);
   if(write(file, buf, BUFFER_SIZE)!=BUFFER_SIZE){
      perror("Failed to write to the register\n");
      return 1;
   }

   close(file);
   cout << "Finished the makeLED Program" << endl;
   return 0;

}
