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

typedef struct Expr {
	primType type;
	union {
		long integer;
		bool boolean;
		char character;
		char* string;
		char* identifier;
		char* bottom;
		struct {
			struct Expr* head;
			struct Expr* tail;
		} pair;
	} data;
} Expr;

/**
 * These are static objects, they must be created with
 * initStaticObjects before use.
 */
Expr* boolT;
Expr* boolF;
Expr* nil;
//ExprMap* identifiers;
void initStaticObjects(void);


/**
 * The following functions instantiate Exprs of their respective types.
 */
Expr* newCharacter(char value);
Expr* newError(char* error);
Expr* newIdentifier(char* name);
Expr* newInteger(long value);
Expr* newPair(Expr* head, Expr* tail);
Expr* newString(char* value);

/**
 * The following functions identify the type of an Expr.
 */
bool isBoolean(Expr* e);
bool isCharacter(Expr* e);
bool isError(Expr* e);
bool isIdentifier(Expr* e);
bool isInteger(Expr* e);
bool isNil(Expr* e);
bool isPair(Expr* e);
bool isString(Expr* e);
