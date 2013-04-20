#ifndef CCARRAYBUILDER_H
#define CCARRAYBUILDER_H

#include <cocos2d.h>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class CCArrayBuilder
{
    CCArrayBuilder(const CCArrayBuilder &) = delete;
    CCArrayBuilder &operator =(const CCArrayBuilder &) = delete;
public:
    CCArrayBuilder();
    /// Constructs with existing array or creates a new one, if nullptr given
    CCArrayBuilder(CCArray *existingArray);
    ~CCArrayBuilder();

    void reset();
    CCArray *getResult() const;

    void addObjects(std::initializer_list<CCObject *> objects);

    /// Creates CCBool
    void addBool(bool value);
    /// Creates CCInteger
    void addInt(int value);
    /// Creates CCFloat
    void addFloat(float value);
    /// Creates CCDouble
    void addDouble(double value);
    /// Creates CCString
    void addPoint(const CCPoint &value);
    /// Creates CCString
    void addRect(const CCRect &value);
    /// Calls \a CCArray::addObject
    void addObject(CCObject *object);

private:
    CCArray *d;
};

NS_CC_EXT_END

#endif // CCARRAYBUILDER_H
