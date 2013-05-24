#include "cceasyhttp.h"
#include "HttpRequest.h"
#include "HttpClient.h"

NS_CC_EXT_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Private helper
namespace {
class PrivateHelper : public CCObject
{
    CCEasyHttp::OnRequestComplete m_onComplete;
    std::string m_cachePath;
    std::vector<std::string> m_headers;
public:
    static void run(const std::string &url,
                    const std::vector<std::string> &headers,
                    CCHttpRequest::HttpRequestType requestType,
                    const CCEasyHttp::OnRequestComplete &onComplete)
    {
        auto *loader = new PrivateHelper();
        loader->m_onComplete = onComplete;

        auto *request = new CCHttpRequest();
        request->setRequestType(CCHttpRequest::kHttpGet);
        request->setUrl(url.c_str());
        request->setResponseCallback(loader, callfuncND_selector(PrivateHelper::responceCallback));
        request->setHeaders(headers);
        request->setRequestType(requestType);
        CCHttpClient::getInstance()->send(request);
    }

    void responceCallback(CCNode *, void *vHttpResponse)
    {
        m_onComplete((CCHttpResponse *)vHttpResponse);
        delete this;
    }
};
} // anonymous namespace

CCEasyHttp::CCEasyHttp()
{
}

void CCEasyHttp::requestGET(const std::string &url,
                            CCEasyHttp::OnRequestComplete onComplete)
{
    PrivateHelper::run(url, std::vector<std::string>(), CCHttpRequest::kHttpGet, onComplete);
}

void CCEasyHttp::requestGET(const std::string &url,
                            const std::vector<std::string> &headers,
                            CCEasyHttp::OnRequestComplete onComplete)
{
    PrivateHelper::run(url, headers, CCHttpRequest::kHttpGet, onComplete);
}

void CCEasyHttp::requestPOST(const std::string &url,
                             const std::vector<std::string> &headers,
                             CCEasyHttp::OnRequestComplete onComplete)
{
    PrivateHelper::run(url, headers, CCHttpRequest::kHttpPost, onComplete);
}

CCEasyHttp::OnRequestComplete CCEasyHttp::actionSaveFile(const std::string &downloadPath,
                                                         const std::function<void (bool)> &callback)
{
    return [=] (CCHttpResponse *response) {
        bool success = false;
        if (response->isSucceed()) {
            if (std::vector<char> *data = response->getResponseData()) {
                if (FILE *file = fopen(downloadPath.c_str(), "w")) {
                    size_t size = ::fwrite(data->data(), sizeof(char), data->size(), file);
                    if (size != data->size()) {
                        CCLOGWARN("Cannot write all data: %d at all, %d written",
                                  (int)size, (int)data->size());
                    }
                    fclose(file);
                } else {
                    CCLOGWARN("Cannot open file '%s' for writing.", downloadPath.c_str());
                }
            } else {
                CCLOGWARN("HTTP request returned no data");
            }
        } else {
            const std::string error = response->getErrorBuffer()
                    ? response->getErrorBuffer() : "";
            CCLOGWARN("HTTP request error '%s'', status code %d", error.c_str(),
                      response->getResponseCode());
        }
        callback(success);
    };
}

CCEasyHttp::OnRequestComplete CCEasyHttp::actionSaveImage(const std::string &downloadPath, const std::function<void (bool)> &callback)
{
    return [=] (CCHttpResponse *response) {
        bool success = false;
        if (response->isSucceed()) {
            if (std::vector<char> *data = response->getResponseData()) {
                CCImage *image = new CCImage();
                if (image->initWithImageData((void *)data->data(), data->size())) {
                    if (image->saveToFile(downloadPath.c_str())) {
                        success = true;
                    } else {
                        CCLOGERROR("CCImageLoadQueue -- got empty data");
                    }
                } else {
                    CCLOGWARN("HTTP request returned no valid image data, data size = %d", (int)data->size());
                }
                CC_SAFE_DELETE(image);
            } else {
                CCLOGWARN("HTTP request returned no data");
            }
        } else {
            std::string error = response->getErrorBuffer() ? response->getErrorBuffer() : "";
            CCLOGWARN("HTTP request error '%s'', status code %d", error.c_str(), response->getResponseCode());
        }
        callback(success);
    };
}

NS_CC_EXT_END
