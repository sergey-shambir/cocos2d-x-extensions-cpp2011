
include(cocos2d-x-extensions-cpp2011.prf)

equals(COCOS2D_VERSION_MAJOR, 3) {
    HEADERS += \
        $$PWD/cocos2dx_v3/cocos2dx-cxx11.h \
        $$PWD/cocos2dx_v3/async/cccalllambda.h \
        $$PWD/cocos2dx_v3/async/cceasyhttp.h \
        $$PWD/cocos2dx_v3/async/ccimageloadqueue.h \
        $$PWD/cocos2dx_v3/cloud/CCCloudStorage.h \
        $$PWD/cocos2dx_v3/utils/ccforeach.h \
        $$PWD/cocos2dx_v3/utils/ccdictionarybuilder.h \
        $$PWD/cocos2dx_v3/utils/ccarraybuilder.h \
        $$PWD/cocos2dx_v3/utils/ccbinarydata.h \
        $$PWD/cocos2dx_v3/utils/ccstrongptr.h

    SOURCES += \
        $$PWD/cocos2dx_v3/async/ccimageloadqueue.cpp \
        $$PWD/cocos2dx_v3/utils/ccdictionarybuilder.cpp \
        $$PWD/cocos2dx_v3/utils/ccarraybuilder.cpp \
        $$PWD/cocos2dx_v3/async/cccalllambda.cpp \
        $$PWD/cocos2dx_v3/async/cceasyhttp.cpp \
        $$PWD/cocos2dx_v3/utils/ccbinarydata.cpp \
        $$PWD/cocos2dx_v3/cloud/CCCloudStorage.cpp
}

equals(COCOS2D_VERSION_MAJOR, 2) {
    HEADERS +=  \
        $$PWD/cocos2dx_v2/cocos2dx-cxx11.h \
        $$PWD/cocos2dx_v2/async/ccimageloadqueue.h \
        $$PWD/cocos2dx_v2/async/cceasyhttp.h \
        $$PWD/cocos2dx_v2/async/cccalllambda.h \
        $$PWD/cocos2dx_v2/cloud/CCCloudStorage.h \
        $$PWD/cocos2dx_v2/utils/ccforeach.h \
        $$PWD/cocos2dx_v2/utils/ccdictionarybuilder.h \
        $$PWD/cocos2dx_v2/utils/ccbinarydata.h \
        $$PWD/cocos2dx_v2/utils/ccarraybuilder.h

    SOURCES +=  \
        $$PWD/cocos2dx_v2/async/ccimageloadqueue.cpp \
        $$PWD/cocos2dx_v2/async/cceasyhttp.cpp \
        $$PWD/cocos2dx_v2/async/cccalllambda.cpp \
        $$PWD/cocos2dx_v2/cloud/CCCloudStorage.cpp \
        $$PWD/cocos2dx_v2/utils/ccdictionarybuilder.cpp \
        $$PWD/cocos2dx_v2/utils/ccbinarydata.cpp \
        $$PWD/cocos2dx_v2/utils/ccarraybuilder.cpp
}

HEADERS += \
    $$PWD/liboauthcpp/src/HMAC_SHA1.h \
    $$PWD/liboauthcpp/src/SHA1.h \
    $$PWD/liboauthcpp/src/base64.h \
    $$PWD/liboauthcpp/src/urlencode.h

SOURCES += \
    $$PWD/liboauthcpp/src/HMAC_SHA1.cpp \
    $$PWD/liboauthcpp/src/SHA1.cpp \
    $$PWD/liboauthcpp/src/base64.cpp \
    $$PWD/liboauthcpp/src/urlencode.cpp

OTHER_FILES += \
    $$PWD/README.md

