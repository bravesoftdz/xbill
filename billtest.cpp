//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("test.cpp", Form1);
USEFORM("viewer.cpp", Form2);
USEFORM("encoder.cpp", Form3);
USEFORM("about.cpp", AboutBox);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "TM eBill Encoder v1.0.0";
         Application->CreateForm(__classid(TForm3), &Form3);
         Application->CreateForm(__classid(TAboutBox), &AboutBox);
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
