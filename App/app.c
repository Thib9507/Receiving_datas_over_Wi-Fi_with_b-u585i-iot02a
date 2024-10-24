#include "app.h"

typedef enum
{
  CONSOLE_OK  = 0,
  CONSOLE_ERROR = -1
} Console_Status;

UART_HandleTypeDef Console_UARTHandle;

Console_Status webserver_console_config(void)
{
  /* Set parameter to be configured */
  Console_UARTHandle.Instance                    = USART1;
  Console_UARTHandle.Init.BaudRate               = 115200;
  Console_UARTHandle.Init.WordLength             = UART_WORDLENGTH_8B;
  Console_UARTHandle.Init.StopBits               = UART_STOPBITS_1;
  Console_UARTHandle.Init.Parity                 = UART_PARITY_NONE;
  Console_UARTHandle.Init.Mode                   = UART_MODE_TX_RX;
  Console_UARTHandle.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
  Console_UARTHandle.Init.OverSampling           = UART_OVERSAMPLING_16;
  Console_UARTHandle.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
  Console_UARTHandle.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
  Console_UARTHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  /* Initialize the UART mode */
  if (HAL_UART_Init(&Console_UARTHandle) != HAL_OK)
  {
    return CONSOLE_ERROR;
  }

  /* Disable the UART FIFO mode */
  if (HAL_UARTEx_DisableFifoMode(&Console_UARTHandle) != HAL_OK)
  {
    return CONSOLE_ERROR;
  }

  return CONSOLE_OK;
}

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

	// Console_Status console_value = webserver_console_config();

	uint8_t Test[] = "Hello World !!!\r\n"; //Data to send
	HAL_UART_Transmit(&Console_UARTHandle,Test,sizeof(Test),10);// Sending in normal mode
	HAL_Delay(1000);

	printf("test");

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

	a =	MX_WIFI_Socket_listen(wifi_obj_get(),sock_fd, 1); // backlog here 5 is the number of connection accepted

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
	int32_t get_request_sock = MX_WIFI_Socket_accept(wifi_obj_get(), sock_fd, (struct mx_sockaddr *)remote_host, &addr_len);

		if (get_request_sock < 0 ){
				return SOCKET_ACCEPTING_FAILED;
		}

	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_7, GPIO_PIN_SET); // turn on the green LED to indicate that you can connect to IP address

	static unsigned char get_request_recv_buffer[300]; // create a buffer to stock the response
	memset((void*)get_request_recv_buffer, 0, sizeof(get_request_recv_buffer)); // Clear the buffer

	int32_t get_request_recv_nb_bytes = MX_WIFI_Socket_recv(wifi_obj_get(), get_request_sock, (uint8_t *)get_request_recv_buffer, 300, 0); // function to receive the client's request

		if (get_request_recv_nb_bytes < 0){
				return GET_REQUEST_RECEIVING_FAILED;
		}

	const char* web_page =
		"HTTP/1.1 200 OK\r\n"
		"Server: STM32U585-DK\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Access-Control-Allow-Methods: GET\r\n"
		"Access-Control-Allow-Headers: cache-control, last-event-id, X-Requested-With\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: 1338\r\n"
		"Connection: close\r\n"
		"\r\n"
		"\n"
		"<!DOCTYPE html>\n"
		"<html lang=\"fr\">\n"
		"<head>\n"
		"    <meta charset=\"UTF-8\">\n"
		"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		"    <link rel='icon' href='data:,'>\n"
		"    <title>Formulaire POST</title>\n"
		"    <style>\n"
		"        label { display: block; margin-top: 10px; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h2>Formulaire de configuration</h2>\n"
		"    <form id=\"configForm\" method=\"POST\">\n"
		"        <label for=\"apn\">APN SIM :</label>\n"
		"        <select id=\"apn\" name=\"apn\" required>\n"
		"            <option value=\"free\">Free</option>\n"
		"            <option value=\"orange\">Orange</option>\n"
		"            <option value=\"bouygues\">Bouygues</option>\n"
		"        </select>\n"
		"        <button type=\"submit\">Envoyer</button>\n"
		"    </form>\n"
		"    <script>\n"
		"        document.getElementById('configForm').addEventListener('submit', function(event) {\n"
		"            event.preventDefault();\n"
		"            const form = event.target;\n"
		"            const formData = new FormData(form);\n"
		"            const ip = window.location.hostname;\n"
		"            fetch(`http://${ip}/submit`, {\n"
		"                method: 'POST',\n"
		"                body: formData\n"
		"            })\n"
		"            .then(response => response.text())\n"
		"            .then(data => alert(\"Formulaire envoyé avec succès !\"))\n"
		"            .catch(error => alert(\"Erreur lors de l'envoi du formulaire\"));\n"
		"        });\n"
		"    </script>\n"
		"</body>\n"
		"</html>";

	int32_t get_request_send_nb_bytes = MX_WIFI_Socket_send(wifi_obj_get(), get_request_sock, (const uint8_t *)web_page, strlen(web_page), 0); // function to send the web page to the client

		if (get_request_send_nb_bytes < 0){
				return GET_REQUEST_SENDING_FAILED;
		}


	int32_t post_request_sock = MX_WIFI_Socket_accept(wifi_obj_get(), sock_fd, (struct mx_sockaddr *)remote_host, &addr_len);

		if (post_request_sock < 0 ){
				return SOCKET_ACCEPTING_FAILED;
		}

	static unsigned char post_request_recv_buffer[1000]; // create a buffer to stock the response
	memset((void*)post_request_recv_buffer, 0, sizeof(post_request_recv_buffer)); // Clear the buffer

	int32_t post_request_recv_nb_bytes = MX_WIFI_Socket_recv(wifi_obj_get(), post_request_sock, (uint8_t *)post_request_recv_buffer, 1000, 0); // function to receive the client's request

		if (post_request_recv_nb_bytes < 0){
				return GET_REQUEST_RECEIVING_FAILED;
		}

	a = MX_WIFI_Socket_close(wifi_obj_get(), sock_fd); // Closing socket function

	if (a != MX_WIFI_STATUS_OK){
		return SOCKET_CLOSING_FAILED;
	}

	return APP_OK;
}
