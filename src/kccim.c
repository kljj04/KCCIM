// kccim.c

#include "../include/kccim.h"
#include "../internal/kccim_internal.h"
#include "../include/table.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

kccim_t kccim_init(void)
{
    kccim_t ctx = (kccim_t)malloc(sizeof(struct kccim_context));
    if (ctx)
    {
        ctx->state = S_EMPTY;
        ctx->cho = -1;
        ctx->jung = -1;
        ctx->jong = 0;
        ctx->is_korean = 1; // 기본값은 한글 입력 모드
        ctx->comp_length = 0;
        ctx->commit_buf[0] = L'\0';
        ctx->comp_buf[0] = L'\0';
        ctx->result_buf[0] = L'\0';
    }
    return ctx;
}

void kccim_free(kccim_t ctx)
{
    if (ctx)
    {
        free(ctx);
    }
}

const wchar_t* kccim_get_str(kccim_t ctx)
{
    if (!ctx)
    {
        return L"";
    }
    return ctx->result_buf;
}

kccim_info_t kccim_get_info(kccim_t ctx)
{
    kccim_info_t info = {0};
    if (ctx)
    {
        info.is_korean = ctx->is_korean;
        info.comp_length = ctx->comp_length;
        info.comp_status = ctx->state;
        info.comp_str = ctx->comp_buf;
    }
    return info;
}

void kccim_commit(kccim_t ctx)
{
    if (!ctx) 
    {
        return;
    }

    if (ctx->comp_buf[0] != L'\0')
    {
        wcscat_s(ctx->commit_buf, 1024, ctx->comp_buf);
    }

    ctx->state = S_EMPTY;
    ctx->cho = -1;
    ctx->jung = -1;
    ctx->jong = 0;
    ctx->comp_buf[0] = L'\0';
    ctx->comp_length = 0;
    wcscpy_s(ctx->result_buf, 1024, ctx->commit_buf);
}

void kccim_set_korean(kccim_t ctx, int is_korean)
{
    if (ctx)
    {
        if (ctx->is_korean == 1 && is_korean == 0)
        {
            kccim_commit(ctx);
        }
        ctx->is_korean = is_korean;
    }
}

