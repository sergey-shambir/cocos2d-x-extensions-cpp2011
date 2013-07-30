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

#include "cceasyhttp.h"
#include "network/HttpClient.h"

NS_CC_EXT_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Private helper
namespace {
class PrivateHelper : public Object
{
    EasyHttp::OnRequestComplete m_onComplete;
    std::string m_cachePath;
    std::vector<std::string> m_headers;
public:
    static void run(const std::string &url,
                    const std::vector<std::string> &headers,
                    HttpRequest::Type requestType,
                    const EasyHttp::OnRequestComplete &onComplete)
    {
        auto *loader = new PrivateHelper();
        loader->m_onComplete = onComplete;

        auto *request = new HttpRequest();
        request->setRequestType(HttpRequest::Type::GET);
        request->setUrl(url.c_str());
        request->setResponseCallback(loader, httpresponse_selector(PrivateHelper::responceCallback));
        request->setHeaders(headers);
        request->setRequestType(requestType);
        HttpClient::getInstance()->send(request);
    }

    void responceCallback(HttpClient* client, HttpResponse* response)
    {
        CC_UNUSED_PARAM(client);
        m_onComplete(response);
        delete this;
    }
};
} // anonymous namespace

EasyHttp::EasyHttp()
{
}

void EasyHttp::requestGET(const std::string &url,
                          EasyHttp::OnRequestComplete onComplete)
{
    PrivateHelper::run(url, std::vector<std::string>(), HttpRequest::Type::GET, onComplete);
}

void EasyHttp::requestGET(const std::string &url,
                          const std::vector<std::string> &headers,
                          EasyHttp::OnRequestComplete onComplete)
{
    PrivateHelper::run(url, headers, HttpRequest::Type::GET, onComplete);
}

void EasyHttp::requestPOST(const std::string &url,
                           const std::vector<std::string> &headers,
                           EasyHttp::OnRequestComplete onComplete)
{
    PrivateHelper::run(url, headers, HttpRequest::Type::POST, onComplete);
}

EasyHttp::OnRequestComplete EasyHttp::actionSaveFile(const std::string &downloadPath,
                                                     const std::function<void (bool)> &callback)
{
    return [=] (HttpResponse *response) {
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

EasyHttp::OnRequestComplete EasyHttp::actionSaveImage(const std::string &downloadPath, const std::function<void (bool)> &callback)
{
    return [=] (HttpResponse *response) {
        bool success = false;
        if (response->isSucceed()) {
            if (std::vector<char> *data = response->getResponseData()) {
                std::unique_ptr<Image> image(new Image());
                if (image->initWithImageData((void *)data->data(), data->size())) {
                    if (image->saveToFile(downloadPath.c_str())) {
                        success = true;
                    } else {
                        CCLOGERROR("CCImageLoadQueue -- got empty data");
                    }
                } else {
                    CCLOGWARN("HTTP request returned no valid image data, data size = %d", (int)data->size());
                }
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
