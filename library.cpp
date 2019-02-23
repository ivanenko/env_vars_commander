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
#include <string>
#include <algorithm>
#include <codecvt>
#include <locale>
#include <fstream>
#include <sstream>

#include "library.h"
#include "wfxplugin.h"
#include "extension.h"
#include "plugin_utils.h"
#include "dialogs.h"
#include "descriptions.h"

extern char **environ;

#define _createstr u"/<Create New>"

int gPluginNumber, gCryptoNr;
tProgressProcW gProgressProcW = NULL;
tLogProcW gLogProcW = NULL;
tRequestProcW gRequestProcW = NULL;
tCryptProcW gCryptProcW = NULL;

tExtensionStartupInfo *gExtensionStartupInfo = NULL;


void DCPCALL FsGetDefRootName(char* DefRootName,int maxlen)
{
    strncpy(DefRootName, "Env variables", maxlen);
}

void DCPCALL ExtensionInitialize(tExtensionStartupInfo* StartupInfo)
{
    gExtensionStartupInfo = StartupInfo;
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
    memcpy(FindData->cFileName, _createstr+1, MAX_PATH);
    FindData->ftCreationTime = get_now_time();
    FindData->ftLastWriteTime = get_now_time();

    wcharstring wPath(Path);
    std::replace(wPath.begin(), wPath.end(), u'\\', u'/');

    int total = 0;
    for(char **current = environ; *current; current++) {
        total++;
    }

    pResources pRes = new tResources;
    pRes->nCount = 0;
    pRes->nSize = 0;
    pRes->resource_array = new WIN32_FIND_DATAW[total];

    int i=0;
    for(char **current = environ; *current; current++) {
        pRes->nSize++;
        std::string s(*current);
        int nPos = s.find("=");
        if(nPos != std::string::npos){
            s = s.substr(0, nPos);
        }

        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
        std::u16string utf16 = convert.from_bytes(s);
        size_t str_size = (MAX_PATH > utf16.size()+1)? (utf16.size()+1): MAX_PATH;
        memcpy(pRes->resource_array[i].cFileName, utf16.data(), sizeof(WCHAR) * str_size);
        pRes->resource_array[i].dwFileAttributes = 0;
        pRes->resource_array[i].nFileSizeLow = 0;
        pRes->resource_array[i].nFileSizeHigh = 0;
        pRes->resource_array[i].ftCreationTime = get_now_time();
        pRes->resource_array[i].ftLastWriteTime = get_now_time();
        pRes->resource_array[i].ftLastAccessTime = get_now_time();

        i++;
    }

    return (HANDLE) pRes;
}

BOOL DCPCALL FsFindNextW(HANDLE Hdl,WIN32_FIND_DATAW *FindData)
{
    pResources pRes = (pResources) Hdl;
    if(!pRes)
        return false;

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

int DCPCALL FsGetFileW(WCHAR* RemoteName, WCHAR* LocalName, int CopyFlags, RemoteInfoStruct* ri)
{
    if(CopyFlags & FS_COPYFLAGS_RESUME)
        return FS_FILE_NOTSUPPORTED;

    try{
        std::ofstream ofs;
        //std::stringstream ofs;
        wcharstring wRemoteName(RemoteName), wLocalName(LocalName);
        wRemoteName = wRemoteName.substr(1);

        ofs.open(toUTF8(wLocalName.data()), std::ios::binary | std::ofstream::out | std::ios::trunc);
        if(!ofs || ofs.bad())
            return FS_FILE_WRITEERROR;

        std::string var = toUTF8(RemoteName+1);
        char *value = getenv(var.c_str());
        char *descr = find_descr(var.c_str());

        ofs << "Variable: ";
        ofs << var;
        ofs << "\n\n";
        ofs <<  value; // (value==NULL)? "<empty>": value;
        if(descr){
            ofs << "\n\n### description ### \n";
            ofs << descr;
        }

        ofs.flush();

        if(CopyFlags & FS_COPYFLAGS_MOVE)
            FsDeleteFileW(RemoteName);

    } catch (std::runtime_error & e){
        gRequestProcW(gPluginNumber, RT_MsgOK, (WCHAR*)u"Error", (WCHAR*) fromUTF8(e.what()).c_str(), NULL, 0);
        return FS_FILE_READERROR;
    }

    //ofstream closes file in destructor
    return FS_FILE_OK;
}

int DCPCALL FsPutFileW(WCHAR* LocalName,WCHAR* RemoteName,int CopyFlags) {
    return FS_FILE_NOTSUPPORTED;
}

BOOL DCPCALL FsDeleteFileW(WCHAR* RemoteName)
{
    std::string var = toUTF8(RemoteName+1);
    return unsetenv(var.c_str()) == 0;
}

int DCPCALL FsExecuteFileW(HWND MainWin, WCHAR* RemoteName, WCHAR* Verb)
{
    wcharstring wVerb(Verb), wRemoteName(RemoteName);

    if(wVerb.find((WCHAR*)u"open") == 0){
        wcharstring wRemote(RemoteName), wCreate((WCHAR*)_createstr);
        if (wRemote == wCreate){
            show_test_dialog(gExtensionStartupInfo);
            //show_new_dialog(gExtensionStartupInfo);
        } else {
            std::string var = toUTF8(RemoteName+1);
            show_edit_dialog(var, gExtensionStartupInfo);
        }
    }

    return FS_EXEC_OK;
}



/**************************************************************************************/
/*********************** content plugin = custom columns part! ************************/
/**************************************************************************************/

#define fieldcount 1
char* fieldnames[fieldcount] = {"Value"};
int fieldtypes[fieldcount] = {ft_string};
char* fieldunits_and_multiplechoicestrings[fieldcount] = {""};
int fieldflags[fieldcount] = {contflags_substsize};
int sortorders[fieldcount] = {-1};

int DCPCALL FsContentGetSupportedField(int FieldIndex,char* FieldName,char* Units,int maxlen)
{
    if (FieldIndex<0 || FieldIndex>=fieldcount)
        return ft_nomorefields;

    memcpy(FieldName, fieldnames[FieldIndex], maxlen-1);
    strcpy(Units, fieldunits_and_multiplechoicestrings[FieldIndex]);
    return fieldtypes[FieldIndex];
}

int DCPCALL FsContentGetValueW(WCHAR* FileName, int FieldIndex, int UnitIndex, void* FieldValue, int maxlen, int flags)
{
    std::string var = toUTF8(FileName+1);
    char *value = NULL;
    switch (FieldIndex) {
        case 0:  // "Value"
            //*(int *)FieldValue = 333;
            value = getenv(var.c_str());
            if(value){
                strncpy((char*)FieldValue, value, maxlen-1);
            } else {
                strncpy((char*)FieldValue, "", 1);
            }

            break;

        default:
            return ft_nosuchfield;
    }

    return fieldtypes[FieldIndex];  // very important!
}

int DCPCALL FsContentGetSupportedFieldFlags(int FieldIndex)
{
    if (FieldIndex==-1)
        return contflags_substsize | contflags_edit;
    else if (FieldIndex<0 || FieldIndex>=fieldcount)
        return 0;
    else
        return fieldflags[FieldIndex];
}

int DCPCALL FsContentGetDefaultSortOrder(int FieldIndex)
{
    if (FieldIndex<0 || FieldIndex>=fieldcount)
        return 1;
    else
        return sortorders[FieldIndex];
}
