//---------------------------------------------------------------------------

#ifndef billqr2H
#define billqr2H
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
class TForm6 : public TForm
{
__published:	// IDE-managed Components
    TQuickRep *QuickRep1;
    TQRBand *QRBand1;
    TQRBand *QRBand2;
    TQRLabel *QRLabel1;
    TQuery *Query1;
    TQRBand *QRBand3;
    TQRChildBand *QRChildBand1;
    TQRDBText *QRDBText1;
    TQRDBText *QRDBText2;
    TQRDBText *QRDBText3;
    TQRDBText *QRDBText4;
    TQRDBText *QRDBText5;
private:	// User declarations
public:		// User declarations
    __fastcall TForm6(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm6 *Form6;
//---------------------------------------------------------------------------
#endif
