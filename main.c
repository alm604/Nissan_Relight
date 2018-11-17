#include "iostm8s103f3.h"
#define POWER_IN        PD_IDR_bit.IDR6
#define POWER_OUT       PD_ODR_bit.ODR5
#define LIGHT           PA_IDR_bit.IDR3
#define SPI_CLK         PC_ODR_bit.ODR7
#define SPI_DAT         PC_ODR_bit.ODR6
#define SPI_LATCH       PC_ODR_bit.ODR5
#define KEY             PB_IDR_bit.IDR5
#include "cpu.c"

/*
ch0-ch13 - led
ch14 - display


*/
unsigned char bright;
unsigned char s_timer;
unsigned char bright_day;
unsigned char bright_night;
unsigned int code;
const unsigned int demo1[] = {2, 6, 14, 76, 104, 112, 2098, 6166,        7182, 21580, 25704, 25200, 10802, 6678};//14-8
const unsigned int demo2[] = {2, 6, 12, 72, 96, 50, 2070, 6156,          5192, 17504, 24626, 10774, 6668};
const unsigned int demo3[] = {0, 2, 4, 8, 64, 32, 16, 2048, 4096, 1024, 16384, 8192, 512, 256, 258, 260, 264, 320, 288, 272, 2304, 4352, 1280, 16640, 8448, 768, 770, 772, 776, 832, 800, 784, 2816, 4864, 1792, 17152, 8960, 8962, 8964, 8968, 9024, 8992, 8976, 11008, 13056, 9984, 25344, 25346, 25348, 25352, 25408, 25376, 25360, 27392, 29440, 26368, 26370, 26372, 26376, 26432, 26400, 26384, 28416, 30464, 30466, 30468, 30472, 30528, 30496, 30480, 32512, 32514, 32516, 32520, 32576, 32544, 32528, 32530, 32532, 32536, 32592, 32560, 32562, 32564, 32568, 32624, 32626, 32628, 32632, 32634, 32636, 32638};
// 2, 6, 14, 76, 104, 2090, 6158, 7174, 28754, 25704, 25200, 10800, 
const unsigned int prog[] = {2, 4, 8, 64, 32, 16, 2048, 4096, 1024, 16384, 8192, 512, 256, 768, 8960, 25344, 26368, 30464, 32512, 32528, 32560, 32624, 32632, 32636, 32638, 65406}; 

const unsigned char loga[64]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,18,22,25,28,29,30,33,36,39,42,46,49,53,56,60,64,68,72,77,81,86,90,95,100,105,110,116,121,127,132,138,144,150,156,163,169,176,182,189,196,203,210,218,225,233,240,248,255};

void spi_pulse(void){
  SPI_CLK = 1;
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  SPI_CLK = 0;
}

void spi_send(unsigned int data){
  unsigned char i;
  for(i=0;i<16;i++){
    if(data & (1<<i)){
      SPI_DAT = 1;
    }else{
      SPI_DAT = 0;
    }
    spi_pulse();
  }
  SPI_LATCH = 1;
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  SPI_LATCH = 0;
}

void animate_on(void){
  unsigned char i;
  TIM2_CCR3H = loga[bright];
  TIM2_CCR3L = 0;
  TIM2_CCR2H = 0;
  TIM2_CCR2L = 0;
  for(i=0;i<26;i++){
    spi_send(prog[i]);
    delay(9000);
  }
  for(i=0;i<bright;i++){
    TIM2_CCR2H = loga[i];
    TIM2_CCR2L = 0;
    delay(9000);
  }
}

void animate_off(void){
  unsigned char i;
  for(i=0;i<bright;i++){
    TIM2_CCR2H = loga[63-i];
    TIM2_CCR2L = 0;
    delay(4000);
  }
  for(i=0;i<26;i++){
    spi_send(prog[25-i]);
    delay(9000);
  }
  TIM2_CCR3H = loga[bright];
  TIM2_CCR3L = 0;
  TIM2_CCR2H = 0;
  TIM2_CCR2L = 0;
}

void set_bright(void){
  
  unsigned char i;
  unsigned char dir = 0;
  for(i=0;i<3;i++){
    TIM2_CCR2H = 0;
    TIM2_CCR2L = 0;
    delay(20000);
    TIM2_CCR2H = 0x80;
    TIM2_CCR2L = 0;
    delay(20000);
  }
  
  if(LIGHT){
    bright = bright_night;
  }else{
    bright = bright_day;
  }
  TIM2_CCR2H = loga[bright];
  TIM2_CCR2L = 0;
  TIM2_CCR2H = loga[bright];
  TIM2_CCR2L = 0;
  while(KEY){
    if(!dir){
      if(bright){
        bright--;
      }else{
        dir = 1;
      }
    }else{
      if(bright < 63){
        bright++;
      }else{
        dir = 0;
      }
    }
    TIM2_CCR2H = loga[bright];
    TIM2_CCR2L = 0;
    TIM2_CCR3H = loga[bright];
    TIM2_CCR3L = 0;
    delay(10000);
  }
  EEPROM_Unlock();
  if(LIGHT){
    bright_night = bright;
    EEPROM_Program_Byte(0x4001, bright_night);
  }else{
    bright_day = bright;
    EEPROM_Program_Byte(0x4000, bright_day);
  }
  EEPROM_Lock();
}


void main(void){
  unsigned char i=0;
  unsigned int keypress = 0;
  cpu_init();
  asm("rim");
  bright_day = *((__far unsigned char*)(unsigned int)0x4000);
  bright_night = *((__far unsigned char*)(unsigned int)0x4001);
  if(LIGHT){
    bright=bright_night;
  }else{
    bright=bright_day;
  }
  s_timer = 0;
  animate_on();
  code =0;
  while(1){

    if(!POWER_IN){
      animate_off();
      POWER_OUT = 0;
    }
    if(KEY){
      delay(0xFFFF);
      while(KEY){
        keypress++;
        if(!keypress)set_bright();
      }
    }
    /*
    spi_send(demo3[i]);
    i++;
    if(i==91)i=0;
    delay(9000);
    //animate_on();
    */
    if(LIGHT){
      while(bright!=bright_night){
        if(bright>bright_night){
          bright--;
        }
        if(bright<bright_night){
          bright++;
        }
        TIM2_CCR2H = loga[bright];
        TIM2_CCR2L = 0;
        TIM2_CCR3H = loga[bright];
        TIM2_CCR3L = 0;
        delay(2000);
      }
    }else{
      while(bright!=bright_day){
        if(bright>bright_day){
          bright--;
        }
        if(bright<bright_day){
          bright++;
        }
        TIM2_CCR2H = loga[bright];
        TIM2_CCR2L = 0;
        TIM2_CCR3H = loga[bright];
        TIM2_CCR3L = 0;
        delay(2000);      
      }
    }
   /*
    TIM2_CCR2H = i;
    TIM2_CCR2L = 0;
    TIM2_CCR3H = 255-i;
    TIM2_CCR3L = 0;
    i++;
    spi_send(code);
    delay(3000);
    */
  }
}
