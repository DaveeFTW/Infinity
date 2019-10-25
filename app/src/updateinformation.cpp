/*

Copyright (C) 2015, David "Davee" Morgan

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include "updateinformation.h"

UpdateInformation::UpdateInformation(void)
    : m_coreVersion(-1)
    , m_subsetVersion(-1)
{
}

const std::string& UpdateInformation::version(void) const
{
    return m_version;
}

void UpdateInformation::setVersion(const std::string& version)
{
    m_version = version;
}

const std::string& UpdateInformation::url(void) const
{
    return m_url;
}

void UpdateInformation::setUrl(const std::string& url)
{
    m_url = url;
}

int UpdateInformation::coreVersion(void) const
{
    return m_coreVersion;
}

void UpdateInformation::setCoreVersion(int version)
{
    m_coreVersion = version;
}

int UpdateInformation::subsetVersion(void) const
{
    return m_subsetVersion;
}

void UpdateInformation::setSubsetVersion(int version)
{
    m_subsetVersion = version;
}
