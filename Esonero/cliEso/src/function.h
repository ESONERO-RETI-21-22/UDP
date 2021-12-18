/*
 ============================================================================
 Name        : function.h
 Author      : Alberto Ricchiuti, Beatrice Scavo
 Description : UDP program
 ============================================================================
 */
#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "protocol.h"
#include <ctype.h>

short getInput(question *op); //takes the operation to be transmitted to the server
short numericalControl(char num[MAX_LENGTH]); //checks the numbers entered for the operation
short analyzeInput(char operator, char num1[MAX_LENGTH], char num2[MAX_LENGTH]); //checks the operation entered in input
void splitParam(char param[], char result[], short pos); //splits parameters for the server address
#endif /* FUNCTION_H_ */
