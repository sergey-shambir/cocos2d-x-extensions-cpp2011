
#pragma once

#if defined(CC_CODEGENERATOR_SERIALIZATION)

namespace std {
    class string {};

    template <class T>
    class queue {};
}
class CCArray {};
class CCDictionary {};
class CCPointer {};

#else

#include <cocos2d.h>

#endif

struct __attribute__((annotate("cocos2d-x: do not serialize"))) ISerializableRecord
{
    virtual ~ISerializableRecord() {}
    virtual void saveDataTo(CCDictionary *dict) const = 0;
    virtual void loadDataFrom(CCDictionary *dict) = 0;
};

struct SoldierRecord : ISerializableRecord
{
    int health;
    int fatigue;
};

struct NinjaRecord : public SoldierRecord
{
    bool hasKatana;
};

struct MerchantRecord : public ISerializableRecord
{
    bool acceptsBarter;
    int moneyAmount;
    int honor;
};
