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

#ifndef COCOS2D_CCS3_H
#define COCOS2D_CCS3_H

#include "cocos2d.h"
#include <functional>

namespace cocos2d {

class CloudError;
class CloudDir;
class CloudFile;

std::string ccEncodeToBase64(const std::string &data);

class AmazonS3Cloud
{
public:
    /// Please call only from main thread.
    static AmazonS3Cloud &get();

    void setSharedSecretAccessKey(const std::string &secretKey);
    void setSharedAccessKey(const std::string &key);

    CloudDir *createDir(const std::string &bucket);
    CloudFile *createFile(const std::string &bucket, const std::string &key);
};

class CloudError : public CCObject
{
public:
    /// Returned pointer is from autoreleased cocos2d::String.
    const char *prettyPrint() const;

    /// True if response returned data, but post-processing failed.
    bool didOccuredAtPostprocess;
    int httpStatusCode;
    std::string url;
    /// Info for developer, can be empty.
    std::string developerNote;
};

class CloudDir : public CCObject
{
public:
    virtual ~CloudDir() {}

    /// Updates list of directory files in separate thread.
    /// @param onFinish Called in main thread, and either files or error is NULL.
    virtual void updateFilesListAsync(std::function<void(CCArray *files, CloudError *error)> const& onFinish) = 0;

    /// Updates list of directory files in separate thread.
    /// @param prefix Lists only keys with given prefix.
    /// @param onFinish Called in main thread, and either files or error is NULL.
    virtual void updateFilesListAsync(std::string const& prefix,
                                      std::function<void(CCArray *files, CloudError *error)> const& onFinish) = 0;

    /// Creates file handle with given key.
    virtual CloudFile *createFile(const std::string &key) = 0;
};

class CloudFile : public CCObject
{
public:
    virtual ~CloudFile() {}

    /// Downloads file from remote server.
    /// @param downloadPath Path relative to cocos2d writeable path.
    /// @param onFinish Called in main thread, on success NULL error passed.
    virtual void downloadFromCloud(const std::string &downloadPath,
                               std::function<void(CloudError *error)> const& onFinish) = 0;

    /// Deletes given file from remote server.
    virtual void deleteOnCloud(std::function<void(CloudError *error)> const& onFinish) = 0;

    /// Uploads given local file using cloud file bucket and key.
    virtual void uploadToCloud(const std::string &localFilePath,
                               std::function<void(CloudError *error)> const& onFinish) = 0;

    virtual const std::string &getName() const = 0;
    virtual const std::string &getLastModified() const = 0;
};

} // namespace cocos2d

#endif // COCOS2D_CCS3_H
