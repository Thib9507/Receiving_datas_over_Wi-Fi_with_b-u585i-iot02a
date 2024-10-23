#include "app.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////     APP SETTINGS (SHOULD BE UPDATE)    /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																															////
// network info																												////
const mx_char_t *SSID = "iPhone_de_Thibault"; // replaced by xxxx (security issue) : fill in the SSID of your network		////
const mx_char_t *Password = "88888888"; // replaced by xxxx (security issue) : fill in the password of your network			////
																															////
int32_t socket_timeout = 0xFFFFFFFF; // Setting time to connect to the server												////
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

	a = MX_WIFI_Socket_setsockopt(wifi_obj_get(), sock_fd, (int32_t) 4095, (int32_t)4102, &socket_timeout, (int32_t) 4); // set the infinite timeout to let the time to connect to the IP address

		if (a != MX_WIFI_STATUS_OK){
				return SOCKET_SETING_OPTION_FAILED;
		}

	struct mx_sockaddr *server_addr = (struct mx_sockaddr*) malloc(sizeof(struct mx_sockaddr));
	server_addr->sa_family = MX_AF_INET;
	server_addr->sa_len=16; // Number of bytes of the structure
		server_addr->sa_data[0] = 0;          // port of our application : 80 (on 2 Bytes) : sa_data[0] and sa_data[1]
		server_addr->sa_data[1] = 80;
		server_addr->sa_data[2] = module_IP[0];
		server_addr->sa_data[3] = module_IP[1];           // IP of our server : each sa_data between sa_data[2] and sa_data[5] is a member (a Byte) of your IP address
		server_addr->sa_data[4] = module_IP[2];
		server_addr->sa_data[5] = module_IP[3];
		server_addr->sa_data[6] = 0;
		server_addr->sa_data[7] = 0;
		server_addr->sa_data[8] = 0;
		server_addr->sa_data[9] = 0;          // the rest of sa_data (sa_data[6] to sa_data[13]) need to be set at 0
		server_addr->sa_data[10] = 0;
		server_addr->sa_data[11] = 0;
		server_addr->sa_data[12] = 0;
		server_addr->sa_data[13] = 0;

	a = MX_WIFI_Socket_bind(wifi_obj_get(), sock_fd, (struct mx_sockaddr *)server_addr, (int32_t)sizeof(struct mx_sockaddr_in)); // Linking the IP address obtained with the socket

		if (a != MX_WIFI_STATUS_OK){
			return SOCKET_BINDING_FAILED;
		}

	a =	MX_WIFI_Socket_listen(wifi_obj_get(),sock_fd, 5); // backlog here 5 is the number of connection accepted

		if (a != MX_WIFI_STATUS_OK){
				return SOCKET_LISTENING_FAILED;
		}

	struct mx_sockaddr *remote_host = (struct mx_sockaddr*) malloc(sizeof(struct mx_sockaddr));
	remote_host->sa_family = 0;
	remote_host->sa_len=0; // Number of bytes of the structure
		remote_host->sa_data[0] = 0;          // port of our application : 80 (on 2 Bytes) : sa_data[0] and sa_data[1]
		remote_host->sa_data[1] = 0;
		remote_host->sa_data[2] = 0;
		remote_host->sa_data[3] = 0;           // IP of our server : each sa_data between sa_data[2] and sa_data[5] is a member (a Byte) of your IP address
		remote_host->sa_data[4] = 0;
		remote_host->sa_data[5] = 0;
		remote_host->sa_data[6] = 0;
		remote_host->sa_data[7] = 0;
		remote_host->sa_data[8] = 0;
		remote_host->sa_data[9] = 0;          // the rest of sa_data (sa_data[6] to sa_data[13]) need to be set at 0
		remote_host->sa_data[10] = 0;
		remote_host->sa_data[11] = 0;
		remote_host->sa_data[12] = 0;
		remote_host->sa_data[13] = 0;

	uint32_t addr_len = 16; // setting the length of the structure above to put it in the MX_WIFI_Socket_accept function

	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_7, GPIO_PIN_RESET); // turn on the green LED to indicate that you can connect to IP address

	// accept function will block the device until a connection request, in the function, addr is used to stock the IP address of the remote device
	int32_t client_sock = MX_WIFI_Socket_accept(wifi_obj_get(), sock_fd, (struct mx_sockaddr *)remote_host, &addr_len);

		if (client_sock < 0 ){
				return SOCKET_ACCEPTING_FAILED;
		}

	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_7, GPIO_PIN_SET); // turn on the green LED to indicate that you can connect to IP address

	static unsigned char recv_buffer[300]; // create a buffer to stock the response
	memset((void*)recv_buffer, 0, sizeof(recv_buffer)); // Clear the buffer

	int32_t nb = MX_WIFI_Socket_recv(wifi_obj_get(), client_sock, (uint8_t *)recv_buffer, 300, 0); // function to receive the client's request

		if (nb < 0){
				return GET_REQUEST_RECEIVING_FAILED;
		}

	const char* web_page = "HTTP/1.1 200 OK\r\nServer: STM32U585-DK\r\nAccess-Control-Allow-Origin: * \r\nAccess-Control-Allow-Methods: GET\r\nAccess-Control-Allow-Headers: cache-control, last-event-id, X-Requested-With\r\nContent-Type: text/html; charset=utf-8\r\nAccept-Ranges: bytes\r\nContent-Length: 89\r\nConnection: close\r\n\r\n\n<!DOCTYPE html><html><head><link rel='icon' href='data:,'></head><body>TEST</body></html>";

	int32_t nb_bytes = MX_WIFI_Socket_send(wifi_obj_get(), client_sock, (const uint8_t *)web_page, strlen(web_page), 0); // function to send the web page to the client

		if (nb_bytes < 0){
				return GET_REQUEST_SENDING_FAILED;
		}

	/*static unsigned char recv_buffer1[300]; // create a buffer to stock the response
	memset((void*)recv_buffer1, 0, sizeof(recv_buffer)); // Clear the buffer

	int32_t nb1 = MX_WIFI_Socket_recv(wifi_obj_get(), client_sock, (uint8_t *)recv_buffer1, 300, 0); // function to receive the client's request

		if (nb1 < 0){
				return GET_REQUEST_RECEIVING_FAILED;
		}*/

	a = MX_WIFI_Socket_close(wifi_obj_get(), sock_fd); // Closing socket function

	if (a != MX_WIFI_STATUS_OK){
		return SOCKET_CLOSING_FAILED;
	}

	return APP_OK;
}
