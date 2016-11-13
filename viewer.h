//---------------------------------------------------------------------------

#ifndef viewerH
#define viewerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>

#include "fileprotect.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// Composants gérés par l'EDI
    TOpenDialog *OpenDialog1;
    TStatusBar *StatusBar1;
    TRichEdit *Display;
    TTimer *Timer1;
    TTimer *Timer2;
    TTimer *Timer3;
    TTimer *PendingTimer;
    TMainMenu *MainMenu1;
    TMenuItem *N1;
    TMenuItem *Load1;
    TMenuItem *About1;
    TMenuItem *Operation1;
    TMenuItem *AutoForward1;
    TMenuItem *AutoReverse1;
    TMenuItem *N2;
    TMenuItem *Exit1;
    TMenuItem *N3;
    TMenuItem *SetTiming1;
    TMenuItem *N4;
    TMenuItem *Forward1;
    TMenuItem *PreviousPage1;
    TMenuItem *N5;
    TMenuItem *Home1;
    TMenuItem *LastPage1;
    TMenuItem *Gotopage1;
    TProgressBar *ProgressBar1;
    TPanel *Panel1;
    TButton *btEnd;
    TButton *Button3;
    TButton *Button4;
    TButton *Button5;
    TButton *Button6;
    TButton *btHome;
    TMenuItem *Extt1;
    TMenuItem *ListAll1;
    TMenuItem *GroupList1;
    TMenuItem *CallDetail1;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall DisplaySelectionChange(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall Timer2Timer(TObject *Sender);
    void __fastcall Timer3Timer(TObject *Sender);
    void __fastcall PendingTimerTimer(TObject *Sender);
    void __fastcall Load1Click(TObject *Sender);
    void __fastcall SetTiming1Click(TObject *Sender);
    void __fastcall AutoForward1Click(TObject *Sender);
    void __fastcall AutoReverse1Click(TObject *Sender);
    void __fastcall Forward1Click(TObject *Sender);
    void __fastcall PreviousPage1Click(TObject *Sender);
    void __fastcall Home1Click(TObject *Sender);
    void __fastcall LastPage1Click(TObject *Sender);
    void __fastcall Gotopage1Click(TObject *Sender);
    void __fastcall SetLayout();
    void __fastcall FormResize(TObject *Sender);
    void __fastcall test1Click(TObject *Sender);
    void __fastcall StatusBar1DrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
    void __fastcall About1Click(TObject *Sender);
    void __fastcall Exit1Click(TObject *Sender);
    void __fastcall ListAll1Click(TObject *Sender);
    void __fastcall GroupList1Click(TObject *Sender);
    void __fastcall CallDetail1Click(TObject *Sender);

private:	// Déclarations de l'utilisateur
    char * nextbuff, *currbuff, *prevbuff;
    int nextbufflen, currbufflen, prevbufflen;
    fileindex idx[1024];
    int idxmax, idxcurr, idxnext, idxprev;
    int *currpageindex;
    int currpageindexlen;
    int *nextpageindex;
    int nextpageindexlen;
    int *prevpageindex;
    int prevpageindexlen;
    int currpage;
    bool loading;
    int pagemin, pagemax, pagecurr;
    bool currbuffready, nextbuffready, prevbuffready;
    CStringProtect csp;
    AnsiString indexfilename;
    void __fastcall DisplayPage(int page, int maxlen);
    void __fastcall DisplayNextSubPage();
    void __fastcall LoadPageIndex(int idx, int **pageindex, int *pageindexlen);
    void __fastcall GotoPage(int page);
    int __fastcall SelectPage(int rpage);
    int htextlen,pendingrpage;
    AnsiString hangingtext;
    void __fastcall DoDisplay(const char *line); 


public:		// Déclarations de l'utilisateur
    __fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------

class TForm2Exception {

    public:
        char fname[255];
        int eline;
        TForm2Exception(char *filename, int line) {
            strcpy(fname, filename);
            eline = line;
        }
};

#endif
