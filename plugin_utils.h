/*
Wfx plugin for working with Linux environment variables

Copyright (C) 2019 Ivanenko Danil (ivanenko.danil@gmail.com)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
        Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
*/

#ifndef ENV_VARS_COMMANDER_PLUGIN_UTILS_H
#define ENV_VARS_COMMANDER_PLUGIN_UTILS_H

#include <string>
#include <ctime>
#include <vector>
#include <sys/stat.h>
#include "common.h"

typedef std::basic_string<WCHAR> wcharstring;

typedef struct {
    int nCount;
    int nSize;
    WIN32_FIND_DATAW* resource_array;
} tResources, *pResources;

FILETIME get_now_time()
{
    time_t t2 = time(0);
    int64_t ft = (int64_t) t2 * 10000000 + 116444736000000000;
    FILETIME file_time;
    file_time.dwLowDateTime = ft & 0xffff;
    file_time.dwHighDateTime = ft >> 32;
    return file_time;
}

FILETIME parse_iso_time(std::string time)
{
    struct tm t, tz;
    strptime(time.c_str(), "%Y-%m-%dT%H:%M:%S%z", &t);
    long int gmtoff = t.tm_gmtoff;
    time_t t2 = mktime(&t) + gmtoff; //TODO gmtoff doesnt correct here
    int64_t ft = (int64_t) t2 * 10000000 + 116444736000000000;
    FILETIME file_time;
    file_time.dwLowDateTime = ft & 0xffff;
    file_time.dwHighDateTime = ft >> 32;
    return file_time;
}

std::vector<wcharstring> split(const wcharstring s, WCHAR separator)
{
    std::vector<std::basic_string<WCHAR> > result;

    int prev_pos = 0, pos = 0;
    while(pos <= s.size()){
        if(s[pos] == separator){
            result.push_back(s.substr(prev_pos, pos-prev_pos));
            prev_pos = ++pos;
        }
        pos++;
    }

    result.push_back(s.substr(prev_pos, pos-prev_pos));

    return result;
}

// convert 2-bytes string to UTF8 encoded string
std::string toUTF8(const WCHAR *p)
{
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert2;
    return convert2.to_bytes(std::u16string((char16_t*) p));
}

// convert UTF8 to 2-bytes string
wcharstring fromUTF8(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert2;
    std::u16string utf16 = convert2.from_bytes(str);
    return wcharstring((WCHAR*)utf16.data());
}

BOOL file_exists(const std::string& filename)
{
    struct stat buf;
    return stat(filename.c_str(), &buf) == 0;
}

#endif //ENV_VARS_COMMANDER_PLUGIN_UTILS_H
