//---------------------------------------------------------------------------
#ifndef wproprotH
#define wproprotH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TWinPropRot : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *X;
        TCheckBox *Y;
        TCheckBox *Z;
        TEdit *Val;
        TButton *Button1;
        TButton *Button2;
        TRadioButton *RadRad;
        TRadioButton *RadStop;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TWinPropRot(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWinPropRot *WinPropRot;
//---------------------------------------------------------------------------
#endif
