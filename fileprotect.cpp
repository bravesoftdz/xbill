//---------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#include <vcl.h>

#pragma hdrstop

#include "fileprotect.h"
#include "test.h"
#include "jumble.h"
#include "analyzer.h"
#include "dbcreate.h"
#include <bzlib.h>
#pragma package(smart_init)



//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TFileProtect::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TFileProtectDo::TFileProtectDo(bool CreateSuspended, AnsiString src, AnsiString dest, CStringProtect &csp, TProgressBar *pb, TStatusPanel *sp, bool *flag, TTable *SummaryTable)
    : TThread(CreateSuspended)
{
    Priority = tpNormal;
    infile = src;
//    outfile = dest;
    ioset = true;
    int len;
    ifstream is(src.c_str());
    is.seekg (0, ios::end);
    len = is.tellg();
    is.close();
    ProgressBar = pb;
    StatusPanel = sp;
    if(ProgressBar!=NULL) {
        ProgressBar->Position=0;
        ProgressBar->Min=0;
        ProgressBar->Max = len;
        ProgressBar->Step=IN_BUFFER_SIZE;
    }
    this->FreeOnTerminate=true;
    this->csp = csp;
    AnsiString rawfilename = ExtractFileName(src);
    codedfilename = IncludeTrailingPathDelimiter(dest)+ChangeFileExt(rawfilename, ".tm");
    indexfilename = IncludeTrailingPathDelimiter(dest)+ChangeFileExt(rawfilename, ".idx");
    configfilename = IncludeTrailingPathDelimiter(dest)+ChangeFileExt(rawfilename, ".cfg");
    this->dest = dest;
    this->rawfilename = rawfilename;
    this->flag = flag;
    this->SummaryTable = SummaryTable;


//*-*-*--*-*-*
}
//---------------------------------------------------------------------------
void __fastcall TFileProtectDo::UpdateProgress()
{
    if(ProgressBar!=NULL) ProgressBar->StepBy(currstep);
}

void __fastcall TFileProtectDo::UpdateStatus(){
    if(StatusPanel!=NULL) StatusPanel->Text = status;
}

void __fastcall TFileProtectDo::Execute()
{
    //---- Place thread code here ----
//    CStringProtect csp;
    status="Encoding file";
    Synchronize(UpdateStatus);
/*    ifstream is("jumble.cfg", ios::binary);
    is.seekg(0, ios::end);
    unsigned int len = is.tellg();
    is.seekg(0,ios::beg);
    char *bcfg = new char[len];
    unsigned int len2=20*len;
    char *cfg= new char[len2];
    is.read(bcfg, len);
    is.close();
    BZ2_bzBuffToBuffDecompress(cfg, &len2, bcfg, len,0,0);
    stringstream ss;
    ss.write(cfg, len2);
    ss >> csp;
    delete[] bcfg;
    delete[] cfg;  */

    status="Saving configuration file.";
    Synchronize(UpdateStatus);
    if(!SaveConfig(csp, configfilename.c_str())) {
        status="Error: Cannot save configuration file";
        Synchronize(UpdateStatus);
        Terminate();
        *flag = true;
        return;
    }


    // init done ...
    fileprotect bs;

    const int sobzsize = sizeof(fileprotect);

    char buff_in[IN_BUFFER_SIZE], buff_out[OUT_BUFFER_SIZE],  sb[sobzsize];

//    ifstream is(infile.c_str(), ios::binary);
    status="Opening source file.";
    Synchronize(UpdateStatus);

    FILE *f = fopen(infile.c_str(), "rb");

    if(!f) {
        status="Error: Cannot open source file";
        Synchronize(UpdateStatus);
        Terminate();
        *flag = true;
        return;
    }
            // get length of file:
    fseek(f, 0, SEEK_END);
    int length = ftell(f),rd=0, ret, insize;
    unsigned int outsize;
    fseek(f,0, SEEK_SET);
    status="Creating coded file.";
    Synchronize(UpdateStatus);

    ofstream os(codedfilename.c_str(), ios::binary);
    if(!os.is_open() || os.bad()){
        status="Error: Cannot create target file";
        Synchronize(UpdateStatus);
        Terminate();
        *flag = true;
        return;
    }

    char *bestr, *bdstr;
    int bestrlen, bdstrlen;
    int ip, op, lp;



    int readsize;
    readsize = IN_BUFFER_SIZE>length ? length : IN_BUFFER_SIZE ;
    int nb_pages=0;
    start_page = 0;
    stop_page=0;
    char lastchar=0xa;
    bool newpage;
    const int pageindexmax = 2000;
    int pageindex[pageindexmax];
    int pageindexlen = pageindexmax;

    int nextptr = 0;
    fileindex fidx;
//    char index_filename[256];
  //  sprintf(index_filename, "%s.idx", infile.c_str());
    status="Creating index file.";
    Synchronize(UpdateStatus);

    FILE *fo = fopen(indexfilename.c_str(), "wb");
    if(!fo) {
        status="Error: Cannot create index file";
        Synchronize(UpdateStatus);
        Terminate();
        *flag = true;
        return;
    }

    status="Processing source file.";
    Synchronize(UpdateStatus);
    while(!feof(f)){
        insize = fread(buff_in, 1, IN_BUFFER_SIZE, f);
        rd+=IN_BUFFER_SIZE;
//        insize = rd <length ? IN_BUFFER_SIZE : length-(length/IN_BUFFER_SIZE)*IN_BUFFER_SIZE;
//        insize = is.gcount();
        if(insize<=0) {
                status="Error: Problems reading source file.";
                Synchronize(UpdateStatus);
                fclose(f);
                Terminate();
                *flag = true;
                return;
        }

        pageindexlen = pageindexmax;
        if(!analyze_packet(buff_in, insize, &lastchar, &nb_pages, &newpage, pageindex, &pageindexlen)) {
                status="Error: Problem while analyzing source file.";
                Synchronize(UpdateStatus);
                fclose(f);
                Terminate();
                *flag = true;
                return;
        }
        stop_page=start_page+nb_pages;
        if(newpage) start_page++;
        fidx.page_start = start_page;
        fidx.page_stop = stop_page;
        start_page = stop_page;

        outsize=OUT_BUFFER_SIZE;

        status="Compressing data chunk from source file.";
        Synchronize(UpdateStatus);

        ret = BZ2_bzBuffToBuffCompress(buff_out, &outsize, buff_in, insize, 1, 0, 30);
        status="Compressing done.";
        Synchronize(UpdateStatus);
        switch (ret) {
            case BZ_OK : bs.in = insize; bs.bzout = outsize;
                        status="Applying protection to data.";
                        Synchronize(UpdateStatus);
                         csp.Apply(&bestr, &bestrlen, buff_out, outsize, &ip, &op, &lp);
                        status="Protection applied.";
                        Synchronize(UpdateStatus);
                         bs.ip = ip;
                         bs.op = op;
                         bs.lp = lp;
                         bs.encout = bestrlen;
                         memcpy(sb, &bs, sobzsize);
                        status="Writing data chunk to main file.";
                        Synchronize(UpdateStatus);
                         os.write(sb,sobzsize);
                         os.write(bestr, bestrlen);
                         status="Done writing data chunk to main file.";
                         fidx.target = nextptr;
                         status="Writing index file.";
                         Synchronize(UpdateStatus);
                         ret = fwrite(&fidx, sizeof(fileindex), 1, fo);
                         if(ret != 1) {
                            status="Error: Problem while populating index file.";
                            Synchronize(UpdateStatus);
                            Terminate();
                            os.close();
                            fclose(f);
                            fclose(fo);
                            *flag = true;
                            return;
                         }
                         ret = fwrite(&pageindexlen, sizeof(int), 1, fo);
                         if(ret != 1) {
                            status="Error: Problem while populating index file.";
                            Synchronize(UpdateStatus);
                            Terminate();
                            os.close();
                            fclose(f);
                            fclose(fo);
                            *flag = true;
                            return;
                         }
                         ret = fwrite(pageindex, sizeof(int), pageindexlen, fo);
                         if(ret != pageindexlen) {
                            status="Error: Problem while populating index file.";
                            Synchronize(UpdateStatus);
                            Terminate();
                            os.close();
                            fclose(f);
                            fclose(fo);
                            *flag = true;
                            return;
                         }
                         status="Done writing to index file.";
                         Synchronize(UpdateStatus);
                         nextptr += sobzsize + bestrlen;
                         delete[] bestr;
                         currstep = insize;
                         Synchronize(UpdateProgress);
                         break;
            case BZ_OUTBUFF_FULL : break;
            case BZ_PARAM_ERROR :

                                status="Panic: Please contact TMR&D - Errno(1).";
                                Synchronize(UpdateStatus);
                                  fclose(f);
                                  os.close();
                                  fclose(fo);
                                 *flag = true;
                                  return;
            case BZ_MEM_ERROR :
                                status="Panic: Please contact TMR&D - Errno(2).";
                                Synchronize(UpdateStatus);
                                fclose(f);
                                os.close();
                                  fclose(fo);
                                *flag = true;
                                return;;
        }
        ftell(f);
    }
//    Application->MessageBoxA("Encoding successful", "Done", MB_OK);
    status="Encoding successful";
    Synchronize(UpdateStatus);
    fclose(f);
    fclose(fo);

    os.close();
    status="Building database";
    Synchronize(UpdateStatus);

    CreateDatabase(infile, dest, rawfilename, SummaryTable);
    status="Building database done !";
    Synchronize(UpdateStatus);

    ioset = false;
    *flag = true;

}
//---------------------------------------------------------------------------

__fastcall TFileProtectRecover::TFileProtectRecover(bool CreateSuspended, AnsiString src, AnsiString dest, TProgressBar *pb, TButton *bt)
    : TThread(CreateSuspended)
{
    Priority = tpNormal;
    infile = src;
    outfile = dest;
    ioset = true;
    int len;
    ifstream is(src.c_str());
    is.seekg (0, ios::end);
    len = is.tellg();
    is.close();
    Button = bt;
    ProgressBar = pb;
    ProgressBar->Position=0;
    ProgressBar->Min=0;
    ProgressBar->Max = len;
    ProgressBar->Step=IN_BUFFER_SIZE;
    ProgressBar->Smooth = true;
    SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clDefault);
    this->FreeOnTerminate=true;
}
//---------------------------------------------------------------------------
void __fastcall TFileProtectRecover::UpdateProgress()
{
    ProgressBar->StepBy(currstep);
}
void __fastcall TFileProtectRecover::Execute()
{
    //---- Place thread code here ----
    CStringProtect csp;
    Button->Enabled = false;

    if(!LoadConfig(csp, "jumble.cfg")){
        Application->MessageBoxA("Configuration file cannot be loaded", "Unrecoverable error !", MB_OK);
        Terminate();
        Button->Enabled = true;
        return;
    }

    // init done ...
    fileprotect bs;

    const int sobzsize = sizeof(fileprotect);

    char *enc_in, *bz_in,  sb[sobzsize];

    FILE *f = fopen(infile.c_str(), "rb");
            // get length of file:
    fseek(f, 0, SEEK_END);
    int length = ftell(f),rd=0, ret, cnt=0;
    unsigned int outsize, insize;
    fseek(f, 0, SEEK_SET);

    ofstream os(outfile.c_str(), ios::binary);

    char *bestr, *bdstr, *buff_in;
    int bestrlen, bdstrlen;
    int ip, op, bz_inlen, rs;


    while(!feof(f) && cnt<length){
        cnt+= rs = fread(sb, 1, sobzsize, f);
        if(rs!=sobzsize){
            Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
            SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);
        }
        memcpy(&bs, sb, sobzsize);
        enc_in = new char[bs.encout];
        cnt+= outsize = fread(enc_in, 1, bs.encout, f);
        if(bs.encout!=outsize) {
            Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
            SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);
        }

        csp.Extract(&bz_in, &bz_inlen, enc_in, bs.encout, bs.ip, bs.op, bs.lp);

        if(bz_inlen != bs.bzout) {
            Application->MessageBoxA("Saiz tak betul", "Alamak !", MB_OK);
            SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);
        }

        delete[] enc_in;
        buff_in = new char[bs.in];
        insize = bs.in;
        ret = BZ2_bzBuffToBuffDecompress(buff_in, &insize, bz_in, bz_inlen, 0, 0);
        switch (ret) {
            case BZ_OK : os.write(buff_in, insize);
                        currstep = sobzsize+outsize;
                         Synchronize(UpdateProgress);
                         break;
            case BZ_OUTBUFF_FULL : Application->MessageBoxA("Buffer tak cukup", "Alamak !", MB_OK);
                                  fclose(f);
                                  os.close();
                                  Button->Enabled = true;
                                  SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);

                                   return;
            case BZ_UNEXPECTED_EOF : Application->MessageBoxA("Fail tak cukup sifat", "Alamak !", MB_OK);
                                  fclose(f);
                                  os.close();
                                  Button->Enabled = true;
                                  SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);
                                   return;
            case BZ_DATA_ERROR_MAGIC : Application->MessageBoxA("Format data tak betul", "Alamak !", MB_OK);
                                  fclose(f);
                                  os.close();
                                  Button->Enabled = true;
                                  SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);
                                   return;
            case BZ_DATA_ERROR : Application->MessageBoxA("Data corrupted", "Aieee !", MB_OK);
                                  fclose(f);
                                  os.close();
                                  Button->Enabled = true;
                                  SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);
                                   return;

            case BZ_PARAM_ERROR : Application->MessageBoxA("Parameter bermasalah", "Salah tu bang aji", MB_OK);
                                  fclose(f);
                                  os.close();
                                  Button->Enabled = true;
                                  SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);
                                  return;
            case BZ_MEM_ERROR : Application->MessageBoxA("Memori tak cukup lah pulak", "Alamak !", MB_OK);
                                fclose(f);
                                os.close();
                                Button->Enabled = true;
                                SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0, clRed);
                                return;;

        }
        delete[] buff_in;
        delete[] bz_in;
        ftell(f);
    }

    fclose(f);
    os.close();
    ioset = false;
    Button->Enabled = true;
}
//---------------------------------------------------------------------------



