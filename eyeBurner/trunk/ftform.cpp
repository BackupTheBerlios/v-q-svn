//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ftform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFirstTimeForm *FirstTimeForm;
//---------------------------------------------------------------------------
__fastcall TFirstTimeForm::TFirstTimeForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFirstTimeForm::CloseButClick(TObject *Sender)
{
        Close();        
}
//---------------------------------------------------------------------------
