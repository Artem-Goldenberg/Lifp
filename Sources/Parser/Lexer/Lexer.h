#ifndef Lexer_h
#define Lexer_h

//#define MaxIdentifierLength 4095
#define MaxIdentifierLength 31

/// Main object for the lexing interface
///
/// ``Lexer`` is a partial object, user can access it only through passing pointers to it to the
/// functions defined in this header
typedef struct Lexer Lexer;

/// Creates and initializes all necessary data structures needed for lexing
///
/// - Warning: You have to call this method before calling anything else!
void initializeLexing(void);

/// Use to create a new lexer object on the given file
///
/// - Parameter info: Program information, including the source text for lexing
Lexer* newLexer(ProgramInfo* info);

/// After creating a new lexer, use this function to retrieve tokens on by one from the file you gave to lexer
Token getToken(Lexer* lexer);

int getInteger(const Lexer* lexer);
float getReal(const Lexer* lexer);
ID getIdentifier(const Lexer* lexer);

Location getTokenStartLocation(const Lexer* lexer);
Location getCurrentLocation(const Lexer* lexer);

#endif /* Lexer_h */
