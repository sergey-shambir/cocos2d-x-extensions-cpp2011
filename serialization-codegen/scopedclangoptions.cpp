/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file scopedclangoptions.cpp
/// \brief

#include "scopedclangoptions.h"

ScopedClangOptions::ScopedClangOptions(const std::vector<std::string> &options)
{
    std::vector<size_t> offsets;
    offsets.reserve(options.size());
    m_pointers.reserve(options.size());

    size_t currentOffset = 0;
    for (const auto& opt : options)
    {
        m_storage.append(opt.data(), 1u + opt.size());

        offsets.push_back(currentOffset);
        currentOffset += 1u + opt.size();
    }

    for (size_t offset : offsets)
    {
        m_pointers.push_back(m_storage.data() + offset);
    }
}

unsigned ScopedClangOptions::size() const
{
    return m_pointers.size();
}

const char *const*ScopedClangOptions::data() const
{
    return m_pointers.data();
}
