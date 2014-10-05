#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <exprmap.h>

// ◥▶◀◤ \\

Expr* head(Expr* e) {
	if (isPair(e)) {
		return e->data.pair.head;
	} else {
		fprintf(stderr, "Cannot get head of non-pair\n");
		exit(1);
	}
}

Expr* tail(Expr* e) {
	if (isPair(e)) {
		return e->data.pair.tail;
	} else {
		fprintf(stderr, "Cannot get tail of non-pair\n");
		exit(1);
	}
}

typedef struct Environment {
	struct Environment* parent;
	ExprMap* identifiers;
} Environment;

Environment* newEnvironment(Environment* parent) {
	Environment* env;
	
	env = malloc(sizeof(Environment));
	if (env == NULL) {
		return NULL;
	}
	env->identifiers = newExprMap(100);
	if (env->identifiers == NULL) {
		free(env);
		return NULL;
	} else {
		return env;
	}
}

bool lookupIdent(Environment* env, char* key, Expr* outputBuffer) {
	ExprMap* currMap;
	Environment* currEnv;
	currMap = env->identifiers;
	if (getExpr(currMap, key, outputBuffer)) {
		return true;
	}

	while (currEnv->parent) {
		currEnv = currEnv->parent;
		currMap = env->identifiers;
		if (getExpr(currMap, key, outputBuffer)) {
			return true;
		}
	}
	return false;
}

bool addIdent(Environment* env, char* key, Expr* expr) {
	ExprMap* currMap;

	currMap = env->identifiers;

	// Add identifier to the lookup table.
	putExpr(currMap, key, expr);
}

// Global Environment
Environment* globalEnv;

void addIdentifier(Expr* expr, Expr* val) {
	char* key;

	if (isIdentifier(expr)) {
		key = expr->data.identifier;
	} else {
		fprintf(stderr, "ERROR: Expr of addIdentifier must be an identifier.\n");
		exit(1);
	}
	// Add identifier to the lookup table.
	addIdent(globalEnv, key, val);
}

Expr* eval(Expr* input);

Expr* readExpr(FILE* stream);

Expr* readInteger(FILE* stream);

Expr* readCharacter(FILE* stream);

Expr* readString(FILE* stream);

Expr* readIdentifier(FILE* stream);

Expr* readPair(FILE* stream);

void trimWhitespace(FILE* stream);

bool taggedWith(Expr* Expr, char* tag);

void print(Expr* prgm);

char peek(FILE* stream)
{
	char c;
	c = getc(stream);
	ungetc(c, stream);
	return c;
}

int main()
{
	initStaticObjects();
	globalEnv = newEnvironment(NULL);

	printf("Welcome to CAN-Scheme. \n⤿ ");

	while(1)
	{
		print(eval(readExpr(stdin)));
		printf("\n⤿ ");
	}

	return 0;
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

bool isIdentChar(char c) {
	return isalpha(c)
		|| c == '+'
		|| c == '-'
		|| c == '!'
		|| c == '?'
		|| c == '*'
		|| c == '/';
}

bool selfEvaluating(Expr* expr) {
	return isBoolean(expr)
		|| isCharacter(expr)
		|| isError(expr)
		|| isInteger(expr)
		|| isString(expr);
}

bool taggedWith(Expr* input, char* tag) {
	Expr* h;
	
	if (isPair(input)) {
		h = head(input);
		return isIdentifier(h)
			&& (strcmp((*h).data.identifier, tag) == 0);
	} else {
		return false;
	}
}

Expr* eval(Expr* input) {
	Expr* val = malloc(sizeof(Expr));
	char* key;
	if (selfEvaluating(input)) {
		return input;
	} else if (taggedWith(input, "quote")) {
		return head(tail(input));
	} else if (isIdentifier(input)) {
		key = input->data.identifier;
		if (lookupIdent(globalEnv, key, val)) {
			return val;
		} else {
			return newError("Unbound identifier.");
		}
	} else {
		return newError("Cannot evaluate expression.");
	}
}

void printList(Expr* list) {
		print(head(list));
	if (isNil(tail(list))) {
		return;
	} else if (isPair(tail(list))) {
		printf(" ");
		printList(tail(list));
	} else {
		printf(" . ");
		print(tail(list));
	}
}

void print(Expr* prgm) {
	Expr* cur = prgm;

	if (cur->type == Integer) {
		printf("%ld", (*cur).data.integer);
	} else if (cur->type == Boolean) {
		if (cur == boolT) {
			printf("#t");
		} else if (cur == boolF) {
			printf("#f");
		} else {
			printf("Unexpected boolean\n");
		}
	} else if (cur->type == Character) {
		printf("#'%c", (*cur).data.character);
	} else if (cur->type == String) {
		printf("\"%s\"", (*cur).data.string);
	} else if (cur->type == Identifier) {
		printf("%s", (*cur).data.identifier);
	} else if (cur->type == Pair) {
		printf("(");
        printList(cur);
		printf(")");
	} else if (cur->type == Nil) {
		printf("()");
	} else if (cur->type == Error) {
		printf("Error: %s", cur->data.bottom);
	} else {
		printf("Error: Unknown expression type.");
	}
}

int parseInt(char* intString)
{
	int base = 10;
	char* leftover;

	return strtol(intString, &leftover, base);
}

Expr* readExpr(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char c;
	Expr* expr;
	char* s = malloc(size);

	trimWhitespace(stream);

	c = getc(stream);

	if (isdigit(c) || c == '-') {
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
	} else if (isIdentChar(c)) {
		ungetc(c, stream);
		expr = readIdentifier(stream);
	} else {
		sprintf(s, "Unexpected character '%c'", c);
		expr = newError(s);
	}
	return expr;
}

Expr* readInteger(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char* num = malloc(size);
	Expr* result = malloc(sizeof(Expr));
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

Expr* readCharacter(FILE* stream) {
	char character;
	Expr* result = malloc(sizeof(Expr));
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

Expr* readString(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char* string = malloc(size);
	Expr* result = malloc(sizeof(Expr));
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

Expr* readIdentifier(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char* string = malloc(size);
	Expr* result = malloc(sizeof(Expr));
	char c;
	int pos = 0;
	
	c = fgetc(stream);
	while (isIdentChar(c)) 
	{
		string[pos++] = c;
		if (pos == sizeof(string))
		{
			realloc(string, sizeof(string) + size);
		}
		c = fgetc(stream);
	}
	ungetc(c, stream);
	
	result = newIdentifier(string);

	addIdentifier(result, result);
	
	return result;
}

Expr* readPair(FILE* stream) {
	Expr* h = malloc(sizeof(Expr));
	Expr* t = malloc(sizeof(Expr));
	Expr* result = malloc(sizeof(Expr));
	char c;

	c = getc(stream);
	if (c == ')') {
		return nil;
	}
	ungetc(c, stream);

	h = readExpr(stream);
	trimWhitespace(stream);
	
	c = fgetc(stream);
	if (c == '.') {
		trimWhitespace(stream);
		t = readExpr(stream);
		c = getc(stream);
		if (c != ')') {
			result = newError("Expected character ')'");
		} else {
			result = newPair(h, t);
		}
	} else {
		ungetc(c, stream);
		t = readPair(stream);
		result = newPair(h, t);
	}
	
	return result;
}

