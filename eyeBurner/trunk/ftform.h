//---------------------------------------------------------------------------

#ifndef ftformH
#define ftformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFirstTimeForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TButton *CloseBut;
        void __fastcall CloseButClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFirstTimeForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFirstTimeForm *FirstTimeForm;
//---------------------------------------------------------------------------
#endif
