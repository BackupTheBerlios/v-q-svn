//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "wprop3.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWinProp3 *WinProp3;
//---------------------------------------------------------------------------
__fastcall TWinProp3::TWinProp3(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TWinProp3::FormShow(TObject *Sender)
{
        Val1->SetFocus();        
}
//---------------------------------------------------------------------------

