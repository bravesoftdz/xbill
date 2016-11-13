//---------------------------------------------------------------------------
#include <stdio.h>
#//include <SysUtils.h>
#include <vcl.h>
#pragma hdrstop

#include "dbcreate.h"
#include <systdate.h>
#include <time.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)
enum TDateTimeFlag {Date, Time, DateTime};


void CreateDatabase(AnsiString datafile, AnsiString dest, AnsiString rawfilename, TTable * SummaryTable) {

    ThousandSeparator  = ',';
    DecimalSeparator  = '.';
    CurrencyString = "RM ";
    CurrencyFormat = 0;
    DateSeparator = '/';
    ShortDateFormat = "mm/dd/yyyy";

    AnsiString dbDetailTableName = ChangeFileExt(rawfilename, "_cdt");
    AnsiString dbSummaryTableName = ChangeFileExt(rawfilename, "_sum");


   TReplaceFlags rf;
    rf << rfReplaceAll << rfIgnoreCase;


//    AnsiString dbPassword = ChangeFileExt(ExtractFileName(OpenDialog1->FileName), ".TMRND");

    TTable *DetailTable = new TTable(0);
    DetailTable->Active = false;
    DetailTable->Exclusive = true;
    DetailTable->DatabaseName = dest;
    DetailTable->TableName = dbDetailTableName;
    DetailTable->TableType = ttParadox;
    DetailTable->FieldDefs->Clear();

    TFieldDef *NewField = DetailTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "line_type";

    NewField = DetailTable->FieldDefs->AddFieldDef(); // define second field
    NewField->DataType = ftString;
    NewField->Name = "line_num";

    NewField = DetailTable->FieldDefs->AddFieldDef(); // define second field
    NewField->DataType = ftString;
    NewField->Name = "call_type";

    NewField = DetailTable->FieldDefs->AddFieldDef(); // define second field
    NewField->DataType = ftDate;
//    NewField->DataType = ftString;
    NewField->Name = "call_date";

    NewField = DetailTable->FieldDefs->AddFieldDef(); // define second field
    NewField->DataType = ftTime;
//    NewField->DataType = ftString;
    NewField->Name = "call_time";

    NewField = DetailTable->FieldDefs->AddFieldDef(); // define second field
    NewField->DataType = ftString;
    NewField->Name = "call_num";

    NewField = DetailTable->FieldDefs->AddFieldDef(); // define second field
    NewField->DataType = ftString;
    NewField->Name = "call_area";

    NewField = DetailTable->FieldDefs->AddFieldDef(); // define second field
//    NewField->DataType = ftInteger;
    NewField->DataType = ftTime;
    NewField->Name =  "call_duration";

    NewField = DetailTable->FieldDefs->AddFieldDef(); // define second field
    NewField->DataType = ftCurrency; //ftString;
    NewField->Name = "call_charge";

    DetailTable->IndexDefs->Clear();

#ifdef BUILD_INDEX_ALONG_TABLE
    TIndexDef *NewIndex = DetailTable->IndexDefs->AddIndexDef(); // add an index
    NewIndex->Name = "Call_Number";
    NewIndex->Fields = "call_num";

    NewIndex = DetailTable->IndexDefs->AddIndexDef(); // add an index
    NewIndex->Name = "Line_Number";
    NewIndex->Fields = "line_num";
#endif


    DetailTable->CreateTable(); // create the table
#ifdef AUTONOMOUS_SUMMARY
    TTable *SummaryTable = new TTable(0);
    SummaryTable->Active = false;
    SummaryTable->Exclusive = true;
    SummaryTable->DatabaseName = dest;
    SummaryTable->TableName = dbSummaryTableName;
    SummaryTable->TableType = ttParadox;
    SummaryTable->FieldDefs->Clear();

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "AccountNumber";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "RefNumber";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "BillDate";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "SummaryLabel";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "LeftValue";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "RightValue";
    NewField->Size=30;
    if(!SummaryTable->Exists) {
        SummaryTable->CreateTable();
    } else {
        TQuery *q = new TQuery(0);
        q->DatabaseName = SummaryTable->DatabaseName;
        q->SQL->Clear();
        q->SQL->Add("drop index "+dbSummaryTableName+".iacnum");
        try {
            q->ExecSQL();
        } catch(...) {
        }
        q->SQL->Clear();
        q->SQL->Add("drop index "+dbSummaryTableName+".irefnum");
        try {
            q->ExecSQL();
        } catch(...) {
        }
        q->SQL->Clear();
        q->SQL->Add("drop index "+dbSummaryTableName+".isummary");
        try {
            q->ExecSQL();
        } catch(...) {
        }
        delete q;

    }
    SummaryTable->Exclusive=false;
    SummaryTable->Open();
#endif
// database initialization process end

// database : building query start
    AnsiString squery;
    DetailTable->Exclusive=false;
    DetailTable->Open();

// database : building query end

    FILE *f  = fopen(datafile.c_str(), "rb");
    char c;
    const int LINE_SIZE = 500, BUFF_SIZE= LINE_SIZE*80;
    int rd, r, j=0, p=0, z=0, detflag,ct=0, xc, linenum=0;
    Currency crdetcost;
    int idetdur=0, idetdt, idettm;
    bool acnum_set = false;
    //3502 06 2006 14:51:54 0046523297       SINGAPURA  00:00:35        1.15
    AnsiString svctype, svcnum, dettype, tmp35, tmp0,
                detdt, dettm, detnb, detdst,detdur,detcost, mline,
                acnum, refnum, det, vall, valr, bdate;
    TDateTime tdt, ttdt;
    tdt.FormatString("dd/mm/yyyy hh:nn:ss");
    TDateTimeFlag tdtf = ::Time;
    unsigned short hr,mn,sec, msec, yr, mt, dy;
    struct tm time_check; 

    char *line = new char[LINE_SIZE], *buff = new char[BUFF_SIZE];

//    // StatusBar->Panels->Item[0].Text="Building database";
     while(!feof(f)) {
        rd = fread(buff, 1, BUFF_SIZE, f);
//        if(ct>30000) break;

        for(r = 0;r<rd;r++) {
            if((line[j] = buff[r])=='\n') {
                line[j-1]=0;
                linenum++;
                p=j;
                //ProcessLine(line);
                j=-1;
                if(line[0]=='1') {
                    j++;
                    continue;
                }


                switch(line[2]) {
                    case '0' :
                        switch(line[3])   {
                            case '4' :
                                    if(acnum_set) continue;
                                    acnum_set=true;
                                    tmp0 = line;
                                    refnum= tmp0.SubString(5,24).Trim();
                                    acnum = tmp0.SubString(29, 16).Trim();
                                    bdate = tmp0.SubString(45,14).Trim();
                                    break;
                            case '6' :
                                    tmp0 = line;
                                    det = tmp0.SubString(6,26).Trim();
                                    vall = tmp0.SubString(32, 18).Trim();
                                    if(vall.Length()>0 && vall[vall.Length()]=='-') {
                                        vall = "-"+vall;
                                        vall=vall.SubString(0,vall.Length()-1);
                                    }
                                    valr = tmp0.SubString(50, 18).Trim();
                                    if(valr.Length()>0 && valr[valr.Length()]=='-') {
                                        valr = "-"+valr;
                                        valr=valr.SubString(0,valr.Length()-1);
                                    }
                                    if(det!="" && (valr!="" || vall!="")) {
                                          SummaryTable->AppendRecord(ARRAYOFCONST((acnum, refnum, bdate, det, vall, valr)));
                                    }
                                    break;

                        }


                        break;
                    case '1' :
                        switch(line[3]) {
                            case '9' : detflag = 1;
                                        break;
                            case '8' :
                                    tmp0 = line;
                                    det = tmp0.SubString(6,26).Trim();
                                    vall = tmp0.SubString(32, 18).Trim();
                                    if(vall.Length()>0 && vall[vall.Length()]=='-') {
                                        vall = "-"+vall;
                                        vall=vall.SubString(0,vall.Length()-1);
                                    }
                                    valr = tmp0.SubString(49, 18).Trim();
                                    if(valr.Length()>0 && valr[valr.Length()]=='-') {
                                        valr = "-"+valr;
                                        valr=valr.SubString(0,valr.Length()-1);
                                    }
                                    if(det!="" && (valr!="" || vall!="")) {
                                          SummaryTable->AppendRecord(ARRAYOFCONST((acnum, refnum, bdate, det, vall,valr)));
                                    }
                                    break;
                        }

                        break;
                    case '2' :
                        switch(line[3]){
                            case '1' :
                                dettype=line+4;
                                dettype=dettype.Trim();
                                break;
                            case '2' : // detail header .. discard
                            case '3' : // total .. discard
                                break;
                        }
                        break;
                    case '3' :
                        switch(line[3]) {
                            case '5' : // extract me
                                mline += EscapeString(dettype)+"\",\"";
                                tmp35 = line+4;
                                mline += detdt = EscapeString(tmp35.SubString(0,10));
                                yr = detdt.SubString(7,4).ToInt();
                                mt = detdt.SubString(4,2).ToInt();
                                dy = detdt.SubString(0,2).ToInt();
                                AnsiString xx(detdt.SubString(4,2)+"/"+detdt.SubString(0,2)+"/"+detdt.SubString(7,4));

                                mline+="\",\"";
                                mline += dettm = EscapeString(tmp35.SubString(12,8));
/*                                hr = dettm.SubString(0,2).ToInt();
                                mn = dettm.SubString(4,2).ToInt();
                                sec = dettm.SubString(7,2).ToInt();

                                AnsiString yy(dettm.SubString(0,2)+":"+dettm.SubString(4,2)+":"+dettm.SubString(7,2));

                                time_check.tm_year = yr - 1900;
                                time_check.tm_mon  = mt - 1;
                                time_check.tm_mday = dy;
                                time_check.tm_hour = hr;
                                time_check.tm_min  = mn;
                                time_check.tm_sec  = sec;
                                time_check.tm_isdst = 0;
                                idettm = idetdt = mktime(&time_check);
  */
                                mline+="\",\"";
                                mline += detnb = EscapeString(tmp35.SubString(21, 17).Trim());
                                mline+="\",\"";
                                mline += detdst = EscapeString(tmp35.SubString(38, 10).Trim());
                                mline+="\",\"";
                                mline += detdur = EscapeString(tmp35.SubString(48, 9).Trim());
                                mline+="\",\"";
//                                tdt = AnsiString(TDateTime.CurrentDate());
//                               AnsiString astmp = AnsiString(tdt.CurrentDate());
  /*                              tdt = TDateTime(AnsiString(tdt.CurrentDate())+" " +detdur);//, tdtf);
                                tdt.DecodeTime(&hr, &mn, &sec, &msec);
                                idetdur = hr*3600+mn*60+sec;
    */
                                mline += detcost = EscapeString(tmp35.SubString(59, 12).Trim());
                                crdetcost = Currency(detcost);
                                mline+="\"";
//                                squery = "insert into calldetail values("+mline+");";
                                //Memo1->Lines->Add(squery);
//                                aquery->SQL->Add(squery);
                                xc=0;

                                do {
                                    try {
//                                        aquery->ExecSQL();
                                          DetailTable->AppendRecord(ARRAYOFCONST((svctype, svcnum, dettype, xx, dettm, detnb, detdst, detdur, crdetcost)));
                                        xc=0;
                                    } catch(...){
                                        xc++;
                                        // StatusBar->Panels->Item[0].Text = "Problem while inserting, retry soon";
                                        Sleep(25);
                                    }
                                }while(xc!=0 && xc<500);
                                if(xc>=500)
                                    // StatusBar->SimpleText = "Insert Error";

//                                aquery->SQL->Clear();

                                if((ct%10000)==0) {
                                    // StatusBar->Panels->Item[0].Text = // StatusBar->Panels->Item[0].Text + ".";
                                    Beep(2400,500);
                                }
                                mline = "\""+svctype+"\",\""+svcnum+"\",\"";
                                ct++;
                                break;
                        }
                        break;
                    case '4' :
                        switch (line[3]) {
                            case '0': // find and separate
                                svctype="";
                                svcnum="";
                                dettype="";
                                mline = "";
                                detflag=0;
                                z=4;
                                while(line[z]!=':') svctype+=line[z++];
                                svctype=EscapeString(svctype.Trim());
                                svcnum = line+z+1;
                                svcnum = EscapeString(svcnum.Trim());
                                mline = "\""+svctype+"\",\""+svcnum+"\",\"";
                                break;
                        }
                        break;

                }
            }
            j++;
        }
    }
    fclose(f);

    DetailTable->Close();


#ifndef AUTONOMOUS_SUMMARY
   TQuery *aquery = new TQuery(0);
   aquery->DatabaseName = dest;
   aquery->SQL->Add("Create Index icall_number on "+dbDetailTableName+" call_num;");
   aquery->ExecSQL();
   aquery->SQL->Clear();

   aquery->SQL->Add("Create Index iline_number on "+dbDetailTableName+" line_num;");
   aquery->ExecSQL();
   aquery->SQL->Clear();
   delete aquery;

#else
   TQuery *aquery = new TQuery(0);
   aquery->DatabaseName = dest;
   aquery->SQL->Add("Create Index icall_number on "+dbDetailTableName+" call_num;");
   aquery->ExecSQL();
   aquery->SQL->Clear();

   aquery->SQL->Add("Create Index iline_number on "+dbDetailTableName+" line_num;");
   aquery->ExecSQL();
   aquery->SQL->Clear();

   aquery->SQL->Add("Create Index iacnum on "+dbSummaryTableName+" AccountNumber;");
   aquery->ExecSQL();
   aquery->SQL->Clear();

   aquery->SQL->Add("Create Index irefnum on "+dbSummaryTableName+" RefNumber;");
   aquery->ExecSQL();
   aquery->SQL->Clear();

   aquery->SQL->Add("Create Index isummary on "+dbSummaryTableName+" SummaryLabel;");
   aquery->ExecSQL();
   aquery->SQL->Clear();
#endif
#ifdef EMBED_PASSWORD
    DetailTable->Exclusive = true;
    DetailTable->Open();
    AddMasterPassword(DetailTable, dbPassword);
    DetailTable->Close();
#endif

    delete DetailTable;
#ifdef AUTONOMOUS_SUMMARY
   delete SummaryTable;
#endif
    delete[] line;
    delete[] buff;
    Beep();
    // StatusBar->Panel->Items[0]->Text = "Database Built ...";
}
//---------------------------------------------------------------------------


void __fastcall ProcessLine(AnsiString line)
{

}

AnsiString __fastcall EscapeString(AnsiString str)
{
    AnsiString res=str,tmp;
    int p=0;
    TReplaceFlags rf;
    rf << rfReplaceAll << rfIgnoreCase;

    res=StringReplace(res, "\"", "\\\"", rf);
    res=StringReplace(res, "\\", "\\\\", rf);
    if((p=res.Pos("'"))!=0) {
        tmp=res.SubString(0,p-1);
        tmp+="'";
        tmp+=res.SubString(p, res.Length()-p+1);

        res = tmp;
    }
//    res=StringReplace(str, "'", "\'", rf);

    return res;

}
