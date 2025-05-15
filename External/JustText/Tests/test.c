#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "JustText.h"

int main(int argc, const char * argv[]) {
    printf("Running Macro Test: ...\n");
    // simple tests to make sure macros works
#define Empty
#define Comma ,
#define WithComma A,B
#define Id(x) x
#define Codes add, sub, mult, div, call, get, put
#define Index  0,   1,    2,   3,    4,   5,   6

    // test `JTCount`
    _Static_assert(JTCount(Codes) == 7, "JTCount not working");

    // test `JTIf`
    _Static_assert(JTIf(Empty, 1, 2) == 2, "JTIf not working");
    _Static_assert(JTIf(Comma, 1, 2) == 1, "JTIf not working");
    _Static_assert(JTIf(WithComma, 1, 2) == 1, "JTIf not working");


#define DeclareVariable(name) static const int name = 0;

    // static const int add = 0; static const int sub = 0; ...
    JTForEach(DeclareVariable, Codes);

    _Static_assert(JTMap(Id, +, Codes) == 0,
                   "Mapping with plain separator now working");


#define IndexAssignment(i, code) [i] = code
    static const int codes[JTCount(Codes)] = {
        // [0] = add, [1] = sub, ...
        JTZipComma(IndexAssignment, (Index), (Codes))
    };
    (void)codes; // unused

#define EnumMember(code) code##Code
    enum Code {
        // addCode, subCode, ...
        JTMapComma(EnumMember, Codes)
    };

    // make sure basic string is working, it should be able to accept multiple arguments
    _Static_assert(JTString(a, b, (c, d))[7] == 'c', "JTString not working");

// print all the codes as: code = index
// now using not zip but enum members

    char storage[1024];
    char* out = storage;

#define Print(code) out += sprintf(out, "%s = %d", JTString(code), EnumMember(code))
#define PrintSpace() ; out += sprintf(out, " ");
    JTMapMakeSep(Print, PrintSpace, Codes);

    sprintf(out, "\n");

    // If fails, then likely a problem with `JTMapMakeSep`
    assert(strcmp(storage,
                  "add = 0 sub = 1 mult = 2 div = 3 call = 4 get = 5 put = 6\n")
           == 0);

    printf("Macro test Ok\n");

    return 0;
}
