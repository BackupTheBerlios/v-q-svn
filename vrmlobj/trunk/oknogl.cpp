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
        if( ! forg ) throw Exception("Wewn�trzny b��d");
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

        tmp = new VTrans( NULL, "Prostopad�o�cian" );
        tmp->addchd( new VShapeBox(tmp) );
        objs.push_back( tmp );

        tmp = new VTrans( NULL, "Cylinder" );
        tmp->addchd( new VShapeCyl(tmp) );
        objs.push_back( tmp );

        tmp = new VTrans( NULL, "Sto�ek" );
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
                switch ( Application->MessageBox( "Aktualnie edytowany plik nie zosta� zapisany, czy zapisa� go teraz?", "Aktualny plik", MB_ICONQUESTION | MB_YESNOCANCEL ) ) {
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
                        Application->MessageBox( (char *) (string("Nie mo�na otworzy� pliku: ")
                                + OpenDial->FileName.c_str()
                                + " : " + strerror(errno)).c_str(), "B��d", MB_OK | MB_ICONERROR );
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
                switch ( Application->MessageBox( "Aktualnie edytowany plik nie zosta� zapisany, czy zapisa� go teraz?", "Aktualny plik", MB_ICONQUESTION | MB_YESNOCANCEL ) ) {
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
                if( ! forg ) throw Exception("Wewn�trzny b��d.");
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
                string s( "Nie mog� zapisa� pliku : " );
                s += SaveDial->FileName.c_str();
                s += " : ";
                s += strerror(errno);
                Application->MessageBox( (char *) s.c_str(), "B��d zapisu",
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
                if( ! fout ) throw Exception( ((String)"Nie mog� otworzy� pliku: " + fname + " : " + strerror(errno)).c_str() );
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
                        string s( "Nie mog� otworzy� pliku : " );
                        s += SaveDial->FileName.c_str();
                        s += " : ";
                        s += strerror(errno);
                        Application->MessageBox( (char *) s.c_str(),
                                "B��d", MB_OK | MB_ICONERROR );
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
                fchgd = true;gNx��Z��M�O9;8FN}5���	�z\���ս��ƎGq���%��E��
i?Vɤh�r9�QO�Y�\�rnv�m��i��R���w�#{o3	�	��N�HF�Z�/��d�Oh�C���xq�t�jC��p��9�({erk	�%sxhA��єx�1�=Î&Ec'"��16n���A��u�b;�h�<`P�<<��P��O :x�G�OBw�p�E�1�i��Ӈgzu���3	�'z_LC�}!��3W?i�D�ɼ}>���zEHP?�H��(d'F�\�p�:���c�1�w�^�n��ь�:98K�ㆣ*���VB�&4?�2�6�s���6�a����}�c�01wb�  ���d
dHN��G�1�ٜ%� �i=L$���
f��0����h�0�*��a��
�P�,Db~KM�6�P1���e��
uMP�.҉�JR �q�$9N���$uUwv�;h�s_l,�S}�uL��V�����!��z�xp-�h�=��P�AX��.$p    ׈��It�����fI��\e}�M��������-�p rH��6�s�����Ԇ��ۃ���gH=J9y!(��=u�e�r����s�e���S ��%b�lq���1�/�y���ù!��\�=η,�רnq���YL̔�R{%�E�?ջʥ�����ã1U2y�ɉ@���> �cP�S���������9@�Ń~��:K���������5  m�f��$�Y;��8������d�!�������z��%�H�Q�Y��<9�i�xcǤ%J�<���7.��7rQ.��Y�#V�H�b��4����01wbP  ���d/�$:`N;2�/♜%�"Mc<�$w�����@��kF�k+��7{��OڔW>Q[{���Z;�n�]7�����:�2�  �:=2�?��ZuR�E'���6���q,Kg����I��Q��͂@7�E"�8416�������.�$ܢ�V��Ԧ5�034~����x��u����ꌍ&���O�`�mno�wcp*m�!4���J����iY۔��L.�9ڜl�l�X�!��g�0m�$z:�-ɉ���+i��ŀ�i*�@�;���<�V�����h���G@�G�Ov��\�T�=�#췓o��* ��"�&�q[�>n�*z0��#�l
00dcW    �Q�Q��Ȝ��V�J�ٴ�.h%z<��$���1�df๬�L-*\�L���V�({���RrD_w���P��дoq�M��B2��X�F|���4B��&!l�Ǝ��Jvt���f�ID$�P�v�x�]/R�G�U�$�|�lۗ�$��lc+_X�YZ��\�6>X{��b�#̩�p�Dэi�Ɉt��!H{��̣�!`��A�Bs���{�Ӓ��Fz�՛��:��##�&�<�-�ʸ�������P�G�6Jzܞ�CF��}$$��'\�x�NC�!�T�xH$$O��8˩����`�8��zp��|���xfu'��Y�p�(�c��#��=[ �pjm\��	�q%��{C��AK�ږC	��LMA�lfS��4�gX	�U2cp�>�ƺc([;��z/_(3�Y��(o+p���B�ƚ!WXr�\��@�ti���=<�(����+g��� I���tԢw��PH��#l%���4i&�8*O��k�����I^�.�!�cz2����)��='�)��Ny& �Rt1�G�p�2$,���_�4$�������3#5�ƦN�r��:�	�&&�Z��*B�	ϛ�6�r�5��v���y=����c�4-n��Gᰨ^�hI�cB�CW�Ab,L���=��rw���Dg��O�D�),!��J����:Jŉ�]rq�F�:q.��^���sis�H�0D��h��\L!�p�Tzt�Rt���`�ˌ�[��gW�ȅb���ƙЊC�7���82�gޞ3v�s��lg����y��[�K��[:� ���(��x���Y�Ҋ��֏HL#�˃.4w������:�rQ�N��?\�'�h�r}m�v��Μ���Ռ���jl�Jb���a��,�y�J�kJ8����xJ��a�>d,=#DY����Et.O�oj ���i�ح���",6����<����d�p�{�e���fW'Ħ0�R��V���yV������������v�p�H��pBλ�4!c����B\�2#�[ϴ�*���#ͪ`��Mh�p��h3,�9�zrf��7'��6�v�T<����J}l����t��&7�-��_8e&29N�+���'������VeM9�"OP	�H�����4�-��@�������ё�xƣ<Eֺ+Oek��N�]�����P���́G�gɩ�M.5�66(S�b����N7��s#h�?��i��)*L8%_�����x{��hp��X ��
�0!����ꮁ�N�Ǒ���ڭR_%�
��K��#U�'�I[}�Ҍ䷬�xB�%��-Z�ݵ�0ٮg"�����z�=����2���bmj7VcF�P�Z��=T�{�G~W��U5b1�D���P�?�.�Y^j��C�y�e��,L�!t��Y%ި��V,\�|��7�0�.	 ��-���������~��&ۻ$����aN���w���]g�d��"7�܈؏�������,o���r������r�)F#�D"*F��@�p�Ϧ�5�p�=��˛Qg�#-�j�(�������:n
�(���0@��D,�zM�eu��@�a�#e�E��_� S]��8!O=|��@0�!�h����W��M���Hf	C�$J�*�bTg�
 u}�헙�y��W���,��43�j��l��r�j}�>����_a�"�i���V��SVMN٥f��q��e��