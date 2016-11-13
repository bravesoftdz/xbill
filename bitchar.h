//---------------------------------------------------------------------------

#ifndef bitcharH
#define bitcharH

#include <iostream>
#include <fstream>
using namespace std;

#include <tools/all.h>


int StringToBitVector(int *dest, int *destlen, char*src, int srclen);
int BitVectorToString(char *dest, int *destlen, int*src, int srclen);
//---------------------------------------------------------------------------
#endif
 