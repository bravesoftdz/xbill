//---------------------------------------------------------------------------

#ifndef fileprotectH
#define fileprotectH
//---------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <DB.hpp>
#include <DBClient.hpp>
#include <DBLocal.hpp>
#include <DBLocalB.hpp>
#include <DBTables.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include "jumble.h"
#include "bzparameter.h"

//---------------------------------------------------------------------------
class TFileProtectDo : public TThread
{
private:
protected:
    void __fastcall Execute();
    AnsiString infile, codedfilename, indexfilename, configfilename;
    AnsiString dbDetailTableName, dbSummaryTableName;
    AnsiString dest, rawfilename;
    TTable *DetailTable, *SummaryTable;     
    bool ioset;
    void __fastcall UpdateProgress();
    void __fastcall UpdateStatus ();
    int currstep, start_page, stop_page;
    TProgressBar *ProgressBar;
    TStatusPanel *StatusPanel;
    AnsiString status;
    CStringProtect csp;
    bool *flag;
public:
    __fastcall TFileProtectDo(bool CreateSuspended, AnsiString src, AnsiString dest,CStringProtect &csp, TProgressBar *pb, TStatusPanel *bt, bool *flag, TTable *SummaryTable);
};
//---------------------------------------------------------------------------

class TFileProtectRecover : public TThread
{
private:
protected:
    void __fastcall Execute();
    AnsiString infile, outfile;
    bool ioset;
    void __fastcall UpdateProgress();
    int currstep;
    TProgressBar *ProgressBar;
    TButton *Button;
public:
    __fastcall TFileProtectRecover(bool CreateSuspended, AnsiString src, AnsiString dest, TProgressBar *pb, TButton *bt);
};
//---------------------------------------------------------------------------

struct fileprotect {
    int in, bzout, encout, ip, op, lp;
};


struct fileindex {
    int page_start, page_stop, target;

};




#endif
