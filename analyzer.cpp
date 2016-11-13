//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma hdrstop

#include "analyzer.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

int analyze_packet(char *src, int srclen, char *lastchar, int *nb_pages, bool *newpage, int *pageindex, int *pageindexlen) {
        if(src==NULL || srclen <=0) return 0;

        char *p = src;
        int cnt=0;
        *newpage = false;
        if(*lastchar==0xa && src[0] =='1') *newpage = true;
        int j=0;
        for(int i=0;i<srclen;i++) {
                if (*lastchar==0xa && src[i]=='1') {
                        cnt++;
                        if(j<*pageindexlen) {
                            if(j==0 && i!=0) pageindex[j++]=0;
                            pageindex[j++]=i;
                        }

                }
                *lastchar = src[i];
        }
        *pageindexlen = j;
        *nb_pages = cnt;
        return 1;
}




