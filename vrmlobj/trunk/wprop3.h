//---------------------------------------------------------------------------
#ifndef wprop3H
#define wprop3H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TWinProp3 : public TForm
{
__published:	// IDE-managed Components
        TEdit *Val1;
        TEdit *Val2;
        TEdit *Val3;
        TButton *Button1;
        TButton *Button2;
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TWinProp3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWinProp3 *WinProp3;
//---------------------------------------------------------------------------
#endif
