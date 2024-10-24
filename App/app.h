#ifndef APP_H
#define APP_H


#include "bsp.h"
#include "mx_wifi.h"
#include "mx_wifi_io.h"
#include "mx_address.h"
#include "init/app_init.h"

#include <stdio.h>
#include <string.h>


typedef char            char_t;


typedef enum { // Structure to indicate the json to send
    AUTOTEST = 0,
	KEEPALIVE = 1,
	NOTHING = 2
} Mode;


typedef enum { // Structure to indicate the app status
    APP_OK = 0,
	HW_START_FAILED = -1,
	WIFI_CONNECTION_FAILED = -2,
	IP_REQUEST_FAILED = -3,
	SOCKET_CREATION_FAILED =-4,
	SOCKET_SETING_OPTION_FAILED = -5,
	SOCKET_BINDING_FAILED = -6,
	SOCKET_LISTENING_FAILED =-7,
	SOCKET_ACCEPTING_FAILED =-8,
	GET_REQUEST_RECEIVING_FAILED = -9,
	GET_REQUEST_SENDING_FAILED = -10,
	SOCKET_CLOSING_FAILED=-11
} App_status;

int8_t app_main( void);

extern unsigned char print_on_uart[1000];

#endif
