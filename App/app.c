#include "app.h"


unsigned char print_on_uart[1000]; // create a buffer to stock the response

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

	read_string_from_flash();

    MX_WIFI_STATUS_T app_status;  // declare a variable to stock the state of the module

    app_status = MX_WIFI_Scan(wifi_obj_get(), 0, NULL,0); // scan is mandatory before connecting request to connect correctly

    (wifi_obj_get())->NetSettings.DHCP_IsEnabled=1; // switch on the DHCP to get an IP address

    app_status = MX_WIFI_Connect(wifi_obj_get(), SSID, Password, MX_WIFI_SEC_WPA_AES);

    HAL_Delay(5000); // waiting for 5s to get connected correctly

		if (app_status != MX_WIFI_STATUS_OK){
			return WIFI_CONNECTION_FAILED ;
		}

    uint8_t module_IP[4]; // declare a vector to stock the IP address of the module

    app_status = MX_WIFI_GetIPAddress(wifi_obj_get(),&module_IP[0],MC_STATION);

		if (app_status != MX_WIFI_STATUS_OK){
			return IP_REQUEST_FAILED;
		}

    int32_t sock_fd = MX_WIFI_Socket_create(wifi_obj_get(), MX_AF_INET, MX_SOCK_STREAM, NET_IPPROTO_TCP); // create a socket with some parameters to use TCP protocol

		if (sock_fd < 0){
			return SOCKET_CREATION_FAILED ;
		}

		app_status = MX_WIFI_Socket_setsockopt(wifi_obj_get(), sock_fd, (int32_t) 4095, (int32_t)4102, &socket_timeout, (int32_t) 4); // set the infinite timeout to let the time to connect to the IP address

		if (app_status != MX_WIFI_STATUS_OK){
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

		app_status = MX_WIFI_Socket_bind(wifi_obj_get(), sock_fd, (struct mx_sockaddr *)server_addr, (int32_t)sizeof(struct mx_sockaddr_in)); // Linking the IP address obtained with the socket

		if (app_status != MX_WIFI_STATUS_OK){
			return SOCKET_BINDING_FAILED;
		}

		app_status = MX_WIFI_Socket_listen(wifi_obj_get(),sock_fd, 1); // backlog here 5 is the number of connection accepted

		if (app_status != MX_WIFI_STATUS_OK){
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

	unsigned char get_request_recv_buffer[300]; // create a buffer to stock the response
	int32_t get_request_sock;

	do {
		// accept function will block the device until a connection request, in the function, addr is used to stock the IP address of the remote device
		get_request_sock = MX_WIFI_Socket_accept(wifi_obj_get(), sock_fd, (struct mx_sockaddr *)remote_host, &addr_len);

			if (get_request_sock < 0 ){
					return SOCKET_ACCEPTING_FAILED;
			}

		memset((void*)get_request_recv_buffer, 0, sizeof(get_request_recv_buffer)); // Clear the buffer

		int32_t get_request_recv_nb_bytes = MX_WIFI_Socket_recv(wifi_obj_get(), get_request_sock, (uint8_t *)get_request_recv_buffer, 300, 0); // function to receive the client's request

			if (get_request_recv_nb_bytes < 0){
					return GET_REQUEST_RECEIVING_FAILED;
			}

	}while(strncmp((char *)get_request_recv_buffer, "GET /", 5) != 0U); // if the request is not a get request, wait an other request

	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_7, GPIO_PIN_SET); // turn off the green LED

		// The following part is the code of the web page sent to the client. This web page is sent by few request because the module can send a limited number of bytes (2482)
	const char* web_page_part1 = "HTTP/1.1 200 OK\r\n"
		"Server: STM32U585-DK\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Access-Control-Allow-Methods: GET\r\n"
		"Access-Control-Allow-Headers: cache-control, last-event-id, X-Requested-With\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: 2479\r\n" // content-length (length of the full web page code) must be correct otherwise the request won't work correctly
		"Connection: close\r\n"
		"\r\n"
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
		"		<label for=\"apn\">APN SIM :</label>\n"
		"        <select id=\"apn\" name=\"apn\" required>\n"
		"            <option value=\"Free\">Free</option>\n"
		"            <option value=\"Orange\">Orange</option>\n"
		"            <option value=\"Bouygues\">Bouygues</option>\n"
		"        </select>\n"
		"		<br>\n"
		"        <label for=\"language\">Defibrillator language :</label>\n"
		"        <select id=\"language\" name=\"language\" required>\n"
		"            <option value=\"DCA_ENG_ENG_ENG\">English</option>\n"
		"            <option value=\"DCA_FRE_FRE_FRE\">Français</option>\n"
		"            <option value=\"DCA_GER_GER_GER\">Deutsche</option>\n"
		"            <option value=\"DCA_SPA_SPA_SPA\">Spanish</option>\n"
		"        </select>\n"
		"		<br>\n"
		"        <label for=\"language\">DCA energy (in joule):</label>\n"
		"        <select id=\"dca_nrj\" name=\"dca_nrj\" required>\n"
		"            <option value=\"15\">15</option>\n"
		"            <option value=\"30\">30</option>\n"
		"            <option value=\"50\">50</option>\n"
		"            <option value=\"70\">70</option>\n"
		"            <option value=\"90\">90</option>\n"
		"            <option value=\"120\">120</option>\n"
		"            <option value=\"150\">150</option>\n"
		"            <option value=\"200\">200</option>\n"
		"        </select>\n"
		"		<br>\n"
		"		<br>\n"
		"        <button type=\"submit\">Envoyer</button>\n"
		"    </form>\n";

	int32_t get_request_send_1_nb_bytes = MX_WIFI_Socket_send(wifi_obj_get(), get_request_sock, (const uint8_t *)web_page_part1, strlen(web_page_part1), 0); // function to send the web page to the client

		if (get_request_send_1_nb_bytes < 0){
				return GET_REQUEST_SENDING_FAILED;
		}

	const char* web_page_part2 = "    <script>\n"
		"        document.getElementById('configForm').addEventListener('submit', function(event) {\n"
		"            event.preventDefault();\n"
		"            const form = event.target;\n"
		"            const formData = new FormData(form);\n"
		"            const jsonData = {};\n"
		"            formData.forEach((value, key) => {\n"
		"                jsonData[key] = value;\n"
		"            });\n"
		"            const ip = window.location.hostname;\n"
		"            fetch(`http://${ip}/submit`, {\n"
		"                method: 'POST',\n"
		"                headers: {\n"
		"                    'Content-Type': 'application/json'\n"
		"                },\n"
		"                body: JSON.stringify(jsonData)\n"
		"            })\n"
		"            .then(response => response.text())\n"
		"            .then(data => alert(\"Formulaire envoyé avec succès !\"))\n"
		"            .catch(error => alert(\"Erreur lors de l'envoi du formulaire\"));\n"
		"        });\n"
		"    </script>\n"
		"</body>\n"
		"</html>";

	int32_t get_request_send_2_nb_bytes = MX_WIFI_Socket_send(wifi_obj_get(), get_request_sock, (const uint8_t *)web_page_part2, strlen(web_page_part2), 0); // function to send the web page to the client

		if (get_request_send_2_nb_bytes < 0){
				return GET_REQUEST_SENDING_FAILED;
		}

	int32_t post_request_sock = MX_WIFI_Socket_accept(wifi_obj_get(), sock_fd, (struct mx_sockaddr *)remote_host, &addr_len);

		if (post_request_sock < 0 ){
				return SOCKET_ACCEPTING_FAILED;
		}

	static unsigned char post_request_recv_buffer[1000]; // create a buffer to stock the response
	memset((void*)post_request_recv_buffer, 0, sizeof(post_request_recv_buffer)); // Clear the buffer

	memset((void*)print_on_uart, 0, sizeof(print_on_uart)); // Clear the buffer

	int32_t post_request_recv_nb_bytes = MX_WIFI_Socket_recv(wifi_obj_get(), post_request_sock, (uint8_t *)post_request_recv_buffer, 1000, 0); // function to receive the client's request

    memcpy(print_on_uart, post_request_recv_buffer, sizeof(post_request_recv_buffer)); // Copier le contenu

		if (post_request_recv_nb_bytes < 0){
				return GET_REQUEST_RECEIVING_FAILED;
		}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//	The following code is used to parse the JSON receipted from the client using the cJSON library 	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////

		// json_start
	char* json_start = strstr((char*)post_request_recv_buffer, "\r\n\r\n"); // find in get_request_recv_buffer the \r\n\r\n characters and send a pointer before these characters
	if (json_start == NULL) {
		return JSON_PARSE_ERROR;
	}

	json_start += 4; // Past the four first characters (\r\n\r\n)

	cJSON* json = cJSON_Parse(json_start); // Entering the JSON char in the library object to use the tools
	if (json == NULL) {
		return JSON_PARSE_ERROR;
	}

	// Extract values with the library tools
	cJSON* json_apn = cJSON_GetObjectItemCaseSensitive(json, "apn");
	cJSON* json_language = cJSON_GetObjectItemCaseSensitive(json, "language");
	cJSON* json_dca_nrj = cJSON_GetObjectItemCaseSensitive(json, "dca_nrj");

	// Checking if the parse worked
	if (cJSON_IsString(json_apn) && (json_apn->valuestring != NULL) &&
		cJSON_IsString(json_language) && (json_language->valuestring != NULL) &&
		cJSON_IsString(json_dca_nrj) && (json_dca_nrj->valuestring != NULL)) {

		T_config config_write;

		memcpy(config_write.apn,json_apn->valuestring,sizeof(config_write.apn));
		memcpy(config_write.language,json_language->valuestring,sizeof(config_write.language));				// concatenate all the strings in an unique structure to write it in the flash
		memcpy(config_write.dca_nrj,json_dca_nrj->valuestring,sizeof(config_write.dca_nrj));

		int8_t writing_status = write_string_to_flash( &config_write);
		if( writing_status < 0) {
			return writing_status;
		}

		read_string_from_flash();
	}

	cJSON_Delete(json); 	// Delete the JSON object and the associate value to free some space

	app_status = MX_WIFI_Socket_close(wifi_obj_get(), sock_fd); // Closing socket function

	if (app_status != MX_WIFI_STATUS_OK){
		return SOCKET_CLOSING_FAILED;
	}

	return APP_OK;
}
