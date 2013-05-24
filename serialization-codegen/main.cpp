/**
 * Program accepts definition file YourFileName.def.h
 * than produces YourFileName.h and YourFileName.cpp.
 */
#include "scopedclangoptions.h"
#include "generator.h"
#include "sourcegenerator.h"
#include "headergenerator.h"

#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;

void checkExistance(const std::string &path)
{
    if (access(path.c_str(), R_OK) != 0)
    {
        printf("Wrong path: %s\n", path.c_str());
        exit(1);
    }
}

void changeDirectory(const std::string &path)
{
    checkExistance(path);
    int status = chdir(path.c_str());
    if (status != 0)
    {
        printf("Directory exists, but we cannot goto it: %s\n",
               path.c_str());
        exit(1);
    }
}

void writeStringToFile(const std::string &content, const std::string &path)
{
    FILE *file = fopen(path.c_str(), "w");
    if (file == nullptr) {
        printf("Cannot open AST implementation file: %s\n", path.c_str());
        exit(0);
    }

    fwrite(content.c_str(), sizeof(char), content.size(), file);
    fclose(file);
}

template<class TGenerator>
void runAndWrite(const std::string &fromFileName, const std::string &fileName, CXTranslationUnit unit)
{
    Generator *gen = new TGenerator();
    gen->init(unit, fromFileName, fileName);
    gen->run();
    writeStringToFile(gen->runResult(), fileName);
    delete gen;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        puts("Usage: GenerateAST <workdir> <input file basename>\n"
             "Basename example: \"MyGameRecords\" for \"MyGameRecords.def\".");
        exit(0);
    }
    const std::string dir(argv[1]);
    const std::string basename(argv[2]);
    const std::string path(basename + ".def.h");
    changeDirectory(dir);
    checkExistance(path);

    std::vector<std::string> optionsArray = {
        "-xc++",
        "-D__STDC_LIMIT_MACROS",
        "-D__STDC_CONSTANT_MACROS",
        "-DCC_CODEGENERATOR_SERIALIZATION",
        "-I/usr/include/x86_64-linux-gnu/c++/4.7/",
        "-I/usr/include/qt4",
        "-I/usr/include/qt4/QtCore",
        "-I/usr/lib/clang/3.2/include",
        "-std=c++0x",
        "-Wall",
        "-Wextra",
//        "-Wdocumentation"
    };
    ScopedClangOptions options(optionsArray);

    CXIndex index = clang_createIndex(false /*bool exclude PCH decls*/,
                                      true /*bool display diagnostic*/);

    CXTranslationUnit unit = clang_parseTranslationUnit(
                index,
                path.c_str(),
                options.data(), /* argv */
                options.size(), /* argc */
                nullptr, /* unsaved files */
                0, /* num unsaved files */
                CXTranslationUnit_DetailedPreprocessingRecord
                );

    runAndWrite<HeaderGenerator>(basename + ".def.h", basename + ".h", unit);
    runAndWrite<SourceGenerator>(basename + ".def.h", basename + ".cpp", unit);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return 0;
}

