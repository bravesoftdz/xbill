//---------------------------------------------------------------------------

#ifndef processthreadH
#define processthreadH
#include <iostream>
#include<fstream>
using namespace std;
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "test.h"
#include "bzparameter.h"
//---------------------------------------------------------------------------


class BZCompressProc : public TThread
{
private:
    AnsiString infile, outfile;
    bool ioset;
protected:
    void __fastcall Execute();
    void __fastcall UpdateProgress();
public:
    __fastcall BZCompressProc(bool CreateSuspended, AnsiString src, AnsiString dest);
//    void __fastcall SetIOFile(AnsiString &src, AnsiString &dest);
};
//---------------------------------------------------------------------------

class BZDecompressProc : public TThread
{
private:
    AnsiString infile, outfile;
    bool ioset;
    int currstep;
protected:
    void __fastcall Execute();
    void __fastcall UpdateProgress();
public:
    __fastcall BZDecompressProc(bool CreateSuspended, AnsiString src, AnsiString dest);
//    void __fastcall SetIOFile(AnsiString &src, AnsiString &dest);
};
#endif
