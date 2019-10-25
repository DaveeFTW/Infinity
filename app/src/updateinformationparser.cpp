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

#include "updateinformationparser.h"
#include "updateinformation.h"

#include <rapidjson/document.h>

UpdateInformationPtr parseUpdateInformation(const std::string& json)
{
    rapidjson::Document document;
    document.Parse(json.c_str());
    auto update = std::make_shared<UpdateInformation>();

    auto version = document.FindMember("version");

    if (version != document.MemberEnd())
    {
        if (version->value.IsString())
            update->setVersion(version->value.GetString());
    }

    version = document.FindMember("url");

    if (version != document.MemberEnd())
    {
        if (version->value.IsString())
            update->setUrl(version->value.GetString());
    }

    version = document.FindMember("core");

    if (version != document.MemberEnd())
    {
        if (version->value.IsInt())
            update->setCoreVersion(version->value.GetInt());
    }

    version = document.FindMember("subset");

    if (version != document.MemberEnd())
    {
        if (version->value.IsInt())
            update->setSubsetVersion(version->value.GetInt());
    }

    return update;
}
