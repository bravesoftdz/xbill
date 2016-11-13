//---------------------------------------------------------------------------

#include <stdio.h>
#include <vcl.h>
#pragma hdrstop

#include "prefetch.h"
#include "bzlib.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TPrefetchBillBlock::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---
//------------------------------------------------------------------------

__fastcall TPrefetchBillBlock::TPrefetchBillBlock(CStringProtect &csp, AnsiString filename, int pos, int size, char *dest, int *destlen, TProgressBar *pb, TStatusPanel *sb, bool *readyflag, bool CreateSuspended)
    : TThread(CreateSuspended)
{
    this->filename = filename;
    this->pos = pos;
    this->size=size;
    this->dest =dest;
    this->destlen = destlen;
    this->pb = pb;
    this->sb = sb;
    this->csp = csp;
    this->readyflag = readyflag;
    this->FreeOnTerminate=true;
}
//---------------------------------------------------------------------------
void __fastcall TPrefetchBillBlock::UpdateStatus() {
    if (sb) sb->Text = status;
    if (pb) pb->StepIt();
}

void __fastcall TPrefetchBillBlock::UpdateReadyFlag_true() {
    *readyflag=true;
}

void __fastcall TPrefetchBillBlock::UpdateReadyFlag_false() {
    *readyflag=false;
}

void __fastcall TPrefetchBillBlock::Execute()
{
    //---- Place thread code here ----
    *readyflag = false;
    FILE *f = fopen(filename.c_str(), "rb");
    if(!f) {
        if(sb) {
            status = " Prefetch : Cannot open data file.";
            Synchronize(UpdateStatus);
        }
        Terminate();
        Synchronize(UpdateReadyFlag_true);
        return;
    }
    const int sobzsize = sizeof(fileprotect);
    fileprotect bs;
    char c, *enc_in, *bz_in;

    char *bestr, *bdstr, *buff_in;
    int bestrlen, bdstrlen;
    int ip, op, bz_inlen, rs,  ret;
    unsigned int insize;

    fseek(f, pos, SEEK_SET);

    if(fread(&bs, sobzsize, 1, f)!=1){
        if(sb) {
            status = " Prefetch : file read anomaly";
            Synchronize(UpdateStatus);
        }
        fclose(f);
        Terminate();
        Synchronize(UpdateReadyFlag_true);
        return;
    }

    enc_in = new char[bs.encout];

    if(bs.encout!=fread(enc_in, 1, bs.encout, f)) {
        if(sb) {
            status = " Prefetch : file read anomaly";
            Synchronize(UpdateStatus);
        }
        fclose(f);
        Terminate();
        Synchronize(UpdateReadyFlag_true);
        delete[] enc_in;
        return;
    }
    fclose(f);

    if(sb) {
        status = " Prefetch : Checking data integrity.";
        Synchronize(UpdateStatus);
    }

    if(Terminated) {
        status = " Prefetch : Thread killed.";
        Synchronize(UpdateStatus);
        delete[] enc_in;
        return;
    }
    csp.Extract(&bz_in, &bz_inlen, enc_in, bs.encout, bs.ip, bs.op, bs.lp);
    if(Terminated) {
        status = " Prefetch : Thread killed.";
        Synchronize(UpdateStatus);
        delete[] bz_in;
        delete[] enc_in;
        return;
    }

    if(bz_inlen != bs.bzout) {
        if(sb) {
            status = " Prefetch : Data integrity compromised.";
            Synchronize(UpdateStatus);
        }
        fclose(f);
        Terminate();
        Synchronize(UpdateReadyFlag_true);
        delete[] bz_in;
        delete[] enc_in;
        return;
    }

    if(sb) {
        status = " Prefetch : Data integrity OK.";
        Synchronize(UpdateStatus);
    }

    delete[] enc_in;

    buff_in = new char[bs.in];
    insize = bs.in;

    if(sb) {
        status = " Prefetch : Extracting Data...";
        Synchronize(UpdateStatus);
    }

    ret = BZ2_bzBuffToBuffDecompress(buff_in, &insize, bz_in, bz_inlen, 0, 0);

    switch (ret) {
        case BZ_OK : *destlen = insize;
                     memcpy(dest, buff_in, insize);
                     break;
        case BZ_OUTBUFF_FULL :
                     if(sb) {
                        status = " Prefetch : Data extraction failed...";
                        Synchronize(UpdateStatus);
                     }
                     delete[] buff_in;
                     delete[] bz_in;
                    Synchronize(UpdateReadyFlag_true);
                     return;
        case BZ_UNEXPECTED_EOF :
                     if(sb) {
                        status = " Prefetch : Data extraction failed...";
                        Synchronize(UpdateStatus);
                     }
                     delete[] buff_in;
                     delete[] bz_in;
                    Synchronize(UpdateReadyFlag_true);
                     return;
        case BZ_DATA_ERROR_MAGIC :
                     if(sb) {
                        status = " Prefetch : Data extraction failed...";
                        Synchronize(UpdateStatus);
                     }
                     delete[] buff_in;
                     delete[] bz_in;
                    Synchronize(UpdateReadyFlag_true);
                     return;
        case BZ_DATA_ERROR :
                     if(sb) {
                        status = " Prefetch : Data extraction failed...";
                        Synchronize(UpdateStatus);
                     }
                     delete[] buff_in;
                     delete[] bz_in;
                    Synchronize(UpdateReadyFlag_true);
                     return;
        case BZ_PARAM_ERROR :
                     if(sb) {
                        status = " Prefetch : Data extraction failed...";
                        Synchronize(UpdateStatus);
                     }
                     delete[] buff_in;
                     delete[] bz_in;
                    Synchronize(UpdateReadyFlag_true);
                     return;
        case BZ_MEM_ERROR :
                     if(sb) {
                        status = " Prefetch : Data extraction failed...";
                        Synchronize(UpdateStatus);
                     }
                     delete[] buff_in;
                     delete[] bz_in;
                    Synchronize(UpdateReadyFlag_true);
                     return;
    }
    delete[] buff_in;
    delete[] bz_in;
    if(sb) {
        status = " Prefetch : Extraction completed...";
        Synchronize(UpdateStatus);
    }
    Synchronize(UpdateReadyFlag_true) ; //Synchronize(UpdateReadyFlag_true);

}
//---------------------------------------------------------------------------
