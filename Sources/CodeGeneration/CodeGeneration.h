/* General utility macros to generate some C code statements */
#ifndef CodeGeneration_h
#define CodeGeneration_h

#include "JustText/JustText.h"

// Data: something in parens (data1, data2, ...)

/// Used inside mapping macros, pass the given data to the `macro` as it's own arguments
#define DataApply(data, macro) macro data
/// Same thing as `DataApply` but for not conflicting when we are already the result of another `DataApply`
#define InnerDataApply(data, macro) macro data

// Format:
// (type, name) - variable data written in macros standards

#define GetVarType(data) JTGetFirst data
#define GetVarName(data) JTGetSecond data

/// Accepts data about a field name and a struct variable, expands to taking this field from the variable
#define GetField(fieldData, var) (var->GetVarName(fieldData))

/// Accepts a struct variable, and an array of field datas, expands to comma separated list of those
/// **Can be used only inside another map macro**
#define InnerGetFields(var, ...) JTInnerMapCommaAux(GetField, var, __VA_ARGS__)

/// Assign `var`'s field to the variable with the same name as that field
#define AssignField(fieldData, var) (var->GetVarName(fieldData) = GetVarName(fieldData))

#define CompareField(fieldData, var) (var->GetVarName(fieldData) == GetVarName(fieldData))

/// Accepts a list of names and a `type` and expands to comma separated list of var datas of this type
/// **Use only inside of another map**
#define InnerNamesToVarDatas(type, ...) JTInnerMapCommaAux(NameToVarData, type, __VA_ARGS__)
#define NameToVarData(name, type) (type, name)

/// Accepts a list of var datas, expands to comma sep list of their names
#define InnerGetVarNames(...) JTInnerMapComma(GetVarName, __VA_ARGS__)

/// Expands to the declaration for a given variable data
#define DeclareVar(varData) GetVarType(varData) GetVarName(varData)

#define InnerDeclareParameters(...) \
    JTInnerMapComma(DeclareVar, __VA_ARGS__) \
    JTIf(__VA_OPT__(,), , void)


#endif /* CodeGeneration_h */
