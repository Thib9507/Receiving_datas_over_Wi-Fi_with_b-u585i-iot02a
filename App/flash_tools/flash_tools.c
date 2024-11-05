/*
 * flash_tools.c
 *
 *  Created on: Nov 5, 2024
 *      Author: root
 */

#include "flash_tools/flash_tools.h"



T_config g_config_flash __attribute__((section(".config_data"), aligned(4))); // Config declaration in flash memory




int8_t write_string_to_flash( T_config * config) {
	FLASH_EraseInitTypeDef eraseInit;
	HAL_StatusTypeDef ret;
	uint32_t address = (uint32_t) &g_config_flash;
	uint32_t PageError = 0;
	uint32_t i;

	HAL_FLASH_Unlock(); 	// Unlock flash to manipulate it

	// Little schema to understand the flash memory composition (we will manipulate it below)


    //////////////////////////////////////////////////////////////////////////
    ////                    //                    //                        //
    ////                    //                    //        Page 0    (8KB) //
    ////                    //                    //                        //
    ////                    //                    ////////////////////////////
    ////                    //                    //                        //
    ////                    //                    //        Page 1    (8KB) //
    ////                    //                    //                        //
    ////                    //        Bank 1      ////////////////////////////
    ////                    //                    //        .               //
    ////                    //      127 * 8KB     //        .               //
    ////                    //                    //        .               //
    ////                    //                    //        .               //
    ////                    //                    ////////////////////////////
    ////                    //                    //                        //
    ////                    //                    //       Page 127 (8KB)   //
    ////        Main        //                    //                        //
    ////        flash       //////////////////////////////////////////////////
    ////        memory      //                    //                        //
    ////                    //                    //       Page 0    (8KB)  //
    ////                    //                    //                        //
    ////                    //                    ////////////////////////////
    ////                    //                    //                        //
    ////                    //                    //       Page 1    (8KB)  //
    ////                    //                    //                        //
    ////                    //        Bank 2      ////////////////////////////
    ////                    //                    //        .               //
    ////                    //      127 * 8KB     //        .               //
    ////                    //                    //        .               //
    ////                    //                    //        .               //
    ////                    //                    ////////////////////////////
    ////                    //                    //                        //
    ////                    //                    //      Page 127 (8KB)    //
    ////                    //                    //                        //
    //////////////////////////////////////////////////////////////////////////



	// Structure to feed HAL_FLASHEx_Erase (choose the erase mode)

	eraseInit.TypeErase = FLASH_TYPEERASE_PAGES; // Choosing page erasing mode
	eraseInit.Banks = FLASH_BANK_2; // Selecting the 2nd bank, because we declared the area at the end of the flash memory in the linker file
	eraseInit.NbPages = 1; // Our application required only 1 page to store the config
	eraseInit.Page = (address - 0x08100000) / FLASH_PAGE_SIZE; // Giving the number of the page to erase


	ret = HAL_FLASHEx_Erase(&eraseInit, &PageError); // function to erase flash memory (to write stuff in an area of the flash, the concerned area have to be empty)
	if( ret != HAL_OK) {
		HAL_FLASH_Lock();
		return VOLATILE_WRITING_FAILED;
	}

	uint32_t data[sizeof(T_config) / sizeof(uint32_t)];
	memcpy(data, config, sizeof(T_config));

	// For loop to write each quadwords in the flash (a quadword is a group of 4 words : with u585 MPU, you can write in the flash only with quadword or burst (more complicated)
	for( i = 0; i < sizeof(T_config); i += 16) {

		/* Check overflow */
		if( i/4 >= sizeof( g_config_flash)) {
			HAL_FLASH_Lock();
			return VOLATILE_WRITING_FAILED;
		}

		/*
		 * 1st lap
		 *   i = 0 => i/4 = 0
		 *   writing 4 words => data 0, 1, 2, 3
		 * 2nd lap
		 *   i = 16 => i/4 = 4
		 *   writing 4 words => data 4, 5, 6, 7
		 * 3rd lap
		 *   i = 32 => i/4 = 8
		 *   writing 4 words => data 8, 9, 10, 11
		 *   .
		 *   .
		 *   .
		 */
		ret = HAL_FLASH_Program( FLASH_TYPEPROGRAM_QUADWORD, address, (uint32_t) (data+(i/4)));
		if( ret != HAL_OK) {
			HAL_FLASH_Lock();
			return VOLATILE_WRITING_FAILED;
		}
		address += 16;
	}

	// Re-lock the flash
	HAL_FLASH_Lock();
	return 0;
}





void read_string_from_flash(void) {

	T_config config_read;

	memcpy( &config_read, &g_config_flash, sizeof( T_config)); // read the initial config in flash

	printf("apn: %s\n", config_read.apn);
	printf("language: %s\n", config_read.language);
	printf("dca_nrj: %s\n", config_read.dca_nrj);
}
