//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "wproprot.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWinPropRot *WinPropRot;
//---------------------------------------------------------------------------
__fastcall TWinPropRot::TWinPropRot(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TWinPropRot::FormShow(TObject *Sender)
{
        X->SetFocus();        
}
//---------------------------------------------------------------------------

void __fastcall TWinPropRot::FormCreate(TObject *Sender)
{
        RadRad->Checked = true;        
}
//---------------------------------------------------------------------------

