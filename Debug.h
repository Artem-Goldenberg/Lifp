#ifndef Debug_h
#define Debug_h

#include "Common/Common.h"

#ifdef LifpDebug
#define DebugOnly(...) __VA_ARGS__
#else
#define DebugOnly(...)
#endif

#define DebugArg(arg) DebugOnly(, arg)


#endif /* Debug_h */
