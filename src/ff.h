#ifndef FF_H
#define FF_H

/**
 * @file ff.h
 * @brief FatFS minimal stub for KISS Fuzzer
 * 
 * This is a stub implementation to satisfy compilation.
 * For full functionality, integrate proper FatFS library.
 */

#include <stdint.h>
#include <stddef.h>

// Type definitions
typedef uint8_t     BYTE;
typedef uint16_t    WORD;
typedef uint32_t    DWORD;
typedef uint64_t    QWORD;
typedef uint32_t    UINT;
typedef uint32_t    FSIZE_t;

// Date/Time types  
typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
} FDATE;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} FTIME;

// Result codes
typedef enum {
    FR_OK = 0,              // Succeeded
    FR_DISK_ERR,            // A hard error occurred in the low level disk I/O layer
    FR_INT_ERR,             // Assertion failed
    FR_NOT_READY,           // The physical drive cannot work
    FR_NO_FILE,             // Could not find the file
    FR_NO_PATH,             // Could not find the path
    FR_INVALID_NAME,        // The path name format is invalid
    FR_DENIED,              // Access denied due to prohibited access or directory full
    FR_EXIST,               // Access denied due to prohibited access
    FR_INVALID_OBJECT,      // The file/directory object is invalid
    FR_WRITE_PROTECTED,     // The physical drive is write protected
    FR_INVALID_DRIVE,       // The logical drive number is invalid
    FR_NOT_ENABLED,         // The volume has no work area
    FR_NO_FILESYSTEM,       // There is no valid FAT volume
    FR_MKFS_ABORTED,        // The f_mkfs() aborted due to any problem
    FR_TIMEOUT,             // Could not get a grant to access the volume within defined period
    FR_LOCKED,              // The operation is rejected according to the file sharing policy
    FR_NOT_ENOUGH_CORE,     // LFN working buffer could not be allocated
    FR_TOO_MANY_OPEN_FILES, // Number of open files > FF_FS_LOCK
    FR_INVALID_PARAMETER    // Given parameter is invalid
} FRESULT;

// File system object structure
typedef struct {
    uint8_t     fs_type;        // Filesystem type (0:not mounted)
    uint8_t     pdrv;           // Associated physical drive
    uint8_t     n_fats;         // Number of FATs (1 or 2)
    uint8_t     wflag;          // win[] flag (b0:dirty)
    uint8_t     fsi_flag;       // FSINFO flags (b7:disabled, b0:dirty)
    uint16_t    id;             // Volume mount ID
    uint16_t    n_rootdir;      // Number of root directory entries
    uint16_t    csize;          // Cluster size [sectors]
    DWORD       n_fatent;       // Number of FAT entries, = number of clusters + 2
    DWORD       fatbase;        // FAT base sector
    DWORD       dirbase;        // Root directory base sector/cluster
    DWORD       database;       // Data base sector
    DWORD       winsect;        // Current sector appearing in the win[]
    BYTE        win[512];       // Disk access window for Directory, FAT
} FATFS;

// File object structure
typedef struct {
    FATFS*      fs;             // Pointer to the related file system object
    uint16_t    id;             // File system mount ID of the volume
    uint8_t     attr;           // File attribute
    uint8_t     stat;           // File status flags
    DWORD       sclust;         // File start cluster
    FSIZE_t     objsize;        // File size (directory's table size)
    DWORD       n_contig;       // Contiguous cluster length  
    DWORD       clust;          // Current cluster of fpter (invalid when fptr is 0)
    DWORD       sect;           // Sector number appearing in buf[]
    DWORD       dir_sect;       // Sector number containing the directory entry
    BYTE*       dir_ptr;        // Pointer to the directory entry in the win[]
    DWORD       fptr;           // File read/write pointer
    uint8_t     err;            // Error flag
    BYTE        buf[512];       // File private data read/write window
} FIL;

// Directory object structure
typedef struct {
    FATFS*      fs;             // Pointer to the owner file system object
    uint16_t    id;             // Owner file system mount ID
    uint16_t    attr;           // Attribute mask
    uint16_t    lfn_idx;        // Index of LFN entries
    DWORD       sclust;         // Table start cluster (0:root directory)
    DWORD       clust;          // Current cluster
    DWORD       sect;           // Current sector
    BYTE*       dir;            // Pointer to the current SFN entry in the win[]
    BYTE        fn[12];         // SFN (in/out) {body[8],ext[3],status[1]}
    DWORD       blk_ofs;        // Offset of current entry block
} DIR;

// File information structure  
typedef struct {
    DWORD       fsize;          // File size
    WORD        fdate;          // Modified date
    WORD        ftime;          // Modified time  
    BYTE        fattrib;        // File attribute
    char        fname[13];      // File name
    char        altname[13];    // Alternative file name
} FILINFO;

// File attribute bits for directory entry
#define AM_RDO  0x01            // Read only
#define AM_HID  0x02            // Hidden
#define AM_SYS  0x04            // System
#define AM_DIR  0x10            // Directory
#define AM_ARC  0x20            // Archive

// File access mode and open method flags
#define FA_READ         0x01
#define FA_WRITE        0x02
#define FA_OPEN_EXISTING 0x00
#define FA_CREATE_NEW   0x04
#define FA_CREATE_ALWAYS 0x08
#define FA_OPEN_ALWAYS  0x10
#define FA_OPEN_APPEND  0x30

// Fast seek controls
#define CREATE_LINKMAP  0xFFFFFFFF

// Format options
#define FM_FAT          0x01
#define FM_FAT32        0x02
#define FM_EXFAT        0x04
#define FM_ANY          0x07
#define FM_SFD          0x08

// Filesystem constants
#define FF_MAX_SS       512

// Function prototypes (stubs)
FRESULT f_mount(FATFS* fs, const char* path, BYTE opt);
FRESULT f_unmount(const char* path);
FRESULT f_open(FIL* fp, const char* path, BYTE mode);
FRESULT f_close(FIL* fp);
FRESULT f_read(FIL* fp, void* buff, uint32_t btr, uint32_t* br);
FRESULT f_write(FIL* fp, const void* buff, uint32_t btw, uint32_t* bw);
FRESULT f_lseek(FIL* fp, FSIZE_t ofs);
FRESULT f_truncate(FIL* fp);
FRESULT f_sync(FIL* fp);
FRESULT f_opendir(DIR* dp, const char* path);
FRESULT f_closedir(DIR* dp);
FRESULT f_readdir(DIR* dp, FILINFO* fno);
FRESULT f_findfirst(DIR* dp, FILINFO* fno, const char* path, const char* pattern);
FRESULT f_findnext(DIR* dp, FILINFO* fno);
FRESULT f_mkdir(const char* path);
FRESULT f_unlink(const char* path);
FRESULT f_rename(const char* path_old, const char* path_new);
FRESULT f_stat(const char* path, FILINFO* fno);
FRESULT f_chmod(const char* path, BYTE attr, BYTE mask);
FRESULT f_utime(const char* path, const FILINFO* fno);
FRESULT f_chdir(const char* path);
FRESULT f_chdrive(const char* path);
FRESULT f_getcwd(char* buff, UINT len);
FRESULT f_getfree(const char* path, uint32_t* nclst, FATFS** fatfs);
FRESULT f_getlabel(const char* path, char* label, uint32_t* vsn);
FRESULT f_setlabel(const char* label);
FRESULT f_forward(FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);
FRESULT f_expand(FIL* fp, FSIZE_t szf, BYTE opt);
FRESULT f_mkfs(const char* path, BYTE opt, DWORD au, void* work, UINT len);
FRESULT f_fdisk(BYTE pdrv, const DWORD* szt, void* work);
int f_putc(int chr, FIL* fp);
int f_puts(const char* str, FIL* fp);
int f_printf(FIL* fp, const char* str, ...);
char* f_gets(char* buff, int len, FIL* fp);

#define f_eof(fp)       ((fp)->fptr == (fp)->objsize)
#define f_error(fp)     ((fp)->err)
#define f_tell(fp)      ((fp)->fptr)
#define f_size(fp)      ((fp)->objsize)
#define f_rewind(fp)    f_lseek(fp, 0)
#define f_rewinddir(dp) f_readdir(dp, 0)
#define f_rmdir(path)   f_unlink(path)

#endif /* FF_H */
