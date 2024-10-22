#include "boot.h"


#ifdef _USE_HW_CLI
static void cliBoot(cli_args_t *args);
#endif



bool bootInit(void)
{

#ifdef _USE_HW_CLI
  cliAdd("boot", cliBoot);
#endif  
  return true;
}

uint16_t bootVerifyUpdate(void)
{
  uint32_t addr = 0;
  uint32_t length = 0;
  uint16_t crc;
  uint16_t err_code = OK;
  uint32_t rd_len;
  uint8_t  rd_buf[128];
  firm_tag_t tag;


  do 
  {
    firm_tag_t *p_tag = (firm_tag_t *)&tag;

    flashRead(FLASH_ADDR_UPDATE, (uint8_t *)p_tag, sizeof(firm_tag_t));


    if (p_tag->magic_number != TAG_MAGIC_NUMBER)
    {
      err_code = ERR_BOOT_TAG_MAGIC;
      break;
    }

    if (p_tag->fw_size >= FLASH_SIZE_FIRM)
    {
      err_code = ERR_BOOT_TAG_SIZE;
      break;
    }

    addr   = FLASH_ADDR_UPDATE + p_tag->fw_addr;
    length = p_tag->fw_size;
    crc    = 0;

    uint32_t index;

    index = 0;
    while (index < length)
    {
      rd_len = length-index;
      if  (rd_len > 128)
        rd_len = 128;

      if (flashRead(addr + index, rd_buf, rd_len) != true)
      {
        err_code = ERR_BOOT_FLASH_READ;
        break;
      }

      index += rd_len;

      for (uint32_t i=0; i<rd_len; i++)
      {
        utilUpdateCrc(&crc, rd_buf[i]);
      }
    }

    if (err_code == OK)
    {
      if (p_tag->fw_crc != crc)
      {
        err_code = ERR_BOOT_FW_CRC;
      }
    }
  } while(0);

  return err_code;
}

uint16_t bootVerifyFirm(void)
{
  uint32_t addr = 0;
  uint32_t length = 0;
  uint16_t crc;
  uint16_t err_code = OK;
  uint32_t rd_len;
  uint8_t  rd_buf[128];
  firm_tag_t *p_tag = (firm_tag_t *)(FLASH_ADDR_FIRM);


  do 
  {
    if (p_tag->magic_number != TAG_MAGIC_NUMBER)
    {
      err_code = ERR_BOOT_TAG_MAGIC;
      break;
    }

    if (p_tag->fw_size >= FLASH_SIZE_FIRM)
    {
      err_code = ERR_BOOT_TAG_SIZE;
      break;
    }

    addr   = FLASH_ADDR_FIRM + p_tag->fw_addr;
    length = p_tag->fw_size;
    crc    = 0;

    uint32_t index;

    index = 0;
    while (index < length)
    {
      rd_len = length-index;
      if  (rd_len > 128)
        rd_len = 128;

      if (flashRead(addr + index, rd_buf, rd_len) != true)
      {
        err_code = ERR_BOOT_FLASH_READ;
        break;
      }

      index += rd_len;

      for (uint32_t i=0; i<rd_len; i++)
      {
        utilUpdateCrc(&crc, rd_buf[i]);
      }
    }

    if (err_code == OK)
    {
      if (p_tag->fw_crc != crc)
      {
        err_code = ERR_BOOT_FW_CRC;
        logPrintf("     CRC : 0x%X, 0x%X\n", p_tag->fw_crc, crc);
      }
    }
  } while(0);

  return err_code;
}

uint16_t bootUpdateFirm(void)
{
  uint8_t err_code = OK;
  firm_tag_t tag;


  while(1)
  {
    firm_tag_t *p_tag = (firm_tag_t *)&tag;

    // Read Tag
    //
    flashRead(FLASH_ADDR_UPDATE, (uint8_t *)p_tag, sizeof(firm_tag_t));


    // Erase F/W
    //
    if (flashErase(FLASH_ADDR_FIRM, FLASH_SIZE_TAG + p_tag->fw_size) != true)
    {
      err_code = ERR_BOOT_FLASH_ERASE;
      break;
    }

    // Write F/W
    //
    uint32_t index;
    uint32_t fw_size;

    index = 0;
    fw_size = FLASH_SIZE_TAG + p_tag->fw_size;

    while(index < fw_size)
    {
      uint8_t buf[512];
      uint32_t wr_size;
      uint32_t wr_addr;


      wr_addr = FLASH_ADDR_UPDATE + index;
      wr_size = constrain(fw_size-index, 0, 512);

      if (flashRead(wr_addr, buf, wr_size) != true)
      {
        err_code = ERR_BOOT_FLASH_READ;
        break;
      }

      wr_addr = FLASH_ADDR_FIRM + index;

      if (flashWrite(wr_addr, buf, wr_size) != true)
      {
        err_code = ERR_BOOT_FLASH_WRITE;
        break;
      }

      index += wr_size;
    }

    if (err_code == OK)
    {
      // Verify F/W
      //
      err_code = bootVerifyFirm();
    }
    break;
  }

  return err_code;
}

uint16_t bootJumpFirm(void)
{
  uint16_t err_code = OK;
  
  err_code = bootVerifyFirm();
  if (err_code == OK)
  {
    void (**jump_func)(void) = (void (**)(void))(FLASH_ADDR_FIRM + FLASH_SIZE_TAG + 4); 


    if (((uint32_t)*jump_func) >= FLASH_ADDR_FIRM && ((uint32_t)*jump_func) < (FLASH_ADDR_FIRM + FLASH_SIZE_FIRM))
    {
      logPrintf("[  ] bootJumpFirm()\n");
      logPrintf("     addr : 0x%X\n", (uint32_t)*jump_func);

      resetSetBootMode(0);

      bspDeInit();

      (*jump_func)();
    }
    else
    {
      err_code = ERR_BOOT_INVALID_FW;
    }
  }

  return err_code;
}


#ifdef _USE_HW_CLI
void cliBoot(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    firm_ver_t *p_boot = (firm_ver_t *)(FLASH_ADDR_BOOT + FLASH_SIZE_VEC);
    firm_ver_t *p_firm;
    firm_tag_t *p_tag;

    if (p_boot->magic_number == VERSION_MAGIC_NUMBER)
    {
      cliPrintf("BOOT \n");
      cliPrintf("   %s\n", p_boot->name_str);
      cliPrintf("   %s\n", p_boot->version_str);
      cliPrintf("   0x%X\n", p_boot->firm_addr);
    }
    else
    {
      cliPrintf("No Boot Version\n");
    }


    const char    *firm_name[2] = {"FIRM", "UPDATE"};
    const uint32_t firm_addr[2] = {FLASH_ADDR_FIRM, FLASH_ADDR_UPDATE};

    for (int i=0; i<2; i++)
    {
      p_firm = (firm_ver_t *)(firm_addr[i] + FLASH_SIZE_TAG + FLASH_SIZE_VEC);
      p_tag  = (firm_tag_t *)(firm_addr[i]);

      if (p_firm->magic_number == VERSION_MAGIC_NUMBER)
      {
        cliPrintf("%s \n", firm_name[i]);
        cliPrintf("   %s\n", p_firm->name_str);
        cliPrintf("   %s\n", p_firm->version_str);
        cliPrintf("   0x%X\n", p_firm->firm_addr);
      }
      else
      {
        cliPrintf("No %s Version\n", firm_name[i]);
      }

      if (p_tag->magic_number == TAG_MAGIC_NUMBER)
      {
        cliPrintf("TAG \n");
        cliPrintf("   fw_addr : 0x%X\n", p_tag->fw_addr);
        cliPrintf("   fw_size : %d KB\n", p_tag->fw_size/1024);
        cliPrintf("   fw_crc  : 0x%X\n", p_tag->fw_crc);
      }
      else
      {
        cliPrintf("No %s Tag\n", firm_name[i]);
      }
    }

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "update"))
  {
    uint16_t err_code;

    err_code = bootUpdateFirm();

    logPrintf("bootUpdateFirm()\n");
    logPrintf("  err_code : 0x%X\n", err_code);
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "jump"))
  {
    bootJumpFirm();
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("boot info\n");
    cliPrintf("boot update\n");
    cliPrintf("boot jump\n");
  }  
}
#endif
