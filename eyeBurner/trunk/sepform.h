//---------------------------------------------------------------------------

#ifndef sepformH
#define sepformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TSeparatorForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TButton *OkBut;
        TButton *DelBut;
        TButton *NoBut;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall OkButClick(TObject *Sender);
        void __fastcall DelButClick(TObject *Sender);
        void __fastcall NoButClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TSeparatorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSeparatorForm *SeparatorForm;
//---------------------------------------------------------------------------
#endif
