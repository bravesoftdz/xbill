//---------------------------------------------------------------------------

#ifndef encoderH
#define encoderH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <FileCtrl.hpp>
#include <ComCtrls.hpp>

#include "jumble.h"
#include "fileprotect.h"
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
__published:	// IDE-managed Components
    TButton *Button1;
    TStatusBar *StatusBar1;
    TLabel *Label1;
    TProgressBar *ProgressBar1;
    TOpenDialog *OpenDialog1;
    TProgressBar *ProgressBar2;
    TMainMenu *MainMenu1;
    TMenuItem *File1;
    TMenuItem *Add1;
    TMenuItem *N1;
    TMenuItem *Exit1;
    TMenuItem *About1;
    TLabel *Label3;
    TPopupMenu *PopupMenu1;
    TMenuItem *Add2;
    TMenuItem *Add3;
    TGroupBox *GroupBox1;
    TListBox *ListBox1;
    TLabel *Label2;
    TEdit *Edit2;
    TButton *Button3;
    TLabel *Label4;
    TLabel *Label5;
    TListBox *ListBox2;
    TLabel *Label6;
    TTimer *Timer1;
    TLabel *Label7;
    TEdit *Edit1;
    TLabel *Label8;
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall StatusBar1DrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall About1Click(TObject *Sender);
    void __fastcall Exit1Click(TObject *Sender);
    void __fastcall Add3Click(TObject *Sender);
    void __fastcall Add2Click(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
    TFileProtectDo *fpdo;
    bool flag, running;
    int cnt;
    TTable *SummaryTable; 

public:		// User declarations
    CStringProtect csp;
    __fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
//---------------------------------------------------------------------------
#endif
