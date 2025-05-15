#ifndef JustText_More_h
#define JustText_More_h

/// Basic macro to apply the `next` argument on all the rest arguments and expand them along the way
#define JTApp(next, ...) next(__VA_ARGS__)

/// If the argument is empty, expands to nothing, otherwise expands to its arguments but inserts comma beforehand
#define JTAfterComma(...) , ## __VA_ARGS__

/// The name doesn't lie here (at least not yet)
//#define JTGetThird(first, second, third, ...) third

/// Again, everything here is pretty basic, so it just does what it says
#define JTParens ()

/// Assisting macro for the ``JTGetString``
#define JTGetStringForReal(...) #__VA_ARGS__

/// It may not seem like a useful thing, but it actually is, because below ...
#define JTOne(...) 1

/// Same as ``JTExpand``, we need an alias to avoid setting unavailable bit in some cases
#define JTId(...) __VA_ARGS__

/// Performs an enormous amount of expansions on it's arguments
#define JTMassExpand(...) JTExpand5(__VA_ARGS__)
#define JTExpand5(...) JTExpand4(JTExpand4(JTExpand4(__VA_ARGS__)))
#define JTExpand4(...) JTExpand3(JTExpand3(JTExpand3(__VA_ARGS__)))
#define JTExpand3(...) JTExpand2(JTExpand2(JTExpand2(__VA_ARGS__)))
#define JTExpand2(...) JTExpand1(JTExpand1(JTExpand1(__VA_ARGS__)))
#define JTExpand1(...) JTExpand(JTExpand(JTExpand(__VA_ARGS__)))
#define JTExpand(...) __VA_ARGS__


#endif /* JustText_More_h */
