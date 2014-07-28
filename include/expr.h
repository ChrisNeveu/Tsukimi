typedef enum { false, true } bool;

typedef enum {
	Boolean,
	Character,
	Error,
	Identifier,
	Integer,
	Nil,
	Pair,
	String
} primType;

typedef struct expr {
	primType type;
	union {
		long integer;
		bool boolean;
		char character;
		char* string;
		char* identifier;
		char* bottom;
		struct {
			struct expr* head;
			struct expr* tail;
		} pair;
	} data;
} expr;

/**
 * These are static objects, they must be created with
 * initStaticObjects before use.
 */
expr* boolT;
expr* boolF;
expr* nil;
StrMap* identifiers;
void initStaticObjects(void);


/**
 * The following functions instantiate exprs of their respective types.
 */
expr* newCharacter(char value);
expr* newError(char* error);
expr* newIdentifier(char* name);
expr* newInteger(long value);
expr* newPair(expr* head, expr* tail);
expr* newString(char* value);

/**
 * The following functions identify the type of an expr.
 */
bool isBoolean(expr* e);
bool isCharacter(expr* e);
bool isError(expr* e);
bool isIdentifier(expr* e);
bool isInteger(expr* e);
bool isNil(expr* e);
bool isPair(expr* e);
bool isString(expr* e);
