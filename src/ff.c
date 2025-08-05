/**
 * @file ff.c
 * @brief FatFS minimal stub implementation for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.7.0
 * 
 * This is a stub implementation to satisfy compilation.
 * For full functionality, integrate proper FatFS library.
 */

#include "ff.h"
#include <stdio.h>
#include <string.h>

// Stub implementations - return OK but don't actually do file operations
FRESULT f_mount(FATFS* fs, const char* path, BYTE opt) {
    printf("FatFS STUB: f_mount called\n");
    return FR_OK;
}

FRESULT f_unmount(const char* path) {
    printf("FatFS STUB: f_unmount called\n");
    return FR_OK;
}

FRESULT f_open(FIL* fp, const char* path, BYTE mode) {
    printf("FatFS STUB: f_open called for %s\n", path);
    return FR_NO_FILE; // Indicate file not found for now
}

FRESULT f_close(FIL* fp) {
    printf("FatFS STUB: f_close called\n");
    return FR_OK;
}

FRESULT f_read(FIL* fp, void* buff, uint32_t btr, uint32_t* br) {
    printf("FatFS STUB: f_read called\n");
    *br = 0;
    return FR_OK;
}

FRESULT f_write(FIL* fp, const void* buff, uint32_t btw, uint32_t* bw) {
    printf("FatFS STUB: f_write called (%lu bytes)\n", btw);
    *bw = btw; // Pretend we wrote all bytes
    return FR_OK;
}

FRESULT f_lseek(FIL* fp, FSIZE_t ofs) {
    printf("FatFS STUB: f_lseek called\n");
    return FR_OK;
}

FRESULT f_truncate(FIL* fp) {
    printf("FatFS STUB: f_truncate called\n");
    return FR_OK;
}

FRESULT f_sync(FIL* fp) {
    printf("FatFS STUB: f_sync called\n");
    return FR_OK;
}

FRESULT f_opendir(DIR* dp, const char* path) {
    printf("FatFS STUB: f_opendir called for %s\n", path);
    return FR_NO_PATH; // Indicate directory not found for now
}

FRESULT f_closedir(DIR* dp) {
    printf("FatFS STUB: f_closedir called\n");
    return FR_OK;
}

FRESULT f_readdir(DIR* dp, FILINFO* fno) {
    printf("FatFS STUB: f_readdir called\n");
    fno->fname[0] = 0; // Empty filename indicates end of directory
    return FR_OK;
}

FRESULT f_findfirst(DIR* dp, FILINFO* fno, const char* path, const char* pattern) {
    printf("FatFS STUB: f_findfirst called\n");
    fno->fname[0] = 0;
    return FR_OK;
}

FRESULT f_findnext(DIR* dp, FILINFO* fno) {
    printf("FatFS STUB: f_findnext called\n");
    fno->fname[0] = 0;
    return FR_OK;
}

FRESULT f_mkdir(const char* path) {
    printf("FatFS STUB: f_mkdir called for %s\n", path);
    return FR_OK;
}

FRESULT f_unlink(const char* path) {
    printf("FatFS STUB: f_unlink called for %s\n", path);
    return FR_OK;
}

FRESULT f_rename(const char* path_old, const char* path_new) {
    printf("FatFS STUB: f_rename called\n");
    return FR_OK;
}

FRESULT f_stat(const char* path, FILINFO* fno) {
    printf("FatFS STUB: f_stat called for %s\n", path);
    return FR_NO_FILE;
}

FRESULT f_chmod(const char* path, BYTE attr, BYTE mask) {
    printf("FatFS STUB: f_chmod called\n");
    return FR_OK;
}

FRESULT f_utime(const char* path, const FILINFO* fno) {
    printf("FatFS STUB: f_utime called\n");
    return FR_OK;
}

FRESULT f_getfree(const char* path, DWORD* nclst, FATFS** fatfs) {
    printf("FatFS STUB: f_getfree called\n");
    *nclst = 1000; // Fake 1000 free clusters
    return FR_OK;
}

FRESULT f_getlabel(const char* path, char* label, DWORD* vsn) {
    printf("FatFS STUB: f_getlabel called\n");
    if (label) {
        strcpy(label, "KISS-STUB");
    }
    if (vsn) {
        *vsn = 0x12345678;
    }
    return FR_OK;
}

FRESULT f_mkfs(const char* path, BYTE opt, DWORD au, void* work, UINT len) {
    printf("FatFS STUB: f_mkfs called\n");
    return FR_OK;
}
