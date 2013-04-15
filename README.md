cocos2d-x-extensions-cpp2011
============================

cocos2d-x extensions designed to use benefits of c++11 standard

CCImageLoadQueue
---------------
Class to load images asynchroniosly.
```c++
    const std::string pictureUrl = "your-url-here";
    CCImageLoadQueue::sharedQueue()->maybeAddImageToQueue(pictureUrl, "__cached_picture.jpg", [this] (bool success, const std::string &imageName) {
        const std::string path = CCFileUtils::sharedFileUtils()->getWritablePath() + imageName;
        CCSprite *sprite = CCSprite::create(path.c_str());
        if (sprite)
            addChild(sprite);
    });
```
