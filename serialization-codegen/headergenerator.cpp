#include "headergenerator.h"

HeaderGenerator::HeaderGenerator()
    : Generator("HeaderGenerator")
{
}

void HeaderGenerator::run()
{
    out() << "\n";

    out() << "struct ISerializableRecord\n"
             "{\n"
             "    virtual ~ISerializableRecord() {}\n"
             "    virtual void saveDataTo(CCDictionary *dict) const = 0;\n"
             "    virtual void loadDataFrom(CCDictionary *dict) = 0;\n"
             "};\n"
             "\n";

    visitCursorChildren(clang_getTranslationUnitCursor(unit()), [this]() -> CXChildVisitResult {
        if (isGameRecord() && context().selfName != "struct ISerializableRecord")
            declareRecord();

        if (isLocalNamespace())
            return CXChildVisit_Recurse;

        return CXChildVisit_Continue;
    });
}

void HeaderGenerator::declareRecord()
{
    std::vector<CXCursor> fields;
    std::string baseClass = "ISerializableRecord";
    visitCursorChildren(context().self, [this, &fields, &baseClass]() -> CXChildVisitResult {
        if (context().self.kind == CXCursor_FieldDecl)
            fields.push_back(context().self);
        if (context().self.kind == CXCursor_CXXBaseSpecifier) {
            baseClass = context().selfName;
            const size_t spaceIndex = baseClass.rfind(" ");
            if (spaceIndex != std::string::npos)
                baseClass = baseClass.substr(spaceIndex + 1);
        }
        return CXChildVisit_Continue;
    });

    // make: constructor, save, load
    out() << "struct " << context().selfName << " : public " << baseClass << "\n";
    out() << "{\n";
    out() << "    " << context().selfName << "();\n";
    out() << "    void saveDataTo(CCDictionary *dict) const;\n";
    out() << "    void loadDataFrom(CCDictionary *dict);\n";

    // make: getters
    for (auto cursor : fields) {
        auto printGetter = [this, cursor] (const std::string &typeName) -> void {
            out() << "    " << typeName << " " << getterNameStr(cursor, typeName) << "() const;\n";
        };
        printGetter(interchangeTypeStr(clang_getCursorType(cursor)));
    }
    out() << "\n";

    // make: setters
    for (auto cursor : fields) {
        auto printSetter = [this, cursor] (const std::string &typeName) -> void {
            std::string cname = stringize(cursor);
            if (!cname.empty())
                cname[0] = toupper(cname[0]);
            cname = "set" + cname;
            out() << "    void " << cname << "(" << typeName << ");\n";
        };
        printSetter(interchangeTypeStr(clang_getCursorType(cursor)));
    }
    out() << "\n";

    // make: members
    out() << "protected:\n";
    for (auto cursor : fields) {
        auto type = clang_getCursorType(cursor);
        std::string tname = stringize(type);
        if (tname == "string") // HACK: for std::string
            tname = "std::string";
        out() << "    " << tname << " " << memberNameStr(cursor, type) << ";\n";
    }

    // finish
    out() << "};\n\n";

    // make: getters
}
