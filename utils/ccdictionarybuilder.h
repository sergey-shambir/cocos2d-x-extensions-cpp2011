#ifndef CCDICTIONARYBUILDER_H
#define CCDICTIONARYBUILDER_H

#include <cocos2d.h>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class CCDictionaryBuilder
{
    CCDictionaryBuilder(const CCDictionaryBuilder &) = delete;
    CCDictionaryBuilder &operator =(const CCDictionaryBuilder &) = delete;
public:
    CCDictionaryBuilder();
    /// Constructs with existing dictionary or creates a new one, if nullptr given
    CCDictionaryBuilder(CCDictionary *existingDict);
    ~CCDictionaryBuilder();

    void reset();
    CCDictionary *getResult() const;

    /// Copies keys, skips key if object not exists
    void copyKeys(const std::vector<std::string> &keys, CCDictionary *other);
    /// Copies keys and calls copy() for each object, skips key if object not exists
    void copyKeysAndObjects(const std::vector<std::string> &keys, CCDictionary *other);

    /// Creates CCBool
    void setBool(bool value, const std::string &key);
    /// Creates CCInteger
    void setInt(int value, const std::string &key);
    /// Creates CCFloat
    void setFloat(float value, const std::string &key);
    /// Creates CCDouble
    void setDouble(double value, const std::string &key);
    /// Creates CCString
    void setPoint(const CCPoint &value, const std::string &key);
    /// Creates CCString
    void setRect(const CCRect &value, const std::string &key);

    /// Calls \a CCDictionary::setObject
    void setObject(CCObject *object, const std::string &key);

private:
    CCDictionary *d;
};

NS_CC_EXT_END

#endif // CCDICTIONARYBUILDER_H
