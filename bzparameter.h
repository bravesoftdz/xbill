#ifndef _BZPARAMETER_H_
#define _BZPARAMETER_H_


#define b500k

#ifdef b100k
#define IN_BUFFER_SIZE  131072
#define OUT_BUFFER_SIZE 133082
#endif

#ifdef b200k
#define IN_BUFFER_SIZE  262144
#define OUT_BUFFER_SIZE 265465
#endif

#ifdef b500k
#define IN_BUFFER_SIZE  524288
#define OUT_BUFFER_SIZE 530230
#endif

#define DEC_OUT_BUFFER_SIZE 800000


#endif