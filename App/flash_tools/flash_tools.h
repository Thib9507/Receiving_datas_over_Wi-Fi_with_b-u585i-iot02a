/*
 * flash_tools.h
 *
 *  Created on: Nov 5, 2024
 *      Author: root
 */

#ifndef FLASH_TOOLS_FLASH_TOOLS_H_
#define FLASH_TOOLS_FLASH_TOOLS_H_


#include "app.h"


typedef struct {
	char apn[32];
	char language[32];
	char dca_nrj[32];
} __attribute__((packed)) T_config;


int8_t write_string_to_flash( T_config * config);

void read_string_from_flash(void);



#endif /* FLASH_TOOLS_FLASH_TOOLS_H_ */
