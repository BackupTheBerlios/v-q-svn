//---------------------------------------------------------------------------
#include <vcl.h>
#include <errno.h>

#pragma hdrstop

#include "oknogl.h"
#include "dumpable.h"
#include "property.h"
#include "vobj.h"
#include "objs.h"
#include "wprop.h"
#include "wcode.h"
#include "wproptext.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TWin *Win;
//---------------------------------------------------------------------------
__fastcall TWin::TWin(TComponent* Owner)
        : TForm(Owner)
{
        fnew = true;
        forg << "#VRML V2.0 utf8" << endl;
        if( ! forg ) throw Exception("Wewnêtrzny b³¹d");
        wintit = Application->Title;
}
//---------------------------------------------------------------------------
void __fastcall TWin::MenuFileExitClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void TWin::init_objs()
{
        // tworzymy liste obslugiwanych obiektow
        VTrans *tmp = new VTrans( NULL, "Kula" );
        tmp->addchd( new VShapeSphere( tmp ) );
        objs.push_back( tmp );

        tmp = new VTrans( NULL, "Prostopad³oœcian" );
        tmp->addchd( new VShapeBox(tmp) );
        objs.push_back( tmp );

        tmp = new VTrans( NULL, "Cylinder" );
        tmp->addchd( new VShapeCyl(tmp) );
        objs.push_back( tmp );

        tmp = new VTrans( NULL, "Sto¿ek" );
        tmp->addchd( new VShapeCone(tmp) );
        objs.push_back( tmp );

        // tworzymy liste wyboru obiektow w okienku AddObj
        unsigned i, s = objs.size();
        for( i=0; i < s; i++ )
                AddObj->ObjsList->Items->Add( objs[i]->name.c_str() );
        if( ! s )
                throw 1;

}
//---------------------------------------------------------------------------
void __fastcall TWin::FormCreate(TObject *Sender)
{
        AddObj = new TAddObj(this);
        init_objs();
}
//---------------------------------------------------------------------------

void __fastcall TWin::MenuObjsNewClick(TObject *Sender)
{
        AddObj->ShowModal();
        if( AddObj->ModalResult == mrOk ) {
                fchgd = true;
                fobjs.push_back( objs[AddObj->ObjsList->ItemIndex]->newinst() );
                ObjsLst->Items->Add( AddObj->ObjsList->Items->Strings[AddObj->ObjsList->ItemIndex] );
        }
}
//---------------------------------------------------------------------------


void __fastcall TWin::ObjPropsDrawItem(TWinControl *Control, int Index,
      TRect &Rect, TOwnerDrawState State)
{
        // ...adjust the rectangle so we draw only the left "column".
        TRect R = Rect;
        R.Right = R.Left + ObjHdr->Sections->Items[0]->Width;

        ExtTextOut(ObjProps->Canvas->Handle, R.Left, R.Top,
             ETO_CLIPPED | ETO_OPAQUE, (RECT*)&R,
             ObjProps->Items->Strings[Index].c_str(),
             ObjProps->Items->Strings[Index].Length(), NULL);

        R.Left = R.Right;
        R.Right = Rect.Right;

        VObj *optr = (VObj *) ObjProps->Tag;
        const VObj *opar = optr->parent();
        if( opar != NULL ) {
                if( ! Index ) {
                        ExtTextOut( ObjProps->Canvas->Handle, R.Left, R.Top,
                                ETO_CLIPPED | ETO_OPAQUE, (RECT*)&R, " ",
                                1, NULL);
                        return ;
                } else Index--;
        }

        int optrss = optr->subs.size();
        if( Index < optrss ) {
                string t("Obiekt");
                ExtTextOut( ObjProps->Canvas->Handle, R.Left, R.Top,
                        ETO_CLIPPED | ETO_OPAQUE, (RECT*)&R, t.c_str(),
                        t.length(), NULL);
        } else {
                Index -= optrss;
                string val = optr->props[Index]->get();
                ExtTextOut( ObjProps->Canvas->Handle, R.Left, R.Top,
                        ETO_CLIPPED | ETO_OPAQUE, (RECT*)&R, val.c_str(),
                        val.length(), NULL);

        }
}
//---------------------------------------------------------------------------

void __fastcall TWin::ObjHdrResize(TObject *Sender)
{
        ObjProps->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TWin::ObjHdrSectionResize(
      THeaderControl *HeaderControl, THeaderSection *Section)
{
        ObjProps->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TWin::ObjsLstClick(TObject *Sender)
{
        // jezeli ItemIndex wskazuje na jakis
        // obiekt wyswietl informacje o nim
        if( ObjsLst->ItemIndex != -1 ) {
                ObjPropsAdd( fobjs[ObjsLst->ItemIndex] );
        }
}
//---------------------------------------------------------------------------
void TWin::fobjs_del()
{
        for( unsigned i=0, s=fobjs.size(); i < s; i++ ) {
                delete fobjs[i];
        }
        fobjs.clear();
        AddObj->ObjsList->ItemIndex=0;
        ObjProps->Items->Clear();
        ObjsLst->Items->Clear();
}

//---------------------------------------------------------------------------
void __fastcall TWin::MenuFileOpenClick(TObject *Sender)
{
        if( fchgd ) {
                switch ( Application->MessageBox( "Aktualnie edytowany plik nie zosta³ zapisany, czy zapisaæ go teraz?", "Aktualny plik", MB_ICONQUESTION | MB_YESNOCANCEL ) ) {
                        case IDYES: MenuFileSaveClick( NULL );
                                break;
                        case IDNO: break;
                        case IDCANCEL: return;
                }
        }

        if (OpenDial->Execute()){
                if( fout.is_open() )
                        fout.close();
                fout.open( OpenDial->FileName.c_str(), ios::in | ios::out );
                forg.str( "" );
                forg << fout.rdbuf();
                fobjs_del();
                if( fout ) {
                        fname = OpenDial->FileName;
                        fnew = false;
                        string s(wintit.c_str());
                        s += " : ";
                        s += OpenDial->FileName.c_str();
                        Caption = s.c_str();
                } else {
                        Application->MessageBox( (char *) (string("Nie mo¿na otworzyæ pliku: ")
                                + OpenDial->FileName.c_str()
                                + " : " + strerror(errno)).c_str(), "B³¹d", MB_OK | MB_ICONERROR );
                        Caption = wintit;
                        fnew = true;
                }
                fobjs_del();
        }
}
//---------------------------------------------------------------------------
void __fastcall TWin::MenuFileNewClick(TObject *Sender)
{
        if( fchgd ) {
                switch ( Application->MessageBox( "Aktualnie edytowany plik nie zosta³ zapisany, czy zapisaæ go teraz?", "Aktualny plik", MB_ICONQUESTION | MB_YESNOCANCEL ) ) {
                        case IDYES: MenuFileSaveClick( NULL );
                                break;
                        case IDNO: break;
                        case IDCANCEL: return;
                }
        }

        if( fnew ) {
                fobjs_del();
        } else {
                if( fout.is_open() )
                        fout.close();
                forg.str( "#VRML V2.0 utf8" );
                forg << endl;
                if( ! forg ) throw Exception("Wewnêtrzny b³¹d.");
                fobjs_del();
                fnew = true;
                Caption = wintit;
                Application->Title = Caption;
        }
}
//---------------------------------------------------------------------------
void TWin::filesave()
{
        fout << forg.str();

        for( unsigned i=0, s=fobjs.size(); i < s; i ++ ) {
                (*fobjs[i])>> fout;
        }
        fout << endl;

        if( fout.bad() || fout.fail() ) {
                string s( "Nie mogê zapisaæ pliku : " );
                s += SaveDial->FileName.c_str();
                s += " : ";
                s += strerror(errno);
                Application->MessageBox( (char *) s.c_str(), "B³¹d zapisu",
                             MB_OK | MB_ICONERROR );
                return;
        }
        fnew = false;
        fchgd = false;
}
//---------------------------------------------------------------------------
bool TWin::fileclose()
{
        if( fnew ) {
        }
        return true;
}
//---------------------------------------------------------------------------
void __fastcall TWin::MenuFileSaveClick(TObject *Sender)
{
        // nowy plik i jakies obiekty
        if( fnew ) {
                MenuFileSaveAsClick( Sender );
        } else {
                if( fout.is_open() ) {
                        fout.close();
                        fout.open( fname.c_str(), ios::trunc | ios::out );
                }
                if( ! fout ) throw Exception( ((String)"Nie mogê otworzyæ pliku: " + fname + " : " + strerror(errno)).c_str() );
                filesave();
        }
}
//---------------------------------------------------------------------------

void __fastcall TWin::MenuFileSaveAsClick(TObject *Sender)
{
        if( SaveDial->Execute() ) {
                if( fout.is_open() )
                        fout.close();
                fout.open(SaveDial->FileName.c_str(), ios::trunc | ios::out );
                if( ! fout ) {
                        fnew = true;
                        string s( "Nie mogê otworzyæ pliku : " );
                        s += SaveDial->FileName.c_str();
                        s += " : ";
                        s += strerror(errno);
                        Application->MessageBox( (char *) s.c_str(),
                                "B³¹d", MB_OK | MB_ICONERROR );
                        return;
                }
                filesave();
                string s(wintit.c_str());
                s += " : ";
                fname = SaveDial->FileName;
                s += SaveDial->FileName.c_str();
                Caption = s.c_str();
                Application->Title = Caption;
        }
}
//---------------------------------------------------------------------------
void TWin::ObjPropsAdd( const VObj * opar )
{
        ObjProps->Tag = (int) opar;
        ObjProps->Items->Clear();
        if( opar->parent() != NULL )
                ObjProps->Items->Add( ".." );
        unsigned i,s;
        for( i=0, s = opar->subs.size(); i < s ; i++  )
                ObjProps->Items->Add( opar->subs[i]->name.c_str() );

        for( i=0, s = opar->props.size(); i < s ; i++  )
                ObjProps->Items->Add( opar->props[i]->name.c_str() );

        ObjProps->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TWin::ObjPropsDblClick(TObject *Sender)
{
        int Index = ObjProps->ItemIndex;
        if( Index >= 0 ) {
                VObj * optr = (VObj*)ObjProps->Tag;
                const VObj * opar = optr->parent();
                if( opar ) {
                        if( ! Index ) {
                                ObjPropsAdd(opar);
                                return;
                        }
                        Index--;
                }
                int optrss = optr->subs.size();
                if( Index < optrss ) {
                        ObjPropsAdd(optr->subs[Index]);
                        return;
                }
                Index -= optrss;
                try {
                        optr->props[Index]->set( fchgd );
                        ObjProps->Invalidate();
                } catch(...) {
                }
        }
}
//---------------------------------------------------------------------------
void __fastcall TWin::MenuObjsDelClick(TObject *Sender)
{
        if( ObjsLst->ItemIndex != -1 ) {
                fchgd = true;gNxøZãàMûO9;8FN}5ã×ç	İz\ÛáôÕ½ëãÆGq·ô–%İ¯EÃÃ
i?VÉ¤h¡r9¬QOíYã\îrnvñmîæi£ÆR×äöw÷#{o3	¯	…¤NHF¦ZÌ/¿¯dÎOhÿCÁ£šxqtó†jC“p”¼9Ú({erk	Ø%sxhA©ÙÑ”x1Í=Ã&Ec'"¾é16n¶ÓA‘ä‡uÃb;øh™<`Pó<<›P±ÎO :xœG®OBwùp×Eñˆ1‰iÁÂÓ‡gzuéşº3	¦'z_LC‡}!ñè3W?iò’D¦É¼}>¶ÁázEHP?ÉHé(d'F¾\æp£:ô¯Âcƒ1§w§^›n†±ÑŒê:98Kğã†£*¹ÈVBá&4?Ç2î6“sœ—î6òa˜³¹}Ïc01wbà  ÿû¤d
dHNÏæG1¢Ùœ%â ©i=L$ÏÀÂ
fÁ•0„¢“èhá0Ç*”öa¥÷
¶P›,Db~KMê6çP1¦ªîe¡ê
uMPŸ.Ò‰èJR ›qÌ$9NË»“$uUwvÖ;h°s_l,ëS}ÆuLŠöVêÀèåî!úÒzùxp-´h·=´Pğ²AXª».$p    ×ˆÂàItÂ•ì³İî–fI–Ñ\e}ÖMú¾Êöÿÿÿı-»p rHüŒ6ñsÑÁ™§ªÔ†İÌÛƒ­…„gH=J9y!(ˆ¥=uÕeŒr´Á…ËsÖe•ŞİS ­«%b¶lqµ¦¹1Ï/›yˆ‘×Ã¹!ª’\«=Î·,÷×¨nqÎÅÖYLÌ”øR{%ÊE›?Õ»Ê¥·‰‹­–Ã£1U2yéÉ‰@œ‡> cPä°S³¼šÓğêöÖì 9@ğ±Åƒ~»:KßÿıŸÿÿÿÿò5  m¾fà$»Y;Üø8”«Á±¬Àd‚!ğŒ­ãĞêØzº¢%ÂHŒQÂYÛÊ<9 i¹xcÇ¤%J´<—àÌ7.Ï‰7rQ.ò¦Y»#V‰H“bÁñ4¶ô¬“01wbP  ÿû„d/„$:`N;2À/â™œ%ã"Mc<ì$w€»Šæğš@ÙåkFÜk+Çş7{¶ºOÚ”W>Q[{“¯Z;ìnš]7ÿ±»¹¨:È2€  À:=2®?•åZuR•E'ÇÈÛ6¼ıŒq,Kg³·îåIŸ¨Q§ÊÍ‚@7¤E"ù8416 ìÉßù§–.Ä$Ü¢VÖ¸Ô¦5æ034~åìÁÓx€¤u•†ïÉêŒ&’¬ÁO´`mnoÿwcp*mÍ!4‰•­JòÚïëiYÛ”¤›L.9ÚœlÃlÉXâ¿!•Šg†0m€$z:¡-É‰ŒØÏ+i–ÉÅ€Œi*î˜@Á;‹ÖÖ<áVéäş–ÖhúşìG@ÿG©Ovíõ\ÖTÃ=Î#ì·“oïı* ö"&´q[“>n²*z0ÀÂ#Àl
00dcW    ¶QƒQéøÈœ ØVŞJ¦Ù´‘.h%z<Ÿ„$ÌçÎ1Œdfà¹¬L-*\•LèÑVÆ({ˆÑãRrD_w¤ù¦P“ÉĞ´oq³M§‚B2§ÒXÌF|…¬È4BÎ¸&!láÆ­‚Jvtø²Éf˜ID$´PŸvŒx„]/RGğUÓ$â|ìlÛ—Ù$ëälc+_XéYZ¢æ\Ö6>X{¦Áb¹#Ì©p€DÑiäÉˆtğ!H{ÓÆÌ£Î!`„ˆAçBs¨¶Æ{ÕÓ’§¶FzºÕ›«œ:‹ô##é&›<Î-Ê¸¤„ô”ÑÕòPÄG¢6JzÜºCFƒŠ}$$¢Æ'\¦xêNCã!»TÂxH$$O…•8Ë©‹›Ú`ã8ÔÂzp¨Á|“áÊxfu'ÛÁYÕpğ(Òcé“#õâ=[ ³pjm\ø¡	é¨q%Š{C¡šAK“Ú–C	çÛLMAlfSş¬4ágX	¶U2cpİ>ŸÆºc([;©Âz/_(3†YÂÚ(o+p—ë‡ÎBÌÆš!WXrğ™\“˜@ti“§®=<ç(¿–™º+g„˜¼ IôÒÒtÔ¢w¥æPH‡’#l%Ùáé4i&ì8*OíókÓÚ»‰ÆI^.!Ñcz2ÔäÌù)´À='š)„œNy& Rt1Gpâ2$,á¸é´_¦4$À•Ÿ‚â©É3#5°Æ¦N•r :Õ	é&&Zäı*Bî	Ï›é6ùr‚5—„v„À˜y=Ìî‘•…c‚4-nŸGá°¨^ÖhIÔcB¤CW·Ab,L÷§Ä=œ·rw“¸ÅDg¥®O¬Då®),!ÛÊJ·£ñ’Ë:JÅ‰ù]rq‘F´:q.¹ğ^ñ¤„èğsis†Hı0Däºîhë‘æ\L!ë¡pªTztŞRtµ`åËŒ‘[°ógWÅÈ…b¤¾¹Æ™ĞŠCº7œˆ82ÃgŞ3v½sÍçlgØÉÆ±yÚ[ÌKÁ[:Ò ¥» (ã’ùx”İåYÆÒŠ’ç“ÖHL#ûËƒ.4wñ¬É¨°¹:ÒrQ‚NèÅ?\×'¢hòr}m¬v§õÎœâı§ÕŒ§õûjl‘JbÁª×a„ü,ÃyÙJökJ8ºì”ÂxJ–ùaµ>d,=#DY¸Á±µEt.O–oj À„iøØ­±ì",6šŒÂ<»ø¡»d‘pÌ{³e™şÌfW'Ä¦0R¢õVşª„yV¦‚ÄóğÜô¥¥‰æv…pşH°„pBÎ»4!c€äÆâB\Ä2#û[Ï´ˆ*ôú¥#Íª`í¡ÒMhápÿıh3,Í9’zrfàÕ7'ºÖ6êvÂT<“­«´J}lú¸¸ÛtÉé&7’-ÖÓ_8e&29N+ÓèÏ'§˜öÒğğVeM9ì"OP	¤HÉÒäÂâ4¸-Ãâ‘@¨†—‚ö‰°Ñ‘ªxÆ£<EÖº+Oek‰ÏNå]Œ«‹ °PŸüŞÌGõgÉ©M.5µ66(S½b³ĞìÊN7­¶s#hÁ?‰Ái¡¦)*L8%_„ñéõ¶x{™ÓhpŒùX ”
¶0!——Ââê®…NÖÇ‘ªª¨Ú­R_%¹
İşKÖÚ#Uè'ëI[}ØÒŒä·¬­xBò¥%êş-Z¾İµá0Ù®g"Õ“‚ÀòzÍ=êËŠº2¶–òbmj7VcF¥PZ¢ğ=TÏ{ùG~WéùU5b1“D ‡ì•P÷?Ÿ.üY^j¯ëC¹y¬e½ê,L±!t¦²Y%Ş¨öV,\ú|Õ×7³0Ş.	 †ä-ÇŞôÿÀúÃÜë~ÃÜ&Û»$õğ‹ßaN¨õñw­ıû]gğd“"7‡ÜˆØİí¥·™’Š,o­°œrğƒààºÙr¿)F#»D"*FÚ@Âp†Ï¦Ö5Äp=° Ë›QgÑ#-ğj‹(šãäèğˆ…:n
©(„ˆ‰0@‡…D,ïzMâ±eu“á@aƒ#e›E±_ S]éïº8!O=|İë@0ø!øh”¿…ŞWÿâMø÷’Hf	CÁ$JÑ*ÕbTgĞ
 u}¿í—™Ôy¤ÕWòÿ»,»İ43ûjöÕlÖÚrµj}û>ªª¼Õ_a—"iÃÉíVğSVMNÙ¥f“âq¾œe’³