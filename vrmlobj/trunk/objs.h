//---------------------------------------------------------------------------
#ifndef objsH
#define objsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------
class TAddObj : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TButton *Button2;
        TComboBox *ObjsList;
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TAddObj(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAddObj *AddObj;
//---------------------------------------------------------------------------

#endif
