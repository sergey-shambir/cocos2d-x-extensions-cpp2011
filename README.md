cocos2d-x-extensions-cpp2011
============================

Extensions for cocos2d-x designed to use benefits of c++11 standard. All code is under MIT license.

CCImageLoadQueue
---------------
Class to load images asynchroniosly. (Uses std::function)
```c++
    const std::string pictureUrl = "your-url-here";
    CCImageLoadQueue::sharedQueue()->maybeAddImageToQueue(pictureUrl, "__cached_picture.jpg", [this] (bool success, const std::string &imageName) {
        const std::string path = CCFileUtils::sharedFileUtils()->getWritablePath() + imageName;
        CCSprite *sprite = CCSprite::create(path.c_str());
        if (sprite)
            addChild(sprite);
    });
```

CCForeach
---------------
Adaptor for range-based for. (Template class)
```c++
    // there is CCArray *childrenToAdd
    for (auto node : CCForeach<CCNode>(childrenToAdd))
    {
        addChild(node);
    }
```

Codegeneration for serialization
---------------
Code generator, based on clang-c API. It's very rough implementation, which can be improved in many terms.

Example:
```c++
    struct SoldierRecord : ISerializableRecord
    {
        int health;
        int fatigue;
    };

    struct NinjaRecord : public SoldierRecord
    {
        bool hasKatana;
    };
```

Output (correct implementation file exists too, but it's too big to show):
```c++
    struct SoldierRecord : public ISerializableRecord
    {
        SoldierRecord();
        void saveDataTo(CCDictionary *dict) const;
        void loadDataFrom(CCDictionary *dict);
        int getHealth() const;
        int getFatigue() const;

        void setHealth(int);
        void setFatigue(int);

    protected:
        int health;
        int fatigue;
    };

    struct NinjaRecord : public SoldierRecord
    {
        NinjaRecord();
        void saveDataTo(CCDictionary *dict) const;
        void loadDataFrom(CCDictionary *dict);
        bool hasKatana() const;

        void setHasKatana(bool);

    protected:
        bool m_hasKatana;
    };
```
