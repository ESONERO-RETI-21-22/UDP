/*
 ============================================================================
 Name        : function.c
 Author      : Alberto Ricchiuti, Beatrice Scavo
 Description : TCP program
 ============================================================================
 */
#include "function.h"

short  numericalControl(char num[MAX_LENGTH]){ //checks the numbers entered for the operation
	short ctrl=1, i=0;

	if(strlen(num)>(MAX_LENGTH-1) || strlen(num)==0){
		ctrl=0;
		puts("\n- This number is too big!\n\n");
	}

	while(ctrl==1 && i<strlen(num)){
		if(isdigit(num[i]) || (i==0 && (num[i]=='-' || num[i]=='+') && strlen(num)>1)){
			i++;
		} else {
			ctrl=0;
		}
	}

	return ctrl;
}

short analyzeInput(char operator, char num1[MAX_LENGTH], char num2[MAX_LENGTH]){ //checks the operation entered in input
	short ctrl=1;

	if((operator!='+' && operator!='-' && operator!='*' && operator!='/') || numericalControl(num1)==0 || numericalControl(num2)==0){

		if(operator=='='){
			ctrl=-1;
		} else {
			ctrl=0;
		}

	}

	return ctrl;
}

short getInput(question *op){ //takes the operation to be transmitted to the server
	short flag=1, ctrl=1;
	char operator='0';
	char num1[MAX_LENGTH]={"0"}, num2[MAX_LENGTH]={"0"};

	do{
		operator='\0';
		memset(&num1, '\0', sizeof(num1));
		memset(&num2, '\0', sizeof(num2));

		fflush(stdin);
		scanf("%c %s %s", &operator, num1, num2);

		ctrl=analyzeInput(operator, num1, num2); //checks if the operation is valid

		if(ctrl==0){ //if the input is not valid, it will be entered again
			puts("\n- Insertion error\n\n  Try again. . . \n\n");
			puts("\nInsert the operation below:\n\n\t");
		} else if(ctrl==-1){ //if the operator is '=', the connection will end with flag returned
			puts("\n- Closing connection\n\n");
			flag=0;
		}
	}while(ctrl==0);

	if(flag==1){
		op->operator=operator;
		op->number1=htonl(atoi(num1));
		op->number2=htonl(atoi(num2));
	} else {
		op->operator=operator;
	}

	return flag;
}

void splitParam(char param[], char result[], short pos){ //splits parameters for the server address

	short count=1, i=0, j=0, len=strlen(param);

	for(i=0; i<len; i++){
		if(count==pos){
			result[j]=param[i];
			j++;
		}
		if(param[i]==':'){
			count++;
		}
	}
	if(result[strlen(result)-1]==':'){
		result[strlen(result)-1]='\0';
	}
}
