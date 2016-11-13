//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "viewer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "fileprotect.h"
#include "jumble.h"
#include "bzlib.h"
#include "prefetch.h"
#include "about.h"
#include "billqr.h"
#include "billqr2.h"
#include "billqr3.h"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
    : TForm(Owner)
{
    pendingrpage=-1;
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormCreate(TObject *Sender)
{
    Display->Text = "";
    nextbuff = prevbuff = currbuff = NULL;
    nextbufflen = prevbufflen = currbufflen = 0;
    currpageindex = NULL;
    currpageindexlen = 0;
    prevpageindex = NULL;
    prevpageindexlen = 0;
    nextpageindex = NULL;
    nextpageindexlen = 0;
    loading = false;
    //setup display area
    ProgressBar1->Parent = StatusBar1;

    long style = GetWindowLong(ProgressBar1->Handle, GWL_EXSTYLE);
    style &= ~WS_EX_STATICEDGE;
    SetWindowLong(ProgressBar1->Handle, GWL_EXSTYLE,style);
    SetLayout();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::SetLayout() {
    Display->Top = 0;
    Display->Left = 0;
    Display->Height = Form2->ClientHeight-StatusBar1->Height-Panel1->Height;
    Display->Width = Form2->ClientWidth;
    int pw = Form2->ClientWidth/5;
    ProgressBar1->Top=0;
    for(int i=0;i<StatusBar1->Panels->Count;i++) {
        StatusBar1->Panels->Items[i]->Width = pw;
        ProgressBar1->Left+=pw;
    }
    ProgressBar1->Left-=pw;
    ProgressBar1->Width = pw-4;
    StatusBar1->Width = Form2->ClientWidth;
    Panel1->Top = Display->Height;
    Panel1->Left = (Form2->ClientWidth-Panel1->Width)/2;

}

void __fastcall TForm2::LoadPageIndex(int index, int **pageindex, int *pageindexlen) {
    FILE *fidx = fopen(indexfilename.c_str(), "rb");
    fseek(fidx,0,SEEK_END);
    int idxlen = ftell(fidx), fptr=0;
    fseek(fidx, 0, SEEK_SET);
    bool locked = false;
    int pidx=0, tmp, cnt=0;

    if(*pageindex) delete[] *pageindex;

    while(!feof(fidx) && fptr < idxlen) {
        fptr+=fread(&idx[pidx++], sizeof(fileindex), 1, fidx)*sizeof(fileindex);
        fptr+=fread(&tmp, sizeof(int), 1, fidx)*sizeof(int);
        if(index==cnt++) {
            *pageindexlen = tmp;
            *pageindex = new int[tmp];
            fptr+=fread(*pageindex, sizeof(int), *pageindexlen, fidx)*sizeof(int);
            break;
        } else {
            fseek(fidx, tmp*sizeof(int), SEEK_CUR);
            fptr+=tmp*sizeof(int);
        }
    }
    fclose(fidx);
}

void __fastcall TForm2::GotoPage(int page) {
}



void __fastcall TForm2::DisplaySelectionChange(TObject *Sender)
{
    if(!loading) {
        Display->SelStart = 0;
        Display->SelLength = 0;
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm2::DisplayPage(int page, int maxlen){
    Display->Text = "";
    char c, line[512];
    int j=0;
    if(currpageindex==NULL) throw TForm2Exception(__FILE__, __LINE__);
    for(int i=currpageindex[page];i<maxlen;i++) {
        if((c = currbuff[i])=='\r') continue;
        if(c =='\n') {
            line[j]=0;
            //Display->Lines->Add(line+4);
            DoDisplay(line);
            j=0;
            if(currbuff[i+1]=='1') break;
        } else {
            line[j++]=c;
        }
    }
    if(htextlen == j)  hangingtext = line;
    else hangingtext = "";
    StatusBar1->Panels->Items[3]->Text = "OK";
    Display->SelStart = 0;


}

void __fastcall TForm2::DisplayNextSubPage(){
    char c, line[512];
    for(int i=0, j=0; i<nextpageindex[0];i++) {
        if((c = nextbuff[i])=='\r') continue;
        if(c =='\n') {
            line[j]=0;
            if(htextlen) {
                hangingtext += line;
                //Display->Lines->Add(hangingtext+4);
                DoDisplay(hangingtext.c_str());
                htextlen = 0;
                hangingtext ="";
            } else {
                //Display->Lines->Add(line+4);
                DoDisplay(line);
            }
            j=0;
            if(nextbuff[i+1]=='1') break;
        } else {
            line[j++]=c;
        }
    }
}

int __fastcall TForm2::SelectPage(int rpage) {
    // check rpage to be in range
    if(idx[0].page_start>rpage || idx[idxmax-1].page_stop < rpage) {
        StatusBar1->Panels->Items[3]->Text = "Page out of range";
        return 0;
    }

    // check if rpage is allowed
    if((!nextbuffready && idxcurr !=idxmax-1) || (!prevbuffready && idxcurr!=0)) {
        if(pendingrpage != -1) {
            StatusBar1->Panels->Items[3]->Text = "Still have pending page to be displayed";
            return 0;
        }
        if(idx[idxcurr].page_start >= rpage || idx[idxcurr].page_stop<=rpage) {
            pendingrpage = rpage;
            PendingTimer->Enabled = true;
            StatusBar1->Panels->Items[3]->Text = "Fetching page ... Please wait";
            return 0;
        }
    }

    // page is in range ... now look into prefetched tables ...
    int page;
    // check if rpage is within current buffer
    if(idx[idxcurr].page_start<= rpage && idx[idxcurr].page_stop >= rpage) {
        // ok .. we are still within buffer... now check if we are at the extrimities ..
        if(idx[idxcurr].page_start==rpage) {
            // aiee.. current buffer may only have part of the required page ... check if complete
            if(idxcurr==0 || idx[idxprev].page_stop != rpage) {
                // oufff ... we only need to read the buffer
                // buffer page indexed starts from 0 ..
                page = rpage-idx[idxcurr].page_start;
                if(currbuffready == false) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }
                DisplayPage(page, currbufflen);
            } else {
                // double trouble .. need to load last packet of previous buffer and merge with current buffer
                page = rpage - idx[idxprev].page_start;
                if(!currbuffready || !prevbuffready) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }
                Display->Text = "";
                char c, line[512];

                int j=0;
                for(int i=prevpageindex[page];i<prevbufflen;i++) {
                    if((c = prevbuff[i])=='\r') continue;
                    if(c =='\n') {
                        line[j]=0;
                        //Display->Lines->Add(line+4);
                        DoDisplay(line);
                        j=0;
                        if(prevbuff[i+1]=='1') break;
                    } else {
                        line[j++]=c;
                    }
                }
                page = rpage - idx[idxcurr].page_start;
                for(int i=0;i<currpageindex[page+1];i++) {
                    if((c = currbuff[i])=='\r') continue;
                    if(c =='\n') {
                        line[j]=0;
                        //Display->Lines->Add(line+4);
                        DoDisplay(line);
                        j=0;
                        if(currbuff[i+1]=='1') break;
                    } else {
                        line[j++]=c;
                    }
                }
            }
        } else if(idx[idxcurr].page_stop==rpage) {
            // aiee.. current buffer may only have part of the required page ...
            // check if complete
            if(idx[idxnext].page_start != rpage) {
                // oufff ... we only need to read the buffer
                page = rpage-idx[idxcurr].page_start;
                if(!currbuffready) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }
                DisplayPage(page, currbufflen);
            } else {
                // double trouble .. need to load last packet of current buffer and merge with next buffer
                page = rpage - idx[idxcurr].page_start;
                if(!currbuffready || !nextbuffready) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }
                Display->Text = "";
                char c, line[512];
                int j=0;
                for(int i=currpageindex[page];i<currbufflen;i++) {
                    if((c = currbuff[i])=='\r') continue;
                    if(c =='\n') {
                        line[j]=0;
                        //Display->Lines->Add(line+4);
                        DoDisplay(line);
                        j=0;
                        if(currbuff[i+1]=='1') break;
                    } else {
                        line[j++]=c;
                    }
                }
                page = rpage - idx[idxnext].page_start;
                for(int i=0;i<nextpageindex[page+1];i++) {
                    if((c = nextbuff[i])=='\r') continue;
                    if(c =='\n') {
                        line[j]=0;
                        //Display->Lines->Add(line+4);
                        DoDisplay(line);
                        j=0;
                        if(nextbuff[i+1]=='1') break;
                    } else {
                        line[j++]=c;
                    }
                }
            }
        } else {
            page = rpage-idx[idxcurr].page_start;
            if(!currbuffready) {
                StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                return 0;
            }
            DisplayPage(page, currbufflen);
        }
    } else if(idx[idxnext].page_start< rpage && idx[idxnext].page_stop >= rpage) {
        // we are now within next buffer... now check if we are at the extrimities ..
        if(idx[idxnext].page_stop==rpage) {
            // aiee.. current buffer may only have part of the required page ...
            // check if complete
            if(idxnext>=idxmax-1 || idx[idxnext+1].page_start != rpage) {
                // oufff ... we only need to read the buffer
                // rearrange our indexes
                if(!currbuffready || !nextbuffready) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }

                prevbuffready = false;
                nextbuffready = false;
                if(prevbuff) delete[] prevbuff;
                prevbuff=currbuff;
                currbuff=nextbuff;
                prevbufflen=currbufflen;
                currbufflen=nextbufflen;
                idxprev=idxcurr;
                idxcurr=idxnext;
                nextbuff=NULL;nextbufflen=0;
                if(prevpageindex) delete[] prevpageindex;
                prevpageindex = currpageindex;
                currpageindex = nextpageindex;
                prevpageindexlen = currpageindexlen;
                currpageindexlen = nextpageindexlen;
                nextpageindex= NULL;
                nextpageindexlen = 0;
                prevbuffready = true;
                if(idxnext<idxmax-2) {
                    idxnext = idxnext+1;
                    nextbuff= new char[DEC_OUT_BUFFER_SIZE];
                    nextbufflen = DEC_OUT_BUFFER_SIZE;
                    TPrefetchBillBlock * prefetch = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxnext].target, 0, nextbuff, &nextbufflen, ProgressBar1, StatusBar1->Panels->Items[2], &nextbuffready, false);
                    LoadPageIndex(idxnext, &nextpageindex, &nextpageindexlen);
               }
               page = rpage - idx[idxcurr].page_start;
               DisplayPage(page, currbufflen);
            } else {
                // double trouble .. need to load next packet of previous buffer and merge with current buffer
                if(!currbuffready || !nextbuffready) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }
                prevbuffready = false;
                nextbuffready = false;
                if(prevbuff) delete[] prevbuff;
                prevbuff=currbuff;
                currbuff=nextbuff;
                prevbufflen=currbufflen;
                currbufflen=nextbufflen;
                idxprev=idxcurr;
                idxcurr=idxnext;
                nextbuff=NULL;nextbufflen=0;
                if(prevpageindex) delete[] prevpageindex;
                prevpageindex = currpageindex;
                currpageindex = nextpageindex;
                prevpageindexlen = currpageindexlen;
                currpageindexlen = nextpageindexlen;
                nextpageindex= NULL;
                nextpageindexlen = 0;
                idxnext = idxnext+1;
                prevbuffready = true;
                nextbuff= new char[DEC_OUT_BUFFER_SIZE];
                nextbufflen = DEC_OUT_BUFFER_SIZE;
                nextbuffready=false;
                TPrefetchBillBlock * prefetch = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxnext].target, 0, nextbuff, &nextbufflen, ProgressBar1, StatusBar1->Panels->Items[2], &nextbuffready, false);
                LoadPageIndex(idxnext, &nextpageindex, &nextpageindexlen);
                Cursor  = crHourGlass;
                while(!nextbuffready);
                Cursor  = crDefault;

                // everything is in place .. move ..
                page = rpage - idx[idxcurr].page_start;
                Display->Text = "";
                char c, line[512];
                int j=0;
                for(int i=currpageindex[page];i<currbufflen;i++) {
                    if((c = currbuff[i])=='\r') continue;
                    if(c =='\n') {
                        line[j]=0;
                        //Display->Lines->Add(line+4);
                        DoDisplay(line);
                        j=0;
                        if(currbuff[i+1]=='1') break;
                    } else {
                        line[j++]=c;
                    }
                }
                page = rpage - idx[idxnext].page_start;
                for(int i=0;i<nextpageindex[page+1];i++) {
                    if((c = nextbuff[i])=='\r') continue;
                    if(c =='\n') {
                        line[j]=0;
                        //Display->Lines->Add(line+4);
                        DoDisplay(line);
                        j=0;
                        if(nextbuff[i+1]=='1') break;
                    } else {
                        line[j++]=c;
                    }
                }
                // ok done ..
            }
        } else {
                if(!currbuffready || !nextbuffready) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }

                prevbuffready = false;
                nextbuffready = false;
                if(prevbuff) delete[] prevbuff;
                prevbuff=currbuff;
                currbuff=nextbuff;
                prevbufflen=currbufflen;
                currbufflen=nextbufflen;
                idxprev=idxcurr;
                idxcurr=idxnext;
                nextbuff=NULL;nextbufflen=0;
                if(prevpageindex) delete[] prevpageindex;
                prevpageindex = currpageindex;
                currpageindex = nextpageindex;
                prevpageindexlen = currpageindexlen;
                currpageindexlen = nextpageindexlen;
                nextpageindex= NULL;
                nextpageindexlen = 0;
                prevbuffready = true;
                if(idxnext<idxmax-1) {
                    idxnext = idxnext+1;
                    nextbuff= new char[DEC_OUT_BUFFER_SIZE];
                    nextbufflen = DEC_OUT_BUFFER_SIZE;
                    TPrefetchBillBlock * prefetch = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxnext].target, 0, nextbuff, &nextbufflen, ProgressBar1, StatusBar1->Panels->Items[2], &nextbuffready, false);
                    LoadPageIndex(idxnext, &nextpageindex, &nextpageindexlen);
               }
               page = rpage - idx[idxcurr].page_start;
               DisplayPage(page, currbufflen);
        }
    } else if(idx[idxprev].page_start<= rpage && idx[idxprev].page_stop > rpage) {
        // we are now within prev buffer... now check if we are at the extrimities ..
        if(idx[idxprev].page_start==rpage) {
            // aiee.. current buffer may only have part of the required page ...
            // check if complete
            if(idxprev<1 || idx[idxprev-1].page_stop != rpage) {
                // oufff ... we only need to read the buffer
                if(!currbuffready || !prevbuffready) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }
                prevbuffready = false;
                nextbuffready = false;
                if(nextbuff) delete[] nextbuff;
                nextbuff=currbuff;
                currbuff=prevbuff;
                nextbufflen=currbufflen;
                currbufflen=prevbufflen;
                idxnext=idxcurr;
                idxcurr=idxprev;
                prevbuff=NULL;prevbufflen=0;
                if(nextpageindex) delete[] nextpageindex;
                nextpageindex = currpageindex;
                currpageindex = prevpageindex;
                nextpageindexlen = currpageindexlen;
                currpageindexlen = prevpageindexlen;
                prevpageindex= NULL;
                prevpageindexlen = 0;
                nextbuffready = true;
                if(idxprev>0) {
                    idxprev = idxprev+1;
                    prevbuff= new char[DEC_OUT_BUFFER_SIZE];
                    prevbufflen = DEC_OUT_BUFFER_SIZE;
                    TPrefetchBillBlock * prefetch = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxprev].target, 0, prevbuff, &prevbufflen, ProgressBar1, StatusBar1->Panels->Items[0], &prevbuffready, false);
                    LoadPageIndex(idxprev, &prevpageindex, &prevpageindexlen);
               }
               page = rpage - idx[idxcurr].page_start;
               DisplayPage(page, currbufflen);
            } else {
                // double trouble .. need to load last packet of previous buffer and merge with current buffer
                if(!currbuffready || !prevbuffready) {
                    StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                    return 0;
                }
                prevbuffready = false;
                nextbuffready = false;
                if(nextbuff) delete[] nextbuff;
                nextbuff=currbuff;
                currbuff=prevbuff;
                nextbufflen=currbufflen;
                currbufflen=prevbufflen;
                idxnext=idxcurr;
                idxcurr=idxprev;
                prevbuff=NULL;prevbufflen=0;
                if(nextpageindex) delete[] nextpageindex;
                nextpageindex = currpageindex;
                currpageindex = prevpageindex;
                nextpageindexlen = currpageindexlen;
                currpageindexlen = prevpageindexlen;
                prevpageindex= NULL;
                prevpageindexlen = 0;
                idxprev = idxprev+1;
                prevbuff= new char[DEC_OUT_BUFFER_SIZE];
                prevbufflen = DEC_OUT_BUFFER_SIZE;
                nextbuffready = true;
                TPrefetchBillBlock * prefetch = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxprev].target, 0, prevbuff, &prevbufflen, ProgressBar1, StatusBar1->Panels->Items[0], &prevbuffready, false);
                LoadPageIndex(idxprev, &prevpageindex, &prevpageindexlen);
                Cursor = crHourGlass;
                while(!prevbuffready);
                Cursor = crDefault;

                page = rpage - idx[idxprev].page_start;
                Display->Text = "";
                char c, line[512];
                int j=0;
                for(int i=prevpageindex[page];i<prevbufflen;i++) {
                    if((c = prevbuff[i])=='\r') continue;
                    if(c =='\n') {
                        line[j]=0;
                        //Display->Lines->Add(line+4);
                        DoDisplay(line);
                        j=0;
                        if(prevbuff[i+1]=='1') break;
                    } else {
                        line[j++]=c;
                    }
                }
                page = rpage - idx[idxcurr].page_start;
                for(int i=0;i<currpageindex[page+1];i++) {
                    if((c = currbuff[i])=='\r') continue;
                    if(c =='\n') {
                        line[j]=0;
                        //Display->Lines->Add(line+4);
                        DoDisplay(line);
                        j=0;
                        if(currbuff[i+1]=='1') break;
                    } else {
                        line[j++]=c;
                    }
                }

            }
        } else {
            // OK ... no worries, process prev buffer
            if(!currbuffready || !prevbuffready) {
                StatusBar1->Panels->Items[3]->Text = "Buffer not ready. Please wait and retry";
                return 0;
            }
            prevbuffready = false;
            nextbuffready = false;
            if(nextbuff) delete[] nextbuff;
            nextbuff=currbuff;
            currbuff=prevbuff;
            nextbufflen=currbufflen;
            currbufflen=prevbufflen;
            idxnext=idxcurr;
            idxcurr=idxprev;
            prevbuff=NULL;prevbufflen=0;
            if(nextpageindex) delete[] nextpageindex;
            nextpageindex = currpageindex;
            currpageindex = prevpageindex;
            nextpageindexlen = currpageindexlen;
            currpageindexlen = prevpageindexlen;
            prevpageindex= NULL;
            prevpageindexlen = 0;
            nextbuffready = true;
            if(idxprev>0) {
                idxprev = idxprev-1;
                prevbuff= new char[DEC_OUT_BUFFER_SIZE];
                prevbufflen = DEC_OUT_BUFFER_SIZE;
                TPrefetchBillBlock * prefetch = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxprev].target, 0, prevbuff, &prevbufflen, ProgressBar1, StatusBar1->Panels->Items[0], &prevbuffready, false);
                LoadPageIndex(idxprev, &prevpageindex, &prevpageindexlen);
            }
            page = rpage - idx[idxcurr].page_start;
            DisplayPage(page, currbufflen);
        }
    } else { // now we have big problem .... none of the prefetch can be used
        prevbuffready = false;
        currbuffready = false;
        nextbuffready = false;

        if(nextbuff) delete[] nextbuff;
        if(currbuff) delete[] currbuff;
        if(prevbuff) delete[] prevbuff;
        currbufflen=nextbufflen=prevbufflen=0;
        currbuff=nextbuff=prevbuff=NULL;
        if(nextpageindex) delete[] nextpageindex;
        if(currpageindex) delete[] currpageindex;
        if(prevpageindex) delete[] prevpageindex;
        prevpageindex=currpageindex=nextpageindex=NULL;
        prevpageindexlen=nextpageindexlen=currpageindexlen=0;
        for(int i=0;i<idxmax;i++) {
            if(rpage>=idx[i].page_start && rpage <= idx[i].page_stop) {
                idxcurr = i;
                currbuff= new char[DEC_OUT_BUFFER_SIZE];
                currbufflen = DEC_OUT_BUFFER_SIZE;
                TPrefetchBillBlock * prefetch_curr = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxcurr].target, 0, currbuff, &currbufflen, NULL, StatusBar1->Panels->Items[1], &currbuffready, false);
                LoadPageIndex(idxcurr, &currpageindex, &currpageindexlen);
                if(i>1) {
                    idxprev=i-1;
                    prevbuff= new char[DEC_OUT_BUFFER_SIZE];
                    prevbufflen = DEC_OUT_BUFFER_SIZE;
                    TPrefetchBillBlock * prefetch_prev = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxprev].target, 0, prevbuff, &prevbufflen, NULL, StatusBar1->Panels->Items[0], &prevbuffready, false);
                    LoadPageIndex(idxprev, &prevpageindex, &prevpageindexlen);
                } else {
                    prevbuffready=true;
                }

                if(i<idxmax-1) {
                    idxnext = i+1;
                    nextbuff= new char[DEC_OUT_BUFFER_SIZE];
                    nextbufflen = DEC_OUT_BUFFER_SIZE;
                    TPrefetchBillBlock * prefetch_next = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxnext].target, 0, nextbuff, &nextbufflen, NULL, StatusBar1->Panels->Items[2], &nextbuffready, false);
                    LoadPageIndex(idxnext, &nextpageindex, &nextpageindexlen);
                } else {
                    nextbuffready=true;
                }

                Cursor = crHourGlass;
                StatusBar1->Panels->Items[3]->Text = "Fetching page ... Please wait";
                Timer1->Enabled = true;
                Operation1->Enabled = false;
                Panel1->Enabled = false;
                loading = false;
                Display->Enabled = false;
                break;
            }
        }
//        return SelectPage(rpage);
    }
    return 1;
}
void __fastcall TForm2::Timer1Timer(TObject *Sender)
{
      Timer1->Enabled = false;
      if(!currbuffready || !nextbuffready || !prevbuffready) {
        Timer1->Enabled = true;
      } else {
        Cursor = crDefault;
        Operation1->Enabled = true;
        Panel1->Enabled = true;
        loading = true;
        Display->Enabled = true;
        SelectPage(currpage);
      }
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Timer2Timer(TObject *Sender)
{
    Timer3->Enabled = false;
    loading = true;
    currpage++;
    if(!SelectPage(currpage)) {
        Timer2->Enabled = false;
        AutoForward1->Checked = false;
        AutoForward1->Caption = "Auto &Forward";
        AutoReverse1->Enabled = true;
    }
    loading = false;

}
//---------------------------------------------------------------------------



void __fastcall TForm2::Timer3Timer(TObject *Sender)
{
    Timer2->Enabled = false;
    loading = true;
    currpage--;
    if(!SelectPage(currpage)) {
        Timer3->Enabled = false;
        AutoReverse1->Checked = false;
        AutoReverse1->Caption = "Auto &Reverse";
        AutoForward1->Enabled = true;
    }        
    loading = false;
}
//---------------------------------------------------------------------------


void __fastcall TForm2::PendingTimerTimer(TObject *Sender)
{
    PendingTimer->Enabled = false;
    if(nextbuffready && prevbuffready) {
        loading = true;
        SelectPage(pendingrpage);
        loading = false;
        pendingrpage = -1;
        StatusBar1->Panels->Items[3]->Text = "OK";

        return;
    }
    PendingTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Load1Click(TObject *Sender)
{
    Load1->Enabled=false;
    Display->Text = "";
    if(nextbuff) {
        delete[] nextbuff;
        nextbuff=NULL;
        nextbufflen = 0;
    }
    if(prevbuff) {
        delete[] prevbuff;
        prevbuff=NULL;
        prevbufflen = 0;
    }
    if(currbuff) {
        delete[] currbuff;
        currbuff=NULL;
        currbufflen = 0;
    }
    if(nextpageindex) delete[] nextpageindex;
    if(currpageindex) delete[] currpageindex;
    if(prevpageindex) delete[] prevpageindex;
    prevpageindex=currpageindex=nextpageindex=NULL;
    prevpageindexlen=nextpageindexlen=currpageindexlen=0;

    fileprotect bs;
    const int sobzsize = sizeof(fileprotect);
    char *enc_in, *bz_in,  sb[sobzsize],c;

    char *bestr, *bdstr, *buff_in;
    int bestrlen, bdstrlen;
    int ip, op, bz_inlen, rs,  ret;
    unsigned int insize;

    if(!OpenDialog1->Execute()) {
        return;
    }

    FILE *f = fopen(OpenDialog1->FileName.c_str(), "rb");
    if(!f) {
        Application->MessageBoxA("Configuration file cannot be loaded", "Unrecoverable error !", MB_OK);
        return;
    }
    if(!LoadConfig(csp, ChangeFileExt(OpenDialog1->FileName, ".cfg"))){
        Application->MessageBoxA("Configuration file cannot be loaded", "Unrecoverable error !", MB_OK);
        return;
    }

    char line[120];
    Cursor = crHourGlass;

    if(nextbuff) delete[] nextbuff;
    if(currbuff) delete[] currbuff;
    if(prevbuff) delete[] prevbuff;
    currbufflen=nextbufflen=prevbufflen=0;
    currbuff=nextbuff=prevbuff=NULL;
    if(nextpageindex) delete[] nextpageindex;
    if(currpageindex) delete[] currpageindex;
    if(prevpageindex) delete[] prevpageindex;
    prevpageindex=currpageindex=nextpageindex=NULL;
    prevpageindexlen=nextpageindexlen=currpageindexlen=0;
    currbuffready=nextbuffready=prevbuffready=false;

    // load & analyze index file
    indexfilename = ChangeFileExt(OpenDialog1->FileName, ".idx");
    FILE *fidx = fopen(indexfilename.c_str(), "rb");
    fseek(fidx,0,SEEK_END);
    int idxlen = ftell(fidx), fptr=0;
    fseek(fidx, 0, SEEK_SET);
    bool locked = false;
    int pidx=0, tmp;

    int rdtmp;
    while(!feof(fidx) && fptr < idxlen) {
        rdtmp=fread(&idx[pidx++], sizeof(fileindex), 1, fidx);
        if(rdtmp!=1) break;
    //    fptr+=rdtmp*sizeof(int);
        rdtmp=fread(&tmp, sizeof(int), 1, fidx);
        if(rdtmp!=1) break;
  //      fptr+=rdtmp*sizeof(int);
        if(!locked) {
            currpageindexlen = tmp;
            currpageindex = new int[tmp];
            rdtmp=fread(currpageindex, sizeof(int), currpageindexlen, fidx);
            if(rdtmp!=currpageindexlen) break;
//            fptr+=rdtmp*sizeof(int);
            locked = true;
        } else {
            fseek(fidx, tmp*sizeof(int), SEEK_CUR);
            //fptr+=tmp*sizeof(int);
        }
        fptr = ftell(fidx);
    }
    fclose(fidx);
    idxmax = pidx;
    idxcurr=0; idxnext = 0; idxprev=0;
    pagemin  = idx[idxcurr].page_start;
    pagemax  = idx[idxcurr].page_stop;
    pagecurr = 0;
    currpage=1;
    TPrefetchBillBlock * prefetch_next;

    // end analyze
//--
         rs = fread(sb, 1, sobzsize, f);
        if(rs!=sobzsize){
            Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
        }
        memcpy(&bs, sb, sobzsize);
        enc_in = new char[bs.encout];
//         outsize = ;
        if(bs.encout!=fread(enc_in, 1, bs.encout, f)) {
            Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
        }

        csp.Extract(&bz_in, &bz_inlen, enc_in, bs.encout, bs.ip, bs.op, bs.lp);

        if(bz_inlen != bs.bzout) {
            Application->MessageBoxA("Saiz tak betul", "Alamak !", MB_OK);
        }

        delete[] enc_in;
        buff_in = new char[bs.in];
        insize = bs.in;
        ret = BZ2_bzBuffToBuffDecompress(buff_in, &insize, bz_in, bz_inlen, 0, 0);
        switch (ret) {
            case BZ_OK :

                        loading=true;
                        currbuff=new char[currbufflen=insize];
                        memcpy(currbuff, buff_in, insize);
                        currbuffready=true;
                        DisplayPage(0, insize);
                        loading = false;

                        nextbuff= new char[DEC_OUT_BUFFER_SIZE];
                        nextbufflen = DEC_OUT_BUFFER_SIZE;
                        idxnext = 1;
                        prefetch_next = new TPrefetchBillBlock(csp, OpenDialog1->FileName, idx[idxnext].target, 0, nextbuff, &nextbufflen, NULL, StatusBar1->Panels->Items[2], &nextbuffready, false);
                        LoadPageIndex(idxnext, &nextpageindex, &nextpageindexlen);
                         break;
            case BZ_OUTBUFF_FULL : Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
                                  fclose(f);
                                  Cursor = crDefault;

                                   return;
            case BZ_UNEXPECTED_EOF : Application->MessageBoxA("Fail tak cukup sifat", "Alamak !", MB_OK);
                                  fclose(f);
                                  Cursor = crDefault;
                                   return;
            case BZ_DATA_ERROR_MAGIC : Application->MessageBoxA("Format data tak betul", "Alamak !", MB_OK);
                                  fclose(f);
                                  Cursor = crDefault;
                                   return;
            case BZ_DATA_ERROR : Application->MessageBoxA("Data corrupted", "Aieee !", MB_OK);
                                  fclose(f);
                                  Cursor = crDefault;
                                   return;

            case BZ_PARAM_ERROR : Application->MessageBoxA("Parameter bermasalah", "Salah tu bang aji", MB_OK);
                                  fclose(f);
                                  Cursor = crDefault;
                                  return;
            case BZ_MEM_ERROR : Application->MessageBoxA("Memori tak cukup lah pulak", "Alamak !", MB_OK);
                                fclose(f);
                                  Cursor = crDefault;
                                return;;

        }
        delete[] buff_in;
        delete[] bz_in;
        fclose(f);
        Cursor = crDefault;

}
//---------------------------------------------------------------------------

void __fastcall TForm2::SetTiming1Click(TObject *Sender)
{
    AnsiString timing;
    if(InputQuery("Set timing", "Please input delay before each page display in milisecond", timing)) {
        Timer2->Interval = timing.ToIntDef(Timer2->Interval);
        Timer3->Interval = Timer2->Interval;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm2::AutoForward1Click(TObject *Sender)
{
    Timer2->Enabled = !Timer2->Enabled;
    if(Timer2->Enabled) {
        AutoForward1->Checked = true;
        AutoReverse1->Caption = "Stop Auto &Forward";
        AutoReverse1->Enabled = false;
    } else {
        AutoForward1->Checked = false;
        AutoReverse1->Caption = "Auto &Forward";
        AutoReverse1->Enabled = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm2::AutoReverse1Click(TObject *Sender)
{
    Timer3->Enabled = !Timer3->Enabled;
    if(Timer3->Enabled) {
        AutoReverse1->Checked = true ;
        AutoReverse1->Caption =  "Stop Auto &Reverse";
        AutoForward1->Enabled = false;
    } else {
        AutoReverse1->Checked = false;
        AutoReverse1->Caption = "Auto &Reverse";
        AutoForward1->Enabled = true;
    }
}
//---------------------------------------------------------------------------





void __fastcall TForm2::Forward1Click(TObject *Sender)
{
    loading = true;
    if(!SelectPage(currpage+1)) Application->MessageBoxA("Buffer is not ready, please retry", "Temporary error", MB_OK);
    else currpage++;

    loading = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::PreviousPage1Click(TObject *Sender)
{
    loading = true;
    if(!SelectPage(currpage-1)) Application->MessageBoxA("Buffer is not ready, please retry", "Temporary error", MB_OK);
    else currpage--;
    loading = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Home1Click(TObject *Sender)
{
    loading = true;
    if(!SelectPage(1)) Application->MessageBoxA("Buffer is not ready, please retry", "Temporary error", MB_OK);
    else currpage=1;
    loading = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::LastPage1Click(TObject *Sender)
{
    loading = true;
    if(!SelectPage(idx[idxmax-1].page_stop)) Application->MessageBoxA("Buffer is not ready, please retry", "Temporary error", MB_OK);
    else currpage=idx[idxmax-1].page_stop;
    loading = false;
}
//---------------------------------------------------------------------------



void __fastcall TForm2::Gotopage1Click(TObject *Sender)
{
    AnsiString selpage;
    int tpage;
    if(InputQuery("Select page", "Jump to page", selpage)) {
        tpage = selpage.ToIntDef(currpage);
        loading = true;
        if(!SelectPage(tpage)) Application->MessageBoxA("Buffer is not ready, please retry", "Temporary error", MB_OK);
        else currpage=tpage;
        loading = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm2::FormResize(TObject *Sender)
{
    SetLayout();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::test1Click(TObject *Sender)
{
    ProgressBar1->StepIt();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::StatusBar1DrawPanel(TStatusBar *StatusBar,
      TStatusPanel *Panel, const TRect &Rect)
{
  if (Panel == StatusBar->Panels->Items[4]) {
    ProgressBar1->Top = Rect.Top;
    ProgressBar1->Left = Rect.Left;
    ProgressBar1->Width = Rect.Right - Rect.Left - 15;
    ProgressBar1->Height = Rect.Bottom - Rect.Top;
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm2::About1Click(TObject *Sender)
{
    AboutBox->ShowModal();    
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Exit1Click(TObject *Sender)
{
    Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::DoDisplay(const char *line) {
    TTextAttributes *ta = Display->SelAttributes;
    AnsiString aline = line;//+4;
    CHARFORMAT cf;
    switch(line[2]) {
        case '0' :
            switch(line[3]) {
                case '0' : ta->Size=1; break;
                case '1' :  ta->Color = clBlack;
                            ta->Size = 9;
                            ta->Style = ta->Style << fsBold << fsItalic;
                            break;
                case '2' :  ta->Color = clNavy;
                            ta->Size = 8;
                            break;
                case '7' :
                case '3' :  ta->Color = clBlue;
                            ta->Size = 8;
                            ta->Pitch = fpVariable;
                            break;
                case '4' :  ta->Color = clTeal;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsItalic;
                            break;
                case '5' :  ta->Color = clBlack;
                            ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            break;
                case '9' :
                case '6' :
                            ta->Color = clBlack;
                            ta->Size = 8;
                            break;

/*                case '7' :  ta->Color = clNavy;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsItalic;
                            break;
  */
                case '8' :  ta->Color = clMaroon;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsItalic;
                            break;

            }
            break;

        case '1' :
            switch(line[3]) {
                case '0' : ta->Size=1; break;
                case '1' :  ta->Color = clBlack;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '2' :  ta->Color = clNavy;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '3' :  ta->Color = clBlue;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '4' :  ta->Color = clTeal;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsItalic;
                            break;
                case '5' :  ta->Color = clNavy;
                            ta->Size=8;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '6' : ta->Color = clNavy;
                            ta->Style = ta->Style << fsBold << fsUnderline;
                            ta->Size=8;
                            break;

                case '9' :
                            ta->Style = ta->Style << fsBold;
                            ta->Color = clPurple;
                            ta->Size = 8;
                            break;
                case '7' :  ta->Color = clBlack;
                            ta->Size = 8;
                            break;
                case '8' : ta->Size=9;
                            ta->Color = clBlack;
                            ta->Style = ta->Style << fsBold;
                            break;
            }
            break;

        case '2' :
            switch(line[3]) {
                case '1' :  ta->Color = clBlack;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '2' :  ta->Color = clNavy;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '3' :  ta->Color = clBlue;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '4' :  ta->Color = clTeal;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsItalic;
                            break;
                case '5' :  ta->Color = clNavy;
                            ta->Size=9;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '6' :   ta->Color=clGray;
                            break;
                case '0' :  ta->Color = clNavy;
                            ta->Style = ta->Style << fsBold << fsUnderline;
                            ta->Size=11;

                case '9' :  ta->Color = clBlack;
                            ta->Size = 8;
                            break;
                case '7' :  ta->Color = clBlack;
                            ta->Size = 8;
                            break;
                case '8' : ta->Size=8;
                            ta->Color = clBlack;
//                            ta->Style = ta->Style << fsBold;
                            break;
            }
            break;
        case '3' :
            switch(line[3]) {
                case '0' : ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            ta->Color = clBlack;
                            break;
                case '1' :  ta->Color = clGray;
                            ta->Style = ta->Style << fsItalic;
                            ta->Size = 8;
                            break;
                case '2' :  ta->Color = clTeal;
                            ta->Size = 8;
                            break;
                case '3' :  ta->Color = clBlue;
                            ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            break;
                case '4' :  ta->Color = clTeal;
                            ta->Style = ta->Style << fsItalic;
                            ta->Size = 8;
                            break;
                case '5' :  ta->Color = clGray;
                            ta->Size=8;
                            break;
                case '6' :
                case '9' :
                            ta->Size = 8;
                            ta->Color = clBlack;
                            break;
                case '7' :  ta->Color = clNavy;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsItalic;
                            break;

                case '8' : ta->Size=9;
                            ta->Color = clBlack;
                            ta->Style = ta->Style << fsBold;
                            break;
            }
            break;
        case '4' :
            switch(line[3]) {
                case '0' : ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            ta->Color = clMaroon;
                            break;
                case '1' :  ta->Color = clGray;
                            ta->Style = ta->Style << fsItalic;
                            ta->Size = 8;
                            break;
                case '2' :  ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            ta->Color = clBlack;
                            break;
                case '3' :  ta->Color = clBlue;
                            ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            break;
                case '4' :  ta->Color = clTeal;
                            ta->Style = ta->Style << fsItalic;
                            ta->Size = 8;
                            break;
                case '5' :  ta->Color = clNavy;
                            ta->Size=9;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '6' :
                case '9' :
                            ta->Size = 8;
                            ta->Color = clBlack;
                            break;
                case '7' :  ta->Color = clNavy;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsItalic;
                            break;

                case '8' : ta->Size=9;
                            ta->Color = clBlack;
                            ta->Style = ta->Style << fsBold;
                            break;
            }
            break;

        case '5' :
        default :
            switch(line[3]) {
                case '0' : ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            ta->Color = clBlack;
                            break;
                case '1' :  ta->Color = clGray;
                            ta->Style = ta->Style << fsItalic;
                            ta->Size = 8;
                            break;
                case '2' :  ta->Color = clNavy;
                            ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            break;
                case '3' :  ta->Color = clBlue;
                            ta->Style = ta->Style << fsBold;
                            ta->Size = 8;
                            break;
                case '4' :  ta->Color = clTeal;
                            ta->Style = ta->Style << fsItalic;
                            ta->Size = 8;
                            break;
                case '5' :  ta->Color = clNavy;
                            ta->Size=9;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '6' :
                            ta->Size = 8;
                            ta->Color = clBlack;
                            break;
                case '7' :  ta->Color = clNavy;
                            ta->Size = 8;
                            ta->Style = ta->Style << fsBold;
                            break;

                case '8' : ta->Size=9;
                            ta->Color = clBlack;
                            ta->Style = ta->Style << fsBold;
                            break;
                case '9' : ta->Size=8;
                            ta->Color = clGray;
                            break;

            }
            break;

    }

    Display->SelAttributes = ta;
    Display->Lines->Add(line);

}


void __fastcall TForm2::ListAll1Click(TObject *Sender)
{
    Form5->QuickRep1->Preview();

}
//---------------------------------------------------------------------------

void __fastcall TForm2::GroupList1Click(TObject *Sender)
{
    Form6->QuickRep1->Preview();

}
//---------------------------------------------------------------------------

void __fastcall TForm2::CallDetail1Click(TObject *Sender)
{
        Form7->QuickRep1->Preview();
}
//---------------------------------------------------------------------------

