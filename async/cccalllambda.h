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

#ifndef CCCALLLAMBDA_H
#define CCCALLLAMBDA_H

#include "cocos2d.h"
#include "../ExtensionMacros.h"
#include <functional>
#include <type_traits>

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
