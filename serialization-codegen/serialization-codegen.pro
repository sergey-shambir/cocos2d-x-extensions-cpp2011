QMAKE_CXXFLAGS += -std=c++0x
macx:QMAKE_CXXFLAGS += -stdlib=libc++
macx:QMAKE_LFLAGS += -stdlib=libc++

INCLUDEPATH += $$LLVM_INSTALL_DIR/include
LIBS += -L$$LLVM_INSTALL_DIR/lib -lclang

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    scopedclangoptions.cpp \
    generator.cpp \
    sourcegenerator.cpp \
    headergenerator.cpp

HEADERS += \
    scopedclangoptions.h \
    generator.h \
    sourcegenerator.h \
    headergenerator.h

OTHER_FILES += \
    temp.txt
