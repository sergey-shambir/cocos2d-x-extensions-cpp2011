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

#ifndef CCEASYHTTP_H
#define CCEASYHTTP_H

#include <functional>
#include <vector>
#include <string>
#include "network/HttpResponse.h"

NS_CC_EXT_BEGIN

/**
 * @brief Simplifies client-server interaction to one function call with
 * one action passed as parameter.
 *
 * Example:
 * @code
 * CCEasyHttp::requestGET("http://my-site.org/pic.png",
 *                        CCEasyHttp::actionSaveFile(downloadPath, [] (bool success) {
 *                            CCLOG("SAVE %s", success ? "OK" : "FAILED");
 *                        }));
 * @endcode
 *
 * Since code uses lambdas, callback accepts CCHttpResponse.
 * If you want get additional info like image download path, use lambda capture:
 * @code
 * const std::string downloadPath = WRITABLE_DIR + "cached_pic.png";
 * auto action = CCEasyHttp::actionSaveImage(downloadPath, [=] (bool success) {
 *     if (!success)
 *         CCLOG("Image download to '%s' failed", downloadPath.c_str());
 * });
 * CCEasyHttp::requestGET("http://my-site.org/pic.png", action);
 * @endcode
 *
 */
class EasyHttp
{
public:
    typedef std::function<void (HttpResponse *response)> OnRequestComplete;

    /// @name Various forms of requests
    /// @{
    static void requestGET(const std::string &url,
                           OnRequestComplete onComplete);
    static void requestGET(const std::string &url,
                           const std::vector<std::string> &headers,
                           OnRequestComplete onComplete);
    // TODO: interface not finished, add POST data
    static void requestPOST(const std::string &url,
                            const std::vector<std::string> &headers,
                            OnRequestComplete onComplete);
    /// @}

    /// @name Actions, performed in main thread when request done
    /// @{
    static OnRequestComplete actionSaveFile(const std::string &downloadPath,
                                            const std::function<void (bool success)> &callback);
    static OnRequestComplete actionSaveImage(const std::string &downloadPath,
                                             const std::function<void (bool success)> &callback);
    /// @}

private:
    /// Not instanceable
    EasyHttp();
};

NS_CC_EXT_END

#endif // CCEASYHTTP_H
