/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file scopedclangoptions.h
/// \brief Converts options to form acceptable for clang

#include <string>
#include <vector>

#ifndef SCOPEDCLANGOPTIONS_H
#define SCOPEDCLANGOPTIONS_H

class ScopedClangOptions
{
public:
    ScopedClangOptions(const std::vector<std::string> &options);

    unsigned size() const;
    const char *const *data() const;

private:
    std::string m_storage;
    std::vector<const char *> m_pointers;
};

#endif // SCOPEDCLANGOPTIONS_H
