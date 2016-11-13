//---------------------------------------------------------------------------

#ifndef testH
#define testH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>

#include "bitchar.h"
#include "jumble.h"
#include "processthread.h"

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TButton *Button1;
    TOpenDialog *OpenDialog1;
    TSaveDialog *SaveDialog1;
    TMemo *Memo1;
    TButton *Button2;
    TButton *Button3;
    TProgressBar *ProgressBar1;
    TProgressBar *ProgressBar2;
    TButton *Button4;
    TButton *Button5;
    TMemo *Memo2;
    TMemo *Memo3;
    TButton *Button6;
    TButton *Button7;
    TButton *Button8;
    TProgressBar *ProgressBar3;
    TProgressBar *ProgressBar4;
    TMemo *Memo4;
    TTrackBar *TrackBar1;
    TLabel *Label1;
    TLabel *Label2;
    TGroupBox *GroupBox1;
    TRadioButton *RadioButton1;
    TRadioButton *RadioButton2;
        TButton *Button9;
    TButton *Button10;
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall Button5Click(TObject *Sender);
    void __fastcall Button6Click(TObject *Sender);
    void __fastcall Button7Click(TObject *Sender);
    void __fastcall TrackBar1Change(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall TrackBar1Enter(TObject *Sender);
        void __fastcall Button9Click(TObject *Sender);
        void __fastcall Button8Click(TObject *Sender);
    void __fastcall Button10Click(TObject *Sender);

private:	// User declarations
    CStringProtect jumble;
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------

struct bzsize {
    int in, out;
};

#endif
