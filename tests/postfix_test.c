#include <stdio.h>
#include "../src/scanner.h"
#include "../src/expressions.h"

FILE* test_file;

int main(int argc, char const *argv[])
{
	test_file = fopen("./postfix_file", "w");
	token_type expected_type = BOOLEAN;
	if (expression(expected_type))
	{
		fclose(test_file);
		return 0;
	}
	else
	{
		fclose(test_file);
		return 1;
	}
}

//this function prints term
void printTerm(tTerm term)
{
	
	switch (term.index)
	{
		case EQ_EXPR_IN:
		{
			fprintf(test_file,"=");
		}
			break;
		case NOT_EQ_IN:
		{
			fprintf(test_file,"!=");
		}
			break;
		case LESS_EQ_IN:
		{
			fprintf(test_file,"<=");
		}
			break;
		case MORE_EQ_IN:
		{
			fprintf(test_file,">=");
		}
			break;
		case LESS_IN:
		{
			fprintf(test_file,"<");
		}
			break;
		case MORE_IN:
		{
			fprintf(test_file,"<=");
		}
			break;
		case PLUS_IN:
		{
			fprintf(test_file,"+");
		}
			break;
		case MINUS_IN:
		{
			fprintf(test_file,"-");
		}
			break;
		case MUL_IN:
		{
			fprintf(test_file,"*");
		}
			break;
		case FLOAT_DIV_IN:
		{
			fprintf(test_file,"/");
		}
			break;
		case INT_DIV_IN:
		{
			fprintf(test_file,"\\");
		}
			break;
		case INT_IN:
		{
			if (term.token.type == INTEGER_TOK)
			{
				fprintf(test_file,"%d", term.token.attribute.number);
			}
			else
			{
				fprintf(test_file,"%s", term.token.attribute.string_ptr);
			}
		}
			break;
		case DOUBLE_IN:
		{
			if (term.token.type == FLOATING_POINT_TOK)
			{
				fprintf(test_file,"%d", term.token.attribute.number);
			}
			else
			{
				fprintf(test_file,"%s", term.token.attribute.string_ptr);
			}
		}
			break;
		case STRING_IN:
		{
			fprintf(test_file,"%s\n", term.token.attribute.string_ptr);
		}
			break;
		case DOLAR_IN:
		{
			fprintf(test_file,"\nFine postfix\n");
		}
		default:
		{
			fprintf(test_file,"\nSomething gone wrong");
		}
	}
}