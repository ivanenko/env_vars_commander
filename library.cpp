/*
Wfx plugin for working with Environment Variables

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

#include <iostream>
#include <cstring>
#include <algorithm>

#include "library.h"
#include "wfxplugin.h"


int gPluginNumber, gCryptoNr;
tProgressProcW gProgressProcW = NULL;
tLogProcW gLogProcW = NULL;
tRequestProcW gRequestProcW = NULL;
tCryptProcW gCryptProcW = NULL;

typedef std::basic_string<WCHAR> wcharstring;

void DCPCALL FsGetDefRootName(char* DefRootName,int maxlen)
{
    strncpy(DefRootName, "Env variables", maxlen);
}

int DCPCALL FsInitW(int PluginNr, tProgressProcW pProgressProc, tLogProcW pLogProc, tRequestProcW pRequestProc)
{
    gProgressProcW = pProgressProc;
    gLogProcW = pLogProc;
    gRequestProcW = pRequestProc;
    gPluginNumber = PluginNr;

    return 0;
}

HANDLE DCPCALL FsFindFirstW(WCHAR* Path, WIN32_FIND_DATAW *FindData)
{
    memset(FindData, 0, sizeof(WIN32_FIND_DATAW));

    wcharstring wPath(Path);
    std::replace(wPath.begin(), wPath.end(), u'\\', u'/');


    pResources pRes = NULL;
    try{
        pRes = prepare_folder_result(result, wPath == (WCHAR*)u"/");
    } catch (std::runtime_error & e){
        gRequestProcW(gPluginNumber, RT_MsgOK, (WCHAR*)u"Error", (WCHAR*) fromUTF8(e.what()).c_str(), NULL, 0);
    }

    if(!pRes || pRes->nSize==0)
        return (HANDLE)-1;

    if(pRes->resource_array && pRes->nSize>0)
        memcpy(FindData, pRes->resource_array, sizeof(WIN32_FIND_DATAW));

    return (HANDLE) pRes;
}

BOOL DCPCALL FsFindNextW(HANDLE Hdl,WIN32_FIND_DATAW *FindData)
{
    pResources pRes = (pResources) Hdl;
    pRes->nCount++;
    if(pRes->nCount < pRes->nSize){
        memcpy(FindData, &pRes->resource_array[pRes->nCount], sizeof(WIN32_FIND_DATAW));
        return true;
    } else {
        return false;
    }
}

int DCPCALL FsFindClose(HANDLE Hdl){
    pResources pRes = (pResources) Hdl;
    if(pRes){
        if(pRes->resource_array)
            delete[] pRes->resource_array;

        delete pRes;
    }

    return 0;
}