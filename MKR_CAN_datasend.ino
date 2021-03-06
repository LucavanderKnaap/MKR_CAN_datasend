#include <CAN.h>

//Script to emulate some data for all J1939 pgn's recorded on a liebherr crane. 
//We cycle through three different arrays (both id's and data), so we can quickly emulate data coming in at different rates.
//Level 1, 2 and 3 are outputted over the canbus line at different intervals (by modulo operation). 
//Modulo values have been adjusted so as to emulate the average canbus load in the crane (around 7-8%)



int messageCounter = 0;
int level1cursor = 0;
int level2cursor = 0;
int level3cursor = 0;


//Three arrays which contain all possible canbus ID's (j1939 pgn numbers)
unsigned int messageIdsLev1[3] =  {0xCF00400, 0xCF00300,0xCFE6CEE};
unsigned int messageIdsLev2[6] = {0xCF00503, 0x18F0000F, 0x18F0002D, 0x18FEF003, 
                                   0x18FEF200, 0x18FEF100};
unsigned int messageIdsLev3[17] = {0x18FEEC00, 0x18FDD100, 0x18FE6900, 0x18FE5600, 
                                   0x18FEEF00, 0x18F00617, 0x18FD0900, 0x18FEE6EE,
                                   0x18FEEE00, 0x18FEF500, 0x18FEC1EE, 0x18FEAE17,
                                   0x18FEFC17, 0x18FEE500, 0x18FFE017, 0x18FFED17,
                                   0x18FFEE17};


//Three arrays that contain some data.
const uint8_t messagesArray1Lev1[3][8] =    {{0x01,0x7D,0x7D,0x00,0x00,0x00,0x0C,0x7D}, {0xD1,0x00,0x00,0xFF,0xFF,0xFC,0x00,0xFB},
                                             {0x00,0xFF,0xFF,0xC4,0x00,0x00,0x00,0x00}};
                                             
const uint8_t messagesArray1Lev2[6][8] =   {{0x7D,0xFF,0xFF,0x7D,0x7D,0xFF,0x4E,0xFC}, {0xF0,0x7D,0x7D,0xFC,0x10,0xFF,0xFF,0x7D},
                                             {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 
                                             {0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0x04,0x00,0x00,0xC0,0xFF,0x55,0xFF,0xFF}};
                                             
const uint8_t messagesArray1Lev3[17][8] =   {{0x31,0x31,0x31,0x30,0x2D,0x35,0x2E,0x31}, {0x01,0x30,0x30,0x31,0x30,0x00,0x00,0x00},
                                             {0xFD,0x24,0xCC,0x24,0xFF,0xFF,0xFD,0x04}, {0xB1,0x34,0xFF,0xFF,0x00,0x00,0x00,0xFF},
                                             {0x08,0xFF,0x63,0x00,0x00,0xFF,0xFF,0xFA}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
                                             {0xFF,0xFF,0xFF,0xFF,0x20,0x9B,0x08,0x00}, {0xD4,0x34,0x0B,0x02,0x5E,0x25,0x7D,0x7E},
                                             {0x3D,0x3D,0x7C,0x26,0xFF,0xFF,0x3E,0xFF}, {0xC9,0xFF,0xFF,0x31,0x24,0xFF,0xFF,0xFF},
                                             {0xE0,0x69,0x01,0x00,0xE0,0x69,0x01,0x00}, {0x00,0x00,0x7D,0x7D,0x00,0x00,0x00,0x00},
                                             {0x00,0x7D,0x00,0x00,0x00,0x00,0x00,0x00}, {0xB2,0x01,0x00,0x00,0x00,0x00,0x00,0x00},
                                             {0x04,0x00,0x00,0x00,0x1F,0x00,0x2B,0x00}, {0x7F,0x7F,0x00,0x00,0x00,0x00,0x00,0x00},
                                             {0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF}};

/* 
//A few extra arrays with some different data. Could be used to switch between arrays 1 and arrays 2 to emulate variation in data. But not necessary.
const uint8_t messagesArray2Lev1[3][8] =    {{0x01,0x7D,0x7D,0x00,0x00,0x00,0x0C,0x7D}, {0xD1,0x00,0x00,0xFF,0xFF,0xFC,0x00,0xFB},
                                             {0x00,0xFF,0xFF,0xC4,0x00,0x00,0x00,0x00}};

const uint8_t messagesArray2Lev2[6][8] =   {{0x7D,0xFF,0xFF,0x7D,0x7D,0xFF,0x4E,0xFC}, {0xF0,0x7D,0x7D,0xFC,0x10,0xFF,0xFF,0x7D},
                                             {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
                                             {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

const uint8_t messagesArray2Lev3[17][8] =   {{0x31,0x31,0x31,0x30,0x2D,0x35,0x2E,0x31}, {0x01,0x30,0x30,0x31,0x30,0x00,0x00,0x00},
                                             {0x04,0x25,0xCC,0x24,0xFF,0xFF,0x04,0x25}, {0xB1,0x34,0xFF,0xFF,0x00,0x00,0x00,0xFF},
                                             {0x08,0xFF,0x67,0x00,0x00,0xFF,0xFF,0xFA}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
                                             {0xFF,0xFF,0xFF,0xFF,0x20,0x9B,0x08,0x00}, {0xE8,0x34,0x0B,0x02,0x5E,0x25,0x7D,0x7E}, 
                                             {0x3D,0x3D,0x9F,0x25,0xFF,0xFF,0x3F,0xFF}, {0xC9,0xFF,0xFF,0x33,0x24,0xFF,0xFF,0xFF}, 
                                             {0xE0,0x69,0x01,0x00,0xE0,0x69,0x01,0x00}, {0x00,0x00,0x7D,0x7D,0x00,0x00,0x00,0x00}, 
                                             {0x00,0x7D,0x00,0x00,0x00,0x00,0x00,0x00}, {0xB2,0x01,0x00,0x00,0x00,0x00,0x00,0x00},
                                             {0x04,0x00,0x00,0x00,0x1F,0x00,0x2B,0x00}, {0x7F,0x7F,0x00,0x00,0x00,0x00,0x00,0x00}, 
                                             {0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF}};
                                             
*/

void setup() {
  //Connection through serial usb bus with pc
  Serial.begin(115200);

  Serial.println("CAN Sender");

  // start the CAN bus at 250 kbps
  if (!CAN.begin(250E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  Serial.println(messageCounter);
  
  //Message level 1
  CAN.beginExtendedPacket(messageIdsLev1[level1cursor], 8);
  CAN.write(messagesArray1Lev1[level1cursor], 8);
  CAN.endPacket();
  level1cursor++;


  //Message level 2
  if(messageCounter % 3 == 0){ //Only writes every three loops
    delay(4);
    CAN.beginExtendedPacket(messageIdsLev2[level2cursor], 8);
    CAN.write(messagesArray1Lev2[level2cursor], 8);
    CAN.endPacket();
    level2cursor++;
  }
  
  //Message level 3
  if(messageCounter % 12 == 0){ //only writes every twelve loops
    delay(4);
    CAN.beginExtendedPacket(messageIdsLev3[level3cursor], 8);
    CAN.write(messagesArray1Lev3[level3cursor], 8);
    CAN.endPacket();
    level3cursor++;
  }

  //Up the counter
  messageCounter++;

  //Reset cursors over array values
  if (level1cursor > 2){
    level1cursor = 0;
  }
  if (level2cursor > 5){
    level2cursor = 0;
  }
  if (level3cursor > 16){
    level3cursor = 0;
  }
  
  
  //set delay (with a bit of variation)
  if(messageCounter % 12 == 0){
    delay(35);
  }
  else{
    delay(5);
  }
  
  //delay(1000);
}
