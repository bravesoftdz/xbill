//---------------------------------------------------------------------------

#ifndef dbtestH
#define dbtestH
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
class TForm4 : public TForm
{
__published:	// IDE-managed Components
    TBDEClientDataSet *BDEClientDataSet1;
    TTable *Table1;
    TQuery *Query1;
    TOpenDialog *OpenDialog1;
    TButton *Button1;
    TMemo *Memo1;
    TDatabase *Database1;
    TStatusBar *StatusBar1;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm4(TComponent* Owner);
    void __fastcall ProcessLine(AnsiString line);
protected:
    AnsiString __fastcall EscapeString(AnsiString str);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm4 *Form4;
//---------------------------------------------------------------------------
#endif
