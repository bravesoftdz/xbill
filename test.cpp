//---------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#include <vcl.h>

#pragma hdrstop
  #include <tools/all.h>
#include <tools/utilitis.h>
#include <bzlib.h>
#include <bzlib_private.h>
#include "test.h"
#include "bitchar.h"
#include "jumble.h"
#include "processthread.h"
#include "fileprotect.h"
#include "viewer.h"
#include "encoder.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"



TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------




void __fastcall TForm1::Button3Click(TObject *Sender)
{
    if(OpenDialog1->Execute() && SaveDialog1->Execute()){
        BZCompressProc *bzcompress = new BZCompressProc(false, OpenDialog1->FileName, SaveDialog1->FileName);
//        bzcompress->SetIOFile(OpenDialog1->FileName, SaveDialog1->FileName);
//        bzcompress->Execute();
    }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
//    ProgressBar2->Brush->Color = clTeal;
  //  SendMessage(ProgressBar1->Handle, PBM_SETBARCOLOR, 0, clRed);
    if(OpenDialog1->Execute() && SaveDialog1->Execute()){
        BZDecompressProc *bzdecompress = new BZDecompressProc(false, OpenDialog1->FileName, SaveDialog1->FileName);
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button5Click(TObject *Sender)
{
    int *dest;
    char *dest2;
    int srclen = Memo1->Text.Length(), destlen, destlen2;
    destlen = srclen*sizeof(char)*8;
    destlen2=srclen;
    dest = new int[destlen];
    dest2 = new char[destlen2];

    StringToBitVector(dest, &destlen, Memo1->Text.c_str(), srclen);
    WVector tmp(dest, destlen);
    ofstream is("yoyo.txt");
    is << tmp;
    is.close();
    BitVectorToString(dest2, &destlen2, dest, destlen);
    Memo2->Text = dest2;
    Memo2->Text = Memo2->Text.SubString(0, destlen2);
    delete[] dest;
    delete[] dest2;




}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender)
{
    Button6->Enabled = false;
    Form1->Cursor = crHourGlass;
    Memo3->Font->Color = clRed;
    char *jstr, *estr;
    int jsize, esize, ip,op, lp;
    jumble.Apply(&jstr, &jsize, Memo2->Text.c_str(), Memo2->Text.Length(), &ip, &op, &lp);

    int nberr = TrackBar1->Position, target;
    Label2->Caption = nberr;

    if(RadioButton2->Checked) { // sequential
        target = rand()%jsize;
        for(int i=0;i<nberr;i++) {
            jstr[(target+i)%jsize]=rand()%256;
        }
    } else {
        for(int i=0;i<nberr;i++) {
            target = rand()%jsize;
            jstr[target]=rand()%256;
        }
    }
    jumble.Extract(&estr, &esize, jstr, jsize, ip, op, lp);
    Memo3->Text = estr;
    Memo3->Text = Memo3->Text.SubString(0, esize);
    if(Memo2->Text == Memo3->Text)
//        Application->MessageBoxA("String identical!", "OK", MB_OK);
            Memo3->Font->Color = clGreen;
    else
//        Application->MessageBoxA("String not recovered!", "oups", MB_OK);
            Memo3->Font->Color = clRed;


    delete[] jstr;
    delete[] estr;
/*
    stringstream ss;
    ss << jumble;
    ss.seekg(0, ios::end);
    unsigned int len = ss.tellg();
    unsigned int len2 = int(len*101.0/100.0)+700;
    ss.seekg(0,ios::beg);

    char *buff = new char[len], *dest = new char[len2];
    ss.read(buff, len);
    BZ2_bzBuffToBuffCompress(dest, &len2, buff, len, 9,0,30);
    ofstream os("jum.cfg.bz2",ios::binary);
    os.write(dest, len2);
    os.close();
    os.open("jum2.cfg");
    os << jumble;
    os.close();

    ifstream is("jum.cfg.bz2", ios::binary);
    delete[] dest;
    delete[] buff;

    is.seekg(0, ios::end);
    len = is.tellg();
    is.seekg(0,ios::beg);
    buff = new char[len];
    len2=20*len;
    dest= new char[len2];
    is.read(buff, len);
    is.close();
    BZ2_bzBuffToBuffDecompress(dest, &len2, buff, len,0,0);
    os.open("jum3.cfg");
    os.write(dest, len2);
    os.close();
    delete[] dest;
    delete[] buff;
*/
    Button6->Enabled = true;
    Form1->Cursor = crDefault;

}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button7Click(TObject *Sender)
{

    if(OpenDialog1->Execute() && SaveDialog1->Execute()){
        Button7->Enabled = false;
//        TFileProtectDo *fpdo = new TFileProtectDo(false, OpenDialog1->FileName, SaveDialog1->FileName, Form1->ProgressBar3, Form1->Button7);
        Button7->Enabled=true;
//        bzcompress->SetIOFile(OpenDialog1->FileName, SaveDialog1->FileName);
//        bzcompress->Execute();
    }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{
    Label1->Caption = TrackBar1->Position;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    Cursor = crAppStart;
    Label1->Caption = TrackBar1->Position;
    if(!LoadConfig(jumble, "jumble.cfg")) {
        jumble = CStringProtect(8192,8192*4,0,480,960) ;
        if(!SaveConfig(jumble, "jumble.cfg")) {
                Application->MessageBoxA("Problem with configuration file", " Unrecoverable error", MB_OK);
                Cursor = crDefault;
                Application->Terminate();
        }
    }
    Cursor = crDefault;


}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Enter(TObject *Sender)
{
    double per = TrackBar1->Position/(double)TrackBar1->Max;
    TrackBar1->Max = Memo2->Text.Length();
    TrackBar1->Position = (int)(per*TrackBar1->Max);
    TrackBar1->Frequency = (TrackBar1->Max-TrackBar1->Min)/10;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button9Click(TObject *Sender)
{
        SaveConfig(jumble, "jumble.cfg");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button8Click(TObject *Sender)
{
    if(OpenDialog1->Execute() && SaveDialog1->Execute()){
        TFileProtectRecover *fpdo = new TFileProtectRecover(false, OpenDialog1->FileName, SaveDialog1->FileName, ProgressBar4, Button8);
//        bzcompress->SetIOFile(OpenDialog1->FileName, SaveDialog1->FileName);
//        bzcompress->Execute();
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button10Click(TObject *Sender)
{
    Form3->Show();    
}
//---------------------------------------------------------------------------

