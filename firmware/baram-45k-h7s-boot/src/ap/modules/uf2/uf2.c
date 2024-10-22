/*
 * The MIT License (MIT)
 *
 * Copyright (c) Microsoft Corporation
 * Copyright (c) Ha Thach for Adafruit Industries
 * Copyright (c) Henry Gabryjelski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "uf2.h"
#include "boot/boot.h"


//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+
#define BPB_SECTOR_SIZE           (512)


static uint32_t _flash_size;
static uint16_t _flash_crc = 0;
static uint32_t _flash_len = 0;
static bool is_jump_fw = false;




bool uf2_flash_is_blank(uint32_t addr, uint32_t size)
{
  for ( uint32_t i = 0; i < size; i += sizeof(uint32_t) )
  {
    if ( *(uint32_t*) (addr + i) != 0xffffffff )
    {
      return false;
    }
  }
  return true;
}

void uf2_flash_flush(WriteState *state)
{
  firm_tag_t firm_tag;

  firm_tag.magic_number = TAG_MAGIC_NUMBER;
  firm_tag.fw_addr      = FLASH_SIZE_TAG;
  firm_tag.fw_size      = _flash_len;
  firm_tag.fw_crc       = _flash_crc;
  
  cliPrintf("\n");
  cliPrintf("fw addr : 0x%X\n", firm_tag.fw_addr);
  cliPrintf("fw size : %d B\n", firm_tag.fw_size );
  cliPrintf("fw crc  : 0x%X\n", firm_tag.fw_crc);

  if (flashWrite(FLASH_ADDR_UPDATE, (uint8_t *)&firm_tag, sizeof(firm_tag_t)) != true)
  {
    logPrintf("uf2_flash_flush() fail\n");
  }   
}

void uf2_flash_write(WriteState *state, uint32_t addr, void const *data, uint32_t len)
{
  uint32_t flash_addr;
  bool ret = true;


  flash_addr = FLASH_ADDR_UPDATE + FLASH_SIZE_TAG + addr;

  if (!uf2_flash_is_blank(flash_addr, len))
  {
    ret = flashErase(flash_addr, len);
    if (!ret)
    {
      logPrintf("[%s] flashErase(0x%X, %d)\n", ret?"OK":"E_", flash_addr, len);
    }
  }
  ret = flashWrite(flash_addr, (uint8_t *)data, len);
  if (!ret)
  {
    logPrintf("[%s] flashWrite 0x%X, %d\n", ret?"OK":"E_", flash_addr, len);
  }

  _flash_crc  = utilCalcCRC(_flash_crc, (uint8_t *)data, len);
  _flash_len += len;
}

void uf2_flash_complete(WriteState *state)
{
  uint16_t err_code;

  if (is_jump_fw)
    return;
    
  err_code = bootUpdateFirm();
  logPrintf("[%s] bootUpdateFirm()\n", err_code==OK?"OK":"E_");
  if (err_code == OK)
  {
    is_jump_fw = true;
  }
}

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+

static inline bool is_uf2_block(UF2_Block const *bl)
{
  return (bl->magicStart0 == UF2_MAGIC_START0) &&
         (bl->magicStart1 == UF2_MAGIC_START1) &&
         (bl->magicEnd == UF2_MAGIC_END) &&
         (bl->flags & UF2_FLAG_FAMILYID) &&
         !(bl->flags & UF2_FLAG_NOFLASH);
}



void uf2Init(void)
{
  _flash_size = FLASH_SIZE_FIRM;
}

void uf2Update(void)
{
  static uint8_t state = 0;
  static uint32_t pre_time;

  switch(state)
  {
    case 0:
      if (is_jump_fw)
      {
        is_jump_fw = false;
        pre_time = millis();
        state = 1;
      }
      break;

    case 1:
      if (millis()-pre_time >= 300)
      {
        bootJumpFirm();  
        state = 0;
      }
      break;
  }
}


/*------------------------------------------------------------------*/
/* Write UF2
 *------------------------------------------------------------------*/

/**
 * Write an uf2 block wrapped by 512 sector.
 * @return number of bytes processed, only 3 following values
 *  -1 : if not an uf2 block
 * 512 : write is successful (BPB_SECTOR_SIZE == 512)
 *   0 : is busy with flashing, tinyusb stack will call write_block again with the same parameters later on
 */
int uf2_write_block(uint32_t block_no, uint8_t *data, WriteState *state)
{
  (void)block_no;
  UF2_Block *bl = (void *)data;

  if (!is_uf2_block(bl))
    return -1;

  if (bl->familyID == BOARD_UF2_FAMILY_ID)
  {
    // generic family ID
    uf2_flash_write(state, bl->targetAddr, bl->data, bl->payloadSize);
  }
  else
  {
    // TODO family matches VID/PID
    return -1;
  }

  //------------- Update written blocks -------------//
  if (bl->numBlocks)
  {
    // Update state num blocks if needed
    if (state->numBlocks != bl->numBlocks)
    {
      if (bl->numBlocks >= MAX_BLOCKS || state->numBlocks)
      {
        state->numBlocks = 0xffffffff;
      }
      else
      {
        state->numBlocks = bl->numBlocks;
      }
    }

    if (bl->blockNo < MAX_BLOCKS)
    {
      uint8_t const  mask = 1 << (bl->blockNo % 8);
      uint32_t const pos  = bl->blockNo / 8;

      // only increase written number with new write (possibly prevent overwriting from OS)
      if (!(state->writtenMask[pos] & mask))
      {
        state->writtenMask[pos] |= mask;
        state->numWritten++;
      }

      // flush last blocks
      // TODO numWritten can be smaller than numBlocks if return early
      if (state->numWritten >= state->numBlocks)
      {
        uf2_flash_flush(state);
      }
    }
  }

  return BPB_SECTOR_SIZE;
}
