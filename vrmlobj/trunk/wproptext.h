//---------------------------------------------------------------------------
#ifndef wproptextH
#define wproptextH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TWinPropText : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TButton *Button2;
        TEdit *Val;
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TWinPropText(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWinPropText *WinPropText;
//---------------------------------------------------------------------------
#endif
