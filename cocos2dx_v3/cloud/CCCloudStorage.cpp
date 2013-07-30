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

#include "CCCloudStorage.h"
#include "../liboauthcpp/src/HMAC_SHA1.h"
#include "../liboauthcpp/src/base64.h"
#include "../liboauthcpp/src/urlencode.h"
#include <network/HttpClient.h>
#include "cocos2d_extended.h"
#include <sstream>
#include <algorithm>
#include <zlib.h>

USING_NS_CC_EXT;

namespace cocos2d {

class S3CloudDir;
class S3CloudFile;

static std::string g_S3SharedSecredAccessKey;
static std::string g_S3SharedAccessKey;

////////////////////////////////////////////////////////////////////////////////

namespace S3Utils
{
    /// Encodes URL and prepends with '/', if needed.
    static std::string encodeURI(const std::string &url)
    {
        if (!url.empty() && url[0] == '/')
            return urlencode(url.substr(1));
        return urlencode(url);
    }

    static std::string canonicalResource(const std::string &bucket,
                                         const std::string &key)
    {
        return "/" + bucket + "/" + encodeURI(key);
    }

    static std::string HMACSHA1(const std::string &key,
                         const std::string &stringToSign)
    {
        char digest[21];

        CHMAC_SHA1 signer;
        signer.HMAC_SHA1((unsigned char*)stringToSign.data(), stringToSign.size(),
                         (unsigned char*)key.data(), key.size(),
                         (unsigned char*)digest);

        digest[20] = '\0';
        return std::string(digest, 20);
    }

    static std::string replaceNotAllowed(const std::string &data)
    {
        std::string ret;
        for (char ch : data)
            switch (ch) {
            case '/':
                ret += "%2F";
                break;
            case '+':
                ret += "%2B";
                break;
            default:
                ret += ch;
                break;
            }
        return ret;
    }

    static std::string encodeBase64(const std::string &data)
    {
        /// Sanity check.
        if (data.empty())
            return "";
        return base64_encode((unsigned char const *)data.c_str(), data.size());
    }
} /* S3Utils */

////////////////////////////////////////////////////////////////////////////////

/// Use this class by pointer. Designed for internal use, so be careful.
/// Deletes itself when callback finished.
class S3RequestRAII : public Object
{
public:
    std::map<std::string, std::string> headers;
    std::function<void(CloudError *error)> onError;
    std::function<void(const std::vector<char> &data, CloudError *error)> onSuccess;

    S3RequestRAII()
        : _request(new HttpRequest)
    {
        addEncodedQueryItem("AWSAccessKeyId", g_S3SharedAccessKey);
    }

    void setRequestType(HttpRequest::Type type)
    {
        _request->setRequestType(type);
    }

    void attachFile(const std::string &file)
    {
        if (!file.empty()) {
            if (file[0] == '/')
                _fileWithPostData = file;
            else
                _fileWithPostData = FileUtils::getInstance()->getWritablePath() + file;
        }
    }

    void send()
    {
        CCAssert(onError, "S3Request: no onError handler set");
        CCAssert(onSuccess, "S3Request: no onSuccess handler set");

        std::string url = _urlBase;
        if (!_queryTail.empty())
            url += "?" + _queryTail;

        if (!_fileWithPostData.empty()) {
            std::string devNote;
            if (!loadPostData(devNote)) {
                CloudError *error = new CloudError();
                error->didOccuredAtPostprocess = false;
                error->httpStatusCode = 0;
                error->url = url;
                error->developerNote = devNote;
                onError(error);
                return;
            }
        }

        _request->setUrl(url.c_str());
        _request->setResponseCallback(this, httpresponse_selector(S3RequestRAII::onFinish));

        std::vector<std::string> ccHeaders;
        for (const auto &header : headers)
            ccHeaders.push_back(header.first + ":" + header.second);
        _request->setHeaders(ccHeaders);
        HttpClient::getInstance()->send(_request);
    }

    void addEncodedQueryItem(const std::string &key, const std::string &value)
    {
        if (!_queryTail.empty())
            _queryTail += "&";
        _queryTail += key;
        _queryTail += "=";
        _queryTail += value;
    }

    void addQueryItem(const std::string &key, const std::string &value)
    {
        addEncodedQueryItem(S3Utils::encodeURI(key), S3Utils::encodeURI(value));
    }

    /// Don't modify headers between sign() and send().
    void sign(const std::string &bucket,
              const std::string &key)
    {
        buildUrl(bucket, key);
        const std::string toSign = stringToSign(bucket, key);
        const std::string signature2 = S3Utils::replaceNotAllowed(
                    S3Utils::encodeBase64(
                        S3Utils::HMACSHA1(
                            g_S3SharedSecredAccessKey, toSign)));

        addEncodedQueryItem("Signature", signature2);
        addEncodedQueryItem("Expires", timeString());
    }

private:
    bool loadPostData(std::string &devNote)
    {
        if (FILE *fd = fopen(_fileWithPostData.c_str(), "r")) {
            char buffer[4096];
            while (size_t numRead = fread(buffer, sizeof(char), 4096 - 1, fd))
                _postData.append(buffer, numRead);
            if (_postData.empty()) {
                devNote += "File is empty: \"";
                devNote += _fileWithPostData;
                devNote += "\"";
                return false;
            } else {
                _request->setRequestData(_postData.c_str(), _postData.size());
            }
        } else {
            devNote += "File not found: \"";
            devNote += _fileWithPostData;
            devNote += "\"";
            return false;
        }
        return true;
    }

    void buildUrl(const std::string &bucket,
                  const std::string &key)
    {
        _urlBase = "http://" + bucket + ".s3.amazonaws.com/" + S3Utils::encodeURI(key);
    }

    std::string stringToSign(const std::string &bucket, const std::string &key)
    {
//        std::string headers;
//        for (const auto &a : this->headers) {
//            headers += lowerString(a.first);
//            headers += ":";
//            headers += a.second;
//            headers += "\n";
//        }

        std::string method;
        switch (_request->getRequestType()) {
        case extension::HttpRequest::Type::DELETE:
            method = "DELETE";
            break;
        case extension::HttpRequest::Type::GET:
            method = "GET";
            break;
        case extension::HttpRequest::Type::POST:
            method = "POST";
            break;
        case extension::HttpRequest::Type::PUT:
            method = "PUT";
            break;
        case extension::HttpRequest::Type::UNKNOWN:
            return "";
        }

        std::string mimeTypeString;
        auto mimeType = headers.find("Content-Type");
        if (mimeType != headers.end())
            mimeTypeString += mimeType->second;
        mimeTypeString += "\n";

        return method + "\n\n" + mimeTypeString + timeString() + "\n"
                + S3Utils::canonicalResource(bucket, key);
    }

    std::string timeString()
    {
        if (_dateString.empty()) {
            const long long int epoch = (long long int)time(NULL) + 200;
            char formattedEpoch[40];
            sprintf(formattedEpoch, "%lld", epoch);

            _dateString = formattedEpoch;
        }
        return _dateString;
    }

    std::string lowerString(const std::string &s) const
    {
        std::string ret = s;
        for (char &c : ret)
            c = ::tolower(c);
        return ret;
    }

    void onFinish(HttpClient* client, HttpResponse* response)
    {
        CC_UNUSED_PARAM(client);

        CloudError *error = new CloudError();
        error->didOccuredAtPostprocess = false;
        error->httpStatusCode = response->getResponseCode();
        error->url = response->getHttpRequest()->getUrl();

        std::vector<char> * data = response->getResponseData();
        if (std::vector<char> *header = response->getResponseHeader()) {
            std::string sheader(header->data(), header->size());
            if (std::string::npos != sheader.find("Content-Encoding: gzip"))
                data = ungzipData(data, error);
        }
        bool success = response->isSucceed() && data;
        if (!response->isSucceed() && data && !data->empty())
            error->developerNote.assign(data->data(), data->size());
        if (success) {
            error->didOccuredAtPostprocess = true;
            onSuccess(*data, error);
        } else {
            onError(error);
        }
        error->release();

        delete this;
    }

    /// Sometimes data comes in gzip, just inzip it using zlib.
    /// Can return nullptr, in this case error will be emited in caller function.
    std::vector<char> *ungzipData(std::vector<char> *data, CloudError *error)
    {
        gzFile file = saveDataToTemporaryFile(*data);
        if (!file) {
            error->developerNote = "Cannot uncompress gzip data, temporary file not found.";
            return nullptr;
        }
        _unzippedData.reset(new std::vector<char>());
        char buffer[1025];
        int bytesRead = 0;
        do {
            bytesRead = gzread(file, buffer, 1024);
            if (bytesRead == -1) {
                int status = 0;
                const char *description = gzerror(file, &status);
                error->developerNote = "Error when uncompressing gzip data: ";
                if (description)
                    error->developerNote += description;
                _unzippedData = nullptr;
            } else {
                _unzippedData->insert(_unzippedData->end(), buffer, buffer + bytesRead);
            }
        } while (bytesRead == 1024);
        deleteTemporaryFile(file);
        return _unzippedData.get();
    }

    gzFile saveDataToTemporaryFile(const std::vector<char> &data)
    {
        const std::string tempPath = FileUtils::getInstance()->getWritablePath() + ".mwfc.gzip-temporary";
        FILE *temp = fopen(tempPath.c_str(), "wb");
        if (!temp)
            return nullptr;
        fwrite(data.data(), sizeof(char), data.size(), temp);
        fclose(temp);
        return gzopen(tempPath.c_str(), "r");
    }

    void deleteTemporaryFile(gzFile file)
    {
        gzclose(file);
    }

    Pointer<HttpRequest> _request;
    std::string _dateString;
    std::string _urlBase;
    std::string _queryTail;
    std::string _postData;
    std::string _fileWithPostData;
    std::shared_ptr<std::vector<char>> _unzippedData;
};

////////////////////////////////////////////////////////////////////////////////

class S3CloudFile : public CloudFile
{
public:
    std::string m_cloudDir;
    std::string m_cloudKey;
    std::string m_lastModified;

    void downloadFromCloud(const std::string &downloadPath,
                       std::function<void(CloudError *error)> const& onFinish)
    {
        if (downloadPath.empty())
            return;
        std::function<void(CloudError *error)> savedOnFinish = onFinish;

        S3RequestRAII *raii = new S3RequestRAII();
        raii->setRequestType(HttpRequest::Type::GET);
        raii->onSuccess = [=] (const std::vector<char> &data, CloudError *error) {
            std::string realPath;
            if (downloadPath[0] != '/')
                realPath = FileUtils::getInstance()->getWritablePath() + downloadPath;
            else
                realPath = downloadPath;
            bool success = false;
            if (FILE *file = fopen(realPath.c_str(), "w")) {
                size_t size = ::fwrite(data.data(), sizeof(char),
                                       data.size(), file);
                success = (size == data.size());
                fclose(file);
            }

            savedOnFinish(success ? NULL : error);
        };
        raii->onError = [=](CloudError *error) {
            savedOnFinish(error);
        };

        raii->sign(m_cloudDir, m_cloudKey);
        raii->send();
    }

    void deleteOnCloud(std::function<void(CloudError *error)> const& onFinish)
    {
        std::function<void(CloudError *error)> savedOnFinish = onFinish;
        S3RequestRAII *raii = new S3RequestRAII();
        raii->setRequestType(HttpRequest::Type::DELETE);
        raii->onSuccess = [=] (const std::vector<char> &data, CloudError *error) {
            savedOnFinish(NULL);
        };
        raii->onError = [=](CloudError *error) {
            if (error->httpStatusCode == 204) // deleted OK.
                savedOnFinish(NULL);
            else
                savedOnFinish(error);
        };
        raii->sign(m_cloudDir, m_cloudKey);
        raii->send();
    }

    void uploadToCloud(const std::string &localFilePath,
                       std::function<void(CloudError *error)> const& onFinish)
    {
        std::function<void(CloudError *error)> savedOnFinish = onFinish;
        S3RequestRAII *raii = new S3RequestRAII();
        raii->setRequestType(HttpRequest::Type::PUT);
        raii->attachFile(localFilePath);
        raii->headers["Content-Type"] = "application/octet-stream";
        raii->onSuccess = [=] (const std::vector<char> &, CloudError *error) {
            savedOnFinish(NULL);
        };
        raii->onError = [=](CloudError *error) {
            savedOnFinish(error);
        };
        raii->sign(m_cloudDir, m_cloudKey);
        raii->send();
    }

    const std::string &getName() const
    {
        return m_cloudKey;
    }

    const std::string &getLastModified() const
    {
        return m_lastModified;
    }
};

////////////////////////////////////////////////////////////////////////////////

class S3BucketParser : public SAXDelegator
{
    Array *m_files;
    std::string m_bucketName;
    std::string m_fileKey;
    std::string m_fileLastModified;

    enum State {
        TopLevel = 0,
        BucketLevel,
        BucketName,
        FileLevel,
        FileKey,
        FileLastModified
    } m_state;

    class StateSwitcher
    {
        struct Entry
        {
            std::string tag;
            State from;
            State to;
        };

        std::vector<Entry> m_entries;
        State &m_parentState;

    public:
        StateSwitcher(State &parentState)
            : m_parentState(parentState)
        {
        }

        void addSwitch(State from, State to, const char *tag)
        {
            Entry e;
            e.tag = tag;
            e.from = from;
            e.to = to;
            m_entries.push_back(e);
        }

        void performSwitch(const char *name, bool didTagEnded)
        {
            if (didTagEnded) {
                for (Entry &e : m_entries)
                    if (m_parentState == e.to && e.tag == name) {
                        m_parentState = e.from;
                    }
            } else {
                for (Entry &e : m_entries)
                    if (m_parentState == e.from && e.tag == name) {
                        m_parentState = e.to;
                    }
            }
        }
    };

    StateSwitcher m_stateSwitcher;

public:
    S3BucketParser()
        : m_files(Array::create())
        , m_state(TopLevel)
        , m_stateSwitcher(m_state)
    {
        m_files->retain();
        m_stateSwitcher.addSwitch(TopLevel, BucketLevel, "ListBucketResult");
        m_stateSwitcher.addSwitch(BucketLevel, FileLevel, "Contents");
        m_stateSwitcher.addSwitch(BucketLevel, BucketName, "Name");
        m_stateSwitcher.addSwitch(FileLevel, FileKey, "Key");
        m_stateSwitcher.addSwitch(FileLevel, FileLastModified, "LastModified");
    }

    ~S3BucketParser()
    {
        m_files->release();
    }

    /// @return array of S3CloudFile instances.
    Array *parseFilesList(const char *data, size_t szData)
    {
        SAXParser parser;
        parser.init("UTF-8");
        parser.setDelegator(this);
        parser.parse(data, szData);
        return m_files;
    }

protected:
    bool checkStateAndTag(State state, const char *givenTag, const char *expectedTag) const
    {
        return m_state == state && 0 == strcmp(givenTag, expectedTag);
    }

    void startElement(void *ctx, const char *name, const char **atts)
    {
        CC_UNUSED_PARAM(ctx);
        m_stateSwitcher.performSwitch(name, false);
    }

    void endElement(void *ctx, const char *name)
    {
        CC_UNUSED_PARAM(ctx);
        m_stateSwitcher.performSwitch(name, true);
    }

    void textHandler(void *ctx, const char *s, int len)
    {
        CC_UNUSED_PARAM(ctx);
        if (m_state == BucketName)
            m_bucketName.assign(s, len);
        if (m_state == FileKey)
            m_fileKey.assign(s, len);
        if (m_state == FileLastModified) {
            m_fileLastModified.assign(s, len);
            spawnFile();
        }
    }

    void spawnFile()
    {
        if (m_bucketName.empty() || m_fileKey.empty() || m_fileLastModified.empty())
            return;
        auto ret = new S3CloudFile();
        ret->m_cloudDir = m_bucketName;
        ret->m_cloudKey = m_fileKey;
        ret->m_lastModified = m_fileLastModified;
        m_files->addObject(ret);
        ret->release();

        m_fileKey.clear();
        m_fileLastModified.clear();
    }
};

////////////////////////////////////////////////////////////////////////////////

class S3CloudDir : public CloudDir
{
public:
    std::string m_cloudDir;

    void updateFilesListAsync(std::string const& prefix,
                              std::function<void(Array *files, CloudError *error)> const& onFinish)
    {
        S3RequestRAII *raii = new S3RequestRAII();
        raii->setRequestType(HttpRequest::Type::GET);
        std::function<void(Array *files, CloudError *error)> savedOnFinish = onFinish;
        raii->onSuccess = [=](const std::vector<char> &data, CloudError *error) {
            S3BucketParser parser;
            Array *files = parser.parseFilesList(data.data(), data.size());
            savedOnFinish(files, NULL);
        };
        raii->onError = [=](CloudError *error) {
            savedOnFinish(NULL, error);
        };

        if (!prefix.empty())
            raii->addQueryItem("prefix", prefix);
        raii->sign(m_cloudDir, "");
        raii->send();
    }

    void updateFilesListAsync(std::function<void(Array *files, CloudError *error)> const& onFinish)
    {
        updateFilesListAsync("", onFinish);
    }

    CloudFile *createFile(const std::string &key)
    {
        auto ret = new S3CloudFile();
        ret->m_cloudDir = m_cloudDir;
        ret->m_cloudKey = key;
        ret->autorelease();
        return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////

AmazonS3Cloud *g_amazonS3Instance = nullptr;

void cleanup_amazonS3Instance()
{
    delete g_amazonS3Instance;
    g_amazonS3Instance = 0;
}

AmazonS3Cloud &AmazonS3Cloud::get()
{
    if (!g_amazonS3Instance) {
        g_amazonS3Instance = new AmazonS3Cloud();
        atexit(cleanup_amazonS3Instance);
    }
    return *g_amazonS3Instance;
}

void AmazonS3Cloud::setSharedSecretAccessKey(const std::string &secretKey)
{
    g_S3SharedSecredAccessKey = secretKey;
}

void AmazonS3Cloud::setSharedAccessKey(const std::string &key)
{
    g_S3SharedAccessKey = key;
}

CloudDir *AmazonS3Cloud::createDir(const std::string &bucket)
{
    auto ret = new S3CloudDir();
    ret->m_cloudDir = bucket;
    ret->autorelease();
    return ret;
}

CloudFile *AmazonS3Cloud::createFile(const std::string &bucket, const std::string &key)
{
    auto ret = new S3CloudFile();
    ret->m_cloudDir = bucket;
    ret->m_cloudKey = key;
    ret->autorelease();
    return ret;
}

const char *CloudError::prettyPrint() const
{
    String *s = NULL;
    if (developerNote.empty())
        s = String::createWithFormat("error (url=\"%s\", httpStatusCode=%d, didOccuredAtPostprocess=%s)",
                                       url.c_str(),
                                       httpStatusCode,
                                       didOccuredAtPostprocess ? "true" : "false");
    else
        s = String::createWithFormat("error (url=\"%s\", httpStatusCode=%d, didOccuredAtPostprocess=%s, developerNote=\"%s\")",
                                       url.c_str(),
                                       httpStatusCode,
                                       didOccuredAtPostprocess ? "true" : "false",
                                       developerNote.c_str());
    return s->getCString();
}

std::string ccEncodeToBase64(const std::string &data)
{
    return S3Utils::encodeBase64(data);
}

} // namespace cocos2d
