/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file unitconsumer.cpp
/// \brief Implements Generator - base class for code generation for signle file

#include <iostream>
#include "generator.h"

Generator::Generator(const std::string &subclass)
    : m_subclassName(subclass)
{
}

Generator::~Generator()
{
}

bool Generator::isBaseClass() const
{
    if (context().self.kind != CXCursor_ClassDecl)
        return false;
    if (getFile() != m_unitFile)
        return false;
    return false;
}

bool Generator::isGameRecord() const
{
    if (context().self.kind != CXCursor_StructDecl)
        return false;
    if (getFile() != m_unitFile)
        return false;
    if (getAnnotation(context().self) == "cocos2d-x: do not serialize")
        return false;
    return true;
}

bool Generator::isLocalNamespace() const
{
    if (context().self.kind != CXCursor_Namespace)
        return false;
    if (getFile() != m_unitFile)
        return false;
    return true;
}

std::string Generator::getAnnotation(const CXCursor &cursor) const
{
    std::string result;
    visitCursorChildren(cursor, [this, &result] {
        if (context().self.kind == CXCursor_AnnotateAttr) {
            result = context().selfName;
            return CXChildVisit_Break;
        }
        return CXChildVisit_Continue;
    });
    return result;
}

std::string Generator::getAnnotation() const
{
    return getAnnotation(context().self);
}

std::string Generator::getFile() const
{
    return getLocationFile(clang_getCursorLocation(context().self));
}

const VisitContext &Generator::context() const
{
    return m_context;
}

CXTranslationUnit Generator::unit() const
{
    return m_unit;
}

std::stringstream &Generator::out()
{
    return m_output;
}

void Generator::init(CXTranslationUnit unit, const std::string &fileBasename, const std::string &targetFileName)
{
    (void)targetFileName;
    m_unit = unit;
    m_unitFile = fileBasename;
}

std::string Generator::runResult() const
{
    return m_output.str();
}

void Generator::beginDefineGuards()
{
    out() << "\n"
             "// Do not edit!\n"
             "// File was generated with activerecord-codegen utility.\n"
             "\n";

    out() << "\n";
}

void Generator::endDefineGuards()
{
    out() << "\n";
}

void Generator::writeInclude(const std::string &file, bool angled)
{
    if (angled)
        out() << "#include <" << file << ">\n";
    else
        out() << "#include \"" << file << "\"\n";
}

std::string Generator::interchangeTypeStr(const CXType &type) const
{
    switch (type.kind) {
    case CXType_Int:
        return "int";
    case CXType_Float:
        return "float";
    case CXType_Double:
        return "double";
    case CXType_Bool:
        return "bool";
    case CXType_Unexposed:
        if (stringize(type) == "CCPointer<CCArray>")
            return "CCArray *";
        else if (stringize(type) == "CCPointer<CCDictionary>")
            return "CCDictionary *";
        else if (stringize(type) == "string")
            return "const std::string &";
        else
            reportWarning("Unexposed type: " + stringize(type));
        break;
    default:
        reportWarning("Unhandled type: " + stringize(type));
        break;
    }
    return "";
}

std::string Generator::getterNameStr(const CXCursor &cursor, const std::string &typeStr)
{
    if (typeStr == "bool") {
        return stringize(cursor);
    }
    std::string cname = stringize(cursor);
    if (!cname.empty())
        cname[0] = toupper(cname[0]);
    cname = "get" + cname;
    return cname;
}

std::string Generator::memberNameStr(const CXCursor &cursor, const CXType &type)
{
    if (type.kind == CXType_Bool)
        return "m_" + stringize(cursor);
    return stringize(cursor);
}

std::string Generator::stringize(const CXString &string) const
{
    const char *chars = clang_getCString(string);
    if (!chars)
        return std::string();

    std::string ret(chars);
    clang_disposeString(string);
    return ret;
}

std::string Generator::stringize(const CXCursor &cursor) const
{
    return stringize(clang_getCursorSpelling(cursor));
}

std::string Generator::stringize(const CXType &type, const std::string &varName, bool makeConstReference) const
{
    std::string result;

    if (makeConstReference) {
        switch (type.kind) {
        case CXType_LValueReference:
        case CXType_RValueReference:
        case CXType_Pointer:
        case CXType_Enum:
        case CXType_Bool:
        case CXType_Char_S:
        case CXType_Char_U:
        case CXType_Char16:
        case CXType_Int:
        case CXType_Long:
        case CXType_LongLong:
        case CXType_Float:
        case CXType_Double:
        case CXType_LongDouble:
            makeConstReference = false;
            break;
        default:
            break;
        }
    }

    if (makeConstReference || clang_isConstQualifiedType(type))
        result += "const ";

    switch (type.kind) {
    case CXType_Void:
        result += "void";
        if (!varName.empty())
            result += ' ';
        break;
    case CXType_Bool:
        result += "bool";
        if (!varName.empty())
            result += ' ';
        break;

    case CXType_Int:
        result += "int";
        if (!varName.empty())
            result += ' ';
        break;

    case CXType_Float:
        result += "float";
        if (!varName.empty())
            result += ' ';
        break;

    case CXType_Double:
        result += "double";
        if (!varName.empty())
            result += ' ';
        break;

    case CXType_Pointer: {
        CXType pointee = clang_getPointeeType(type);
        result += stringize(pointee);
        result += " *";
    }
        break;

    case CXType_LValueReference: {
        CXType pointee = clang_getPointeeType(type);
        result += stringize(pointee);
    }
        break;
    case CXType_Record:
    case CXType_Enum: {
        result += stringize(clang_getTypeDeclaration(type));
        if (!varName.empty())
            result += ' ';
    }
        break;

    case CXType_Unexposed:
        result += stringize(clang_getCursorDisplayName(clang_getTypeDeclaration(type)));
        if (!varName.empty())
            result += ' ';
        break;
    case CXType_Overload:
        result += "<CXType_Overload>";
        reportWarning("stringize: CXType_Overload not handled: " + stringize(clang_getCursorDisplayName(clang_getTypeDeclaration(type))));
        break;
    case CXType_Dependent:
        result += "<CXType_Dependent>";
        reportWarning("stringize: CXType_Dependent not handled: " + stringize(clang_getCursorDisplayName(clang_getTypeDeclaration(type))));
        break;
    case CXType_Invalid:
        result += "<CXType_Invalid>";
        reportWarning("stringize: CXType_Invalid not handled: " + stringize(clang_getCursorDisplayName(clang_getTypeDeclaration(type))));
        break;
    default:
        result += "<other CXTypeKind>";
        reportWarning("stringize: such CXTypeKind not handled: " + stringize(clang_getCursorDisplayName(clang_getTypeDeclaration(type))));
        break;
    }

    if (makeConstReference) {
        result += "&";
    }

    result += varName;
    return result;
}

std::string Generator::getLocationFile(const CXSourceLocation &location) const
{
    std::string fileName;
    unsigned line = 0, column = 0;
    getLocationParts(location, fileName, line, column);

    return fileName;
}

void Generator::reportWarning(const CXCursor &cursor, const std::string &message) const
{
    std::string fileName;
    unsigned line = 0, column = 0;
    getLocationParts(cursor, fileName, line, column);

    std::cerr << m_subclassName << ": "
              << "warning[" << line  << ":" << column << "]: "
              << message << std::endl;
}

void Generator::reportWarning(const std::string &message) const
{
    reportWarning(context().self, message);
}

typedef std::function<CXChildVisitResult
(const CXCursor &self, const CXCursor &parent)> VisitorWrapper;

static CXChildVisitResult childVisitor(CXCursor cursor,
                                       CXCursor parent,
                                       CXClientData client_data)
{
    VisitorWrapper &callback = *reinterpret_cast<VisitorWrapper *>(client_data);
    return callback(cursor, parent);
}

void Generator::visitCursorChildren(CXCursor cursor, Generator::Visitor callback) const
{
    VisitorWrapper wrapper = [&callback, this](const CXCursor &self, const CXCursor &parent) {
        pushContext(self, parent);
        auto ret = callback();
        popContext();
        return ret;
    };

    CXClientData data = reinterpret_cast<CXClientData>(&wrapper);
    clang_visitChildren(cursor,
                        childVisitor,
                        data);
}

/**
 * CXSourceLocation - 4-byte number, we need to call clang_getSpellingLocation
 * or clang_getExpansionLocation to retrieve line, column and file name.
 */
void Generator::getLocationParts(const CXSourceLocation &location, std::string &fileName, unsigned &line, unsigned &column) const
{
    CXFile file;
    unsigned offset = 0;
    clang_getSpellingLocation(location, &file, &line, &column, &offset);
    (void)offset;

    fileName = stringize(clang_getFileName(file));
}

void Generator::getLocationParts(const CXCursor &cursor, std::string &fileName, unsigned &line, unsigned &column) const
{
    getLocationParts(clang_getCursorLocation(cursor), fileName, line, column);
}

std::string Generator::getUnitFileBasename()
{
    const size_t endianPos = m_unitFile.rfind(".def.h");
    if (endianPos == std::string::npos)
        return m_unitFile;
    return m_unitFile.substr(0, endianPos);
}

void Generator::pushContext(const CXCursor &self, const CXCursor &parent) const
{
    m_stack.push_back(m_context);

    m_context.self = self;
    m_context.selfName = stringize(self);
    m_context.parent = parent;
    m_context.parentName = stringize(parent);
}

void Generator::popContext() const
{
    m_context = m_stack.back();
    m_stack.pop_back();
}
