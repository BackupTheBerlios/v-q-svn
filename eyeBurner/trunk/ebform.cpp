//---------------------------------------------------------------------------

#include <vcl.h>
#include <sstream>
#pragma hdrstop

#include "ebform.h"
#include "blkform.h"
#include "ftform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma link "CSPIN"
#pragma resource "*.dfm"

using namespace std;

TEyeBurnerForm *EyeBurnerForm;
//---------------------------------------------------------------------------
__fastcall TEyeBurnerForm::TEyeBurnerForm(TComponent* Owner)
        : TForm(Owner), sysregpath("Software\\New\\EyeBurner")
{
        state = 0;
}
//---------------------------------------------------------------------------

void __fastcall TEyeBurnerForm::FormCreate(TObject *Sender)
{
        // counted in seconds
        time_del = 5*60;
        time_sep = 60*60;
        time_brk = 5*60;
        del_cnt = 3;

        sysreg.reset(new TRegistry());
        ConfLoad();

        if( sysreg->OpenKey("Software\\Microsoft\\Windows\\CurrentVersion\\Run", false ) ) {
                if( sysreg->ValueExists("EyeBurner") )
                        AutoChkBox->Checked=true;
                sysreg->CloseKey();
        }

        Application->Restore();
        ShowWindow(Application->Handle, SW_RESTORE);
        SetForegroundWindow(Application->Handle);

        Application->Minimize();
        ShowWindow(Application->Handle, SW_HIDE);

        e_time_sep_max->Caption = (AnsiString)"/"+e_time_sep->Max;
        e_time_sep_min->Caption = e_time_sep->Min;
        e_time_del_max->Caption = (AnsiString)"/"+e_time_del->Max;
        e_time_del_min->Caption = e_time_del->Min;
        e_time_brk_max->Caption = (AnsiString)"/"+e_time_brk->Max;
        e_time_brk_min->Caption = e_time_brk->Min;
        e_del_cnt_max->Caption = (AnsiString)"/"+e_del_cnt->Max;
        e_del_cnt_min->Caption = e_del_cnt->Min;

        e_del_cntChange(this);
        e_time_sepChange(this);
        e_time_brkChange(this);
        e_time_delChange(this);

        del_cnt_cur=del_cnt;
}
//---------------------------------------------------------------------------

void __fastcall TEyeBurnerForm::LoopTimerTimer(TObject *Sender)
{
        static time_t now, lastrun = time(NULL);
        // state: 0 normal, 1 - waiting on answer, 2 - blocked, 3 - delay

        now = time(NULL);

LoopTimerTimer_again:
        switch(state) {
        case 0: // normal
                if( now-lastrun >= time_sep ) {
                        SeparatorForm->DelBut->Enabled = del_cnt_cur > 0 ? true : false;
                        SeparatorForm->Show();
                        SetWindowPos(SeparatorForm->Handle, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );

                        ShowWindow(SeparatorForm->Handle, SW_RESTORE);
                        SetForegroundWindow(SeparatorForm->Handle);

                        lastrun = time(NULL);
                        state = 1;
                }
                break;
        case 1: // waiting on answer
                if( now-lastrun >= 60 || ! SeparatorForm->Visible ) {        // we give 60 seconds for choosing
                        lastrun = time(NULL);
                        if(SeparatorForm->Visible) SeparatorForm->Close();

                        blk();
                        goto LoopTimerTimer_again;
                }
                break;
        case 2: // blocked
                if( now-lastrun >= time_brk ) {
                        lastrun = time(NULL);
                        state = 0;      // unlock
                        del_cnt_cur=del_cnt;
                        BlockForm->Close();
                        delete BlockForm;
                } else {
                        BlockForm->Show();
                        SetWindowPos(BlockForm->Handle, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
                 }
                break;
        case 3: // delayed
                if( now-lastrun >= time_del ) {
                        del_cnt_cur--;
                        state = 0;
                        goto LoopTimerTimer_again;
                }
                break;
        }
}
//---------------------------------------------------------------------------

void __fastcall TEyeBurnerForm::EndTrayPopMenuClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TEyeBurnerForm::OpenTrayPopMenuClick(TObject *Sender)
{
        Show();
        Application->Restore();
        Application->BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TEyeBurnerForm::AutoChkBoxClick(TObject *Sender)
{
        try {
                if( ! sysreg->OpenKey("Software\\Microsoft\\Windows\\CurrentVersion\\Run", true) ) {
                        goto AutoChkBoxClick_failed;
                }
                if(AutoChkBox->Checked) {
                        sysreg->WriteString("EyeBurner", Application->ExeName.c_str());
                } else {
                        if(!sysreg->ValueExists("EyeBurner"))
                                goto AutoChkBoxClick_close;
                        if(!sysreg->DeleteValue("EyeBurner"))
                                goto AutoChkBoxClick_failed;
                }
                goto AutoChkBoxClick_close;
        } catch(...) {}
AutoChkBoxClick_failed:
        AutoChkBox->Checked = ! AutoChkBox->Checked;
AutoChkBoxClick_close:
        try {
                sysreg->CloseKey();
        } catch(...) {
        }
}
//---------------------------------------------------------------------------
void TEyeBurnerForm::ConfLoad()
{
        if( sysreg->OpenKey(sysregpath, false) ) {
                try {
                        time_del = sysreg->ReadInteger("time_del");
                } catch(...) {}
                try {
                        time_sep = sysreg->ReadInteger("time_sep");
                } catch(...) {}
                try {
                        time_brk = sysreg->ReadInteger("time_brk");
                } catch(...) {}
                try {
                        del_cnt = sysreg->ReadInteger("del_cnt");
                } catch(...) {}
                sysreg->CloseKey();
        } else if(!Visible){
                FirstTimeForm = new TFirstTimeForm(NULL);
                FirstTimeForm->ShowModal();
                delete FirstTimeForm;
        }
        e_time_del->Position = time_del/60;
        e_time_sep->Position = time_sep/60;
        e_time_brk->Position = time_brk/60;
        e_del_cnt->Position = del_cnt;
}
void __fastcall TEyeBurnerForm::TrayIconRestore(TObject *Sender)
{
        ConfLoad();
}
//---------------------------------------------------------------------------
void TEyeBurnerForm::ConfSave()
{
        if( ! sysreg->OpenKey(sysregpath, true) ) {
                ConfLoad();
                return;
        }

        int n_time_del, n_time_sep, n_time_brk, n_del_cnt;

        n_time_sep = e_time_sep->Position*60;
        try {
                sysreg->WriteInteger("time_sep", n_time_sep);
        } catch(...) {
                Application->MessageBoxA(
                        "Nie mog� zapisa� konfiguracji",
                        "B��d rejestru", MB_OK | MB_ICONERROR );
                goto ConfSave_close;
        }
        time_sep = n_time_sep;

        n_time_brk = e_time_brk->Position*60;
        try {
                sysreg->WriteInteger("time_brk", n_time_brk);
        } catch(...) {
                Application->MessageBoxA(
                        "Nie mog� zapisa� konfiguracji",
                        "B��d rejestru", MB_OK | MB_ICONERROR );
                goto ConfSave_close;
        }
        time_brk = n_time_brk;

        n_time_del = e_time_del->Position*60;
        try {
                sysreg->WriteInteger("time_del", n_time_del);
        } catch(...) {
                Application->MessageBoxA(
                                          ff060102424f3f000306706600      ����lh ȉ �uo¨���nk  ������    ȉ         ��������   Њ   ����                     1       ����vk    ��       Change1 �����= =  ������ ����lh H� 1   ����nk  ������     �        �� ����    ����  ����8                     MYST    ����nk  ������    �        �� ����    ����  ����                      ff060102423bab000407102e0600e c ����lh @� 8������nk  ������    @�         ��������   ��   ����                     1 X E 3 ����vk    8�       Add1�o������@��# �؋�� �       ����H� ����vk    ��       Change1 ����PIU��  �Y�� U��  ��a���       ����� `�     ����lh �� 1   ����nk  ������     �        8� ����    ����  ����4                     OUTPOST ����lh `� @�{����vk    P�     >cChange1 �����DU��  �Y�� U��  �gV���       ����Ȥ ����nk  ������    Ȍ        h� ����    ����  ����                      ff06010242410f000306801500      ����nk  ������    ��         ��������   X�   ����                     1       ����	
�s��
�� �R>c����nk  �x����     �        (� ����    ����  ����8                     PALED40 ����nk  ������    `�        ȏ ����    ����  ����                      ff060102420032000407401b0100    ����lh �� 3#�>����nk  ������    ��         ��������   ��   ����                     1       ����vk    ��       Change1 �����!�     ������ ����lh 8� 1   ����h� ������ ����@� ����lh � 1   hbin �                        ����nk  �x����     �        ȟ ����    ����  ����8               
      SETUP16 ����nk  �x����     �        h� ����    ����  ����                      ff0601024cd875000407a0db0100    ����nk  �x����    x�         ��������   ��   ����           #          2       ����#B�ȋЋ^*���*�
 �
�^*䐐�� ����lh � 2   ����nk  �x����     �         � ����    ����  ����8                     USA     p���lh `� "s ؂ ���~�� ���~`� �4���� ����p� �R>c� �k= Ȍ ��\�`� Z>�� � ��|	 � $���x� �� �� �  � Ϡ�v�� ���                ����nk  �x����    x�        �� ����    ����  ����                      ff06010242059b00040710780600    ����nk  �x����    `�         ��������   x�   ����           %          1       ����vk %   H�       Change2 ����%�                �# �؋�� �       ����0� (�     ����lh Т 1   ����nk  �x����     �        `� ����    ����  ����8                     VB      ����nk  �x����    ��         � ����    ����  ����                      ff060102ec353f00040780c81300    ����lh � D��Ϩ���nk  �x����    �         ��������   x�   ����                     12      ����vk    �       Change1 �����>�14�>�1	   ����lh p� G  ����nk  �x����     �        إ ����    ����  ����<                     VB40016 