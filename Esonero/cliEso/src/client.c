/*
 ============================================================================
 Name        : client.c
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
	int port=0;
	char addressString[16]={"\0"}, portString[8]={"\0"};
	struct in_addr* ina;
	struct hostent *host;

#if defined WIN32 //initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != NO_ERROR) {
		errorhandler("- Error at WSAStartup()\n\n");
		system("pause");
		return 0;
	}
#endif

	if (argc > 1) { //if parameters are specified they will be checked and possibly used as address

		splitParam(argv[1], addressString, 1);
		splitParam(argv[1], portString, 2);
		if(numericalControl(portString)==1 && atoi(portString)>0 && atoi(portString)<65536){
			host = gethostbyname(addressString);
			if (host == NULL) {
				printf("\n - gethostbyname() failed\n");
				printf("\n - Default parameters will be used\n");
				host=gethostbyname(PROTIP);
				ina = (struct in_addr*) host->h_addr_list[0];
				port=PORT;
			} else {
				ina = (struct in_addr*) host->h_addr_list[0];
				printf("Result of gethostbyname(%s): %s\n", addressString, inet_ntoa(*ina));
				port=atoi(portString);
			}
		}else{
			printf("\n - Port number error\n");
			printf("\n - Default parameters will be used\n");
			host=gethostbyname(PROTIP);
			ina = (struct in_addr*) host->h_addr_list[0];
			port=PORT;
		}
	} else {
		host=gethostbyname(PROTIP);
		ina = (struct in_addr*) host->h_addr_list[0];
		port=PORT;
	}

	int my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); //socket creation
	if (my_socket < 0) {
		errorhandler("- Socket creation failed\n\n");
		closesocket(my_socket);
		clearwinsock();
		system("pause");
		return -1;
	}

	struct sockaddr_in sad; //creating server's address
	memset(&sad, 0, sizeof(sad));
	struct sockaddr_in fromAddr;
	memset(&fromAddr, 0, sizeof(fromAddr));
    int fromSize = sizeof(fromAddr);
	sad.sin_family = PF_INET;
	sad.sin_addr.s_addr = inet_addr(inet_ntoa(*ina)); //server's ip
	sad.sin_port = htons(port); //server's port


	puts("- ONLINE CALCULATOR -\n\n Calculator for operations between two INTEGERS\nAllowed operations:\n'+'\t'-'\t'*'\t'/'\n");
	puts("Input format required:\toperator number1 number2   [Press Enter]\n\nPress Space Bar for separating the input operation elements.\n"
			"If pressing Enter before completing the input the calculator will wait for the remaining part, \nthen press Enter button again.\n\n");
	puts("Entering as operator '=' and then the other two numbers will close the program and the connection.\n\n\n\nEx:\t*  3  4  [Enter]\n\n\tor\t =  4  1  [Enter]");
	while(flag==1){ //until user's operator input is not '=' connection and operation's request will continue
		puts("\n\n\nInsert the operation below:\n\n\t");
		flag=getInput(&op);
		if(flag==1){

			if ((sendto(my_socket, &op, sizeof(op), 0, (struct sockaddr*)&sad, sizeof(sad)) != sizeof(op))) { //sending operation to the server
				errorhandler("\n- sendto() sent a different number of bytes than expected\n\n");
				closesocket(my_socket);
				clearwinsock();
				system("pause");
				return -1;
			}

			if ((recvfrom(my_socket, &res, sizeof(res), 0, (struct sockaddr*)&fromAddr, &fromSize)) <= 0 || sad.sin_addr.s_addr != fromAddr.sin_addr.s_addr) { //receiving server'response for the user
				errorhandler("\n- recvfrom() failed or packet received from unknown source\n\n");
				closesocket(my_socket);
				clearwinsock();
				system("pause");
				return -1;
			}

			res.failedOp=ntohl(res.failedOp); //printing calculation outcome
			op.number1=ntohl(op.number1);
			op.number2=ntohl(op.number2);
			if(res.failedOp==0){
				printf("\n\nResult received from server %s, ip %s : %d %c %d = %s\n\n", host->h_name, inet_ntoa(*ina), op.number1,op.operator,op.number2, res.result);
			} else {
				puts("\nOperation not allowed!\n\n");
			}

		} else {
			if ((sendto(my_socket, &op, sizeof(op), 0, (struct sockaddr*)&sad, sizeof(sad)) != sizeof(op))) { //sending operation to the server
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



	puts("\nTerminating process\n\n");	//process ends
	closesocket(my_socket);
	clearwinsock();
	system("pause");
	return 0;
}
