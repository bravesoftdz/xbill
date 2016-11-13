//---------------------------------------------------------------------------

#include <iostream>
#include<fstream>
using namespace std;
#include <vcl.h>
#pragma hdrstop

#include "processthread.h"
#include "bzlib.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall BZCompressProc::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall BZCompressProc::BZCompressProc(bool CreateSuspended, AnsiString src, AnsiString dest)
        : TThread(CreateSuspended)
{
    Priority = tpNormal;
    infile = src;
    outfile = dest;
    ioset = true;
    int len;
    ifstream is(src.c_str());
    is.seekg (0, ios::end);
    len = is.tellg();
    is.close();
    Form1->ProgressBar1->Position=0;
    Form1->ProgressBar1->Min=0;
    Form1->ProgressBar1->Max = len;
    Form1->ProgressBar1->Step=IN_BUFFER_SIZE;
    this->FreeOnTerminate=true;

}

//---------------------------------------------------------------------------
void __fastcall BZCompressProc::Execute()
{
    //---- Place thread code here ----
    bzsize bs;
    const int sobzsize = sizeof(bzsize);
    char buff_in[IN_BUFFER_SIZE], buff_out[OUT_BUFFER_SIZE],  sb[sobzsize];

    ifstream is(infile.c_str(), ios::binary);
            // get length of file:
    is.seekg (0, ios::end);
    int length = is.tellg(),rd=0, ret, insize;
    unsigned int outsize;
    is.seekg (0, ios::beg);

    ofstream os(outfile.c_str(), ios::binary);

    while(!is.eof()){
        is.read(buff_in, IN_BUFFER_SIZE);
        rd+=IN_BUFFER_SIZE;
//        insize = rd <length ? IN_BUFFER_SIZE : length-(length/IN_BUFFER_SIZE)*IN_BUFFER_SIZE;
        insize = is.gcount();
        outsize=OUT_BUFFER_SIZE;
        ret = BZ2_bzBuffToBuffCompress(buff_out, &outsize, buff_in, insize, 1, 0, 30);
        switch (ret) {
            case BZ_OK : bs.in = insize; bs.out = outsize;
                         memcpy(sb, &bs, sobzsize);
                         os.write(sb,sobzsize);
                         os.write(buff_out, outsize);
                         Synchronize(UpdateProgress);
                         break;
            case BZ_OUTBUFF_FULL : break;
            case BZ_PARAM_ERROR : Application->MessageBoxA("Parameter bermasalah", "Salah tu bang aji", MB_OK);
                                  is.close();
                                  os.close();
                                  return;
            case BZ_MEM_ERROR : Application->MessageBoxA("Memori tak cukup lah pulak", "Alamak !", MB_OK);
                                is.close();
                                os.close();
                                return;;
        }
        is.peek();
    }

    is.close();
    os.close();
    ioset = false;

}
//---------------------------------------------------------------------------
/*void __fastcall SetIOFile(AnsiString &src, AnsiString &dest){

    infile = src;
    outfile = dest;
    ioset = true;
    int len;
    ifstream is(src.c_str());
    is.seekg (0, ios::end);
    len = is.tellg();
    is.close();
    Form1->ProgressBar1->Position=0;
    Form1->ProgressBar1->Min=0;
    Form1->ProgressBar1->Max = len;
}
*/
//---------------------------------------------------------------------------
void __fastcall BZCompressProc::UpdateProgress() {
    if(Form1->ProgressBar1->Max - Form1->ProgressBar1->Position > Form1->ProgressBar1->Step) {
        Form1->ProgressBar1->StepIt();
    } else {
        Form1->ProgressBar1->StepBy(Form1->ProgressBar1->Max - Form1->ProgressBar1->Position);
    }
}



__fastcall BZDecompressProc::BZDecompressProc(bool CreateSuspended, AnsiString src, AnsiString dest)
        : TThread(CreateSuspended)
{
    Priority = tpNormal;
    infile = src;
    outfile = dest;
    ioset = true;
    int len;
    ifstream is(src.c_str());
    is.seekg (0, ios::end);
    len = is.tellg();
    is.close();
    Form1->ProgressBar2->Position=0;
    Form1->ProgressBar2->Min=0;
    Form1->ProgressBar2->Max = len;
    Form1->ProgressBar2->Step=IN_BUFFER_SIZE;

}

void __fastcall BZDecompressProc::UpdateProgress() {
    Form1->ProgressBar2->StepBy(currstep);
}


void __fastcall BZDecompressProc::Execute()
{
    //---- Place thread code here ----
    bzsize bs;
    const int sobzsize = sizeof(bzsize);
    char buff_in[IN_BUFFER_SIZE], buff_out[OUT_BUFFER_SIZE],  sb[sobzsize];

    ifstream is(infile.c_str(), ios::binary);
            // get length of file:
    is.seekg (0, ios::end);
    int length = is.tellg(),rd=0, ret;
    unsigned int insize, outsize;
    is.seekg (0, ios::beg);

    ofstream os(outfile.c_str(), ios::binary);

    while(!is.eof()){
        is.read(sb, sobzsize);
        if(is.gcount()!=sobzsize){
            Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
        }
        memcpy(&bs, sb, sobzsize);
        is.read(buff_out, bs.out);
        outsize = is.gcount();
        if(bs.out!=outsize) {
            Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
        }
        insize = bs.in;
        ret = BZ2_bzBuffToBuffDecompress(buff_in, &insize, buff_out, outsize, 0,0);
        switch (ret) {
            case BZ_OK : os.write(buff_in, insize);
                        currstep = bs.out;
                         Synchronize(UpdateProgress);
                         break;
            case BZ_OUTBUFF_FULL : Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
                                  is.close();
                                  os.close();
                                   return;
            case BZ_UNEXPECTED_EOF : Application->MessageBoxA("Fail tak cukup sifat", "Alamak !", MB_OK);
                                  is.close();
                                  os.close();
                                   return;
            case BZ_DATA_ERROR_MAGIC : Application->MessageBoxA("Format data tak betul", "Alamak !", MB_OK);
                                  is.close();
                                  os.close();
                                   return;
            case BZ_DATA_ERROR : Application->MessageBoxA("Data corrupted", "Aieee !", MB_OK);
                                  is.close();
                                  os.close();
                                   return;

            case BZ_PARAM_ERROR : Application->MessageBoxA("Parameter bermasalah", "Salah tu bang aji", MB_OK);
                                  is.close();
                                  os.close();
                                  return;
            case BZ_MEM_ERROR : Application->MessageBoxA("Memori tak cukup lah pulak", "Alamak !", MB_OK);
                                is.close();
                                os.close();
                                return;;
        }
        is.peek();
    }

    is.close();
    os.close();
    ioset = false;

}