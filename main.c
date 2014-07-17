#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ◥▶◀◤

typedef enum { false, true } bool;

/*
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
*/

typedef enum { Integer, Boolean, Character, String, Pair, Nil, Error } primType;

typedef struct expr {
	primType type;
	union {
		long integer;
		bool boolean;
		char character;
		char* string;
		char* bottom;
        struct {
            struct expr* head;
            struct expr* tail;
        } pair;
	} data;
} expr;

expr* boolT;
expr* boolF;
expr* nil;

expr* newInteger(long value) {
	expr* expr = malloc(sizeof(expr));

	(*expr).type = Integer;
	(*expr).data.integer = value;
	return expr;
}

expr* newCharacter(char value) {
	expr* expr = malloc(sizeof(expr));

	(*expr).type = Character;
	(*expr).data.character = value;
	return expr;
}

expr* newString(char* value) {
	expr* expr = malloc(sizeof(expr));

	(*expr).type = String;
	(*expr).data.string = value;
	return expr;
}

expr* newPair(expr* head, expr* tail) {
	expr* expr = malloc(sizeof(expr));

	(*expr).type = Pair;
	(*expr).data.pair.head = head;
	(*expr).data.pair.tail = tail;
	return expr;
}

expr* newError(char* error) {
	expr* expr = malloc(sizeof(expr));

	(*expr).type = Error;
	(*expr).data.bottom = error; /*  */
	return expr;
}

bool isInteger(expr* expr) {
	return (*expr).type == Integer;
}

bool isBoolean(expr* expr) {
	return (*expr).type == Boolean;
}

bool isCharacter(expr* expr) {
	return (*expr).type == Character;
}

bool isPair(expr* expr) {
	return (*expr).type == Pair;
}

bool isNil(expr* expr) {
	return (*expr).type == Nil;
}

bool isError(expr* expr) {
	return (*expr).type == Error;
}

void initialize(void) {
	boolT = malloc(sizeof(expr));
	(*boolT).type = Boolean;
	(*boolT).data.boolean = true;
	
	boolF = malloc(sizeof(expr));
	(*boolF).type = Boolean;
	(*boolF).data.boolean = false;
	
	nil = malloc(sizeof(expr));
	(*nil).type = Nil;
}

expr* eval(expr* input);
/*
cell* readCell(FILE* stream);

cell* readInt(FILE* stream);

cell* readList(FILE* stream);

cell* readIdentifier(FILE* stream);
*/

expr* readExpr(FILE* stream);

expr* readInteger(FILE* stream);

expr* readCharacter(FILE* stream);

expr* readString(FILE* stream);

expr* readPair(FILE* stream);

void trimWhitespace(FILE* stream);

bool isDigit(char c);

void print(expr* );

char peek(FILE* stream)
{
	char c;
	c = getc(stream);
	ungetc(c, stream);
	return c;
}

int main()
{
	char* stmnt;

	initialize();

	printf("Welcome to CAN-Scheme. \n⤿ ");

	while(1)
	{
		print(readExpr(stdin));
		printf("\n⤿ ");
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
	} while (c == ' ' || c =='\n' || c == EOF);
	ungetc(c, stream);
}

expr* eval(expr* input) {
	return input;
}

void print(expr* prgm) {
	expr* cur = prgm;

	if ((*cur).type == Integer) {
		printf("%ld", (*cur).data.integer);
	} else if ((*cur).type == Boolean) {
		if (cur == boolT) {
			printf("#t");
		} else if (cur == boolF) {
			printf("#f");
		} else {
			printf("Unexpected boolean\n");
		}
	} else if ((*cur).type == Character) {
		printf("#'%c", (*cur).data.character);
	} else if ((*cur).type == String) {
		printf("\"%s\"", (*cur).data.string);
	} else if ((*cur).type == Pair) {
		printf("(");
        print((*cur).data.pair.head);
		printf(" . ");
        print((*cur).data.pair.tail);
		printf(")");
	} else if ((*cur).type == Nil) {
		printf("()");
	} else if ((*cur).type == Error) {
		printf("Error: %s", (*cur).data.bottom);
	} else {
		printf("Error: Unknown expression type.\n0");
	}
}

int parseInt(char* intString)
{
	int base = 10;
	char* leftover;

	return strtol(intString, &leftover, base);
}

expr* readExpr(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char c;
	expr* expr;
	char* s = malloc(size);

	trimWhitespace(stream);

	c = getc(stream);

	if (isDigit(c) || c == '-') {
		ungetc(c, stream);
		expr = readInteger(stream);
	} else if (c == '#') {
		c = getc(stream);
		if (c == 't') {
			expr = boolT;
		} else if (c == 'f') {
			expr = boolF;
		} else if (c == '\'') {
			expr = readCharacter(stream);
		} else {
			sprintf(s, "Unexpected character '%c', expecting 't' or 'f'.", c);
			expr = newError(s);
		}
	} else if (c == '"') {
		expr = readString(stream);
	} else if (c == '(') {
		expr = readPair(stream);
	} else {
		sprintf(s, "Unexpected character '%c'", c);
		expr = newError(s);
	}
	return expr;
}

expr* readInteger(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char* num = malloc(size);
	expr* result = malloc(sizeof(expr));
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
    ungetc(c, stream);

	result = newInteger(parseInt(num));
	return result;
}

/*maybe*/char readEscape(FILE* stream) {
	char character;
	char c;
	
	c = fgetc(stream);
	if (c == 'n') {
		character = '\n';
	} else if (c == 'a') {
		character = '\a';
	} else if (c == 'b') {
		character = '\b';
	} else if (c == 'f') {
		character = '\f';
	} else if (c == 't') {
		character = '\t';
	} else if (c == '\\') {
		character = '\\';
	} else if (c == 's') {
		character = ' ';
	} else {
		return 0;
	}
	return character;
}

expr* readCharacter(FILE* stream) {
	char character;
	expr* result = malloc(sizeof(expr));
	char c;
	
	c = fgetc(stream);
	if (c == '\\') {
		character = readEscape(stream);
		if (!character) {
			return newError("Invalid escape sequence");
		}
	} else {
		character = c;
	}
	result = newCharacter(character);
	return result;
}

expr* readString(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char* string = malloc(size);
	expr* result = malloc(sizeof(expr));
	char c;
	int pos = 0;
	
	c = fgetc(stream);
	while (c != '"') 
	{
		if (c == '\\') {
			c = readEscape(stream);
			if (!c) {
				// Consume the remainder of the string.
				while (c != '"') {
					c = fgetc(stream);
				}
				return newError("Invalid escape sequence in string");
			}
		}
		string[pos++] = c;
		if (pos == sizeof(string))
		{
			realloc(string, sizeof(string) + size);
		}
		c = fgetc(stream);
	}
	
	result = newString(string);
	return result;
}

expr* readPair(FILE* stream) {
	expr* head = malloc(sizeof(expr));
	expr* tail = malloc(sizeof(expr));
	expr* result = malloc(sizeof(expr));
	char c;

    c = getc(stream);
    if (c == ')') {
        return nil;
    }
    ungetc(c, stream);

    head = readExpr(stream);
    trimWhitespace(stream);
	
	c = fgetc(stream);
	if (c == '.') {
        trimWhitespace(stream);
        tail = readExpr(stream);
        result = newPair(head, tail);
    } else {
        ungetc(c, stream);
        tail = readPair(stream);
        result = newPair(head, tail);
    }
	
	return result;
}

/*
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
*/
/*
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
*/
/*
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
*/
/*
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
*/

