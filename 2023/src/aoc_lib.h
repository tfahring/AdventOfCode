#ifndef AOC_LIB_H
#define AOC_LIB_H


//---------------------------------------------------------------------------
// COMPILATION CONTEXT
//---------------------------------------------------------------------------
#if defined(_WIN32)
#    define OS_WINDOWS 1
#elif defined(__gnu_linux__)
#    define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#    define OS_APPLE 1
#endif // OS

#if defined(__clang__)
#   define COMPILER_CLANG 1
#elif defined(_MSC_VER)
#   define COMPILER_CL 1
#elif defined(__GNUC__)
#   define COMPILER_GCC 1
#endif // COMPILER

#if !defined(OS_WINDOWS)
#   define OS_WINDOWS 0
#endif

#if !defined(OS_LINUX)
#   define OS_LINUX 0
#endif

#if !defined(OS_APPLE)
#   define OS_APPLE 0
#endif

#if !defined(COMPILER_CLANG)
#   define COMPILER_CLANG 0
#endif

#if !defined(COMPILER_CL)
#   define COMPILER_CL 0
#endif

#if !defined(COMPILER_GCC)
#   define COMPILER_GCC 0
#endif

#if COMPILER_CL 
#   if _DEBUG
#       define DEBUG_BUILD 1
#       define DEBUG_BREAK()       __debugbreak()
#       define DEBUG_PRINT(string) OutputDebugStringA(string)
#   endif
#endif

#if !defined(DEBUG_BUILD) 
#   define DEBUG_BUILD 0
#   define DEBUG_BREAK()
#endif 

#if DEBUG_BUILD
#define ASSERT(cond) do { if (!(cond)) DEBUG_BREAK(); } while (0)
#else
#define ASSERT(cond)
#endif

//---------------------------------------------------------------------------
// BASE TYPES
//---------------------------------------------------------------------------
#include <stdarg.h>
#include <stdint.h>
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;
typedef uint8_t     b8;
typedef uint16_t    b16;
typedef uint32_t    b32;
typedef uint64_t    b64;
typedef float       f32;
typedef double      f64;

#define true  1
#define false 0

#if OS_WINDOWS
#   define THREAD_LOCAL __declspec(thread)
#   define EXPORT_API   __declspec(dllexport)
#   define IMPORT_API   __declspec(dllimport)

#define OS_THREAD_PROC(name) unsigned long __stdcall name(void *data)
typedef OS_THREAD_PROC(OS_ThreadProc);

#elif OS_LINUX
#   define THREAD_LOCAL __thread
#   define EXPORT_API   __attribute__((visibility("default")))
#   define IMPORT_API   
#endif

#define FUNCTION  static
#define GLOBAL    static
#define READ_ONLY GLOBAL const

typedef void VoidFn(void);

//---------------------------------------------------------------------------
// MACROS
//---------------------------------------------------------------------------
#define STRUCT(name) typedef struct name name; struct name
#define UNION(name)  typedef union  name name; union  name
#define ENUM(name)   typedef enum   name name; enum   name
#define TYPED_ENUM(name,type) typedef type name; enum
#define FLAGS(name) TYPED_ENUM(name,u64)
#define STRUCT_DEF(name) typedef struct name name   

#define ARRAY_LENGTH(x)   (sizeof(x) / sizeof((x)[0]))
#define KILOBYTES(value)  ((value) * 1024LL)
#define MEGABYTES(value)  (KILOBYTES(value)*1024LL)
#define GIGABYTES(value)  (MEGABYTES(value)*1024LL)
#define MIN(a,b)          ((a) < (b)?(a):(b))
#define MAX(a,b)          ((a) > (b)?(a):(b))
#define CLAMP(x,a,b)      (MIN(b, MAX(a,x)))
#define CLAMPU(x,b)       (MIN(x,b))
#define CLAMPL(x,a)       (MAX(x,a))
#define IS_BETWEEN(x,a,b) ((x >= a) && (x <= b))
#define IS_NAN(x)         ((x) != (x))
#define IS_INF(x)         ((x)/(x) != (x)/(x))
#define IS_EVEN(x)        ((x) % 2 == 0)
#define IS_POWER_OF_2(x)  (((x) & ((x)-1)) == 0)
#define IS_ALIGNED(POINTER, BYTE_COUNT) (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)
#define SQUARED(x)        ((x)*(x))
#define CUBED(x)          ((x)*(x)*(x))

#define CONCAT2(x,y) x ## y
#define CONCAT(x,y) CONCAT2(x,y)

//---------------------------------------------------------------------------
// LINKED LIST MACROS
//---------------------------------------------------------------------------
#define DLL_PUSH_BACK_NP(first,last,node,next,prev) ((first == NULL) ?\
                                                    ((first) = (last) = (node), (node)->next = (node)->prev = NULL) :\
                                                    ((node)->prev = (last), (last)->next = (node), (last) = (node), (node)->next = 0))
#define DLL_PUSH_BACK(first,last,node)  DLL_PUSH_BACK_NP(first,last,node,next,prev)
#define DLL_PUSH_FRONT(first,last,node) DLL_PUSH_BACK_NP(last,first,node,prev,next)

#define DLL_REMOVE_NP(first,last,node,next,prev) (((first) == (node)?\
                                                  ((first) = (first)->next, (first)->prev = NULL):\
                                                   (last) == (node) ?\
                                                  ((last) = (last)->prev, (last)->next = NULL):\
                                                  ((node)->next->prev = (node)->prev,\
                                                   (node)->prev->next = (node)->next)))
#define DLL_REMOVE(first,last,node) DLL_REMOVE_NP(first,last,node,next,prev)

#define SLL_QUEUE_PUSH_N(first,last,node,next) ((first) == NULL ?\
                                                (first)=(last)=(node):\
                                               ((last)->next = (node), (last) = (node)),\
                                                (node)->next = 0)
#define SLL_QUEUE_PUSH(first,last,node) SLL_QUEUE_PUSH_N(first,last,node,next)

#define SLL_QUEUE_PUSH_FRONT_N(first,last,node,next) ((first) == NULL ?\
                                                     ((first)=(last)=(node),(node)->next = NULL):\
                                                     ((node)->next = (first), (first) = (node)))
#define SLL_QUEUE_PUSH_FRONT(first,last,node) SLL_QUEUE_PUSH_FRONT_N(first,last,node,next)

#define SLL_QUEUE_POP_N(first,last,next) ((first) == (last) ?\
                                          (first) = (last) = NULL:\
                                          (first) = (first)->next)
#define SLL_QUEUE_POP(first,last) SLL_QUEUE_POP_N(first,last,next)

#define SLL_STACK_PUSH_N(first,node,next) ((node)->next = (first), (first) = (node))
#define SLL_STACK_PUSH(first,node) SLL_STACK_PUSH_N(first,node,next)

#define SLL_STACK_POP_N(first, next) ((first) ==  NULL?\
                                      (first) = (first)->next)
#define SLL_STACK_POP(first) SLL_STACK_POP_N(first, next)


FUNCTION u64 _mem_get_aligned_size(u64 sizeInBytes);
FUNCTION u64 _align_forward(u64 ptr, u64 align);

//------------------------------------------------------------------------
// MEMORY ARENA
//------------------------------------------------------------------------
#define MEM_DEFAULT_RESERVE_SIZE GIGABYTES(1)
STRUCT(MemoryArena) {
    void *base;
    u64   usedBytes;         // Amount of memory currently being used.
    u64   commitedBytes;     // Amount of memory committed.
    u64   capacityInBytes;   // Amount of memory reserved. Cannot be exceeded.
    u64   commitSizeInBytes; // Amount of memory to commit.
};

FUNCTION MemoryArena mem_arena_init(u64 reserveSizeInBytes);
FUNCTION void        mem_arena_free(MemoryArena *arena);
FUNCTION void        mem_arena_reset(MemoryArena *arena);
FUNCTION void*      _mem_arena_push(MemoryArena *arena, u64 size);
FUNCTION void*      _mem_arena_push_and_zero(MemoryArena *arena, u64 size);

#define mem_arena_push(arena, length, type)          (type*)_mem_arena_push(arena, length*sizeof(type))
#define mem_arena_push_and_zero(arena, length, type) (type*)_mem_arena_push_and_zero(arena, length*sizeof(type))

#define MEM_SCRATCH_ARENA_COUNT 4
STRUCT(MEM_ScratchArenaPool) {
    MemoryArena arenas[MEM_SCRATCH_ARENA_COUNT];
    b32       isLocked[MEM_SCRATCH_ARENA_COUNT];    
};

FUNCTION MemoryArena mem_arena_get_scratch(void);
FUNCTION void        mem_arena_release_scratch(MemoryArena *scratch);


//------------------------------------------------------------------------
// STRINGS
//------------------------------------------------------------------------
// Basic C-String Operations
FUNCTION u64 cstr_length(char    *cstr);
FUNCTION u64 wstr_length(wchar_t *wstr);
FUNCTION b32 cstr_compare(char *strA, char *strB);
FUNCTION b32 wstr_compare(wchar_t *strA, wchar_t *strB);

// Character Operations
FUNCTION b32 is_alpha(char c);
FUNCTION b32 is_numeric(char c);
FUNCTION b32 is_alphanumeric(char c);
FUNCTION b32 is_whitespace(char c);

FUNCTION u8 to_lower(u8 c);
FUNCTION u8 to_upper(u8 c);

FUNCTION s64 string_to_int64(u8 *ptr, u64 length);

// UTF-8
FUNCTION s32 utf8_decode(u8 *str, u32 *codepoint);
FUNCTION u64 utf8_get_encoded_size(u32 codepoint);
FUNCTION u8* utf8_encode(u8 *buffer, u32 codepoint);

// UTF-16
FUNCTION s32  utf16_decode(u16 *str, u32 *codepoint);
FUNCTION u64  utf16_get_encoded_size(u32 codepoint);
FUNCTION u16* utf16_encode(u16 *buffer, u32 codepoint);

// UTF Encoded Strings
STRUCT(String8) {
    u8 *buffer;
    u64 length;
    u64 size;
};

STRUCT(String16) {
    u16 *buffer;
    u64  length;
    u64  size;
};

STRUCT(String32) {
    u32 *buffer;
    u64  length;
    u64  size;
};

// String8 Creation
FUNCTION String8 string8( u8  *str, u64 length);
FUNCTION String8 string8_copy(MemoryArena *arena, String8 str);
FUNCTION String8 string8_from_cstr(char *cstr);
FUNCTION String8 string8_from_wstr(MemoryArena *arena, wchar_t *wstr);
FUNCTION String8 string8_from_range(u8 *start, u8 *end);
String8 string8_snprintf(MemoryArena *arena, const char *format, ...);

// String8 Comparison
FUNCTION b32 string8_compare(String8 strA, String8 strB);

// String8 Manipulation?
FUNCTION String8 string8_concat(MemoryArena *arena, String8 str0, String8 str1);
FUNCTION String8 string8_chop(String8 str, u64 amount);
FUNCTION String8 string8_chop_last_slash(String8 str);
FUNCTION String8 string8_chop_last_period(String8 str);

// String16 Creation
FUNCTION String16 string16(u16 *str, u64 length);
FUNCTION String16 string16_copy(MemoryArena *arena, String16 str);
FUNCTION String16 string16_from_wstr(wchar_t *wstr);

// String32 Creation
FUNCTION String32 string32(u32 *str, u64 length);

// String Conversions
FUNCTION char*    cstr_from_string8(MemoryArena  *arena, String8  str);

FUNCTION wchar_t* wstr_from_string8( MemoryArena *arena, String8  str);
FUNCTION wchar_t* wstr_from_string16(MemoryArena *arena, String16 str);

FUNCTION String8  string8_from_string16(MemoryArena *arena, String16 str);
FUNCTION String8  string8_from_string32(MemoryArena *arena, String32 str);

FUNCTION String16 string16_from_string8( MemoryArena *arena, String8  str);
FUNCTION String16 string16_from_string32(MemoryArena *arena, String32 str);

FUNCTION String32 string32_from_string8( MemoryArena *arena, String8  str);
FUNCTION String32 string32_from_string16(MemoryArena *arena, String16 str);

//------------------------------------------------------------------------
// STRING LISTS
//------------------------------------------------------------------------
STRUCT(String8ListNode) {
    String8ListNode *prev;
    String8ListNode *next;
    String8 string;
};

STRUCT(String8List) {
    String8ListNode *first;
    String8ListNode *last;
    u64 nodeCount;
    u64 totalSize;
};

FUNCTION void string8_list_push_ex(String8List *list, String8 string, String8ListNode *node);
FUNCTION void string8_list_push(MemoryArena *arena, String8List *list, String8 string);
FUNCTION void string8_list_remove(String8List *list, String8ListNode **node);

FUNCTION String8List string8_split(MemoryArena *arena, String8 string, u8 *splitChars, u32 splitCharCount);
FUNCTION String8     string8_list_combine_all(MemoryArena *arena, String8List *list);

FUNCTION s64     string8_list_get_index(String8List list, String8 str);
FUNCTION String8 string8_list_get_string_from_idx(String8List list, u64 idx);

//------------------------------------------------------------------------
// OS
//------------------------------------------------------------------------
typedef u64 OS_Handle;

ENUM(CPU_Vendor) {
    CPU_VENDOR_UNKNOWN = 0,
    CPU_VENDOR_INTEL,
    CPU_VENDOR_AMD,
};

FLAGS(CPU_Flags) {
    CPU_FLAG_SSE      = (1 << 0),
    CPU_FLAG_SSE2     = (1 << 1),
    CPU_FLAG_SSE3     = (1 << 2),
    CPU_FLAG_SSSE3    = (1 << 3),
    CPU_FLAG_SSE4P1   = (1 << 4),
    CPU_FLAG_FMA      = (1 << 5),
    CPU_FLAG_SSE4P2   = (1 << 6),
    CPU_FLAG_AVX      = (1 << 7),
    CPU_FLAG_AVX2     = (1 << 8),
    CPU_FLAG_AVX512F  = (1 << 9),
    CPU_FLAG_AVX512PF = (1 << 10),
    CPU_FLAG_AVX512ER = (1 << 11),
    CPU_FLAG_AVX512CD = (1 << 12),
};

STRUCT(CPU_Info) {
    CPU_Vendor vendor;
    CPU_Flags  flags;
    u32        physicalCoreCount;
    u32        logicalCoreCount;
};

STRUCT(OS_Info) {
    b32 isInitialized;
    u32 virtualMemoryPageSize;
    u32 displayRefreshRateHz;
    f32 displayRefreshRateSec;
    u32 mainThreadID;

    String8 binaryPath;
    String8 userPath;
    String8 tempPath;
    String8 workingPath;

    CPU_Info cpu;
};

FUNCTION void os_get_cpu_info(CPU_Info *info);
FUNCTION void os_init(void);

//------------------------------------------------------------------------
// BASIC MEMORY OPS
//------------------------------------------------------------------------
FUNCTION void* os_mem_reserve(u64 sizeInBytes);
FUNCTION void  os_mem_commit(void *memory, u64 sizeInBytes);
FUNCTION void  os_mem_decommit(void *memory, u64 sizeInBytes);
FUNCTION void  os_mem_release(void *memory);
void  os_mem_zero(void *memory, u64 sizeInBytes);
void  os_mem_copy(void *dest, void *src, u64 sizeInBytes);

//------------------------------------------------------------------------
// PATH QUERIES
//------------------------------------------------------------------------
FUNCTION String8 os_get_binary_path(MemoryArena *arena);
FUNCTION String8 os_get_user_data_path(MemoryArena *arena);
FUNCTION String8 os_get_temp_data_path(MemoryArena *arena);
FUNCTION String8 os_get_working_path(MemoryArena *arena);

//---------------------------------------------------------------------------
// TIME
//---------------------------------------------------------------------------
STRUCT(OS_DateTime) {
    u16 year;
    u16 month;
    u16 day;
    u16 hour;
    u16 minute;
    u16 second;
    u16 millisecond;
    u16 microsecond;
};

FUNCTION OS_DateTime os_get_datetime_UTC(void);
FUNCTION OS_DateTime os_get_datetime_local(void);

//---------------------------------------------------------------------------
// COPY TO/FROM CLIPBOARD
//---------------------------------------------------------------------------
FUNCTION void os_copy_string8_to_clipboard( String8  str);
FUNCTION void os_copy_string16_to_clipboard(String16 str);
FUNCTION void os_copy_string32_to_clipboard(String32 str);

FUNCTION String8  os_copy_string8_from_clipboard( MemoryArena *arena);
FUNCTION String16 os_copy_string16_from_clipboard(MemoryArena *arena);
FUNCTION String32 os_copy_string32_from_clipboard(MemoryArena *arena);

//------------------------------------------------------------------------
// LOADING LIBRARIES
//------------------------------------------------------------------------
FUNCTION OS_Handle os_lib_load(String8 fileName);
FUNCTION VoidFn*   os_lib_get_proc(OS_Handle lib, char *procName);
FUNCTION void      os_lib_release(OS_Handle lib);

//------------------------------------------------------------------------
// THREADING
//------------------------------------------------------------------------
FUNCTION OS_Handle os_create_thread(OS_ThreadProc *proc, void *data);
FUNCTION void      os_wait_for_one_thread_to_finish(OS_Handle thread);
FUNCTION void      os_wait_for_all_threads_to_finish(OS_Handle *threads, u64 nThreads);
FUNCTION s32       os_interlocked_increment(s32 volatile *value);
FUNCTION void      os_close_thread_handle(OS_Handle handle);

FUNCTION OS_Handle os_create_mutex(void);
FUNCTION void      os_release_mutex(OS_Handle mutex);
FUNCTION void      os_request_mutex(OS_Handle mutex);
FUNCTION void      os_close_mutex(OS_Handle mutex);

//------------------------------------------------------------------------
// TIMING
//------------------------------------------------------------------------
FUNCTION void os_sleep_ms(u64 time);

//------------------------------------------------------------------------
// BASIC FILE OPS
//------------------------------------------------------------------------
ENUM(OS_FileMode) {
    OS_FILE_READ = 0,
    OS_FILE_WRITE,
};

FUNCTION OS_Handle os_file_open(String8 fileName, OS_FileMode fileMode);
FUNCTION u64       os_file_op(OS_Handle fileID, void *data, u64 size, OS_FileMode fileMode);
FUNCTION void      os_file_close(OS_Handle fileID);
FUNCTION u64       os_file_get_size(OS_Handle fileID);
FUNCTION void      os_file_read_block(OS_Handle fileID, void *blockData, u64 blockSize);
FUNCTION void      os_file_write_block(OS_Handle fileID, void *blockData, u64 blockSize);
FUNCTION String8   os_file_read_all(MemoryArena *arena, String8 fileName);

FUNCTION b32 os_file_delete(String8 fileName);
FUNCTION b32 os_file_rename(String8 fileNameOld, String8 fileNameNew);
FUNCTION b32 os_file_create_directory(String8 path);
FUNCTION b32 os_file_delete_directory(String8 path);

//------------------------------------------------------------------------
// FILE ITERATION
//------------------------------------------------------------------------
STRUCT(OS_FileIter) {
    u8 v[640];
};

FLAGS(OS_FilePropertyFlags) {
    FILE_PROPERTY_IS_FOLDER = (1 << 0),
};

FLAGS(OS_DataAccessFlags) {
    DATA_ACCESS_READ    = (1 << 0),
    DATA_ACCESS_WRITE   = (1 << 1),
    DATA_ACCESS_EXECUTE = (1 << 2),
};

STRUCT(OS_FileProperties) {
    u64                  sizeInBytes;
    OS_DateTime          creationTime;
    OS_DateTime          lastAccessTime;
    OS_DateTime          lastWriteTime;
    OS_FilePropertyFlags filePropertyFlags;
    OS_DataAccessFlags   dataAccessFlags;
};

FUNCTION OS_FileIter os_file_iter_start(String8 path);
FUNCTION b32         os_file_iter_next(MemoryArena *arena, OS_FileIter *fileIter, String8 *fileNameOut, OS_FileProperties *properties);
FUNCTION void        os_file_iter_end(OS_FileIter *fileIter);
FUNCTION String8List os_get_filenames_in_dir(MemoryArena *arena, String8 dir);

//------------------------------------------------------------------------
// COMPRESSION
//------------------------------------------------------------------------
FUNCTION String8 os_zip  (MemoryArena *arena, String8 uncompressed);
FUNCTION String8 os_unzip(MemoryArena *arena, String8 compressed);

//------------------------------------------------------------------------
// SORT
//------------------------------------------------------------------------
typedef int64_t (*qsort_cmp_fn)(void*,void*);
FUNCTION void quicksort(void *v, int64_t sizeOfElement, int64_t left, int64_t right, qsort_cmp_fn fn);

#endif // AOC_LIB_H