//---------------------------------------------------------------------------
#ifndef wpropH
#define wpropH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------
class TWinProp : public TForm
{
__published:	// IDE-managed Components
        TButton *Button2;
        TMemo *Val;
        TButton *Button1;
private:	// User declarations
public:		// User declarations
        __fastcall TWinProp(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWinProp *WinProp;
//---------------------------------------------------------------------------

#endif
