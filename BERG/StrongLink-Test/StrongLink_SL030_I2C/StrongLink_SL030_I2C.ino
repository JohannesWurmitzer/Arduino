
#include <Wire.h>       // I2C Library


#define SL032_WAKEUP_DO 22  // Portpin für Wakeup

#define SL030ADR 0xA0   // standard address for SL030

void SL032_SendCom(unsigned char *g_cCommand);                                           

//Command List, preamble + length + command code + data frame
unsigned char const SelectCard[3] =      {0xBA,0x02,0x01 };       
unsigned char const LoginSector0[11] =   {0xBA,0x0A,0x02,0x00,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  
unsigned char const LoginSector1[11] =   {0xBA,0x0A,0x02,0x01,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char const ReadBlock1[4]   =    {0xBA,0x03,0x03,0x01};                                     
unsigned char const WriteBlock1[20] =    {0xBA,0x13,0x04,0x01,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};                                
unsigned char const ReadValue[4]  =      {0xBA,0x03,0x05,0x04};  
unsigned char const WriteValue[8]  =     {0xBA,0x07,0x06,0x04,0x00,0x00,0x00,0x01};                                
unsigned char const IncrementValue[8] =  {0xBA,0x07,0x08,0x04,0x00,0x00,0x00,0x20};                                
unsigned char const DecrementValue[8] =  {0xBA,0x07,0x09,0x04,0x03,0x00,0x00,0x00};                                          
unsigned char const CopyValue[5] =       {0xBA,0x04,0x0A,0x04,0x05};
unsigned char const ReadULPage5[4] =     {0xBA,0x03,0x10,0x05};                                          
unsigned char const WriteULPage5[8] =    {0xBA,0x07,0x11,0x05,0x11,0x22,0x33,0x44}; 
unsigned char const RATSDesFire[3] =     {0xBA,0x02,0x20};
unsigned char const DesFireGetVersion[4]={0xBA,0x03,0x21,0x60}; 
unsigned char const GetAdditionalData[4]={0xBA,0x03,0x21,0xAF}; 
unsigned char const LightOn[4] =         {0xBA,0x03,0x40,0x01}; 
unsigned char const Halt[4] =            {0xBA,0x03,0x50,0x00};             


/*Command*/
unsigned char const ComSelectCard[3]    = {1,1};                                               
unsigned char const ComLoginSector0[10] = {9,2,0+2,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char const ComReadBlock1[3]    = {2,3,1+8};       
unsigned char const ComWriteBlock1[19]  = {18,4,1+8,
                                          0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
                                          0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};                 
unsigned char const ComIntiPurse1[7]    = {6,6,1+8,0x78,0x56,0x34,0x12};                               
unsigned char const ComReadPurse1[3]    = {2,5,1+8};       
unsigned char const ComIncrPurse1[7]    = {6,8,1+8,0x02,0x00,0x00,0x00};                                
unsigned char const ComDecrPurse1[7]    = {6,9,1+8,0x01,0x00,0x00,0x00};                                
unsigned char const ComCopyValue[4]     = {3,0x0A,1+8,2+8};
unsigned char const ComReadUltralightPage5[3]  = {2,0x10,0x05};
unsigned char const ComWriteUltralightPage5[7] = {6,0x11,0x05,0x12,0x34,0x56,0x78};
unsigned char const ComHalt[2]     = {1,0x50};


unsigned char g_ucReceBuf[70];                                      
unsigned char g_ucReceNum;                                          
unsigned char g_ucWatchTime; 
unsigned char g_ucCardType;
char g_cStatus;                                       
char g_cErr;

// SL032 Boolean Variables
bool  g_bReceOver;                                                 
bool  g_bReceReady;
bool  g_bTimeOut;                                                  


unsigned int uiCounter;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();         // join i2c bus (address optional for master)
  
  Serial.println("Startup System");
  uiCounter = 0;

  Serial1.begin(115200);

  // SL032 Pin Configuration
  pinMode(SL032_WAKEUP_DO, OUTPUT);
  // SL032 Wakeup
  delay(1);   
  digitalWrite(SL032_WAKEUP_DO, LOW);
  delay(1);   
  digitalWrite(SL032_WAKEUP_DO, HIGH);

  g_ucReceNum = 0;
  g_bReceOver = false;
  g_bReceReady = false;
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Cycle Counter:");
  Serial.println(uiCounter++, DEC);
//  char c = 65;
//  Serial.print((char)65);

  // SL032 Wakeup
  delay(10);   
  digitalWrite(SL032_WAKEUP_DO, LOW);
  delay(10);   
  digitalWrite(SL032_WAKEUP_DO, HIGH);
  delay(100);   

//  SL032_SendCom(SelectCard);                              //
/*  Serial1.print(char(0xBA));
  Serial1.print(char(0x02));
  Serial1.print(char(0x01));
  Serial1.print(char(0xB9));
*/
  // Select Mifare card  
  Wire.beginTransmission(SL030ADR/2); // transmit to device #SL030ADR
  Wire.write(0x01);      // len
  Wire.write(0x01);      // cmd Select Mifare Card
  Wire.endTransmission();     // stop transmitting
  delay(40);


  Wire.requestFrom(SL030ADR/2, 1, false);    // request 1 byte from slave device #SL030ADR, which is the len of the protocol
  while (Wire.available()==0);
  Serial.print("available: ");
  Serial.println(Wire.available());
  g_ucReceNum = Wire.read();
  Serial.print("Len: ");
  Serial.print(g_ucReceNum);
  Serial.print(": ");
  
  if (g_ucReceNum > 0){
      Wire.requestFrom(SL030ADR/2, ++g_ucReceNum, true);    // request len bytes from slave device #SL030ADR
      while (Wire.available()!=g_ucReceNum);
      while(Wire.available()){
        Serial.print(Wire.read(), HEX);
        Serial.print(" ");
      }
    
  }

  Serial.println(); 
  delay(360);
}

////////////////////////////////////////////////////////////
//Send command to SL031 via UART
////////////////////////////////////////////////////////////
void SL032_SendCom(unsigned char *g_cCommand)
{    
     unsigned char i,chkdata,sendleg;

//     ES = 0;
     sendleg = *(g_cCommand+1) + 1;
     
     chkdata = 0;
     for(i=0; i<sendleg; i++)
     {    
         chkdata ^= *(g_cCommand+i);
        
//        TI = 0;
//        SBUF = *(g_cCommand+i);
//        while(!TI);
      Serial1.print((char)*(g_cCommand+i));
      Serial.println(*(g_cCommand+i), DEC);
     }
     
//     TI = 0;
//     SBUF = chkdata;
//     while (!TI);
//     TI = 0;
      Serial1.print((char)chkdata);
      Serial.println(chkdata, DEC);
     
     g_bReceReady = true;
     g_bReceOver = false;
//     ES = 1;
}



