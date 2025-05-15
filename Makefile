sources = \
	Sources/Lifp.c \
	Sources/Utils/Issue.c Sources/Utils/Allocator.c \
	Sources/Interpreter/Interpreter.c \
	Sources/Syntax/Syntax.c \
	Sources/Parser/Token.c \
	Sources/Parser/Lexer/Lexer.c \
	Sources/Parser/StringTable/StringTable.c \
	Sources/Parser/Trie/Trie.c \
	build/Lifp.tab.c

lexer.sources = \
	Sources/Lifp.c \
	Sources/Utils/Issue.c Sources/Utils/Allocator.c \
	Sources/Parser/Token.c \
	Sources/Parser/Lexer/Lexer.c \
	Sources/Parser/StringTable/StringTable.c \
	Sources/Parser/Trie/Trie.c

headerDirs = Sources \
	Sources/Utils Sources/CodeGeneration \
	Sources/Interpreter Sources/Syntax Sources/Parser \
	Sources/Parser/Lexer Sources/Parser/StringTable Sources/Parser/Trie \
	build External

headers = $(foreach folder, $(headerDirs), $(wildcard $(folder)/*.h))

CFLAGS += -std=gnu17
CFLAGS += -g -Wall
CFLAGS += -Wno-unused-function

CPPFLAGS += -DLifpDebug  # Save strings, token infos, etc...

# link time optimization for inlining functions in other translation units, disabled for now
#CFLAGS += -flto

eval: evaluate.c $(sources) $(headers) | build
	$(LINK.c) evaluate.c $(sources) $(addprefix -I, $(headerDirs)) -o $@

parser: parser.c $(sources) $(headers) | build
	$(LINK.c) parser.c $(sources) $(addprefix -I, $(headerDirs)) -o $@

lexer: tokenize.c $(lexer.sources) $(headers) | build
	$(LINK.c) tokenize.c $(lexer.sources) $(addprefix -I, $(headerDirs)) -o $@


build/Lifp.tab.h build/Lifp.tab.c: Sources/Parser/Lifp.y | build
	bison -H $< -o build/Lifp.tab.c

build:
	mkdir $@

clean:
	rm -f parser
	rm -f lexer
	rm -fr build
	rm -fr *.dSYM

