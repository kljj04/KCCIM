#include "../include/kccim.h"
#include "../internal/kccim_internal.h"
#include "../include/table.h"
#include <wchar.h>
#include <string.h>

static int get_cho_index(wchar_t ch) {
    wchar_t cho_list[] = L"ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ";
    for (int i = 0; i < 19; i++) { if (cho_list[i] == ch) return i; }
    return -1;
}

static int get_jung_index(wchar_t ch) {
    wchar_t jung_list[] = L"ㅏㅐㅑㅒㅓㅔㅕㅖㅗㅘㅙㅚㅛㅜㅝㅞㅟㅠㅡㅢㅣ";
    for (int i = 0; i < 21; i++) { if (jung_list[i] == ch) return i; }
    return -1;
}

static int get_jong_index(wchar_t ch) {
    if (ch == 0) return 0;
    wchar_t jong_list[] = L" ㄱㄲㄳㄴㄵㄶㄷㄹㄺㄻㄼㄽㄾㄿㅀㅁㅂㅄㅅㅆㅇㅈㅊㅋㅌㅍㅎ";
    for (int i = 1; i < 28; i++) { if (jong_list[i] == ch) return i; }
    return 0;
}

static wchar_t merge_jung(wchar_t current, wchar_t next) {
    if (current == L'ㅗ' && next == L'ㅏ') return L'ㅘ';
    if (current == L'ㅗ' && next == L'ㅐ') return L'ㅙ';
    if (current == L'ㅗ' && next == L'ㅣ') return L'ㅚ';
    if (current == L'ㅜ' && next == L'ㅓ') return L'ㅝ';
    if (current == L'ㅜ' && next == L'ㅔ') return L'ㅞ';
    if (current == L'ㅜ' && next == L'ㅣ') return L'ㅟ';
    if (current == L'ㅡ' && next == L'ㅣ') return L'ㅢ';
    return 0;
}

static wchar_t merge_jong(wchar_t current, wchar_t next) {
    if (current == L'ㄱ' && next == L'ㅅ') return L'ㄳ';
    if (current == L'ㄴ' && next == L'ㅈ') return L'ㄵ';
    if (current == L'ㄴ' && next == L'ㅎ') return L'ㄶ';
    if (current == L'ㄹ' && next == L'ㄱ') return L'ㄺ';
    if (current == L'ㄹ' && next == L'ㅁ') return L'ㄻ';
    if (current == L'ㄹ' && next == L'ㅂ') return L'ㄼ';
    if (current == L'ㄹ' && next == L'ㅅ') return L'ㄽ';
    if (current == L'ㄹ' && next == L'ㅌ') return L'ㄾ';
    if (current == L'ㄹ' && next == L'ㅍ') return L'ㄿ';
    if (current == L'ㄹ' && next == L'ㅎ') return L'ㅀ';
    if (current == L'ㅂ' && next == L'ㅅ') return L'ㅄ';
    return 0;
}

int kccim_put_key(kccim_t ctx, int vk_code)
{
    if (!ctx) return 0;

    if (vk_code == 0x08) 
    {
        if (ctx->jong != 0) {
            wchar_t j = (wchar_t)ctx->jong;
            if (j == L'ㄳ') ctx->jong = L'ㄱ';
            else if (j == L'ㄵ') ctx->jong = L'ㄴ';
            else if (j == L'ㄶ') ctx->jong = L'ㄴ';
            else if (j == L'ㄺ') ctx->jong = L'ㄹ';
            else if (j == L'ㄻ') ctx->jong = L'ㄹ';
            else if (j == L'ㄼ') ctx->jong = L'ㄹ';
            else if (j == L'ㄽ') ctx->jong = L'ㄹ';
            else if (j == L'ㄾ') ctx->jong = L'ㄹ';
            else if (j == L'ㄿ') ctx->jong = L'ㄹ';
            else if (j == L'ㅀ') ctx->jong = L'ㄹ';
            else if (j == L'ㅄ') ctx->jong = L'ㅂ';
            else ctx->jong = 0;
        }
        else if (ctx->jung != -1) {
            wchar_t j = (wchar_t)ctx->jung;
            if (j == L'ㅘ' || j == L'ㅙ' || j == L'ㅚ') ctx->jung = L'ㅗ';
            else if (j == L'ㅝ' || j == L'ㅞ' || j == L'ㅟ') ctx->jung = L'ㅜ';
            else if (j == L'ㅢ') ctx->jung = L'ㅡ';
            else ctx->jung = -1;
        }
        else if (ctx->cho != -1) {
            ctx->cho = -1;
        }
        else {
            size_t len = wcslen(ctx->commit_buf);
            if (len > 0) ctx->commit_buf[len - 1] = L'\0';
        }
        
        memset(ctx->comp_buf, 0, sizeof(ctx->comp_buf));
        goto RENDER;
    }

    if (vk_code == 0x20 || vk_code == 0x0D || vk_code == 0x0A) 
    {
        if (ctx->comp_buf[0] != L'\0') wcscat_s(ctx->commit_buf, 1024, ctx->comp_buf);
        ctx->cho = -1; ctx->jung = -1; ctx->jong = 0;
        memset(ctx->comp_buf, 0, sizeof(ctx->comp_buf));
        
        size_t len = wcslen(ctx->commit_buf);
        if (len < 1023) {
            ctx->commit_buf[len] = (vk_code == 0x20) ? L' ' : L'\n';
            ctx->commit_buf[len + 1] = L'\0';
        }
        wcscpy_s(ctx->result_buf, 1024, ctx->commit_buf);
        return 1;
    }

    if (vk_code < 0 || vk_code >= 256) return 0;

    int target_vk = vk_code;
    if (vk_code >= 'A' && vk_code <= 'Z') {
        if (vk_code != 'R' && vk_code != 'E' && vk_code != 'Q' && vk_code != 'T' && vk_code != 'W' && vk_code != 'O' && vk_code != 'P') {
            target_vk = vk_code + 32;
        }
    }

    wchar_t input_cho  = (wchar_t)chosung_table[target_vk];
    wchar_t input_jung = (wchar_t)jungsung_table[target_vk];

    if (input_cho != 0) {
        if (ctx->cho == -1) { ctx->cho = input_cho; } 
        else if (ctx->cho != -1 && ctx->jung == -1) {
            wchar_t solo[2] = { (wchar_t)ctx->cho, L'\0' };
            wcscat_s(ctx->commit_buf, 1024, solo);
            ctx->cho = input_cho;
        } 
        else if (ctx->cho != -1 && ctx->jung != -1 && ctx->jong == 0) { ctx->jong = input_cho; } 
        else if (ctx->cho != -1 && ctx->jung != -1 && ctx->jong != 0) {
            wchar_t merged = merge_jong((wchar_t)ctx->jong, input_cho);
            if (merged != 0) { ctx->jong = merged; } 
            else {
                wcscat_s(ctx->commit_buf, 1024, ctx->comp_buf);
                ctx->cho = input_cho; ctx->jung = -1; ctx->jong = 0;
            }
        }
    } 
    else if (input_jung != 0) {
        if (ctx->cho != -1 && ctx->jung == -1) { ctx->jung = input_jung; } 
        else if (ctx->cho != -1 && ctx->jung != -1 && ctx->jong == 0) {
            wchar_t merged = merge_jung((wchar_t)ctx->jung, input_jung);
            if (merged != 0) { ctx->jung = merged; } 
            else {
                wcscat_s(ctx->commit_buf, 1024, ctx->comp_buf);
                wchar_t solo[2] = { input_jung, L'\0' };
                wcscat_s(ctx->commit_buf, 1024, solo);
                ctx->cho = -1; ctx->jung = -1; ctx->jong = 0;
            }
        }
        else if (ctx->cho != -1 && ctx->jung != -1 && ctx->jong != 0) {
            wchar_t next_cho = 0;
            wchar_t j = (wchar_t)ctx->jong;
            if (j == L'ㄳ') { ctx->jong = L'ㄱ'; next_cho = L'ㅅ'; }
            else if (j == L'ㄵ') { ctx->jong = L'ㄴ'; next_cho = L'ㅈ'; }
            else if (j == L'ㄶ') { ctx->jong = L'ㄴ'; next_cho = L'ㅎ'; }
            else if (j == L'ㄺ') { ctx->jong = L'ㄹ'; next_cho = L'ㄱ'; }
            else if (j == L'ㄻ') { ctx->jong = L'ㄹ'; next_cho = L'ㅁ'; }
            else if (j == L'ㄼ') { ctx->jong = L'ㄹ'; next_cho = L'ㅂ'; }
            else if (j == L'ㄽ') { ctx->jong = L'ㄹ'; next_cho = L'ㅅ'; }
            else if (j == L'ㄾ') { ctx->jong = L'ㄹ'; next_cho = L'ㅌ'; }
            else if (j == L'ㄿ') { ctx->jong = L'ㄹ'; next_cho = L'ㅍ'; }
            else if (j == L'ㅀ') { ctx->jong = L'ㄹ'; next_cho = L'ㅎ'; }
            else if (j == L'ㅄ') { ctx->jong = L'ㅂ'; next_cho = L'ㅅ'; }
            else { next_cho = j; ctx->jong = 0; }

            int c_idx = get_cho_index((wchar_t)ctx->cho);
            int j_idx = get_jung_index((wchar_t)ctx->jung);
            int d_idx = get_jong_index((wchar_t)ctx->jong);
            wchar_t combined = (wchar_t)(0xAC00 + (c_idx * 588) + (j_idx * 28) + d_idx);
            wchar_t solo[2] = { combined, L'\0' };
            wcscat_s(ctx->commit_buf, 1024, solo);

            ctx->cho = next_cho; ctx->jung = input_jung; ctx->jong = 0;
        }
        else {
            if (ctx->comp_buf[0] != L'\0') wcscat_s(ctx->commit_buf, 1024, ctx->comp_buf);
            wchar_t solo[2] = { input_jung, L'\0' };
            wcscat_s(ctx->commit_buf, 1024, solo);
            ctx->cho = -1; ctx->jung = -1; ctx->jong = 0;
        }
    }
    else {
        if (ctx->comp_buf[0] != L'\0') wcscat_s(ctx->commit_buf, 1024, ctx->comp_buf);
        size_t len = wcslen(ctx->commit_buf);
        if (len < 1023) { ctx->commit_buf[len] = (wchar_t)vk_code; ctx->commit_buf[len + 1] = L'\0'; }
        ctx->cho = -1; ctx->jung = -1; ctx->jong = 0;
    }

RENDER:
    memset(ctx->comp_buf, 0, sizeof(ctx->comp_buf));
    if (ctx->cho != -1 && ctx->jung == -1) {
        ctx->comp_buf[0] = (wchar_t)ctx->cho;
    } 
    else if (ctx->cho != -1 && ctx->jung != -1) {
        int c_idx = get_cho_index((wchar_t)ctx->cho);
        int j_idx = get_jung_index((wchar_t)ctx->jung);
        int d_idx = get_jong_index((wchar_t)ctx->jong);
        if (c_idx != -1 && j_idx != -1) {
            ctx->comp_buf[0] = (wchar_t)(0xAC00 + (c_idx * 588) + (j_idx * 28) + d_idx);
        } else {
            ctx->comp_buf[0] = (wchar_t)ctx->cho;
        }
    }

    wcscpy_s(ctx->result_buf, 1024, ctx->commit_buf);
    wcscat_s(ctx->result_buf, 1024, ctx->comp_buf);
    return 1;
}