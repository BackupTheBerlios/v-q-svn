//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "objs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TAddObj *AddObj;
//---------------------------------------------------------------------------
__fastcall TAddObj::TAddObj(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAddObj::FormShow(TObject *Sender)
{
        ObjsList->ItemIndex = 0;        
}
//---------------------------------------------------------------------------
