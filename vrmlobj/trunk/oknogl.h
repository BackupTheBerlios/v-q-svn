//---------------------------------------------------------------------------
#ifndef WinglH
#define WinglH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>

#include <memory>
#include <sstream>
#include <fstream>

#include "vobj.h"

//---------------------------------------------------------------------------
class TWin : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *Menu;
        TMenuItem *MenuFile;
        TMenuItem *MenuFileExit;
        TListBox *ObjsLst;
        TSplitter *Split;
        TMenuItem *N1;
        TMenuItem *MenuFileNew;
        TMenuItem *MenuObjs;
        TMenuItem *MenuObjsNew;
        TMenuItem *MenuObjsDel;
        TMenuItem *MenuFileOpen;
        TPanel *Panel1;
        THeaderControl *ObjHdr;
        TListBox *ObjProps;
        TOpenDialog *OpenDial;
        TSaveDialog *SaveDial;
        TMenuItem *MenuFileSave;
        TMenuItem *MenuFileSaveAs;
        TMenuItem *PokakodVRML1;
        TStatusBar *ObjPropsHint;
        void __fastcall MenuFileExitClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall MenuObjsNewClick(TObject *Sender);
        void __fastcall ObjPropsDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
        void __fastcall ObjHdrResize(TObject *Sender);
        void __fastcall ObjHdrSectionResize(
          THeaderControl *HeaderControl, THeaderSection *Section);
        void __fastcall ObjsLstClick(TObject *Sender);
        void __fastcall MenuFileOpenClick(TObject *Sender);
        void __fastcall MenuFileNewClick(TObject *Sender);
        void __fastcall MenuFileSaveClick(TObject *Sender);
        void __fastcall MenuFileSaveAsClick(TObject *Sender);
        void __fastcall ObjPropsDblClick(TObject *Sender);
        void __fastcall MenuObjsDelClick(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall PokakodVRML1Click(TObject *Sender);
        void __fastcall ObjPropsMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall ObjsLstDblClick(TObject *Sender);
private:	// User declarations
        vector< VObj * > objs;
        void init_objs();       // inicjowanie wektora objs
        void fobjs_del();       // usuwa wszelkie obiektu z fobjs
        vector< VObj * > fobjs; // obiekty dla danego pliku

        ostringstream forg;     // zawartosc oryginalnego pliku
        fstream fout;        // plik wyjsciowy
        bool fnew;          // czy to nowy plik
        bool fchgd;              // czy plik zmieniony od ost. zapisu
        bool fileclose(); // zamyka plik
        void filesave();

        String wintit, fname;
        void ObjPropsAdd( const VObj * );
public:		// User declarations
        __fastcall TWin(TComponent* Owner);
__published:
        TColorDialog *WinPropCol;
};
//---------------------------------------------------------------------------
extern PACKAGE TWin *Win;
//---------------------------------------------------------------------------

#endif
