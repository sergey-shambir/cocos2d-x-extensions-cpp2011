#ifndef CCCALLLAMBDA_H
#define CCCALLLAMBDA_H

#include "cocos2d.h"
#include "../ExtensionMacros.h"

NS_CC_EXT_BEGIN

class CCCallLambda : public cocos2d::CCActionInstant
{
public:
    static CCCallLambda *create(const std::function<void(void)> &callback);
    CCFiniteTimeAction *reverse(void) override;

protected:
    CCObject *copyWithZone(cocos2d::CCZone *pZone) override;
    void update(float time) override;
    CCCallLambda(const std::function<void(void)> &callback);

    std::function<void(void)> m_callback;
};

NS_CC_EXT_END

#endif // CCCALLLAMBDA_H
