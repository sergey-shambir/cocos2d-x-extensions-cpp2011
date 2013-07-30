/*
 * Copyright (c) 2013 Sergey Shambir
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef CCDICTIONARYBUILDER_H
#define CCDICTIONARYBUILDER_H

#include <cocos2d.h>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class DictionaryBuilder
{
    DictionaryBuilder(const DictionaryBuilder &) = delete;
    DictionaryBuilder &operator =(const DictionaryBuilder &) = delete;
public:
    DictionaryBuilder();
    /// Constructs with existing dictionary or creates a new one, if nullptr given
    DictionaryBuilder(Dictionary *existingDict);
    ~DictionaryBuilder();

    void reset();
    Dictionary *getResult() const;

    /// Copies keys, skips key if object not exists
    void copyKeys(const std::vector<std::string> &keys, Dictionary *other);

    /// Creates CCBool
    void setBool(bool value, const std::string &key);
    /// Creates CCInteger
    void setInt(int value, const std::string &key);
    /// Creates CCFloat
    void setFloat(float value, const std::string &key);
    /// Creates CCDouble
    void setDouble(double value, const std::string &key);
    /// Creates CCString
    void setPoint(const Point &value, const std::string &key);
    /// Creates CCString
    void setRect(const Rect &value, const std::string &key);

    /// Calls \a CCDictionary::setObject
    void setObject(Object *object, const std::string &key);

private:
    Dictionary *d;
};

NS_CC_EXT_END

#endif // CCDICTIONARYBUILDER_H
