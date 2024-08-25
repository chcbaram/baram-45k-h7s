#include "Dev_Inf.h"



struct StorageInfo const StorageInfo  =  
{
   "W25Q16JV_BARAM-45K-H7S", 	 					          // Device Name 
   SPI_FLASH,                   					        // Device Type
   0x90000000,                						        // Device Start Address
   0x00200000,              						          // Device Size in 2 MBytes
   0x100,                     						        // Programming Page Size 256 Bytes
   0xFF,                       						        // Initial Content of Erased Memory
// Specify Size and Address of Sectors (view example below)
   {
    {0x00000020, 0x00010000},     				 		    // Sector Num : 32 ,Sector Size: 64 KBytes
    {0x00000000, 0x00000000},
   }      
};