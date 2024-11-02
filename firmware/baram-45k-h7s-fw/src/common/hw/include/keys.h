#ifndef KEYS_H_
#define KEYS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"




bool keysInit(void);
bool keysIsBusy(void);
bool keysUpdate(void);
bool keysGetPressed(uint16_t row, uint16_t col);
bool keysReadBuf(uint8_t *p_data, uint32_t length);
bool keysReadColsBuf(uint16_t *p_data, uint32_t rows_cnt);

#ifdef __cplusplus
}
#endif

#endif