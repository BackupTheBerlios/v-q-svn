//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "property.h"
#include "wproptext.h"
#include "wpropbool.h"
#include "wprop3.h"
#include "wprop4.h"
#include "wprop2.h"
#include "wproprot.h"
#include "oknogl.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
void Property::set( bool & chg )
{
        WinPropText->Val->Text = val.c_str();
        if( WinPropText->ShowModal() == mrOk ) {
                string tmp1;
                if( chk( WinPropText->Val->Text, tmp1 ) ) {
                        tmp1 =WinPropText->Val->Text.c_str();
                        if( tmp1 != val ) {
                                chg = true;
                                val = tmp1;
                        }
                } else if( ! tmp1.empty() )
                        Application->MessageBox( (char *)tmp1.c_str(), "Z³a wartoœæ", MB_OK | MB_ICONERROR);
        }
}
//---------------------------------------------------------------------------
void PropFloat::set( bool & chg )
{
        WinPropText->Val->Text = get().c_str();
        if( WinPropText->ShowModal() == mrOk ) {
                string tmp1;
                if( chk( WinPropText->Val->Text, tmp1 ) ) {
                        tmp1 = replaceAll(WinPropText->Val->Text.c_str(), decsep, ".");
                        if( tmp1 != val ) {
                                chg = true;
                                val = tmp1;
                        }
                } else if( ! tmp1.empty() )
                        Application->MessageBox( (char *)tmp1.c_str(), "Z³a wartoœæ", MB_OK | MB_ICONERROR);
        }
}

//---------------------------------------------------------------------------
string PropFloat::replaceAll(string s, string f, string r) {
        unsigned int found = s.find(f);
        while(found != string::npos) {
                s.replace(found, f.length(), r);
                found = s.find(f);
        }
        return s;
}
//---------------------------------------------------------------------------
string PropFloat::get()
{
        return replaceAll(val, ".", decsep );
}

//---------------------------------------------------------------------------
bool PropFloat::chk( const AnsiString & v, string & a )
{
        try {
                (void) v.ToDouble();
        } catch(...) {
                a = "Nieprawid³owy format liczby zmiennoprzecinkowej.";
                return false;
        }
        return true;
}
//---------------------------------------------------------------------------
void PropBool::set( bool & chgd )
{
        if( val == "TRUE" )
                WinPropBool->Val->ItemIndex = 0;
        else WinPropBool->Val->ItemIndex = 1;

        if( WinPropBool->ShowModal() == mrOk ) {
                string nval = WinPropBool->Val->ItemIndex ? "FALSE" : "TRUE";
                if( val != nval ) {
                        chgd = true;
                        val = nval;
                }

        }
}
//---------------------------------------------------------------------------
void PropVect3::set( bool & chgd )
{
        string v1, v2, v3, v = get();
        string::size_type bpos, pos;
        pos = v.find(" ");
        if( pos == string::npos )
                throw Exception("Wewnêtrzny b³¹d: PropVect3::set - nieprawid³owy format val");
        v1 = v.substr(0, pos);
        bpos = pos+1;
        pos = v.find(" ", bpos);
        if( pos == string::npos )
                throw Exception("Wewnêtrzny b³¹d: PropVect3::set - nieprawid³owy format val");
        v2 = v.substr( bpos, pos - bpos );
        v3 = v.substr( pos+1 );

        WinProp3->Val1->Text = v1.c_str();
        WinProp3->Val2->Text = v2.c_str();
        WinProp3->Val3->Text = v3.c_str();

        if( WinProp3->ShowModal() == mrOk ) {
                bool ok = true;
                if( ! chk( WinProp3->Val1->Text, v1 ) ) {
                        v = "Liczba 1: " + v1 + "\n";
                        ok = false;
                }
                if( ! chk( WinProp3->Val2->Text, v1 ) ) {
                        v += "Liczba 2: " + v1 + "\n";
                        ok = false;
                }
                if( ! chk( WinProp3->Val3->Text, v1 ) ) {
                        v = "Liczba 3: " + v1  + "\n";
                        ok = false;
                }
                if( ! ok )
                        Application->MessageBox( (char *) v.c_str(), "Z³a wartoœæ", MB_OK | MB_ICONERROR);
                else {
                        v1 = (string) WinProp3->Val1->Text.c_str()
                                + " " + WinProp3->Val2->Text.c_str()
                                + " " + WinProp3->Val3->Text.c_str();
                        v1 = replaceAll( v1, decsep, ".");
                        if( v1 != val ) {
                                val = v1;
                                chgd = true;
                        }
                }
        }
}
//---------------------------------------------------------------------------
void PropVect2::set( bool & chgd )
{
        string v1, v2, v = get();
        string::size_type pos;
        pos = v.find(" ");
        if( pos == string::npos )
                throw Exception("Wewnêtrzny b³¹d: PropVect3::set - nieprawid³owy format val");
        v1 = v.substr(0, pos);
        v2 = v.substr( pos + 1 );

        WinProp2->Val1->Text = v1.c_str();
        WinProp2->Val2->Text = v2.c_str();

        if( WinProp2->ShowModal() == mrOk ) {
                bool ok = true;
                if( ! chk( WinProp2->Val1->Text, v1 ) ) {
                        v = "Liczba 1: " + v1 + "\n";
                        ok = false;
                }
                if( ! chk( WinProp2->Val2->Text, v1 ) ) {
                        v += "Liczba 2: " + v1 + "\n";
                        ok = false;
                }
                if( ! ok )
                        Application->MessageBox( (char *) v.c_str(), "Z³a wartoœæ", MB_OK | MB_ICONERROR);
                else {
                        v1 = (string) WinProp2->Val1->Text.c_str()
                                + " " + WinProp2->Val2->Text.c_str();
                        v1 = replaceAll(v1, decsep, ".");
                        if( v1 != val ) {
                                val = v1;
                                chgd = true;
                        }
                }
        }
}
//---------------------------------------------------------------------------
void PropRot::set( bool & chgd )
{
        string v1, v2, v3, v4, v = get();
        string::size_type bpos, pos;
        pos = v.find(" ");
        if( pos == string::npos )
                throw Exception("Wewnêtrzny b³¹d: PropVect3::set - nieprawid³owy format val");
        v1 = v.substr(0, pos);
        bpos = pos+1;
        pos = v.find(" ", bpos);
        if( pos == string::npos )
                throw Exception("Wewnêtrzny b³¹d: PropVect3::set - nieprawid³owy format val");
        v2 = v.substr( bpos, pos - bpos );

        bpos = pos+1;
        pos = v.find(" ", bpos);
        if( pos == string::npos )
                throw Exception("Wewnêtrzny b³¹d: PropVect3::set - nieprawid³owy format val");
        v3 = v.substr( bpos, pos - bpos );
        v4 = v.substr( pos+1 );

        WinPropRot->X->Checked = v1 == "1" ? true : false;
        WinPropRot->Y->Checked = v2 == "1" ? true : false;
        WinPropRot->Z->Checked = v3 == "1" ? true : false;
        WinPropRot->Val->Text = v4.c_str();

        if( WinPropRot->ShowModal() == mrOk ) {
                try {
                        double rot = WinPropRot->Val->Text.ToDouble();
                        v1 = (string) (WinPropRot->X->Checked ? "1" : "0" )
                                + " " + (WinPropRot->Y->Checked ? "1" : "0")
                                + " " + (WinPropRot->Z->Checked ? "1" : "0")
                                + " "
                                + ( WinPropRot->RadRad->Checked ? WinPropRot->Val->Text.c_str()
                                : AnsiString::FloatToStrF( (rot*M_PI)/180,
                                  AnsiString::sffNumber, 5, 2 ).c_str() );

                        v = replaceAll(v1, decsep, ".");
                        if( v != val ) {
                                val = v;
                                chgd = true;
                        }
                } catch(...) {
                        Application->MessageBox( (char *) v.c_str(), "Z³a wartoœæ", MB_OK | MB_ICONERROR);
                }
        }
}
//---------------------------------------------------------------------------
void PropCol::set( bool & chgd )
{
        string v = get();
        unsigned Color = 0;
        double d;
        string::size_type bpos, pos;
        pos = v.find(" ");
        if( pos == string::npos )
                throw Exception("Wewnêtrzny b³¹d: PropVect3::set - nieprawid³owy format val");
        d = ((AnsiString) (v.substr(0, pos).c_str())).ToDouble();
        Color = (unsigned)(0xff*d);
        bpos = pos+1;
        pos = v.find(" ", bpos);
        if( pos == string::npos )
                throw Exception("Wewnêtrzny b³¹d: PropVect3::set - nieprawid³owy format val");
        d = ((AnsiString) (v.substr( bpos, pos - bpos ).c_str())).ToDouble();
        Color |= (unsigned)(0xff*d) << 8;
        d = ((AnsiString) (v.substr( pos+1 ).c_str())).ToDouble();
        Color |= (unsigned)(0xff*d) << 16;
        Win->WinPropCol->Color = Color;
        Win->WinPropCol->Execute();
        Color = (unsigned) Win->WinPropCol->Color;
        string col = AnsiString::FloatToStrF( (Color & 0xff) * ((double)1/0xff),
                        AnsiString::sffNumber, 5, 2 ).c_str();
        col += " ";
        col += AnsiString::FloatToStrF( ((Color >> 8) & 0xff) * ((double)1/0xff),
                        AnsiString::sffNumber, 5, 2 ).c_str();
        col += " ";
        col += AnsiString::FloatToStrF( ((Color >> 16) & 0xff) * ((double)1/0xff),
                        AnsiString::sffNumber, 5, 2 ).c_str();
        col = replaceAll(col, decsep, ".");
        if( val != col ) {
                chgd = true;
                val = col;
        };
}
