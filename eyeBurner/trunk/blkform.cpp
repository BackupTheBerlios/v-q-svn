//---------------------------------------------------------------------------

#include <vcl.h>
#include <time.h>
#pragma hdrstop

#include "blkform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBlockForm *BlockForm;
//---------------------------------------------------------------------------
__fastcall TBlockForm::TBlockForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TBlockForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
        CanClose = false;        
}
//---------------------------------------------------------------------------
void __fastcall TBlockForm::FormShow(TObject *Sender)
{
        ShowCursor(false);
        randw = Width-InfoLab->Width;
        randh = Height-InfoLab->Height;
        srand((unsigned) time(NULL));
        InfoTimerTimer(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TBlockForm::FormHide(TObject *Sender)
{
        ShowCursor(true);
}
//---------------------------------------------------------------------------
void __fastcall TBlockForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        ShowCursor(true);
}
//---------------------------------------------------------------------------
void __fastcall TBlockForm::InfoTimerTimer(TObject *Sender)
{
        int w, h;
        InfoLab->Left = rand()%randw;
        InfoLab->Top = rand()%randh;
}
//---------------------------------------------------------------------------

