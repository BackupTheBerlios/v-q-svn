//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "wpropbool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TWinPropBool *WinPropBool;
//---------------------------------------------------------------------------
__fastcall TWinPropBool::TWinPropBool(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TWinPropBool::FormShow(TObject *Sender)
{
        Val->SetFocus();        
}
//---------------------------------------------------------------------------

