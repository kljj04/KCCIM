// kccim_internal.h

#ifndef KCCIM_INTERNAL_H
#define KCCIM_INTERNAL_H

#include "../include/kccim.h"

#define S_EMPTY 0
#define S_CHO   1
#define S_JUNG  2
#define S_JONG  3

struct kccim_context {
    int state;
    int cho;
    int jung;
    int jong;

    int is_korean;
    int comp_length;

    wchar_t commit_buf[1024];
    wchar_t comp_buf[4];
    wchar_t result_buf[1024];
};

#endif