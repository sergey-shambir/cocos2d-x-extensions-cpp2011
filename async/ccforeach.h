#ifndef COCOS2DX_EXTENSIONS_CCFOREACH_H
#define COCOS2DX_EXTENSIONS_CCFOREACH_H

#include <cocos2d.h>
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
