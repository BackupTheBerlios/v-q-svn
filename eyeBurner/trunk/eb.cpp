//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("ebform.cpp", EyeBurnerForm);
USEFORM("sepform.cpp", SeparatorForm);
USEFORM("blkform.cpp", BlockForm);
USEFORM("ftform.cpp", FirstTimeForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "EyeBurner";
                 Application->CreateForm(__classid(TEyeBurnerForm), &EyeBurnerForm);
                 Application->CreateForm(__classid(TSeparatorForm), &SeparatorForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        catch (...)
        {
                 try
                 {
                         throw Exception("");
                 }
                 catch (Exception &exception)
                 {
                         Application->ShowException(&exception);
                 }
        }
        return 0;
}
//---------------------------------------------------------------------------
