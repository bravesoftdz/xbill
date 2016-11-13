//---------------------------------------------------------------------------

#ifndef jumbleH
#define jumbleH
#include <Classes.hpp>


#include <iostream>
#include <fstream>
using namespace std;

#include <tools/all.h>
#include <coding/block/ldpc.h>
#include "bitchar.h"

//---------------------------------------------------------------------------

class CStringJumbleException {
public:
    int err_mesg;
    CStringJumbleException(int mesg){
        err_mesg = mesg;
    }
};

class CStringProtect {
    protected :
        CInterleaver inner_interleaver, outer_interleaver;
        int ib, ob; //inner_block, outer_block;
        int ipad, opad;
        int ldpc_M,ldpc_N;
        WLDPCParityCheck ldpc;
        int ApplyInner(char *dest, int *destlen, char*src, int srclen, int *padsize, int *lpad){
            if(dest==NULL || src == NULL || *destlen <=0 || srclen <= 0)
                throw CStringJumbleException(INVALID_RANGE);
            int len = sizeof(char)*8*srclen;
            int k = ldpc_N-ldpc_M;
            *lpad = k-(len % k);
            len+=*lpad;
            WVector bitstream(len), outstream;
            StringToBitVector(bitstream.vect, &len, src, srclen);
//            bitstream.taille = len;
            WVector encoded = ldpc.encode(bitstream);

            outstream = inner_interleaver.Apply(encoded);
            if(outstream.taille>(int)((*destlen)*8*sizeof(char))) throw CStringJumbleException(INVALID_RANGE);
            BitVectorToString(dest, destlen, outstream.vect, outstream.taille);
            *padsize = ipad = inner_interleaver.get_padsize();
            return ipad;
        }
        int ApplyOuter(char *dest, int *destlen, char*src, int srclen, int *padsize){
            if(dest==NULL || src == NULL || *destlen <=0 || srclen <= 0)
                throw CStringJumbleException(INVALID_RANGE);

            int len = sizeof(char)*8;
            outer_interleaver.Apply(dest, destlen, src, srclen);
            *padsize = opad = outer_interleaver.get_padsize();
            return opad;
        }

        int ExtractInner(char *dest, int *destlen, char*src, int srclen, int padsize, int lpad){
            if(dest==NULL || src == NULL || *destlen <=0 || srclen <= 0)
                throw CStringJumbleException(INVALID_RANGE);
            int len = sizeof(char)*8*srclen;
            WVector bitstream(len), outstream;
            StringToBitVector(bitstream.vect, &len, src, srclen);
            inner_interleaver.set_padsize(padsize);
            outstream = inner_interleaver.Extract(bitstream);
            int maxit = MAX_ITERATION;
            MAX_ITERATION=10;
            WVector decoded = ldpc.decode(2*outstream-1);
            MAX_ITERATION=maxit;
            if(decoded.taille>((*destlen)*8*sizeof(char))) throw CStringJumbleException(INVALID_RANGE);
            BitVectorToString(dest, destlen, decoded.vect, decoded.taille-lpad);
            return ipad;
        }
        int ExtractOuter(char *dest, int *destlen, char*src, int srclen, int padsize){
            if(dest==NULL || src == NULL || *destlen <=0 || srclen <= 0)
                throw CStringJumbleException(INVALID_RANGE);
            outer_interleaver.set_padsize(padsize);
            outer_interleaver.Extract(dest, destlen, src, srclen);
            return opad;
        }

    public :
        CStringProtect(int inner_blocksize=128, int outer_blocksize=1024, int seed=0, int M=1024, int N=2048) {
            ib = inner_blocksize-inner_blocksize%(sizeof(char)*8);
            ob = outer_blocksize;
            inner_interleaver = CInterleaver(ib, seed);
            outer_interleaver = CInterleaver(ob, seed+1);
            ldpc_M = M;
            ldpc_N = N;
            ldpc = WLDPCParityCheck(M,N);
            ldpc.make_dense_mixed();
        }

        void Apply(char ** dest, int * destlen, char *src, int srclen, int *ipad, int *opad, int* lpad) {
            int tmp = (1+ldpc_N/ldpc_M)*(srclen+ib)+ib;
            char *idest = new char[tmp];
            int ip, op  ;
            ip = ApplyInner(idest, &tmp, src, srclen, ipad, lpad);
            *destlen = tmp + ob;
            *dest = new char[*destlen];
            op = ApplyOuter (*dest, destlen, idest, tmp, opad);
            delete[] idest;
        }

        void Extract(char ** dest, int * destlen, char *src, int srclen, int ipad, int opad, int lpad) {
            char *odest = new char[srclen];
            int tmp = srclen;
            ExtractOuter(odest, &tmp, src, srclen, opad);
            *dest = new char[tmp];
            *destlen = tmp;
            ExtractInner(*dest, destlen, odest, tmp, ipad, lpad);
            delete[] odest;
        }
        ~CStringProtect(){
        }
        friend ostream& operator<<(ostream &os,  CStringProtect &csp);
        friend istream& operator>>(istream &is,  CStringProtect &csp);

};

ostream& operator<<(ostream &os,  CStringProtect &csp);
istream& operator>>(istream &is,  CStringProtect &csp);


int SaveConfig(CStringProtect &csp, AnsiString filename);
int LoadConfig(CStringProtect &csp, AnsiString filename);

#endif

