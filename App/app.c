#include "app.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////     APP SETTINGS (SHOULD BE UPDATE)    /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																															////
// network info																												////
const mx_char_t *SSID = "xxxx"; // replaced by xxxx (security issue) : fill in the SSID of your network						////
const mx_char_t *Password = "xxxx"; // replaced by xxxx (security issue) : fill in the password of your network				////
																															////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int8_t app_main( void) {
    /* Initialize bsp resources */


    MX_WIFI_STATUS_T a;  // declare a variable to stock the state of the module

    a = MX_WIFI_Scan(wifi_obj_get(), 0, NULL,0); // scan is mandatory before connecting request to connect correctly

    (wifi_obj_get())->NetSettings.DHCP_IsEnabled=1; // switch on the DHCP to get an IP address

    a = MX_WIFI_Connect(wifi_obj_get(), SSID, Password, MX_WIFI_SEC_WPA_AES);

    HAL_Delay(5000); // waiting for 5s to get connected correctly

		if (a != MX_WIFI_STATUS_OK){
			return WIFI_CONNECTION_FAILED ;
		}

    uint8_t module_IP[4]; // declare a vector to stock the IP address of the module

    a = MX_WIFI_GetIPAddress(wifi_obj_get(),&module_IP[0],MC_STATION);

		if (a != MX_WIFI_STATUS_OK){
			return IP_REQUEST_FAILED;
		}

    int32_t sock_fd = MX_WIFI_Socket_create(wifi_obj_get(), MX_AF_INET, MX_SOCK_STREAM, NET_IPPROTO_TCP); // create a socket with some parameters to use TCP protocol

		if (sock_fd < 0){
			return SOCKET_CREATION_FAILED ;
		}

		/*
	struct mx_sockaddr *server_addr = (struct mx_sockaddr*) malloc(sizeof(struct mx_sockaddr));
	server_addr->sa_family = MX_AF_INET;
	server_addr->sa_len=16; // Number of bytes of the structure
		server_addr->sa_data[0] = 0x1F;          // port of our application : 8080 (in hexadecimal on 2 Bytes) : sa_data[0] and sa_data[1]
		server_addr->sa_data[1] = 0x90;
		server_addr->sa_data[2] = 192;
		server_addr->sa_data[3] = 168;           // IP of our server : each sa_data between sa_data[2] and sa_data[5] is a member (a Byte) of your IP address
		server_addr->sa_data[4] = 20;
		server_addr->sa_data[5] = 71;
		server_addr->sa_data[6] = 0;
		server_addr->sa_data[7] = 0;
		server_addr->sa_data[8] = 0;
		server_addr->sa_data[9] = 0;          // the rest of sa_data (sa_data[6] to sa_data[13]) need to be set at 0
		server_addr->sa_data[10] = 0;
		server_addr->sa_data[11] = 0;
		server_addr->sa_data[12] = 0;
		server_addr->sa_data[13] = 0;

	// Connection to the Server
	a = MX_WIFI_Socket_connect(wifi_obj_get(), sock_fd, (const struct mx_sockaddr *)server_addr, (int32_t)sizeof(struct mx_sockaddr_in));

		if (a != MX_WIFI_STATUS_OK){
			return SOCKET_CONNECTION_FAILED;
		}

	if (currentMode == KEEPALIVE) {
		// Post request for KEEPALIVE
		const char* post_request =
				"POST /SDM/public/v1/devices/alive HTTP/1.1\r\n"
				"Host: 192.168.20.71:8080\r\n"
				"Accept: application/json\r\n"
				"Content-Type: application/json; charset=utf-8\r\n"
				"Authorization: Basic xxxx\r\n" // !!!!!!!!!!!!!!!! code replace by xxxx (security issue) : fill in your identification code
				"Content-Length: 267\r\n"
				"\r\n"
				"{"
				"\"serialNumber\":\"99051190\","
				"\"applicationId\":\"2.16.756.5.25.4.6.2.1\","
				"\"deviceId\":\"TEST_PVL_TCI\","
				"\"versionId\":\"51104\","
				"\"hardwareId\":\"0\","
				"\"settingSet\":\"Batman\""
				"}";

		a = MX_WIFI_Socket_send(wifi_obj_get(), sock_fd, (const uint8_t *)post_request, strlen(post_request), 0); // function to send the post request

		// prepare the stuff for the receive function
		static unsigned char recv_buffer[100]; // create a buffer to stock the response
		memset((void*)recv_buffer, 0, sizeof(recv_buffer)); // Clear the buffer

		int32_t nb = MX_WIFI_Socket_recv(wifi_obj_get(), sock_fd, (uint8_t *)recv_buffer, 100, 0); // function to receive the response from the server

			if (nb < 0){
				return POST_REQUEST_RECEIVING_FAILED;
			}
	}

	else if (currentMode == AUTOTEST) {
		// Post request for AUTOTEST
		const char* post_request =
				"POST /SDM/public/v1/devices/test HTTP/1.1\r\n"
				"Host: 192.168.20.71:8080\r\n"
				"Accept: application/json\r\n"
				"Content-Type: application/json; charset=utf-8\r\n"
				"Authorization: Basic xxxx\r\n" // !!!!!!!!!!!!!!!! code replace by xxxx (security issue) : fill in your identification code

				"Content-Length: 404\r\n"
				"\r\n"
				"{"
				"\"serialNumber\":\"99051190\","
				"\"applicationId\":\"2.16.756.5.25.4.6.2.1\","
				"\"level\":\"INFO\","
				"\"created\":\"2024-10-10T10:19:57.789+02:00\","
                "\"content\": \"<!DOCTYPE html><html><body><div id=\\\"date-utc1\\\"></div><script>function dateUTC1() {const now = new Date();const utc1 = new Date(now.getTime() );document.getElementById(\\\"date-utc1\\\").innerText = utc1;}dateUTC1();</script></body></html>\"}' "
				"}";

		a = MX_WIFI_Socket_send(wifi_obj_get(), sock_fd, (const uint8_t *)post_request, strlen(post_request), 0); // function to send the post request

		// prepare the stuff for the receive function
		static unsigned char recv_buffer[500]; // create a buffer to stock the response
		memset((void*)recv_buffer, 0, sizeof(recv_buffer)); // Clear the buffer

		int32_t nb = MX_WIFI_Socket_recv(wifi_obj_get(), sock_fd, (uint8_t *)recv_buffer, 100, 0); // function to receive the response from the server

			if (nb < 0){
				return POST_REQUEST_RECEIVING_FAILED;
			}
	}
*/
   a = MX_WIFI_Socket_close(wifi_obj_get(), sock_fd); // Ajoute la fonction pour fermer le socket

	if (a != MX_WIFI_STATUS_OK){
		return SOCKET_CLOSING_FAILED;
	}

    return APP_OK;
}
