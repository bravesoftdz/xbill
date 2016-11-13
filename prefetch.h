//---------------------------------------------------------------------------

#ifndef prefetchH
#define prefetchH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include "jumble.h"
#include "fileprotect.h"

//---------------------------------------------------------------------------
class TPrefetchBillBlock : public TThread
{            
private:
protected:
    void __fastcall Execute();
    AnsiString filename;
    int pos, size, *destlen;
    char *dest;
    TProgressBar *pb;
    TStatusPanel* sb ;
    AnsiString status;
    void __fastcall UpdateStatus();
    void __fastcall UpdateReadyFlag_false();
    void __fastcall UpdateReadyFlag_true();
    CStringProtect csp;
    bool *readyflag;
public:
    __fastcall TPrefetchBillBlock(CStringProtect &csp, AnsiString filename, int pos, int size, char *dest, int *destlen, TProgressBar *pb, TStatusPanel *sb, bool *readyflag, bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
