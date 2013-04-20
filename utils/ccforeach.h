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

#ifndef COCOS2DX_EXTENSIONS_CCFOREACH_H
#define COCOS2DX_EXTENSIONS_CCFOREACH_H

#include <cocos2d.h>
#include <type_traits>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

/**
 * Adaptor to use range-based for with CCArray.
 * @code
 * for (auto node : CCForeach<CCNode>(childrenToAdd))
 * {
 *     addChild(node);
 * }
 * @endcode
 */
template <class T>
class CCForeach
{
    CCObject **m_start;
    CCObject **m_end;
public:
    class iterator
    {
        CCObject **pObj;
    public:
        iterator(CCObject **obj) : pObj(obj) {}

        bool operator != (const iterator &other) const
        {
            return pObj != other.pObj;
        }

        const iterator &operator ++()
        {
            ++pObj;
            return *this;
        }

        const T *operator *() const
        {
            return static_cast<T*>(*pObj);
        }

        T *operator *()
        {
            return static_cast<T*>(*pObj);
        }

        const T **operator ->() const
        {
            return static_cast<T**>(pObj);
        }

        T **operator ->()
        {
            return static_cast<T**>(pObj);
        }
    };

    CCForeach(CCArray *array)
    {
        static_assert(std::is_base_of<CCObject, T>::value, "template parameter of CCForeach should be subclass of CCObject");
        if ((array) && (array)->data->num > 0)
        {
            m_start = array->data->arr;
            m_end = array->data->arr + array->data->num;
        }
        else
        {
            m_start = 0;
            m_end = 0;
        }
    }

    iterator begin()
    {
        return iterator(m_start);
    }

    iterator end()
    {
        return iterator(m_end);
    }
};

NS_CC_EXT_END

#endif // COCOS2DX_EXTENSIONS_CCFOREACH_H
