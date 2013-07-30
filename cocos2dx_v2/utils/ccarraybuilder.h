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

#ifndef CCARRAYBUILDER_H
#define CCARRAYBUILDER_H

#include <cocos2d.h>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class ArrayBuilder
{
    ArrayBuilder(const ArrayBuilder &) = delete;
    ArrayBuilder &operator =(const ArrayBuilder &) = delete;
public:
    ArrayBuilder();
    /// Constructs with existing array or creates a new one, if nullptr given
    ArrayBuilder(CCArray *existingArray);
    ~ArrayBuilder();

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
