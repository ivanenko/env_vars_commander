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

#include <string>
#include <cstring>
#include <map>
#include "extension.h"
#include "descriptions.h"

const char* dialog_new = R"(
object DialogBox: TDialogBox
  Left = 2260
  Height = 187
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Top = 177
  Width = 445
  Caption = 'New Environment variable'
  ClientHeight = 187
  ClientWidth = 445
  OnShow = DialogBoxShow
  Position = poScreenCenter
  LCLVersion = '1.6.0.4'
  object Label1: TLabel
    Left = 13
    Height = 17
    Top = 9
    Width = 84
    Caption = 'Variable name:'
    ParentColor = False
  end
  object edtVarName: TEdit
    Left = 13
    Height = 27
    Top = 32
    Width = 419
    TabOrder = 0
    Text = ''
  end
  object BitBtn2: TBitBtn
    AnchorSideRight.Side = asrCenter
    AnchorSideBottom.Side = asrCenter
    Left = 272
    Height = 32
    Top = 144
    Width = 75
    Anchors = [akRight, akBottom]
    Default = True
    DefaultCaption = True
    Kind = bkOK
    ModalResult = 1
    TabOrder = 1
    OnClick = ButtonClick
  end
  object btnCancel: TBitBtn
    AnchorSideRight.Side = asrCenter
    AnchorSideBottom.Side = asrCenter
    Left = 357
    Height = 32
    Top = 144
    Width = 75
    Anchors = [akRight, akBottom]
    Cancel = True
    DefaultCaption = True
    Kind = bkCancel
    ModalResult = 2
    TabOrder = 2
    OnClick = ButtonClick
  end
  object Label2: TLabel
    Left = 13
    Height = 17
    Top = 72
    Width = 31
    Caption = 'Value'
    ParentColor = False
  end
  object edtValue: TEdit
    Left = 13
    Height = 27
    Top = 96
    Width = 419
    TabOrder = 3
    Text = ''
  end
end
)";

const char* dialog_edit = R"(
object DialogBox: TDialogBox
  AnchorSideBottom.Side = asrBottom
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Edit Environment variable'
  Left = 2264
  Height = 165
  Top = 186
  Width = 432
  Anchors = [akBottom]
  ClientHeight = 164
  ClientWidth = 432
  OnShow = DialogBoxShow
  Position = poScreenCenter
  LCLVersion = '1.8.0.4'
  object Label1: TLabel
    Left = 10
    Height = 17
    Top = 8
    Width = 49
    Caption = 'Variable:'
    ParentColor = False
  end
  object varName: TLabel
    Left = 72
    Height = 17
    Top = 8
    Width = 52
    Caption = 'varName'
    ParentColor = False
  end
  object btnCancel: TBitBtn
    Left = 345
    Height = 32
    Top = 122
    Width = 75
    Cancel = True
    DefaultCaption = True
    Kind = bkCancel
    ModalResult = 2
    TabOrder = 0
    OnClick = ButtonClick
  end
  object btnOK: TBitBtn
    Left = 260
    Height = 32
    Top = 122
    Width = 75
    Default = True
    DefaultCaption = True
    Kind = bkOK
    ModalResult = 1
    TabOrder = 1
    OnClick = ButtonClick
  end
  object lblDescription: TLabel
    Left = 10
    Height = 17
    Top = 85
    Width = 77
    Caption = ''
    ParentColor = False
  end
  object edtValue: TMemo
    Left = 8
    Height = 51
    Top = 30
    Width = 412
    Lines.Strings = (
      ''
    )
    TabOrder = 0
  end
end
)";

//object edtValue: TEdit
//        Left = 8
//Height = 27
//Top = 30
//Width = 412
//TabOrder = 2
//end


const char* test = R"(
object DialogBox: TDialogBox
  AnchorSideBottom.Side = asrBottom
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Edit Environment variable'
  Left = 2264
  Height = 155
  Top = 186
  Width = 432
  Anchors = [akBottom]
  ClientHeight = 150
  ClientWidth = 432
  OnShow = DialogBoxShow
  Position = poScreenCenter
  LCLVersion = '1.8.0.4'
  object ListBox1: TListBox
    Left = 5
    Height = 80
    Top = 10
    Width = 200
    Columns = 2
    ItemHeight = 23
    ScrollWidth = 98
    TabOrder = 1
  end
  object Memo1: TMemo
    Left = 220
    Height = 100
    Top = 10
    Width = 200
    Lines.Strings = (
      'sdfsdfsdsdfsfdsdfsdfsdfsdf:\asas\asasas\:sdfsdfsdfsdfsdfsdf:sdfsdfsdfsdfsdfsdf'
      ''
      'sdfsdfsfsdf ffffff'
    )
    TabOrder = 0
  end

end
)";

extern char **environ;
tExtensionStartupInfo *gExtensionInfo = NULL;
std::string gVarName;

intptr_t DCPCALL DlgProcEdit(uintptr_t pDlg, char *DlgItemName, intptr_t Msg, intptr_t wParam, intptr_t lParam)
{
    char *value = NULL, *descr = NULL;

    switch (Msg){
        case DN_INITDIALOG:
            gExtensionInfo->SendDlgMsg(pDlg, "varName", DM_SETTEXT, (intptr_t)gVarName.c_str(), 0);

            value = getenv(gVarName.c_str());
            if(value)
                //gExtensionInfo->SendDlgMsg(pDlg, "edtValue", DM_LISTADDSTR, (intptr_t)value, 0);
                gExtensionInfo->SendDlgMsg(pDlg, "edtValue", DM_SETTEXT, (intptr_t)value, 0);

            descr = find_descr(gVarName.c_str());
            if(descr)
                gExtensionInfo->SendDlgMsg(pDlg, "lblDescription", DM_SETTEXT, (intptr_t)descr, 0);
            else
                gExtensionInfo->SendDlgMsg(pDlg, "lblDescription", DM_SETTEXT, (intptr_t)"", 0);

            break;

        case DN_CLICK:
            if(strcmp(DlgItemName, "btnOK")==0){
                char *new_value = (char*)gExtensionInfo->SendDlgMsg(pDlg, "edtValue", DM_GETTEXT, 0, 0);
                if(strcmp(value, new_value) != 0)
                    setenv(gVarName.c_str(), value, 1);

                gExtensionInfo->SendDlgMsg(pDlg, DlgItemName, DM_CLOSE, 1, 0);
            } else if(strcmp(DlgItemName, "btnCancel")==0){
                gExtensionInfo->SendDlgMsg(pDlg, DlgItemName, DM_CLOSE, 2, 0);
            }

            break;
    }
    return 0;
}

void show_edit_dialog(std::string varName, tExtensionStartupInfo *pExtension)
{
    gExtensionInfo = pExtension;
    gVarName = varName;

    gExtensionInfo->DialogBoxLFM((intptr_t)dialog_edit, strlen(dialog_edit), DlgProcEdit);
}

//========== New dialog ==============
intptr_t DCPCALL DlgProcNew(uintptr_t pDlg, char *DlgItemName, intptr_t Msg, intptr_t wParam, intptr_t lParam)
{
    char *value = NULL, *name = NULL;

    switch (Msg){
        case DN_INITDIALOG:
            break;

        case DN_CLICK:
            if(strcmp(DlgItemName, "btnOK")==0){
                name = (char*)gExtensionInfo->SendDlgMsg(pDlg, "edtVarName", DM_GETTEXT, 0, 0);
                value = (char*)gExtensionInfo->SendDlgMsg(pDlg, "edtValue", DM_GETTEXT, 0, 0);

                int res = setenv(name, value, 0);

                gExtensionInfo->SendDlgMsg(pDlg, DlgItemName, DM_CLOSE, (res==0)? 1: 2, 0);
            } else if(strcmp(DlgItemName, "btnCancel")==0){
                gExtensionInfo->SendDlgMsg(pDlg, DlgItemName, DM_CLOSE, 3, 0);
            }

            break;
    }
    return 0;
}

int show_new_dialog(tExtensionStartupInfo *pExtension)
{
    gExtensionInfo = pExtension;
    return gExtensionInfo->DialogBoxLFM((intptr_t)dialog_new, strlen(dialog_new), DlgProcEdit);
}

//============ test dialog ===========
intptr_t DCPCALL DlgProcTest(uintptr_t pDlg, char *DlgItemName, intptr_t Msg, intptr_t wParam, intptr_t lParam)
{
    char *value = NULL, *name = NULL;

    switch (Msg){
        case DN_INITDIALOG:
            gExtensionInfo->SendDlgMsg(pDlg, "ListBox1", DM_LISTADDSTR, (intptr_t)"aaaaaaaa\teeeeee", 0);
            gExtensionInfo->SendDlgMsg(pDlg, "ListBox1", DM_LISTADDSTR, (intptr_t)"aaa\t11111", 0);
            gExtensionInfo->SendDlgMsg(pDlg, "ListBox1", DM_LISTADDSTR, (intptr_t)"aaa\ts", 0);
            break;

        case DN_CLICK:
            if(strcmp(DlgItemName, "btnOK")==0){
                gExtensionInfo->SendDlgMsg(pDlg, DlgItemName, DM_CLOSE, 1, 0);
            } else if(strcmp(DlgItemName, "btnCancel")==0){
                gExtensionInfo->SendDlgMsg(pDlg, DlgItemName, DM_CLOSE, 3, 0);
            }

            break;
    }
    return 0;
}

int show_test_dialog(tExtensionStartupInfo *pExtension)
{
    gExtensionInfo = pExtension;
    return gExtensionInfo->DialogBoxLFM((intptr_t)test, strlen(test), DlgProcTest);
}