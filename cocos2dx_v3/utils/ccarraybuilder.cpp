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

#include "ccarraybuilder.h"

USING_NS_CC_EXT;

ArrayBuilder::ArrayBuilder()
    : d(CCArray::create())
{
    d->retain();
}

ArrayBuilder::ArrayBuilder(Array *existingArray)
    : d(existingArray)
{
    if (!d)
        d = CCArray::create();
    d->retain();
}

ArrayBuilder::~ArrayBuilder()
{
    d->release();
}

void ArrayBuilder::reset()
{
    d->release();
    d = CCArray::create();
    d->retain();
}

cocos2d::Array *ArrayBuilder::getResult() const
{
    return d;
}

void ArrayBuilder::addObjects(std::initializer_list<Object *> objects)
{
    for (auto obj : objects)
        if (obj)
            d->addObject(obj);
}

void ArrayBuilder::addBool(bool value)
{
    auto obj = CCBool::create(value);
    d->addObject(obj);
}

void ArrayBuilder::addInt(int value)
{
    auto obj = CCInteger::create(value);
    d->addObject(obj);
}

void ArrayBuilder::addFloat(float value)
{
    auto obj = CCFloat::create(value);
    d->addObject(obj);
}

void ArrayBuilder::addDouble(double value)
{
    auto obj = CCDouble::create(value);
    d->addObject(obj);
}

void ArrayBuilder::addPoint(const Point &value)
{
    auto obj = CCString::createWithFormat("{%f,%f}", value.x, value.y);
    d->addObject(obj);
}

void ArrayBuilder::addRect(const Rect &value)
{
    auto obj = CCString::createWithFormat("{%f,%f,%f,%f}",
                                          value.origin.x, value.origin.y,
                                          value.size.width, value.size.height);
    d->addObject(obj);
}

void ArrayBuilder::addObject(Object *object)
{
    d->addObject(object);
}
