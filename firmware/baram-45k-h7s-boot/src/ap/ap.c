#include "ap.h"
#include "uf2/uf2.h"





void apInit(void)
{
  #ifdef _USE_HW_CLI  
  cliOpen(HW_UART_CH_CLI, 115200);  
  #endif
  logBoot(false);

  uf2Init();
  usbInit();
}

void apMain(void)
{
  uint32_t pre_time;

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }

    #ifdef _USE_HW_CLI
    cliMain();
    #endif
    usbUpdate();
    uf2Update();    
  }
}
