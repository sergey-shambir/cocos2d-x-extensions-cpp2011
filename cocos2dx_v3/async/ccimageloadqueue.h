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

#ifndef COCOS2DX_EXTENSIONS_CCIMAGELOADQUEUE_H
#define COCOS2DX_EXTENSIONS_CCIMAGELOADQUEUE_H

#include <cocos2d.h>

namespace cocos2d {

class ImageLoadQueue
{
public:
    typedef std::function<void (bool success)> OnComplete;

    /// @brief Thread-safe getter
    static ImageLoadQueue *getInstance();

    /**
     * @brief Loads image from url asynchroniosly, calls callback in main thread.
     * @param imageUrl Url to load image
     * @param imageName Name for cache file (written to CCFileUtils writable path)
     * @param onComplete Callback, called in any case
     */
    void addImageToQueue(const std::string &imageUrl, const std::string &downloadName, const OnComplete &onComplete);

    /// @brief Similar to \a addImageToQueue, but does nothing if image is cached
    void maybeAddImageToQueue(const std::string &imageUrl, const std::string &downloadName, const OnComplete &onComplete);

    /**
     * @brief isCached
     * @param imageName
     * @return True if cached file exists (but doesn't check its content)
     */
    bool isCached(const std::string &downloadName) const;

private:
    ImageLoadQueue();
    static pthread_mutex_t ms_mutex;
};

} /* cocos2d */

#endif // COCOS2DX_EXTENSIONS_CCIMAGELOADQUEUE_H
