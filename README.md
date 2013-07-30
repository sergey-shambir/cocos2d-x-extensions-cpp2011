cocos2d-x-extensions-cpp2011
============================

Extensions for cocos2d-x designed to use benefits of c++11 standard. All code is under MIT license. Works with both cocos2dx 2.x and 3.x. Use this code on own risk, be ready to fix it.

CCImageLoadQueue
---------------
Class to load images asynchroniosly. (Uses std::function)
```c++
    ImageLoadQueue::getInstance()->maybeAddImageToQueue(imageURL, imagePath, [=] (bool success) {
    Sprite *sprite = Sprite::create(imagePath.c_str());
    if (sprite) {
        const Size winSize = Director::getInstance()->getWinSize();
        sprite->setPosition(Point(winSize.width * 0.2, winSize.height * 0.4));
        sprite->setScale(0.2);
        addChild(sprite);
    }
});
```

CCForeach (Foreach in 3.x)
---------------
Adaptor for range-based for (implemented as template class).
```c++
    // there is Array *childrenToAdd
    for (auto node : Foreach<CCNode>(childrenToAdd))
    {
        addChild(node);
    }
```

CCCallLambda (CallLambda in 3.x)
---------------
CCAction that calls std::function<void()>, so it can be used with lambda. Use capture list to store additional parameters with callback, do not forget about references and autorelease!

```c++
    // Assume that 'int *m_count' defined as class member.
    m_count = new int(0);
    auto caller = CCCallLambda::create([&] () {
        CCLOG("Lambda call #%d", ++(*m_count));
    });
```

Cloud API
---------------
Have only Amazon S3 backend. Can list files in bucket, as well as download, upload and delete selected files from/to/on cloud.
```c++
    AmazonS3Cloud::get().setSharedSecretAccessKey("Ha ha! Find your own keys pair.");
    AmazonS3Cloud::get().setSharedAccessKey("Otherwise you cannot test it.");

    // Get list of files in your bucket 'games.kitties', we need only files
    // with prefix 'user129487512/' in their key.
    CloudDir *dir = AmazonS3Cloud::get().createDir("games.kitties");
    dir->updateFilesListAsync("user129487512/", [](Array *a, CloudError *e) {
        if (e)
            CCLOG("Update error: '%s'", e->prettyPrint());
        else
            // download all files owned by this user.
            for (CloudFile *i : Foreach<CloudFile>(a)) {
                std::string suffix = i->getName();
                size_t nameStart = suffix.find_last_of('/') + 1;
                if (suffix.size() > nameStart)
                    suffix = suffix.substr(nameStart);
                i->downloadFromCloud(suffix, [suffix](CloudError *error) {
                    if (error)
                        CCLOG("Download error: '%s'", error->prettyPrint());
                    else {
                        const std::string path = FileUtils::getInstance()->fullPathForFilename(suffix.c_str());
                        CCLOG("Download finished, see '%s'", path.c_str());
                    }
                });
            }
    });
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

Output (correct implementation file generated too, but it's too big to show):
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
