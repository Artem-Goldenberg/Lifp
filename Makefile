
includes = External . Parser Parser/Syntax Parser/Lexer Parser/Trie \
	Parser/StringTable

pre:
	gcc -E $(addprefix -I, $(includes)) Parser/Syntax/ignore.c
