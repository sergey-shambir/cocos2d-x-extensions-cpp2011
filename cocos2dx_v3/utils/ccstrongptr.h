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

#ifndef COCOS2D_CXX11_CCPOINTER_H
#define COCOS2D_CXX11_CCPOINTER_H

#include "cocos2d.h"

namespace cocos2d {

/// Smart pointer that retains passed object and releases it in destructor or
/// when another object passed.
/// Recommended to use with member variables (especially if they are not added
/// to scene, like CCDictionary and CCArray), in asynchronous calls.
template <class TObject>
class StrongPtr
{
public:
    StrongPtr() : _o(nullptr)
    {
    }

    StrongPtr(TObject *o) : _o(o)
    {
        if (_o)
            _o->retain();
    }

    ~StrongPtr()
    {
        if (_o)
            _o->release();
    }

    StrongPtr(const StrongPtr &a) : _o(a._o)
    {
        if (_o)
            _o->retain();
    }

    StrongPtr &operator =(const StrongPtr &a)
    {
        if (a._o)
            a._o->retain();
        if (_o)
            _o->release();
        _o = a._o;
    }

    operator TObject *() const
    {
        return _o;
    }

    TObject operator *() const
    {
        CCAssert(_o, "dereferencing null strong pointer");
        return _o;
    }

    TObject *operator ->() const
    {
        CCAssert(_o, "dereferencing null strong pointer");
        return _o;
    }

private:
    TObject *_o;
};

} /* cocos2d */

#define CC_SYNTHESIZE_STRONG(type, var, methodSuffix) \
    private: \
        CCPointer<type> var; \
    public: \
        type *get##methodSuffix() { return var; } \
        void set##methodSuffix(type *value) { var = value; }

#endif // COCOS2D_CXX11_CCPOINTER_H
