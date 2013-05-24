#ifndef CCEASYHTTP_H
#define CCEASYHTTP_H

#include <functional>
#include <vector>
#include <string>
#include "HttpResponse.h"

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
class CCEasyHttp
{
public:
    typedef std::function<void (CCHttpResponse *response)> OnRequestComplete;

    /// @name Various forms of requests
    /// @{
    static void requestGET(const std::string &url,
                           OnRequestComplete onComplete);
    static void requestGET(const std::string &url,
                           const std::vector<std::string> &headers,
                           OnRequestComplete onComplete);
    // TODO: not finished
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
    CCEasyHttp();
};

NS_CC_EXT_END

#endif // CCEASYHTTP_H
