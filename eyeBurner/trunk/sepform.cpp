//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "sepform.h"
#include "ebform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSeparatorForm *SeparatorForm;
//---------------------------------------------------------------------------
__fastcall TSeparatorForm::TSeparatorForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSeparatorForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        EyeBurnerForm->LoopTimerTimer(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TSeparatorForm::OkButClick(TObject *Sender)
{
        Close();
        EyeBurnerForm->blk();
}
//---------------------------------------------------------------------------
void __fastcall TSeparatorForm::DelButClick(TObject *Sender)
{
        EyeBurnerForm->state = 3;
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TSeparatorForm::NoButClick(TObject *Sender)
{
        EyeBurnerForm->state = 0;
        EyeBurnerForm->del_cnt_cur=EyeBurnerForm->del_cnt;
        Close();
}
//---------------------------------------------------------------------------

