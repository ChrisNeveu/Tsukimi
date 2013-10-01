#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum { false, true } bool;

typedef struct cell
{
	int carTag;
	union
	{
		char* atom;
		struct cell* list;
	} car;
	struct cell* cdr;
} cell;

void eval(char* statement);

cell* readCell(FILE* stream);

cell* readInt(FILE* stream);

cell* readList(FILE* stream);

cell* readIdentifier(FILE* stream);

void trimWhitespace(FILE* stream);

struct cell* parse(char* str);

bool isDigit(char c);

void print(cell* );

int main()
{
	char* stmnt;

	printf("Welcome to CAN-Scheme. \n--> ");

	while(1)
	{
		print(readCell(stdin));
		printf("\n--> ");
	}

	return 0;
}

bool isDigit(char c)
{
	if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || 
		c == '5' || c == '6' || c == '7' || c == '8' || c == '9')
	{
		return true;
	}
	return false;
}

void trimWhitespace(FILE* stream)
{
	char c;

	do
	{
		c = fgetc(stream);
	} while (c == ' ');
	ungetc(c, stream);
}

void eval(char* input)
{
	/*char** stmnt;

	stmnt = strSplit(stmnt, input, ' ');
	int len = (sizeof(stmnt)/sizeof(stmnt[0]));
	printf("--%i--", len);
	for(int i = 0; i < len; i++)
	{
		printf("%s ", stmnt[i]);	
	}*/
}

cell* readCell(FILE* stream)
{
	char c;

	trimWhitespace(stream);

	c = fgetc(stream);

	if (isDigit(c) || c == '-')
	{
		ungetc(c, stream);
		return readInt(stream);
	}
	else if (c == '(')
	{
		return readList(stream);
	}
	else
	{
		ungetc(c, stream);
		return readIdentifier(stream);
	}
}

void print(cell* prgm)
{
	cell* cur = prgm;

	printf("(");
	while (cur != NULL)
	{
		if ((*cur).carTag)
		{
			print((*cur).car.list);
		}
		else
		{
			printf("%s", (*cur).car);
		}
		cur = (*cur).cdr;
		if (cur != NULL)
		{
			printf(" ");
		}
	}
	printf(")");
}

cell* readInt(FILE* stream)
{
	size_t size = 32 * sizeof(char);
	char* num = malloc(size);
	cell* result = malloc(sizeof(cell));
	char c;
	int pos = 0;
	
	c = fgetc(stream);
	while (c != ' ' && c != ')' && c != '\n') 
	{
		num[pos++] = c;
		if (pos == sizeof(num))
		{
			realloc(num, sizeof(num) + size);
		}
		c = fgetc(stream);
	}

	if (c == ')')
	{
		ungetc(c, stream);
	}

	//result.car = parseInt(num);
	(*result).car.atom = num;
	(*result).carTag = 0;
	return result;
}

cell* readIdentifier(FILE* stream)
{
	size_t size = 32;
	char* ident = malloc(size);
	cell* result = malloc(sizeof(cell));
	char c;
	int pos = 0;

	c = fgetc(stream);
	while (c != ' ' && c != ')' && c != '\n')
	{
		ident[pos++] = c;
		if (pos == sizeof(ident))
		{
			realloc(ident, sizeof(ident) + size);
		}
		c = fgetc(stream);
	}

	if (c == ')')
	{
		ungetc(c, stream);
	}

	(*result).car.atom = ident;
	(*result).carTag = 0;
	return result;
}

cell* readList(FILE* stream)
{
	char c;
	cell* result = malloc(sizeof(cell));
	cell* cur;

	c = fgetc(stream);
	if (c == ')')
	{
		//return nil
	}
	ungetc(c, stream);

	(*result).car.list = readCell(stream);
	(*result).carTag = 1;
	cur = (*result).car.list;

	c = fgetc(stream);
	while (c != ')')
	{
		ungetc(c, stream);
		(*cur).cdr = readCell(stream);
		cur = (*cur).cdr;
		c = fgetc(stream);
	}
	c = fgetc(stream); //Pop off the \n
	return result;
}

int parseInt(char* intString)
{
	int base = 10;
	char* leftover;

	return strtol(intString, &leftover, base);
}

