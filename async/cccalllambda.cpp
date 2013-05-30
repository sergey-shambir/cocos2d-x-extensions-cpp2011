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
