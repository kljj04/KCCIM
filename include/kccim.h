// kccim.h

#ifndef KCCIM_H
#define KCCIM_H

#include <stddef.h>
#include <wchar.h>

#ifdef KCCIM_EXPORTS
    #define KCCIM_API __declspec(dllexport)
#else
    #define KCCIM_API __declspec(dllimport)
#endif

typedef struct kccim_context* kccim_t;

typedef struct {
    int is_korean;
    int comp_length;
    int comp_status;
    const wchar_t* comp_str;
} kccim_info_t;

KCCIM_API kccim_t kccim_init(void);
KCCIM_API void kccim_free(kccim_t ctx);
KCCIM_API int kccim_put_key(kccim_t ctx, int vk_code);
KCCIM_API const wchar_t* kccim_get_str(kccim_t ctx);
KCCIM_API kccim_info_t kccim_get_info(kccim_t ctx);
KCCIM_API void kccim_commit(kccim_t ctx);
KCCIM_API void kccim_set_korean(kccim_t ctx, int is_korean);


#endif
