//---------------------------------------------------------------------------
#ifndef wcodeH
#define wcodeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TWinCode : public TForm
{
__published:	// IDE-managed Components
        TMemo *Code;
private:	// User declarations
public:		// User declarations
        __fastcall TWinCode(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWinCode *WinCode;
//---------------------------------------------------------------------------
#endif
