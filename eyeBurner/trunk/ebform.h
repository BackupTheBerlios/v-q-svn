//---------------------------------------------------------------------------

#ifndef ebformH
#define ebformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "trayicon.h"
#include <Menus.hpp>
#include <time.h>
#include <ComCtrls.hpp>
#include "CSPIN.h"
#include <memory>
#include <vcl\registry.hpp>

#include "sepform.h"

using namespace std;
//---------------------------------------------------------------------------
class TEyeBurnerForm : public TForm
{
        friend TSeparatorForm; 
__published:	// IDE-managed Components
        TTimer *LoopTimer;
        TTrayIcon *TrayIcon;
        TPopupMenu *TrayPopMenu;
        TMenuItem *OpenTrayPopMenu;
        TMenuItem *EndTrayPopMenu;
        TPageControl *PageCon;
        TTabSheet *OptionsTabSheet;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TCheckBox *AutoChkBox;
        TButton *ConfSaveBut;
        TButton *RegClnBut;
        TTabSheet *AboutTabSheet;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label12;
        TTrackBar *e_time_sep;
        TTrackBar *e_time_brk;
        TTrackBar *e_time_del;
        TTrackBar *e_del_cnt;
        TLabel *e_time_sep_cur;
        TLabel *e_time_brk_cur;
        TLabel *e_time_del_cur;
        TLabel *e_del_cnt_cur;
        TLabel *e_time_sep_max;
        TLabel *e_time_brk_max;
        TLabel *e_time_del_max;
        TLabel *e_del_cnt_max;
        TLabel *e_time_sep_min;
        TLabel *e_time_brk_min;
        TLabel *e_time_del_min;
        TLabel *e_del_cnt_min;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall LoopTimerTimer(TObject *Sender);
        void __fastcall EndTrayPopMenuClick(TObject *Sender);
        void __fastcall OpenTrayPopMenuClick(TObject *Sender);
        void __fastcall AutoChkBoxClick(TObject *Sender);
        void __fastcall TrayIconRestore(TObject *Sender);
        void __fastcall ConfSaveButClick(TObject *Sender);
        void __fastcall RegClnButClick(TObject *Sender);
        void __fastcall Label9Click(TObject *Sender);
        void __fastcall Label7Click(TObject *Sender);
        void __fastcall e_time_sepChange(TObject *Sender);
        void __fastcall e_time_brkChange(TObject *Sender);
        void __fastcall e_time_delChange(TObject *Sender);
        void __fastcall e_del_cntChange(TObject *Sender);
private:	// User declarations
protected:
        void blk();
        
        //!
        int del_cnt_cur;
        //! current state of timer routine
        char state;
        //! time between breaks, for example an hour
        int time_sep;
        //! time length of break in work, i.e. 5 minutes
        int time_brk;
        //! time of breaking delay, i.e. 5 minutes
        int time_del;
        //! count of allowed break's delays
        int del_cnt;
        //! system registry
        auto_ptr<TRegistry> sysreg;
        //! registry path for this software
        const char *sysregpath;
        //! load configuration from sysreg
        void ConfLoad();
        //! read configuration from controls, update info, save it in registry
        void ConfSave();
public:		// User declarations
        __fastcall TEyeBurnerForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TEyeBurnerForm *EyeBurnerForm;
//---------------------------------------------------------------------------
#endif
