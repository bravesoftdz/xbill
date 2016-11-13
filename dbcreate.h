//---------------------------------------------------------------------------

#ifndef dbcreateH
#define dbcreateH


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
#include <Provider.hpp>
#include <ComCtrls.hpp>




void __fastcall ProcessLine(AnsiString line);
AnsiString __fastcall EscapeString(AnsiString str);
void CreateDatabase(AnsiString datafile, AnsiString dest, AnsiString rawfilename, TTable *SummaryTable);
//---------------------------------------------------------------------------
#endif
