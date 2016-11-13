//---------------------------------------------------------------------------


#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include <vcl.h>

#include <tools/all.h>
#include <tools/interleaver.h>
#include<bzlib.h>

#pragma hdrstop

#include "jumble.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)



ostream& operator<<(ostream &os,  CStringProtect &csp) {
    os <<  csp.inner_interleaver << csp.outer_interleaver << " " << csp.ib << " " << csp.ob << " " << csp.ipad << " " << csp.opad<< " " << csp.ldpc_M << " " << csp.ldpc_N << " " <<csp.ldpc;
    return os;
}

istream& operator>>(istream &is,  CStringProtect &csp) {
    is >>  csp.inner_interleaver >> csp.outer_interleaver  >> csp.ib  >> csp.ob  >> csp.ipad  >> csp.opad >> csp.ldpc_M  >> csp.ldpc_N  >>csp.ldpc;
    return is;
}

int SaveConfig(CStringProtect &csp, AnsiString filename) {
    stringstream ss;
    ss << csp;
    ss.seekg(0, ios::end);
    unsigned int len = ss.tellg();
    unsigned int len2 = int(len*101.0/100.0)+700;
    ss.seekg(0,ios::beg);

    char *buff = new char[len], *dest = new char[len2];
    ss.read(buff, len);
    BZ2_bzBuffToBuffCompress(dest, &len2, buff, len, 9,0,30);
    ofstream os(filename.c_str(), ios::binary);
    if(!os.is_open()) return 0;
    os.write(dest, len2);
    os.close();
    delete[] dest;
    delete[] buff;
    return 1;
}

int LoadConfig(CStringProtect &csp, AnsiString filename){
    FILE *f = fopen(filename.c_str(), "rb");

    if(!f) {
        return 0;
    }
//    is.seekg(0, ios::end);
    fseek(f,0,SEEK_END);
    int len = ftell(f); //is.tellg();
    fseek(f,0, SEEK_SET);
  //  is.seekg(0,ios::beg);

    char *buff = new char[len];
    unsigned int len2=20*len;
    char *dest= new char[len2];
    int rl=fread(buff, 1, len, f);
    fclose(f);
    if(rl!=len) {
//        Application->MessageBoxA("Error reading configuration file", "Unrecoverable Error", MB_OK);
  //      Terminate();
        return 0;
    }
//    is.close();
    BZ2_bzBuffToBuffDecompress(dest, &len2, buff, len,0,0);
    stringstream ss;
    ss.write(dest, len2);
    ss >> csp;
    delete[] dest;
    delete[] buff;
    return 1;
}
