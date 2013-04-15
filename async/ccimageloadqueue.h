#ifndef CCIMAGELOADQUEUE_H
#define CCIMAGELOADQUEUE_H

#include <cocos2d.h>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class CCImageLoadQueue
{
public:
    typedef std::function<void (bool success, const std::string &imageName)> OnComplete;

    /// @brief Thread-safe getter
    static CCImageLoadQueue *sharedQueue();

    /**
     * @brief Loads image from url asynchroniosly, calls callback in main thread.
     * @param imageUrl Url to load image
     * @param imageName Name for cache file (written to CCFileUtils writable path)
     * @param onComplete Callback, called in any case
     */
    void addImageToQueue(const std::string &imageUrl, const std::string &imageName, const OnComplete &onComplete);

    /// @brief Similar to \a addImageToQueue, but does nothing if image is cached
    void maybeAddImageToQueue(const std::string &imageUrl, const std::string &imageName, const OnComplete &onComplete);

    /**
     * @brief isCached
     * @param imageName
     * @return True if cached file exists (but doesn't check its content)
     */
    bool isCached(const std::string &imageName) const;

private:
    CCImageLoadQueue();
    static pthread_mutex_t ms_mutex;

    std::string m_cacheDir;
};

NS_CC_EXT_END

#endif // CCIMAGELOADQUEUE_H
