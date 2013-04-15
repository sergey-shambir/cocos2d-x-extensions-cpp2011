#include "ccimageloadqueue.h"
#include "HttpRequest.h"
#include "HttpClient.h"
#include "HttpResponse.h"

USING_NS_CC;
USING_NS_CC_EXT;

CCImageLoadQueue *g_instanceOfImageLoadQueue = 0;
pthread_mutex_t CCImageLoadQueue::ms_mutex;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Private image loader
namespace {
class PrivateImageLoader : public CCObject
{
    std::function<void (bool)> m_onComplete;
    std::string m_cachePath;
public:
    static void createAndRun(const std::string &url, const std::string &cachePath, std::function<void (bool)> onComplete)
    {
        auto *loader = new PrivateImageLoader();
        loader->m_onComplete = onComplete;
        loader->m_cachePath = cachePath;

        auto *request = new CCHttpRequest();
        request->setRequestType(CCHttpRequest::kHttpGet);
        request->setUrl(url.c_str());
        request->setResponseCallback(loader, callfuncND_selector(PrivateImageLoader::responceCallback));
        CCHttpClient::getInstance()->send(request);
    }

    void responceCallback(CCNode *, void *vHttpResponse)
    {
        bool success = false;
        CCHttpResponse *response = (CCHttpResponse *)vHttpResponse;
        if (response->isSucceed()) {
            if (std::vector<char> *data = response->getResponseData()) {
                FILE *fout = ::fopen(m_cachePath.c_str(), "w");
                if (fout == NULL) {
                    CCLOGERROR("CCImageLoadQueue -- Cannot write to file '%s'", m_cachePath.c_str());
                } else {
                    CCLOGINFO("CCImageLoadQueue -- opened '%s', data size = %d", m_cachePath.c_str());
                    size_t numwrite = ::fwrite(data->data(), sizeof(char), data->size(), fout);
                    ::fclose(fout);
                    if (numwrite != data->size())
                        CCLOGERROR("CCImageLoadQueue -- Cannot write all data to '%s'", m_cachePath.c_str());
                    else
                        success = true;
                }
            } else {
                CCLOGERROR("CCImageLoadQueue -- got empty data");
            }
        } else {
            std::string error = response->getErrorBuffer() ? response->getErrorBuffer() : "";
            CCLOGERROR("CCImageLoadQueue -- Image load error: %s, status code: %d", error.c_str(), response->getResponseCode());
        }
        m_onComplete(success);
        delete this;
    }
};

void ccImageLoadQueueCleanup()
{
    delete g_instanceOfImageLoadQueue;
}

class PThreadLocker
{
    pthread_mutex_t *m_mutex;
public:
    PThreadLocker(pthread_mutex_t *mutex) : m_mutex(mutex)
    {
        pthread_mutex_lock(m_mutex);
    }

    ~PThreadLocker()
    {
        pthread_mutex_unlock(m_mutex);
    }
};

} // anonymous namespace
////////////////////////////////////////////////////////////////////////////////////////////////////

CCImageLoadQueue *CCImageLoadQueue::sharedQueue()
{
    if (g_instanceOfImageLoadQueue == 0) {
        PThreadLocker lock(&ms_mutex);
        if (g_instanceOfImageLoadQueue == 0) {
            g_instanceOfImageLoadQueue = new CCImageLoadQueue();
            atexit(ccImageLoadQueueCleanup);
        }
    }
    return g_instanceOfImageLoadQueue;
}

void CCImageLoadQueue::addImageToQueue(const std::string &imageUrl, const std::string &imageName, const OnComplete &onComplete)
{
    PrivateImageLoader::createAndRun(imageUrl, m_cacheDir + imageName, [=] (bool success) {
        onComplete(success, imageName);
    });
}

void CCImageLoadQueue::maybeAddImageToQueue(const std::string &imageUrl, const std::string &imageName, const CCImageLoadQueue::OnComplete &onComplete)
{
    if (isCached(imageName))
        onComplete(true, imageName);
    else
        addImageToQueue(imageUrl, imageName, onComplete);
}

bool CCImageLoadQueue::isCached(const std::string &imageName) const
{
    return CCFileUtils::sharedFileUtils()->isFileExist(m_cacheDir + imageName);
}

CCImageLoadQueue::CCImageLoadQueue()
    : m_cacheDir(CCFileUtils::sharedFileUtils()->getWritablePath())
{
}
