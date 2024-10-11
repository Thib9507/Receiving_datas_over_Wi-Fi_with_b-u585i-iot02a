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
	SOCKET_CREATION_FAILED = -4,
	SOCKET_CONNECTION_FAILED = -5,
	POST_REQUEST_SENDING_FAILED =-6,
	POST_REQUEST_RECEIVING_FAILED=-7,
	SOCKET_CLOSING_FAILED=-8
} App_status;

int8_t app_main( void);

#endif
