//---------------------------------------------------------------------------

#ifndef billqr3H
#define billqr3H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <DB.hpp>
#include <DBTables.hpp>
#include <ExtCtrls.hpp>
#include <QRCtrls.hpp>
#include <QuickRpt.hpp>
//---------------------------------------------------------------------------
class TForm7 : public TForm
{
__published:	// IDE-managed Components
    TQuickRep *QuickRep1;
    TQRBand *QRBand1;
    TQRBand *QRBand2;
    TQRBand *QRBand3;
    TTable *Table1;
    TQRLabel *QRLabel1;
    TQRLabel *QRLabel2;
    TQRDBText *QRDBText1;
    TQRDBText *QRDBText2;
    TQRDBText *QRDBText3;
    TQRDBText *QRDBText4;
    TQRDBText *QRDBText5;
    TQRDBText *QRDBText6;
    TQRDBText *QRDBText7;
    TQRDBText *QRDBText8;
    TQRDBText *QRDBText9;
    TQRBand *QRBand4;
private:	// User declarations
public:		// User declarations
    __fastcall TForm7(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm7 *Form7;
//---------------------------------------------------------------------------
#endif
