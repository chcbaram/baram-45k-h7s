#include "ap.h"


static void bootUp(void);
static void jumpToBoot(void);
static void jumpToFirm(void);





void apInit(void)
{
  bootUp();
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

void bootUp(void)
{
  uint32_t boot_param;


  boot_param = resetGetBootMode();
  if (boot_param & (1<<MODE_BIT_FIRM))
  {
    boot_param &= ~(1<<MODE_BIT_FIRM);
    resetSetBootMode(boot_param);   
    jumpToFirm(); 
  }

  jumpToBoot();

  logPrintf("Xip Mode..\n"); 
}

void jumpToBoot(void)
{
  void (**jump_func)(void);
  uint32_t firm_size = FLASH_SIZE_BOOT;
  uint32_t copy_len  = 1024;
  uint8_t *p_buf     = (uint8_t *)SRAM_ADDR_BOOT;

  for (int i = 0; i < firm_size; i += copy_len)
  {
    flashRead(FLASH_ADDR_BOOT + i, &p_buf[i], copy_len);
  }

  jump_func = (void (**)(void))(SRAM_ADDR_BOOT + 4); 

  logPrintf("[  ] Jump To Bootloader\n");
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
}

void jumpToFirm(void)
{
  logPrintf("[  ] Jump To Firmware\n");
}