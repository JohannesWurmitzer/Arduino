#define SL032_WAKEUP_DO 23  // Portpin für Wakeup

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

  SL032_SendCom(SelectCard);                              //
/*  Serial1.print(char(0xBA));
  Serial1.print(char(0x02));
  Serial1.print(char(0x01));
  Serial1.print(char(0xB9));
*/  
  delay(20);
  Serial.print("Serial 1 Counter:");
  Serial.println(Serial1.available());
  while (Serial1.available()){
    Serial.print(Serial1.read(), HEX);
    Serial.print(" ");
  }
  Serial.println(); 
  delay(360);
}

////////////////////////////////////////////////////////////
//Send command to SL031
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

