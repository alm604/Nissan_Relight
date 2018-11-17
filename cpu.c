#define OPT2  0x4803 
// 8 977 944 94 37
void EEPROM_Unlock( void ){
  FLASH_DUKR = 0xAE;
  FLASH_DUKR = 0x56;
}

void EEPROM_Lock( void){
 FLASH_IAPSR_bit.WR_PG_DIS = 0;
}

void EEPROM_Program_Byte(unsigned int Address, unsigned char Data){
  *((__far unsigned char*)(unsigned int)Address)       = *((unsigned char*)(&Data));
}

void cpu_init(void){

  
  
  EEPROM_Unlock();
  EEPROM_Program_Byte(0x4803, 0x02);
  EEPROM_Program_Byte(0x4804, 0xFD);
  EEPROM_Lock();
//FLASH_Unlock (FLASH_MEMTYPE_DATA); 
//FLASH_ProgramOptionByte(OPT2, 0x02);   
//FLASH_ProgramOptionByte(OPT2+1, 0xFD);
//FLASH_Lock (FLASH_MEMTYPE_DATA);  

  CLK_CKDIVR_CPUDIV = 0;

  CLK_CKDIVR_HSIDIV = 0;
  //power out
  PD_DDR_bit.DDR5 = 1;
  PD_CR1_bit.C15 = 1;
  PD_ODR_bit.ODR5 = 1;
  // SPI
  PC_DDR = 0xE0;
  PC_CR1 = 0xE0;
  PC_ODR = 0x00;
  
  TIM2_CR1_bit.ARPE = 1;
  TIM2_PSCR = 0x10; 
  TIM2_ARRH = 0xFF;
  TIM2_ARRL = 0x00;
  TIM2_CCMR2_OC2M = 6;
  TIM2_CCMR2_OC2PE = 1;
  TIM2_CCER1_bit.CC2E = 1;
  TIM2_CCER1_bit.CC2P = 0;
  TIM2_CCMR3_OC3M = 6;
  TIM2_CCMR3_OC3PE = 1;
  TIM2_CCER2_bit.CC3E = 1;
  TIM2_CCER2_bit.CC3P = 0;
  TIM2_CR1_bit.CEN = 1;

  
  
  
  
}

void delay(unsigned int del){
  unsigned char i;
  while(del){
    i = 20;
    while(i){
      i--;
    }
    del--;
  }
}