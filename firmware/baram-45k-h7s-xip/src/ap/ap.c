#include "ap.h"






void apInit(void)
{

  #if 0
  void (**jump_func)(void) = (void (**)(void))(FLASH_ADDR_FIRM + FLASH_SIZE_TAG + 4); 


  qspiSetXipMode(true);

  logPrintf("[  ] Jump To Firmware\n");
  if (((uint32_t)*jump_func) >= FLASH_ADDR_FIRM && ((uint32_t)*jump_func) < (FLASH_ADDR_FIRM + FLASH_SIZE_FIRM))
  {  
    logPrintf("[OK] addr : 0x%X\n", (uint32_t)*jump_func);
    bspDeInit();

    (*jump_func)();
  }
  else
  {
    logPrintf("[E_] Invalid Jump Address\n");
  }  
  #else
  void (**jump_func)(void);
  uint32_t firm_size = 128 * 1024;
  uint32_t copy_len  = 1024;
  uint8_t *p_buf     = (uint8_t *)SRAM_ADDR_BOOT;

  for (int i = 0; i < firm_size; i += copy_len)
  {
    flashRead(FLASH_ADDR_FIRM + i, &p_buf[i], copy_len);
  }

  jump_func = (void (**)(void))(SRAM_ADDR_BOOT + 4); 

  logPrintf("[  ] Jump To Firmware\n");
  if (((uint32_t)*jump_func) >= SRAM_ADDR_BOOT && ((uint32_t)*jump_func) < (SRAM_ADDR_BOOT + FLASH_SIZE_BOOT))
  {  
    logPrintf("[OK] addr : 0x%X\n", (uint32_t)*jump_func);
    bspDeInit();

    (*jump_func)();
  }
  else
  {
    logPrintf("[E_] Invalid Jump Address\n");
  }  

  #endif
}

void apMain(void)
{
  uint32_t pre_time;

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 100)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
  }
}
