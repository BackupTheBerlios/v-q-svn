//---------------------------------------------------------------------------

#ifndef blkformH
#define blkformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TBlockForm : public TForm
{
__published:	// IDE-managed Components
        TTimer *InfoTimer;
        TLabel *InfoLab;
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall InfoTimerTimer(TObject *Sender);
private:	// User declarations
protected:
        unsigned randw, randh; 
public:		// User declarations
        __fastcall TBlockForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TBlockForm *BlockForm;
//---------------------------------------------------------------------------
#endif
