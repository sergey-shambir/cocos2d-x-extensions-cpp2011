/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file recordgenerator.cpp
/// \brief

#include "sourcegenerator.h"
#include <iostream>

SourceGenerator::SourceGenerator()
    : Generator("SourceGenerator")
{
}

void SourceGenerator::run()
{
    beginDefineGuards();
    writeInclude(getUnitFileBasename() + ".h");
    writeInclude("../../utils/ccdictionarybuilder.h");
    out() << "\n";

    visitCursorChildren(clang_getTranslationUnitCursor(unit()), [this]() -> CXChildVisitResult {
        if (isGameRecord())
            defineRecord();

        if (isLocalNamespace())
            return CXChildVisit_Recurse;

        return CXChildVisit_Continue;
    });

    endDefineGuards();
}

void SourceGenerator::defineRecord()
{
    std::vector<CXCursor> fields;
    std::string base;
    visitCursorChildren(context().self, [this, &fields, &base]() -> CXChildVisitResult {
        if (context().self.kind == CXCursor_FieldDecl)
            fields.push_back(context().self);
        if (context().self.kind == CXCursor_CXXBaseSpecifier && context().selfName != "struct ISerializableRecord") {
            base = context().selfName;
            const std::string prefix = "struct ";
            if (base.size() >= prefix.size() && base.substr(0, prefix.size()) == prefix)
                base = base.substr(prefix.size());
        }
        return CXChildVisit_Continue;
    });

    // make: constructor
    out() << context().selfName << "::" << context().selfName << "()\n";
    bool isFirstInitializer = true;
    auto printFunc = [&isFirstInitializer, this](const CXCursor &cursor, const CXType &type, const std::string &initializer) {
        out() << "    " << (isFirstInitializer ? ":" : ",") << " ";
        out() << memberNameStr(cursor, type) << "(" << initializer << ")\n";
        isFirstInitializer = false;
    };

    for (auto cursor : fields) {
        auto type = clang_getCursorType(cursor);
        switch (type.kind) {
        case CXType_Pointer:
            printFunc(cursor, type, "NULL");
            break;
        case CXType_Double:
        case CXType_Float:
            printFunc(cursor, type, "0.0");
            break;
        case CXType_Int:
        case CXType_UInt:
        case CXType_Short:
        case CXType_UShort:
        case CXType_Char_S:
        case CXType_Char_U:
            printFunc(cursor, type, "0");
            break;
        case CXType_Bool:
            printFunc(cursor, type, "false");
            break;
        case CXType_Unexposed:
            if (stringize(type) == "CCPointer<CCArray>")
                printFunc(cursor, type, "CCArray::create()");
            else if (stringize(type) == "CCPointer<CCDictionary>")
                printFunc(cursor, type, "CCDictionary::create()");
            else if (stringize(type) != "string" && stringize(type) != "queue<int>")
                reportWarning("Unexposed type (constructor): " + stringize(type));
            break;
        default:
            break;
        }
    }
    out() << "{\n"
          << "}\n"
          << "\n";

    // make: void save(CCDictionary *dict) const
    out() << "void " << context().selfName << "::saveDataTo(CCDictionary *dict) const\n"
          << "{\n";
    if (!base.empty())
        out() << "    " << base << "::saveDataTo(dict);\n";
    out() << "    CCDictionaryBuilder h(dict);\n";
    for (auto cursor : fields) {
        auto type = clang_getCursorType(cursor);
        auto printSave = [this, cursor, type] (const std::string &typeNameCapitalized) -> void {
            out() << "    h.set"
                  << typeNameCapitalized
                  << "("<< memberNameStr(cursor, type) << ", \"" << stringize(cursor) << "\");\n";
        };

        switch (type.kind) {
        case CXType_Int:
            printSave("Int");
            break;
        case CXType_Float:
        case CXType_Double:
            printSave("Double");
            break;
        case CXType_Bool:
            printSave("Bool");
            break;
        case CXType_Unexposed:
            if (stringize(type) == "CCPointer<CCArray>")
                printSave("Array");
            else if (stringize(type) == "CCPointer<CCDictionary>")
                printSave("Dictionary");
            else if (stringize(type) == "string")
                printSave("String");
            else
                reportWarning("Unexposed type (save): " + stringize(type));
            break;
        default:
            reportWarning("Unhandled type (save): " + stringize(type));
            break;
        }
    }
    out() << "}\n"
          << "\n";

    // make: void load(CCDictionary *dict)
    out() << "void " << context().selfName << "::loadDataFrom(CCDictionary *dict)\n"
          << "{\n";
    if (!base.empty())
        out() << "    " << base << "::loadDataFrom(dict);\n";
    out() << "    DictionaryReader h(dict);\n";
    for (auto cursor : fields) {
        auto type = clang_getCursorType(cursor);
        auto printLoad = [this, cursor, type] (const std::string &typeNameCapitalized) -> void {
            out() << "    " << memberNameStr(cursor, type) << " = h.get"
                  << typeNameCapitalized
                  << "(\"" << stringize(cursor) << "\");\n";
        };

        switch (type.kind) {
        case CXType_Int:
            printLoad("Int");
            break;
        case CXType_Float:
        case CXType_Double:
            printLoad("Double");
            break;
        case CXType_Bool:
            printLoad("Bool");
            break;
        case CXType_Unexposed:
            if (stringize(type) == "CCPointer<CCArray>")
                printLoad("Array");
            else if (stringize(type) == "CCPointer<CCDictionary>")
                printLoad("Dictionary");
            else if (stringize(type) == "string")
                printLoad("String");
            else
                reportWarning("Unexposed type (load): " + stringize(type));
            break;
        default:
            reportWarning("Unhandled type (load): " + stringize(type));
            break;
        }
    }
    out() << "}\n"
          << "\n";

    // make: getters
    for (auto cursor : fields) {
        auto printGetter = [this, cursor] (const CXType &type) -> void {
            const std::string typeName = interchangeTypeStr(type);
            out() << typeName << " " << context().selfName << "::" << getterNameStr(cursor, typeName) << "() const\n"
                  << "{\n"
                  << "    return " << memberNameStr(cursor, type) << ";\n"
                  << "}\n"
                  << "\n";
        };
        printGetter(clang_getCursorType(cursor));
    }
    out() << "\n";

    // make: setters
    for (auto cursor : fields) {
        auto printSetter = [this, cursor] (const CXType &type) -> void {
            std::string cname = stringize(cursor);
            if (!cname.empty())
                cname[0] = toupper(cname[0]);
            cname = "set" + cname;
            out() << "void " << context().selfName << "::" << cname << "(" << interchangeTypeStr(type) << " var)\n"
                  << "{\n"
                  << "    " << memberNameStr(cursor, type) << " = var;\n"
                  << "}\n"
                  << "\n";
        };
        printSetter(clang_getCursorType(cursor));
    }
    out() << "\n";
}
