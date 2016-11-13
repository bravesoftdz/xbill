//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("viewer.cpp", Form2);
USEFORM("about.cpp", AboutBox);
USEFORM("billqr.cpp", Form5);
USEFORM("billqr2.cpp", Form6);
USEFORM("billqr3.cpp", Form7);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "TM eBill Viewer v1.0.0";
         Application->CreateForm(__classid(TForm2), &Form2);
         Application->CreateForm(__classid(TAboutBox), &AboutBox);
         Application->CreateForm(__classid(TForm5), &Form5);
         Application->CreateForm(__classid(TForm6), &Form6);
         Application->CreateForm(__classid(TForm7), &Form7);
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
