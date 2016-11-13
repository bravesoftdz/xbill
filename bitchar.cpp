//---------------------------------------------------------------------------


#pragma hdrstop

#include "bitchar.h"

int StringToBitVector(int *dest, int *destlen, char*src, int srclen){
    if(srclen<=0) return -1;
    if(src==NULL) return -2;
    if(dest==NULL) return -3;
    if(destlen<=NULL) return -4;
    int sizeof_char = sizeof(char)*8;
    int tmplen = srclen*sizeof_char;
    if (*destlen<tmplen) return -2;
    *destlen =tmplen;
    //dest = new int[*destlen];

    for(int i = 0,t=0; i < srclen; i++) {
        for(int j=0, m=(1<<sizeof_char);j<sizeof_char;j++) {
            m >>= 1;
            dest[t++] = (src[i] & m )  ? 1 : 0;
        }
    }
    return 1;
}

int BitVectorToString(char *dest, int *destlen, int *src, int srclen){
    if(srclen<=0) return -1;
    if(src==NULL) return -2;
    if(dest==NULL) return -3;
    if(destlen<=NULL) return -4;
    int sizeof_char = sizeof(char)*8;
    int tmplen = srclen/sizeof_char;
    if (*destlen<tmplen) return -5;
    *destlen =tmplen;
//    dest = new int[*destlen];
    dest[0]=0;
    for(int i=0, m=(1<<(sizeof_char-1)), j=0; i <srclen;i++) {
        if(src[i]) {
            dest[j] |= m;
        }
        m >>=1;
        if (m==0) {
            if(j<(*destlen-1)) dest[++j]=0;
            m=(1<<(sizeof_char-1));

        }
    }
    return 1;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
