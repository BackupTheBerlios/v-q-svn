//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "wproptext.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TWinPropText *WinPropText;
//---------------------------------------------------------------------------
__fastcall TWinPropText::TWinPropText(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TWinPropText::FormShow(TObject *Sender)
{
        Val->SetFocus();        
}
//---------------------------------------------------------------------------

