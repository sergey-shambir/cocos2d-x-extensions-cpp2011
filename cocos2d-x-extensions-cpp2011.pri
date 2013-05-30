HEADERS += \
    $$PWD/utils/ccforeach.h \
    $$PWD/async/ccimageloadqueue.h \
    $$PWD/utils/ccdictionarybuilder.h \
    $$PWD/utils/ccarraybuilder.h \
    $$PWD/async/cccalllambda.h \
    $$PWD/cocos2dx-cxx11.h \
    $$PWD/async/cceasyhttp.h \
    $$PWD/utils/ccbinarydata.h

SOURCES += \
    $$PWD/async/ccimageloadqueue.cpp \
    $$PWD/utils/ccdictionarybuilder.cpp \
    $$PWD/utils/ccarraybuilder.cpp \
    $$PWD/async/cccalllambda.cpp \
    $$PWD/async/cceasyhttp.cpp \
    $$PWD/utils/ccbinarydata.cpp

OTHER_FILES += \
    $$PWD/README.md

include(cocos2d-x-extensions-cpp2011.prf)

