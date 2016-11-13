//---------------------------------------------------------------------------
#ifndef passwordH
#define passwordH

#include <vcl\BDE.hpp>
#include <vcl\DBTables.hpp>
#include <vcl\DB.hpp>

void __fastcall AddMasterPassword(TTable *, String);
void __fastcall AddAuxPassword(TTable *, String, String, PRVType);
void __fastcall RemoveMasterPassword(TTable *);
void __fastcall RemoveAuxPassword(TTable *, String, String);

short __fastcall GetSecNoFromName(hDBIDb, String, String);

//---------------------------------------------------------------------------
#endif
