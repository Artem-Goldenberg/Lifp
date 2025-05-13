#ifdef LifpDebug
#define DebugOnly(...) __VA_ARGS__
#else
#define DebugOnly(...)
#endif

#define DebugArg(arg) DebugOnly(, arg)
