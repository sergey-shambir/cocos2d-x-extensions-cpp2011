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
