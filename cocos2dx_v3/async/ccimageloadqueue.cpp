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

#include "ccimageloadqueue.h"
#include "network/HttpClient.h"

namespace cocos2d {

using namespace cocos2d::extension;

ImageLoadQueue *g_instanceOfImageLoadQueue = 0;
pthread_mutex_t ImageLoadQueue::ms_mutex;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Private image loader
namespace {
class PrivateImageLoader : public Object
{
    std::function<void (bool)> m_onComplete;
    std::string m_cachePath;
public:
    static void createAndRun(const std::string &url, const std::string &cachePath, std::function<void (bool)> onComplete)
    {
        auto *loader = new PrivateImageLoader();
        loader->m_onComplete = onComplete;
        loader->m_cachePath = cachePath;

        auto *request = new HttpRequest();
        request->setRequestType(HttpRequest::Type::GET);
        request->setUrl(url.c_str());
        request->setResponseCallback(loader, httpresponse_selector(PrivateImageLoader::responceCallback));
        HttpClient::getInstance()->send(request);
    }

    void responceCallback(HttpClient* client, HttpResponse* response)
    {
        CC_UNUSED_PARAM(client);
        bool success = false;
        if (response->isSucceed()) {
            if (std::vector<char> *data = response->getResponseData()) {
                std::unique_ptr<Image> image(new Image());
                if (image->initWithImageData(data->data(), data->size())) {
                    success = true;
                    image->saveToFile(m_cachePath.c_str());
                } else {
                    CCLOGERROR("CCImageLoadQueue -- Cannot init image with data, data size = %d", (int)data->size());
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

ImageLoadQueue *ImageLoadQueue::getInstance()
{
    if (g_instanceOfImageLoadQueue == 0) {
        PThreadLocker lock(&ms_mutex);
        if (g_instanceOfImageLoadQueue == 0) {
            g_instanceOfImageLoadQueue = new ImageLoadQueue();
            atexit(ccImageLoadQueueCleanup);
        }
    }
    return g_instanceOfImageLoadQueue;
}

void ImageLoadQueue::addImageToQueue(const std::string &imageUrl, const std::string &downloadName, const OnComplete &onComplete)
{
    PrivateImageLoader::createAndRun(imageUrl, FileUtils::getInstance()->fullPathForFilename(downloadName.c_str()), [=] (bool success) {
        onComplete(success);
    });
}

void ImageLoadQueue::maybeAddImageToQueue(const std::string &imageUrl, const std::string &downloadName, const ImageLoadQueue::OnComplete &onComplete)
{
    if (isCached(downloadName))
        onComplete(true);
    else
        addImageToQueue(imageUrl, downloadName, onComplete);
}

bool ImageLoadQueue::isCached(const std::string &downloadName) const
{
    return CCFileUtils::getInstance()->isFileExist(FileUtils::getInstance()->fullPathForFilename(downloadName.c_str()));
}

ImageLoadQueue::ImageLoadQueue()
{
}

} /* cocos2d */
