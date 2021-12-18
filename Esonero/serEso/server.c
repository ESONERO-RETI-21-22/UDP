/*
 ============================================================================
 Name        : server.c
 Author      : Alberto Ricchiuti, Beatrice Scavo
 Description : UDP program
 ============================================================================
 */
#include "protocol.h"
#include "function.h"



int main(int argc, char *argv[]) {

	setvbuf(stdout, NULL, _IONBF, 0); //output buffer cleaner

	question op={'\0', 0, 0};
	answer res={"\0", 0};
	short flag=1;

#if defined WIN32	//initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != NO_ERROR) {
		errorhandler("\n- Error at WSAStartup()\n\n");
		system("pause");
		return 0;
	}
#endif

	int my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); //creating socket
	if (my_socket < 0) {
		errorhandler("\n- Socket creation failed\n\n");
		clearwinsock();
		system("pause");
		return -1;
	}

	//address assignment
	struct sockaddr_in sad;
	struct in_addr* ina;
	struct hostent *host;
	struct in_addr addr;
	memset(&addr, 0, sizeof(addr));
	int port= PORT;
	host=gethostbyname(PROTIP);
	ina = (struct in_addr*) host->h_addr_list[0];
	memset(&sad, 0, sizeof(sad)); //ensures that extra bytes contain 0
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(inet_ntoa(*ina));
	sad.sin_port = htons(port); /*converts values between the host and
	 	 	 	 	 	 	 	 network byte order. Specifically, htons() converts 16-bit quantities
	 	 	 	 	 	 	 	 from host byte order to network byte order.*/

	if (bind(my_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		errorhandler("\n- bind() failed\n\n"); //binding socket and address
		closesocket(my_socket);
		clearwinsock();
		system("pause");
		return -1;
	}


	//accepting a new connection
	struct sockaddr_in cad; // structure for the client address
	memset(&cad, 0, sizeof(cad));
	int client_len = sizeof(cad); // set the size of the client
	printf("\n- SERVER-SIDE -\n\n\n- Waiting for a client to connect...\n\n");
	while (1) { //server never ends its process, always waiting for client

		while(flag==1){ //as long as the client doesn't close the connection, the server will respond to its requests
			if ((recvfrom(my_socket, &op, sizeof(op), 0, (struct sockaddr*)&cad, &client_len )) <= 0) { //server receives the client requests
				errorhandler("\n- recvfrom() failed or packet received from unknown source\n\n");
				closesocket(my_socket);
				clearwinsock();
				system("pause");
				return -1;
			}

			flag=executeOp(&op,&res); //calculating the operation's result

			if(flag==1){
				memset(&addr, 0, sizeof(addr));
				addr.s_addr = inet_addr(inet_ntoa(cad.sin_addr));
				memset(&host,0,sizeof(host));
				host = gethostbyaddr((char *) &addr, 4, AF_INET);
				printf("Operation '%c %d %d' requested by the client %s, ip %s\n\n",op.operator,op.number1,op.number2, host->h_name, inet_ntoa(cad.sin_addr));
				if ((sendto(my_socket, &res, sizeof(res), 0, (struct sockaddr*)&cad, sizeof(cad))) <= 0) { //sending the response to the client
					errorhandler("\n- sendto() sent a different number of bytes than expected\n\n");
					closesocket(my_socket);
					clearwinsock();
					system("pause");
					return -1;
				}
			}
			memset(&op,0,sizeof(op)); //cleaning the values of the structs used for trasmission
			memset(&res,0,sizeof(res));
		}

		flag=1; //preparation for the next client

	}


	puts("\n- Terminating Service\n\n"); //terminating process
	closesocket(my_socket);
	clearwinsock();
	system("pause");
	return 0;
}



