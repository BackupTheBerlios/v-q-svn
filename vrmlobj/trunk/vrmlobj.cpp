//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

USERES("vrmlobj.res");
USEFORM("oknogl.cpp", Win);
USEUNIT("dumpable.cpp");
USEUNIT("property.cpp");
USEUNIT("vobj.cpp");
USEFORM("objs.cpp", AddObj);
USEFORM("wproptext.cpp", WinPropText);
USEFORM("wpropbool.cpp", WinPropBool);
USEFORM("wprop3.cpp", WinProp3);
USEFORM("wprop2.cpp", WinProp2);
USEFORM("wprop4.cpp", WinProp4);
USEFORM("wproprot.cpp", WinPropRot);
USEFORM("wcode.cpp", WinCode);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TWin), &Win);
                 Application->CreateForm(__classid(TWinPropText), &WinPropText);
                 Application->CreateForm(__classid(TWinPropBool), &WinPropBool);
                 Application->CreateForm(__classid(TWinProp3), &WinProp3);
                 Application->CreateForm(__classid(TWinProp2), &WinProp2);
                 Application->CreateForm(__classid(TWinProp4), &WinProp4);
                 Application->CreateForm(__classid(TWinPropRot), &WinPropRot);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
