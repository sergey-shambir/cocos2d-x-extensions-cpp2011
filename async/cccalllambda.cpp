#include "cccalllambda.h"

NS_CC_EXT_BEGIN

CCCallLambda *CCCallLambda::create(const std::function<void (void)> &callback)
{
    auto pRet = new CCCallLambda(callback);
    pRet->autorelease();
    return pRet;
}

cocos2d::CCObject *CCCallLambda::copyWithZone(cocos2d::CCZone *pZone)
{
    cocos2d::CCZone *pNewZone = NULL;
    CCCallLambda *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (CCCallLambda*) (pZone->m_pCopyObject);
    } else {
        pRet = new CCCallLambda(m_callback);
        pZone = pNewZone = new cocos2d::CCZone(pRet);
    }
    // copy member data
    pRet->m_callback = m_callback;
    CCFiniteTimeAction::copyWithZone(pZone);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

void CCCallLambda::update(float time)
{
    CC_UNUSED_PARAM(time);
    m_callback();
}

CCFiniteTimeAction *CCCallLambda::reverse()
{
    return static_cast<CCCallLambda *>(copy()->autorelease());
}

CCCallLambda::CCCallLambda(const std::function<void (void)> &callback)
    : m_callback(callback)
{
}

NS_CC_EXT_END
