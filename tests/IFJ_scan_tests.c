
/* *******************************(IFJ 2017)********************************* */
/*  Course: Formal Languages and Compilers (IFJ) - FIT VUT Brno 2017/18       */
/*  Project: Implementation of the IFJ17 imperative language translator       */
/*                                                                            */
/*  Authors: Tomáš Nereča : xnerec00 : ()% (team leader)                      */
/*           Samuel Obuch : xobuch00 : ()%                                    */
/*           Jiří Vozár   : xvozar04 : ()%                                    */
/*           Ján Farský   : xfarsk00 : ()%                                    */
/* ************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

void TEST01()
{
	my_source=fopen("test1", "r");
	int i = 0;//0-false 1-true

	printf("[TEST01] Basic Test Int,Single Op and EOF\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	getNextToken(tmp_string, &type, my_source);
	if(type==INTEGER)
	{
    getNextToken(tmp_string, &type, my_source);
	    if(type==SINGLE_OPERATOR)
			{
	      getNextToken(tmp_string, &type, my_source);
		      if(type==INTEGER)
					{
	  	        getNextToken(tmp_string, &type, my_source);
	              if(type==SINGLE_OPERATOR)
								{
	                getNextToken(tmp_string, &type, my_source);
		                if(type==INTEGER)
										{
	  	                getNextToken(tmp_string, &type, my_source);
			                  if(type==END_OF_FILE)
												{
                          i = 1;
		                    }
		                }
	              }
		       }
	    }
	}

	if (!i) // scan test failed
		printf("BASIC TEST01 FAILED\n\n");
	else
		printf("TEST01 OK\n\n");
	//printf("%s", tmp_string->str);
	fclose(my_source);
}

void TEST02()
{
	my_source=fopen("test2", "r");
	int i = 0;//0-false 1-true

	printf("[TEST02] Basic Test KEY WORDs\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	while(type!=END_OF_FILE)
	{
	getNextToken(tmp_string, &type, my_source);
	  if(type==KEY_WORD)
		{
			i = 1;
		}
		else
		{
			i = 0;
			break;
		}
	}

	if (!i) // scan test failed
		printf("BASIC TEST02 FAILED\n\n");
	else
		printf("TEST02 OK\n\n");
	//printf("%s", tmp_string->str);
	fclose(my_source);
}

void TEST03()
{
	my_source=fopen("test3", "r");
	int i = 0;//0-false 1-true

	printf("[TEST03] Basic Test RESERVED KEY WORDs\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	while(type!=END_OF_FILE)
	{
	getNextToken(tmp_string, &type, my_source);
	  if(type==RESERVED_KEY_WORD)
		{
			i = 1;
		}
		else
		{
			i = 0;
			break;
		}
	}

	if (!i) // scan test failed
		printf("BASIC TEST03 FAILED\n\n");
	else
		printf("TEST03 OK\n\n");
	//printf("%s", tmp_string->str);
	fclose(my_source);
}

void TEST04()
{
	my_source=fopen("test4", "r");
	int i = 0;

	printf("[TEST04] Test correct identification \n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	while(type!=END_OF_FILE)
	{
	  getNextToken(tmp_string, &type, my_source);
		if (type==0)
			printf("%d) SINGLE_OPERATOR         %d\n",i ,type);
			else if (type==1)
				printf("%d) DOUBLE_OPERATOR         %d\n",i ,type);
				else if (type==2)
					printf("%d) END_OF_FILE             %d\n",i ,type);
					else if (type==3)
						printf("%d) KEY_WORD                %d\n",i ,type);
						else if (type==4)
							printf("%d) RESERVED_KEY_WORD       %d\n",i ,type);
							else if (type==5)
								printf("%d) IDENTIFIER              %d\n",i ,type);
								else if (type==6)
									printf("%d) STRING                  %d\n",i ,type);
									else if (type==7)
										printf("%d) INTEGER                 %d\n",i ,type);
										else if (type==8)
											printf("%d) FLOATING_POINT          %d\n",i ,type);
											else if (type==9)
												printf("%d) FLOATING_POINT_EXPONENT %d\n",i ,type);
												else
												{
												  printf("TEST04 UNKNOW TOKEN\n\n");
													break;
												}
	  i++;
	}
	fclose(my_source);
}

int main (int argc, char* argv[])
{
  FILE* my_source;
  string* tmp_string;
  strInit(tmp_string);
  enum tokens type;

  printf ("---------------------IFJ Scanner - Tests-----------------------\n");
  printf ("---------------------------------------------------------------\n\n");

	TEST01();
	TEST02();
	TEST03();
	TEST04();

  printf("\n----- Scanner - The End of Tests -----\n");

  return(0);
}
