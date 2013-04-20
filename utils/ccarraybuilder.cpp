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

CCArrayBuilder::CCArrayBuilder()
    : d(CCArray::create())
{
    d->retain();
}

CCArrayBuilder::CCArrayBuilder(cocos2d::CCArray *existingArray)
    : d(existingArray)
{
    if (!d)
        d = CCArray::create();
    d->retain();
}

CCArrayBuilder::~CCArrayBuilder()
{
    d->release();
}

void CCArrayBuilder::reset()
{
    d->release();
    d = CCArray::create();
    d->retain();
}

cocos2d::CCArray *CCArrayBuilder::getResult() const
{
    return d;
}

void CCArrayBuilder::addObjects(std::initializer_list<cocos2d::CCObject *> objects)
{
    for (auto obj : objects)
        if (obj)
            d->addObject(obj);
}

void CCArrayBuilder::addBool(bool value)
{
    auto obj = CCBool::create(value);
    d->addObject(obj);
}

void CCArrayBuilder::addInt(int value)
{
    auto obj = CCInteger::create(value);
    d->addObject(obj);
}

void CCArrayBuilder::addFloat(float value)
{
    auto obj = CCFloat::create(value);
    d->addObject(obj);
}

void CCArrayBuilder::addDouble(double value)
{
    auto obj = CCDouble::create(value);
    d->addObject(obj);
}

void CCArrayBuilder::addPoint(const cocos2d::CCPoint &value)
{
    auto obj = CCString::createWithFormat("{%f,%f}", value.x, value.y);
    d->addObject(obj);
}

void CCArrayBuilder::addRect(const cocos2d::CCRect &value)
{
    auto obj = CCString::createWithFormat("{%f,%f,%f,%f}",
                                          value.origin.x, value.origin.y,
                                          value.size.width, value.size.height);
    d->addObject(obj);
}

void CCArrayBuilder::addObject(cocos2d::CCObject *object)
{
    d->addObject(object);
}
