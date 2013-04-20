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

#include "ccdictionarybuilder.h"

USING_NS_CC_EXT;

CCDictionaryBuilder::CCDictionaryBuilder()
    : d(cocos2d::CCDictionary::create())
{
    d->retain();
}

CCDictionaryBuilder::CCDictionaryBuilder(cocos2d::CCDictionary *existingDict)
    : d(existingDict)
{
    if (!d)
        d = cocos2d::CCDictionary::create();
    d->retain();
}

CCDictionaryBuilder::~CCDictionaryBuilder()
{
    d->release();
}

void CCDictionaryBuilder::reset()
{
    d->release();
    d = cocos2d::CCDictionary::create();
    d->retain();
}

cocos2d::CCDictionary *CCDictionaryBuilder::getResult() const
{
    return d;
}

void CCDictionaryBuilder::copyKeys(const std::vector<std::string> &keys, cocos2d::CCDictionary *other)
{
    if (other)
        for (const std::string &key : keys)
            if (cocos2d::CCObject *obj = other->objectForKey(key))
                d->setObject(obj, key);
}

void CCDictionaryBuilder::copyKeysAndObjects(const std::vector<std::string> &keys, cocos2d::CCDictionary *other)
{
    if (other)
        for (const std::string &key : keys)
            if (cocos2d::CCObject *obj = other->objectForKey(key))
                d->setObject(obj->copy()->autorelease(), key);
}

void CCDictionaryBuilder::setBool(bool value, const std::string &key)
{
    auto obj = cocos2d::CCBool::create(value);
    d->setObject(obj, key);
}

void CCDictionaryBuilder::setInt(int value, const std::string &key)
{
    auto obj = cocos2d::CCInteger::create(value);
    d->setObject(obj, key);
}

void CCDictionaryBuilder::setFloat(float value, const std::string &key)
{
    auto obj = cocos2d::CCFloat::create(value);
    d->setObject(obj, key);
}

void CCDictionaryBuilder::setDouble(double value, const std::string &key)
{
    auto obj = cocos2d::CCDouble::create(value);
    d->setObject(obj, key);
}

void CCDictionaryBuilder::setPoint(const cocos2d::CCPoint &value, const std::string &key)
{
    auto obj = cocos2d::CCString::createWithFormat("{%f,%f}", value.x, value.y);
    d->setObject(obj, key);
}

void CCDictionaryBuilder::setRect(const cocos2d::CCRect &value, const std::string &key)
{
    auto obj = cocos2d::CCString::createWithFormat("{%f,%f,%f,%f}",
                                          value.origin.x, value.origin.y,
                                          value.size.width, value.size.height);
    d->setObject(obj, key);
}

void CCDictionaryBuilder::setObject(cocos2d::CCObject *object, const std::string &key)
{
    d->setObject(object, key);
}
