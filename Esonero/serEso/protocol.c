/*
 ============================================================================
 Name        : protocol.c
 Author      : Alberto Ricchiuti, Beatrice Scavo
 Description : UDP program
 ============================================================================
 */

#include "protocol.h"

void errorhandler(char *error_message) {
	printf("%s", error_message);
}

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}
