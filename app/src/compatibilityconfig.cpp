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

#include "compatibilityconfig.h"
#include "compatibilitymodule.h"

#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>

namespace
{
    struct CompatibilityConfigReader
        : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, CompatibilityConfigReader>
    {
        enum States
        {
            EXPECT_ARRAY,
            EXPECT_OBJECT,
            EXPECT_KEY,
            EXPECT_VALUE
        };

        CompatibilityConfigReader(void)
            : m_state(EXPECT_ARRAY)
        {
        }

        bool StartArray(void)
        {
            if (m_state != EXPECT_ARRAY)
                return false;

            m_state = EXPECT_OBJECT;
            return true;
        }

        bool StartObject(void)
        {
            if (m_state != EXPECT_OBJECT)
                return false;

            m_state = EXPECT_KEY;
            m_currentModule = std::make_shared<CompatibilityModule>();
            return true;
        }

        bool String(const char* str, rapidjson::SizeType length, bool)
        {
            if (m_state == EXPECT_KEY)
            {
                m_lastKey = std::string(str, length);
                m_state = EXPECT_VALUE;
                return true;
            }
            else if (m_state == EXPECT_VALUE)
            {
                auto value = std::string(str, length);

                if (m_lastKey == "name")
                    m_currentModule->setName(value);
                else if (m_lastKey == "author")
                    m_currentModule->setAuthor(value);
                else if (m_lastKey == "path")
                    m_currentModule->setPath(value);

                m_state = EXPECT_KEY;
                return true;
            }
            else
            {
                return false;
            }
        }

        bool EndObject(rapidjson::SizeType)
        {
            if (m_state == EXPECT_KEY)
            {
                m_moduleList.push_back(m_currentModule);
                m_state = EXPECT_OBJECT;
                return true;
            }

            return false;
        }

        bool EndArray(rapidjson::SizeType) { return (m_state == EXPECT_OBJECT); }

        bool Default(void) { return false; }

        States m_state;
        std::string m_lastKey;
        CompatibilityModulePtr m_currentModule;
        CompatibilityModuleList m_moduleList;
    };
} // anonymous

CompatibilityModuleList parseCompatibilityConfig(const std::string& json)
{
    rapidjson::Reader reader;
    CompatibilityConfigReader handler;
    rapidjson::StringStream ss(json.c_str());

    if (!reader.Parse(ss, handler))
    {
        // TODO: handle errors
        return {};
    }

    return handler.m_moduleList;
}

std::string generateCompatibilityConfig(const CompatibilityModuleList& list)
{
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartArray();

    std::for_each(list.cbegin(), list.cend(), [&writer](auto module) {
        writer.StartObject();
        writer.Key("name");
        writer.String(module->name().c_str());
        writer.Key("author");
        writer.String(module->author().c_str());
        writer.Key("path");
        writer.String(module->path().c_str());
        writer.EndObject();
    });

    writer.EndArray();
    return std::string(s.GetString());
}
