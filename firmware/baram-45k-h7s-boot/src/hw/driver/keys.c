#include "keys.h"


#ifdef _USE_HW_KEYS
#include "button.h"
#include "qbuffer.h"
#include "cli.h"



#if CLI_USE(HW_KEYS)
static void cliCmd(cli_args_t *args);
#endif





bool keysInit(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // ROW 0
  //
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pin   = GPIO_PIN_0;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);

  // COL 0
  //
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pin   = GPIO_PIN_0;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  delay(2);

#if CLI_USE(HW_KEYS)
  cliAdd("keys", cliCmd);
#endif

  return true;
}

bool keysIsBusy(void)
{
  return false;
}

bool keysUpdate(void)
{
  return true;
}

bool keysReadBuf(uint8_t *p_data, uint32_t length)
{
  return true;
}

bool keysGetPressed(uint16_t row, uint16_t col)
{
  bool ret = false;

  
  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET)
  {
    ret = true;
  }

  return ret;
}

void keysUpdateEvent(void)
{
}

#if CLI_USE(HW_KEYS)
void cliCmd(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliShowCursor(false);


    while(cliKeepLoop())
    {
      keysUpdate();
      delay(10);

      cliPrintf("     ");
      for (int cols=0; cols<MATRIX_COLS; cols++)
      {
        cliPrintf("%02d ", cols);
      }
      cliPrintf("\n");

      for (int rows=0; rows<MATRIX_ROWS; rows++)
      {
        cliPrintf("%02d : ", rows);

        for (int cols=0; cols<MATRIX_COLS; cols++)
        {
          if (keysGetPressed(rows, cols))
            cliPrintf("O  ");
          else
            cliPrintf("_  ");
        }
        cliPrintf("\n");
      }
      cliMoveUp(MATRIX_ROWS+1);
    }
    cliMoveDown(MATRIX_ROWS+1);

    cliShowCursor(true);
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("keys info\n");
  }
}
#endif

#endif