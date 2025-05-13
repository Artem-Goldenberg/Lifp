/// Type which interpreter will return
///
/// All interpreter really does is it transform one syntax node into the other with, perhaps,
/// variables substituted for some other elements. It is very similar to evaluating terms of lambda calculus
typedef const Element* Value;

/// Interpreter just exposes one `evalElement` function
Value evalElement(const Element* node);
