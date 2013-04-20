HEADERS += \
    $$PWD/utils/ccforeach.h \
    $$PWD/async/ccimageloadqueue.h \
    ../../cocos2d-x-extensions-cpp2011/utils/ccdictionarybuilder.h \
    ../../cocos2d-x-extensions-cpp2011/utils/ccarraybuilder.h

SOURCES += \
    $$PWD/async/ccimageloadqueue.cpp \
    ../../cocos2d-x-extensions-cpp2011/utils/ccdictionarybuilder.cpp \
    ../../cocos2d-x-extensions-cpp2011/utils/ccarraybuilder.cpp

INCLUDEPATH *= $$PWD/async
INCLUDEPATH *= $$PWD/utils

