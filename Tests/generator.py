import dataclasses as dc

@dc.dataclass
class Symbol:
    value: str

@dc.dataclass
class NonTerminal(Symbol):
    pass

@dc.dataclass
class Terminal(Symbol):
    pass

@dc.dataclass
class Rule:
    initial: Symbol
    result: list[Symbol]

# def toRule(args: tuple[str, list[str]]):
#     init = 

# LifpRules = list(map(toRule, [
#     ()
# ]))

# Rules = {
#     "Program"
# }
