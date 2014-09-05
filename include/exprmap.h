#include <stdlib.h>
#include <string.h>
#include <expr.h>

typedef struct ExprMap ExprMap;

/**
 * Creates a new  ExprMap.
 *
 * @param capacity The number of top-level slots this string map
 * should allocate. This parameter must be > 0.
 *
 * @returns A pointer to a string map object, 
 * or null if a new string map could not be allocated.
 */
ExprMap* newExprMap(size_t capacity);

/**
 * Releases all memory held by an ExprMap
 *
 * @param map: A pointer to an ExprMap. This parameter cannot be null.
 * If the supplied ExprMap has been previously released, the
 * behaviour of this function is undefined.
 *
 * @returns void
 */
void deleteExprMap(ExprMap* map);

/**
 * Returns the value associated with the supplied key.
 *
 * @param map A pointer to an ExprMap. This parameter cannot be null.
 * @param key A pointer to a null-terminated C string. This parameter cannot
 * be null.
 * @param outputBuffer A pointer to an output buffer which will contain the value,
 * if it exists.
 *
 * @returns True if the queried expression was copied to the outputBuffer.
 */
bool getExpr(ExprMap* map, char* key, Expr* outputBuffer);

/**
 * Queries the existence of a key.
 *
 * @param map The ExprMap to be operated on.
 * @param key The key to look up. This must be an identifier.
 *
 * @returns Whether the key exists.
 */
bool exprExists(ExprMap* map, char* key);

/**
 * Associates a value with the supplied key. If the key is already
 * associated with a value, the previous value is replaced.
 *
 * @param map The ExprMap to be operated on.
 * @param key A pointer to a null-terminated C string. This parameter
 * cannot be null. The string must have a string length > 0. The
 * string will be copied.
 * @param value A pointer to an Expr.
 *
 * @returns Whether the association succeeded.
 */
bool putExpr(ExprMap* map, char* key, Expr* value);
