//---------------------------------------------------------------------------
#ifndef wpropboolH
#define wpropboolH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TWinPropBool : public TForm
{
__published:	// IDE-managed Components
        TComboBox *Val;
        TButton *Button1;
        TButton *Button2;
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TWinPropBool(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWinPropBool *WinPropBool;
//---------------------------------------------------------------------------
#endif
