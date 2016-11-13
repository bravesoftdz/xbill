//---------------------------------------------------------------------------

#ifndef billqrH
#define billqrH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <DB.hpp>
#include <DBTables.hpp>
#include <ExtCtrls.hpp>
#include <QuickRpt.hpp>
#include <QRCtrls.hpp>
//---------------------------------------------------------------------------
class TForm5 : public TForm
{
__published:	// IDE-managed Components
    TTable *Table1;
    TQuickRep *QuickRep1;
    TQRBand *QRBand1;
    TQRLabel *QRLabel1;
    TQRBand *QRBand2;
    TQRLabel *QRLabel2;
    TQRLabel *QRLabel3;
    TQRLabel *QRLabel5;
    TQRLabel *QRLabel6;
    TQRLabel *QRLabel7;
    TQRBand *QRBand3;
    TQRDBText *QRDBText1;
    TQRDBText *QRDBText2;
    TQRDBText *QRDBText3;
    TQRDBText *QRDBText4;
    TQRDBText *QRDBText5;
    TQRBand *QRBand4;
    TQRLabel *QRLabel4;
    TQRLabel *QRLabel8;
    TQRSysData *QRSysData1;
    TQRLabel *QRLabel9;
    TQRSysData *QRSysData2;
private:	// User declarations
public:		// User declarations
    __fastcall TForm5(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm5 *Form5;
//---------------------------------------------------------------------------
#endif
