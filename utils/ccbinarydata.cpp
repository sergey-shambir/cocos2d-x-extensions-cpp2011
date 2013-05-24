#include "ccbinarydata.h"

NS_CC_EXT_BEGIN

CCBinaryData *CCBinaryData::createAsCopy(const std::string &data)
{
    if (CCBinaryData *d = new CCBinaryData())
    {
        auto placeholder = (char *)malloc(sizeof(char) * (data.size() + 1));
        ::memcpy(placeholder, data.c_str(), sizeof(char) * (data.size() + 1));
        d->m_data = placeholder;
        d->m_size = data.size();
        d->m_isAllocated = true;
        d->m_isNullTerminated = true;
        return d;
    }
    return NULL;
}

CCBinaryData *CCBinaryData::createAsCopy(const std::vector<char> &data)
{
    if (CCBinaryData *d = new CCBinaryData())
    {
        auto placeholder = (char *)malloc(sizeof(char) * (data.size() + 1));
        ::memcpy(placeholder, data.data(), sizeof(char) * data.size());
        placeholder[data.size()] = '\0';
        d->m_data = placeholder;
        d->m_size = data.size();
        d->m_isAllocated = true;
        d->m_isNullTerminated = true;
        return d;
    }
    return NULL;
}

CCBinaryData *CCBinaryData::createAsCopy(const char *data, int size)
{
    if (CCBinaryData *d = new CCBinaryData())
    {
        auto placeholder = (char *)malloc(sizeof(char) * (size + 1));
        ::memcpy(placeholder, data, sizeof(char) * size);
        placeholder[size] = '\0';
        d->m_data = placeholder;
        d->m_size = size;
        d->m_isAllocated = true;
        d->m_isNullTerminated = true;
        return d;
    }
    return NULL;
}

CCBinaryData *CCBinaryData::createAsCopy(const char *nullTerminatedData)
{
    if (CCBinaryData *d = new CCBinaryData())
    {
        auto size = ::strlen(nullTerminatedData);
        auto placeholder = (char *)malloc(sizeof(char) * (size + 1));
        ::memcpy(placeholder, nullTerminatedData, sizeof(char) * size);
        placeholder[size] = '\0';
        d->m_data = placeholder;
        d->m_size = size;
        d->m_isAllocated = true;
        d->m_isNullTerminated = true;
        return d;
    }
    return NULL;
}

CCBinaryData *CCBinaryData::createAsReference(const std::string &data)
{
    if (CCBinaryData *d = new CCBinaryData())
    {
        d->m_data = data.c_str();
        d->m_size = data.size();
        d->m_isAllocated = false;
        d->m_isNullTerminated = true;
        return d;
    }
    return NULL;
}

CCBinaryData *CCBinaryData::createAsReference(const std::vector<char> &data)
{
    if (CCBinaryData *d = new CCBinaryData())
    {
        d->m_data = data.data();
        d->m_size = data.size();
        d->m_isAllocated = false;
        d->m_isNullTerminated = false;
        return d;
    }
    return NULL;
}

CCBinaryData *CCBinaryData::createAsReference(const char *data, int size)
{
    if (CCBinaryData *d = new CCBinaryData())
    {
        d->m_data = data;
        d->m_size = size;
        d->m_isAllocated = false;
        d->m_isNullTerminated = false;
        return d;
    }
    return NULL;
}

CCBinaryData *CCBinaryData::createAsReference(const char *nullTerminatedData)
{
    if (CCBinaryData *d = new CCBinaryData())
    {
        auto size = ::strlen(nullTerminatedData);
        d->m_data = nullTerminatedData;
        d->m_size = size;
        d->m_isAllocated = false;
        d->m_isNullTerminated = true;
        return d;
    }
    return NULL;
}

CCBinaryData *CCBinaryData::canonicalData(CCBinaryData *source)
{
    CCAssert(source, "CCBinaryData::canonicalData : 'source' cannot be null");
    if (source->m_isAllocated && source->m_isNullTerminated)
        return source;
    return CCBinaryData::createAsCopy(source->data(), source->size());
}

const char *CCBinaryData::data() const
{
    return m_data;
}

int CCBinaryData::size() const
{
    return m_size;
}

bool CCBinaryData::isNullTerminated() const
{
    return m_isNullTerminated;
}

CCBinaryData::~CCBinaryData()
{
    if (m_isAllocated)
        std::free(const_cast<char *>(m_data));
}

CCBinaryData::CCBinaryData()
    : m_data(0)
    , m_size(0)
    , m_isNullTerminated(false)
    , m_isAllocated(false)
{
}

void CCBinaryData::appendChunk(const char *data, int size)
{
    CCAssert(data, "CCBinaryData::appendChunk : 'data' cannot be null");
    CCAssert(m_isAllocated, "CCBinaryData::appendChunk : binary data is reference");
    char *placeholder = (char *)std::realloc(const_cast<char *>(m_data), m_size + size + 1);
    memcpy(placeholder + m_size, data, size);
    placeholder[m_size + size] = '\0';

    m_data = placeholder;
    m_size += size;
}

NS_CC_EXT_END