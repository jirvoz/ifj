#include "scanner.h"

int main () {
	FILE* my_source;
	my_source=fopen("sample", "r");
	string* tmp_string;
	strInit(tmp_string);

	enum tokens type;

	getNextToken(tmp_string, &type, my_source);

	printf("%s", tmp_string->str);

	return 0;
}