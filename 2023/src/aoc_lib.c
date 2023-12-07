//------------------------------------------------------------------------
// MEMORY ARENA
//------------------------------------------------------------------------
static u64 
_mem_get_aligned_size(u64 sizeInBytes) {
    u64 result = (sizeInBytes % 4 == 0) ? sizeInBytes : sizeInBytes + 4 - sizeInBytes % 4;
    return(result);
}

static u64 
_align_forward(u64 ptr, u64 align) {
    ASSERT(IS_POWER_OF_2(align));

    u64 p = ptr;
    u64 a = (u64)align;
    u64 modulo = p & (a - 1); // same as p % a, but faster as a is a power of two.

    if (modulo != 0) {
        p += a - modulo;
    }
    return(p);
}

FUNCTION MemoryArena 
mem_arena_init(u64 reserveSizeInBytes) {
    MemoryArena result = {0};
    result.base = os_mem_reserve(reserveSizeInBytes);
    if (result.base) {
        result.usedBytes = 0;
        result.commitSizeInBytes = 0;
        result.capacityInBytes = reserveSizeInBytes;
        result.commitSizeInBytes = os_info.virtualMemoryPageSize;
    }
    return(result);
}

FUNCTION void 
mem_arena_free(MemoryArena *arena) {
    ASSERT(arena); 
    os_mem_release(arena->base);
    arena->base = NULL;
    arena->usedBytes = 0;
    arena->capacityInBytes = 0;
}

FUNCTION void* 
_mem_arena_push(MemoryArena *arena, u64 sizeInBytes) {
    ASSERT(arena);
    if (sizeInBytes > arena->capacityInBytes) {
        // NOTE(tim): not handled.
    }

    u64 oldP = (u64)((u8*)arena->base + arena->usedBytes);
    u64 newP = _align_forward(oldP + sizeInBytes, 32);
    u64 alignedSizeInBytes = newP - oldP;

    if (!IS_ALIGNED(oldP, 32) || !IS_ALIGNED(newP, 32)) {
        DEBUG_BREAK();
    }

    if ((arena->usedBytes + alignedSizeInBytes) > arena->capacityInBytes) {
        // NOTE(tim): not handled.
    }
    
    if ((arena->usedBytes + alignedSizeInBytes) > arena->commitedBytes) {
        u64 sizeToCommit = alignedSizeInBytes + arena->commitSizeInBytes-1;
        sizeToCommit -= sizeToCommit % arena->commitSizeInBytes;
        os_mem_commit((u8*)arena->base + arena->commitedBytes, sizeToCommit);
        arena->commitedBytes += sizeToCommit;
    }

    void *result = (void*)((u8*)arena->base + arena->usedBytes);

    ASSERT(IS_ALIGNED(result, 32));

    arena->usedBytes += alignedSizeInBytes;
    return(result);
}

FUNCTION void* 
_mem_arena_push_and_zero(MemoryArena *arena, u64 sizeInBytes) {
    ASSERT(arena);
    void *result = _mem_arena_push(arena, sizeInBytes);
    os_mem_zero(result, sizeInBytes);
    return(result);
}

FUNCTION void 
mem_arena_pop(MemoryArena *arena, u64 sizeInBytes) {
    if (sizeInBytes >= arena->usedBytes) {
        arena->usedBytes = 0;
    }
    else {
        arena->usedBytes -= sizeInBytes;
    }
}

FUNCTION void 
mem_arena_reset(MemoryArena *arena) {
    ASSERT(arena);
    arena->usedBytes = 0;
}

FUNCTION u64 
mem_arena_get_reset_point(MemoryArena *arena) {
    ASSERT(arena);
    return(arena->usedBytes);
}

FUNCTION void 
mem_arena_reset_to(MemoryArena *arena, u64 resetPoint) {
    ASSERT(arena);
    arena->usedBytes = resetPoint;
}


FUNCTION MemoryArena 
mem_arena_get_scratch(void) {
    MemoryArena result = {0};
    // Allocate on first call
    if (scratchArenaPool.arenas[0].base == NULL) {
        for (u64 idx = 0; idx < MEM_SCRATCH_ARENA_COUNT; ++idx) {
            scratchArenaPool.arenas[idx] = mem_arena_init(MEM_DEFAULT_RESERVE_SIZE);
        }
    }

    for (u64 idx = 0; idx < MEM_SCRATCH_ARENA_COUNT; ++idx) {
        if (!scratchArenaPool.isLocked[idx]) {
            result = scratchArenaPool.arenas[idx];
            scratchArenaPool.isLocked[idx] = true;
            break;
        }
    }
    return(result);
}

FUNCTION void 
mem_arena_release_scratch(MemoryArena *scratch) {
    for (u64 idx = 0; idx < MEM_SCRATCH_ARENA_COUNT; ++idx) {
        if (scratch->base == scratchArenaPool.arenas[idx].base) {
            scratchArenaPool.isLocked[idx] = false;
            mem_arena_reset(scratch);
        }
    }
}


//------------------------------------------------------------------------
// STRINGS
//------------------------------------------------------------------------
// Basic C-String Operations
FUNCTION u64 
cstr_length(char *cstr) {
    u64 result = 0;
    while (cstr[result] != '\0') { result++; };
    return(result);
}

FUNCTION u64 
wstr_length(wchar_t *wstr) {
    u64 result = 0;
    while (wstr[result] != L'\0') {
        if (wstr[++result] == L'\0') {return(result);};
        if (wstr[++result] == L'\0') {return(result);};
        if (wstr[++result] == L'\0') {return(result);};
        ++result;
    };
    return(result);
}

FUNCTION b32 
cstr_compare(char *strA, char *strB) {
    b32 result = (strA == strB);
    if (strA && strB) {
        while (*strA && *strB && (*strA == *strB)) {
            ++strA;
            ++strB;
        }
        result = ((*strA == 0) && (*strB == 0));
    }
    return(result);
}

FUNCTION b32 
wstr_compare(wchar_t *strA, wchar_t *strB) {
    b32 result = (strA == strB);
    if (strA && strB) {
        while (*strA && *strB && (*strA == *strB)) {
            ++strA;
            ++strB;
        }
        result = ((*strA == 0) && (*strB == 0));
    }
    return(result);
}

// Character Operations
FUNCTION b32 
is_alpha(char c) {
    b32 isAlphaLowerCase = (c  >= 'a' && c <= 'z');
    b32 isAlphaUpperCase = (c  >= 'A' && c <= 'Z');
    return(isAlphaLowerCase || isAlphaUpperCase);
}

FUNCTION b32 
is_numeric(char c) {
    b32 isNumeric = (c  >= '0' && c <= '9');
    return(isNumeric);
}

FUNCTION b32 
is_alphanumeric(char c) {
    return(is_alpha(c) || is_numeric(c));
}

FUNCTION b32 
is_whitespace(char c) {
    return((c == ' ') || (c == '\t') || (c == '\r'));
}

FUNCTION u8 
to_lower(u8 c) {
    b32 isAlphaUpperCase = (c  >= 'A' && c <= 'Z');
    if (isAlphaUpperCase) {
        c += 32;
    }
    return(c);
}

FUNCTION u8 
to_upper(u8 c) {
   b32 isAlphaLowerCase = (c  >= 'a' && c <= 'z');
    if (isAlphaLowerCase) {
        c -= 32;
    }
    return(c);
}

FUNCTION s64 
string_to_int64(u8 *ptr, u64 length) {
    s64 result = 0;
    s64 mult   = 1;
    for (s64 idx = (length-1); idx >= 0; --idx) {
        u8 val  = (ptr[idx] - (u8)'0');
        result += (s64)val * mult;
        mult   *= 10;
    }
    return(result);
}

// UTF-8
FUNCTION s32 
utf8_decode(u8 *str, u32 *codepoint) {
    u8 codeLengths[] = {
        1, 1, 1, 1, // 000xx
        1, 1, 1, 1,
        1, 1, 1, 1, 
        1, 1, 1, 1,
        0, 0, 0, 0, // 100xx
        0, 0, 0, 0, 
        2, 2, 2, 2, // 110xx
        3, 3,       // 1110x
        4,          // 11110
        0           // 11111
    };
    // Masks are in order: 1111 1111, 0111 1111, 0001 1111, 0000 1111, 0000 0111
    s32 leadingCodeMask[] = {0x00, 0x7F, 0x1F, 0x0F, 0x07};
    s32 finalShift[]      = {0, 18, 12, 6, 0};

    *codepoint = 0;
    s32 codepointLength = 0;

    codepointLength = codeLengths[str[0] >> 3];
    if (codepointLength > 0) {
        *codepoint  = (u32)(str[0] & leadingCodeMask[codepointLength]) << 18;
        switch (codepointLength) {
            case 4: *codepoint |= ((str[3] & 0x3F) << 0);
            case 3: *codepoint |= ((str[2] & 0x3F) << 6);
            case 2: *codepoint |= ((str[1] & 0x3F) << 12);
        }
    }
    *codepoint >>= finalShift[codepointLength];
    return(codepointLength);
}

FUNCTION u64 
utf8_get_encoded_size(u32 codepoint) {
    if (codepoint <= 0x007F) {
        return 1;
    }
    else if (codepoint <= 0x07FF) {
        return 2;
    }
    else if (codepoint <= 0xFFFF) {
        return 3;
    }
    else if (codepoint <= 0x10FFFF) {
        return 4;
    }
    else {
        ASSERT(false);
        return(0);
    }
}

FUNCTION u8* 
utf8_encode(u8 *buffer, u32 codepoint) {
    if (codepoint <= 0x007F) {
        buffer[0] = (u8)(codepoint);
        buffer += 1;
    }
    else if (codepoint <= 0x07FF) {
        buffer[0] = (u8)(0xC0 | (codepoint >> 6));
        buffer[1] = (u8)(0x80 | (codepoint & 0x3F));
        buffer += 2;
    }
    else if (codepoint <= 0xFFFF) {
        buffer[0] = (u8)(0xE0 | (codepoint >> 12));
        buffer[1] = (u8)(0x80 | ((codepoint >> 6) & 0x3F));
        buffer[2] = (u8)(0x80 | (codepoint & 0x3F));
        buffer += 3;
    }
    else if (codepoint <= 0x10FFFF) {
        buffer[0] = (u8)(0xF0 | (codepoint  >> 18));
        buffer[1] = (u8)(0x80 | ((codepoint >> 12) & 0x3F));
        buffer[2] = (u8)(0x80 | ((codepoint >> 6)  & 0x3F));
        buffer[3] = (u8)(0x80 | (codepoint & 0x3F));
        buffer += 4;
    }
    else {
        ASSERT(false);
    }
    return(buffer);
}

// UTF-16
FUNCTION s32 
utf16_decode(u16 *str, u32 *codepoint) {
    s32 codepointLength = 1;
    if (str[0] < 0xD800 || str[0] > 0xDFFF) {
        *codepoint = str[0];
        codepointLength = 1;
    }
    else if (str[0] >= 0xD800 && str[0] <= 0xDBFF &&
             str[1] >= 0xDC00 && str[1] <= 0xDFFF) 
    {
        u16 w1 = str[0] - 0xD800;
        u16 w2 = str[1] - 0xDC00;
        *codepoint = (w1 << 10) | w2  + 0x10000;
        codepointLength = 2;
    }
    return(codepointLength);
}

FUNCTION u64 
utf16_get_encoded_size(u32 codepoint) {
    u64 codepointLength = 0;
    if (codepoint < 0xD800) {
        codepointLength = 1;
    }
    else if (codepoint <= 0x10FFFF) 
    {
        codepointLength = 2;
    }
    return(codepointLength);
}

FUNCTION u16* 
utf16_encode(u16 *buffer, u32 codepoint) {
    if (codepoint < 0xD800) {
        buffer[0] = (u16)codepoint;
        buffer++;
    }
    else if (codepoint <= 0x10FFFF) 
    {
        u32 uPrime = codepoint - 0x10000;
        buffer[0] = (u16)(0xD800 | (uPrime >> 10));
        buffer[1] = (u16)(0xDC00 | (uPrime & 0x03FF));
        buffer+=2;
    }
    return(buffer); 
}

// String8 Creation
FUNCTION String8 
string8( u8  *str, u64 length) {
    String8 result = {str, length, length * sizeof(u8)};
    return(result);
}

FUNCTION String8 
string8_copy(MemoryArena *arena, String8 str) {
    u8 *buffer = mem_arena_push(arena, str.length, u8);
    os_mem_copy(buffer, str.buffer, str.size);
    String8 result = string8(buffer, str.length);
    return(result);
}

FUNCTION String8 
string8_from_cstr(char *cstr) {
    String8 result = {(u8*)cstr, cstr_length(cstr)};
    return(result);
}

FUNCTION String8 
string8_from_wstr(MemoryArena *arena, wchar_t *wstr) {
    String16 str16 = string16_from_wstr(wstr);
    String8 result = string8_from_string16(arena, str16);
    return(result);
}

FUNCTION String8 
string8_from_range(u8 *start, u8 *end) {
    String8 result = {start, (u64)(end - start)};
    return(result);
}

String8 
string8_snprintf(MemoryArena *arena, const char *format, ...) {
    va_list args;
    va_start(args, format);

    String8 result = {0};
    char tmp;
    u64 formattedBufferSize = stbsp_vsnprintf(&tmp, 1, format, args);

    u8 *buffer = mem_arena_push(arena, formattedBufferSize + 1, u8);
    stbsp_vsnprintf((char*)buffer, formattedBufferSize + 1, format, args);

    result = string8(buffer, formattedBufferSize);

    va_end(args);
    return(result);
}

// String8 Comparison
FUNCTION b32 
string8_compare(String8 strA, String8 strB) {
    b32 result = strA.length == strB.length;
    if (result) {
        for (s64 idx = 0; idx < strA.length; ++idx) {
            if (strA.buffer[idx] != strB.buffer[idx]) {
                result = false;
                break;
            }
        }
    }
    return(result);
}

// String8 Manipulation?
FUNCTION String8 
string8_concat(MemoryArena *arena, String8 str0, String8 str1) {
    String8 result = {0};
    u64 combinedSize = str0.length + str1.length;
    u8 *buffer = mem_arena_push_and_zero(arena, combinedSize + 1, u8);
    os_mem_copy(buffer, str0.buffer, str0.length);
    os_mem_copy(buffer + str0.length, str1.buffer, str1.length);
    result = string8(buffer, combinedSize);
    return(result);
}

// String8 Manipulation?
FUNCTION String8 
string8_chop(String8 str, u64 amount) {
    String8 result = {str.buffer, str.length - CLAMPU(amount, str.length)};
    return(result);
}

FUNCTION String8 
string8_chop_last_slash(String8 str) {
    String8 result = str;
    if (str.length > 0) {
        u64 slashIdx = 0; 
        for (s64 idx = str.length - 1; idx >= 0; --idx) {
            if (str.buffer[idx] == '/' || str.buffer[idx] == '\\') {
                slashIdx = idx;
                break;
            }
        }
        result.length = slashIdx;
    }
    return(result);
}

FUNCTION String8 
string8_chop_last_period(String8 str) {
    String8 result = str;
    if (str.length > 0) {
        u64 periodIdx = 0; 
        for (s64 idx = str.length - 1; idx >= 0; --idx) {
            if (str.buffer[idx] == '.') {
                periodIdx = idx;
                break;
            }
        }
        result.length = periodIdx;
    }
    return(result);
}

FUNCTION String8 
string8_to_lower(MemoryArena *arena, String8 str) {
    String8 result = string8_copy(arena, str);

    for (u64 idx = 0; idx < result.length; ++idx) {
        result.buffer[idx] = to_lower(result.buffer[idx]);
    }
    return(result);
}

// String16 Creation
FUNCTION String16 
string16(u16 *str, u64 length) {
    String16 result = {str, length, length * sizeof(u16)};
    return(result);
}

FUNCTION String16 
string16_from_wstr(wchar_t *wstr) {
    String16 result = string16((u16*)wstr, wstr_length(wstr));
    return(result);
}

FUNCTION String16
string16_copy(MemoryArena *arena, String16 str) {
    u16 *buffer = mem_arena_push(arena, str.length, u16);
    os_mem_copy(buffer, str.buffer, str.size);
    String16 result = string16(buffer, str.length);
    return(result);
}

// String32 Creation
FUNCTION String32 
string32(u32 *str, u64 length) {
    String32 result = {str, length, length * sizeof(u32)};
    return(result);
}

// String Conversions
FUNCTION char* 
cstr_from_string8(MemoryArena *arena, String8 str) {
    char *result = mem_arena_push(arena, str.length + 1, char);
    os_mem_copy(result, str.buffer, str.length);
    result[str.length] = '\0';
    return(result);
}

FUNCTION wchar_t* 
wstr_from_string8(MemoryArena *arena, String8 str) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 str16 = string16_from_string8(&scratch, str);
    wchar_t *result = wstr_from_string16(arena, str16);
    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION wchar_t* 
wstr_from_string16(MemoryArena *arena, String16 str) {
    wchar_t *result = mem_arena_push(arena, str.length + 1, wchar_t);
    os_mem_copy(result, str.buffer, str.length*sizeof(u16));
    result[str.length] = L'\0';
    return(result);
}

FUNCTION String8 
string8_from_string16(MemoryArena *arena, String16 str) {
    String8 result = {0};

    u64 bufferIdx = 0;
    u64 encodedSize = 0;
    while (bufferIdx < str.length) {
        u32 codepoint;
        bufferIdx += utf16_decode(str.buffer, &codepoint);
        encodedSize += utf8_get_encoded_size(codepoint);
    }
    u8* memory = mem_arena_push(arena, encodedSize, u8);
    if (memory) {
        u8 *ptr = memory;
        bufferIdx = 0;
        while (bufferIdx < str.length) {
            u32 codepoint;
            bufferIdx += utf16_decode(str.buffer + bufferIdx, &codepoint);
            ptr = utf8_encode(ptr, codepoint);
        }
        result.buffer = memory;
        result.length = encodedSize;
        result.size   = encodedSize;
    }
    return(result);
}

FUNCTION String8 
string8_from_string32(MemoryArena *arena, String32 str) {
    String8 result = {0};

    u64 bufferIdx = 0;
    u64 encodedSize = 0;
    while (bufferIdx < str.length) {
        encodedSize += utf8_get_encoded_size(str.buffer[bufferIdx++]);
    }
    u8* memory = mem_arena_push(arena, encodedSize, u8);
    if (memory) {
        u8 *ptr = memory;
        bufferIdx = 0;
        while (bufferIdx < str.length) {
            ptr = utf8_encode(ptr, str.buffer[bufferIdx++]);
        }
        result.buffer = memory;
        result.length = encodedSize;
        result.size   = encodedSize;
    }
    return(result);
}

FUNCTION String16 
string16_from_string8(MemoryArena *arena, String8 str) {
    String16 result = {0};

    u64 bufferIdx = 0;
    u64 encodedSize = 0;
    while (bufferIdx < str.length) {
        u32 codepoint;
        bufferIdx += utf8_decode(str.buffer, &codepoint);
        encodedSize += utf16_get_encoded_size(codepoint);
    }
    u16* memory = mem_arena_push(arena, encodedSize, u16);
    if (memory) {
        u16 *ptr = memory;
        bufferIdx = 0;
        while (bufferIdx < str.length) {
            u32 codepoint;
            bufferIdx += utf8_decode(str.buffer + bufferIdx, &codepoint);
            ptr = utf16_encode(ptr, codepoint);
        }
        result.buffer = memory;
        result.length = encodedSize;
        result.size   = encodedSize * sizeof(u16);
    }
    return(result);
}

FUNCTION String16 
string16_from_string32(MemoryArena *arena, String32 str) {
    String16 result = {0};

    u64 bufferIdx = 0;
    u64 encodedSize = 0;
    while (bufferIdx < str.length) {
        encodedSize += utf16_get_encoded_size(str.buffer[bufferIdx++]);
    }
    u16* memory = mem_arena_push(arena, encodedSize, u16);
    if (memory) {
        u16 *ptr = memory;
        bufferIdx = 0;
        while (bufferIdx < str.length) {
            ptr = utf16_encode(ptr, str.buffer[bufferIdx++]);
        }
        result.buffer = memory;
        result.length = encodedSize;
        result.size   = encodedSize * sizeof(u16);
    }
    return(result);
}

FUNCTION String32 
string32_from_string8( MemoryArena *arena, String8 str) {
    String32 result = {0};

    u64 bufferIdx = 0;
    u64 nCodepoints = 0;
    while (bufferIdx < str.length) {
        u32 codepoint;
        bufferIdx += utf8_decode(str.buffer, &codepoint);
        nCodepoints++;
    }

    u32* memory = mem_arena_push(arena, nCodepoints, u32);
    if (memory) {
        bufferIdx = 0;
        for (u32 glyphIdx = 0; glyphIdx < nCodepoints; ++glyphIdx) {
            u32 codepoint;
            bufferIdx += utf8_decode(str.buffer + bufferIdx, &codepoint);
            memory[glyphIdx] = codepoint;
        }
        result.buffer = memory;
        result.length = nCodepoints;
        result.size   = nCodepoints * sizeof(u32);
    }
    return(result);
}

FUNCTION String32 
string32_from_string16(MemoryArena *arena, String16 str) {
    String32 result = {0};

    u64 bufferIdx = 0;
    u64 nCodepoints = 0;
    while (bufferIdx < str.length) {
        u32 codepoint;
        bufferIdx += utf16_decode(str.buffer, &codepoint);
        nCodepoints++;
    }

    u32* memory = mem_arena_push(arena, nCodepoints, u32);
    if (memory) {
        bufferIdx = 0;
        for (u32 glyphIdx = 0; glyphIdx < nCodepoints; ++glyphIdx) {
            u32 codepoint;
            bufferIdx += utf16_decode(str.buffer + bufferIdx, &codepoint);
            memory[glyphIdx] = codepoint;
        }
        result.buffer = memory;
        result.length = nCodepoints;
        result.size   = nCodepoints * sizeof(u32);
    }
    return(result);
}

//------------------------------------------------------------------------
// STRING LISTS
//------------------------------------------------------------------------
FUNCTION void 
string8_list_push_ex(String8List *list, String8 string, String8ListNode *node) {
    node->string = string;
    DLL_PUSH_BACK(list->first, list->last, node);
    list->nodeCount++;
    list->totalSize += string.length;
}

FUNCTION void 
string8_list_push(MemoryArena *arena, String8List *list, String8 string) {
    String8ListNode *node = mem_arena_push(arena, 1, String8ListNode);
    string8_list_push_ex(list, string, node);
}

FUNCTION void 
string8_list_remove(String8List *list, String8ListNode **node) {
    u64 len = (*node)->string.length;
    DLL_REMOVE(list->first, list->last, *node);
    (*node) = NULL;
    list->nodeCount--;
    list->totalSize -= len;
}

FUNCTION String8List 
string8_split(MemoryArena *arena, String8 string, u8 *splitChars, u32 splitCharCount) {
    String8List result = {0};

    u8 *currentChar = string.buffer;
    u8 *startOfCurrentWord = string.buffer;
    for (; currentChar < (string.buffer + string.length); ++currentChar) {

        b32 isSplitChar = false;
        for (u32 splitIdx = 0; splitIdx < splitCharCount; ++splitIdx) {
            if (*currentChar == splitChars[splitIdx]) {
                isSplitChar = true;
                break;
            }
        }

        if (isSplitChar) {
            if (currentChar > startOfCurrentWord) {
                string8_list_push(arena, &result, string8_from_range(startOfCurrentWord, currentChar));
            }
            startOfCurrentWord = currentChar + 1;
        }
    }
    if (currentChar > startOfCurrentWord) {
        string8_list_push(arena, &result, string8_from_range(startOfCurrentWord, currentChar));
    }
    return(result);
}

FUNCTION String8 
string8_list_combine_all(MemoryArena *arena, String8List *list) {
    u8 *str = mem_arena_push(arena, list->totalSize, u8);
    String8 result = string8(str, list->totalSize);

    String8ListNode *node = list->first;
    s64 currentSize = 0;
    while (node) {
        os_mem_copy(str + currentSize, node->string.buffer, node->string.length);
        node = node->next;
    }

    return(result);
}

FUNCTION s64 
string8_list_get_index(String8List list, String8 str) {
    s64 idx = 0;
    String8ListNode *node = list.first;
    while(node) {
        if (string8_compare(str, node->string)) {
            return(idx);
        }
        idx++;
        node = node->next;
    }
    return(-1);
}

FUNCTION String8 
string8_list_get_string_from_idx(String8List list, u64 idx) {
    String8 result = {0};
    if (idx < list.nodeCount) {
        String8ListNode *node = list.first;
        for (u64 i = 0; i < idx; ++i) {
            node = node->next;
        }
        result = node->string;
    }
    return(result);
}

FUNCTION u64 
string8_to_u64(String8 str, u64 base) {
    u64 result = 0;
    if (base == 16) {    
        u64 mult = pow_f32(base,(str.length - 1));
        for (u64 idx = 0; idx < str.length; ++idx) {
            u64 val = to_lower(str.buffer[idx]) - (u8)'0';
            if (val > 9) {
                val -= 39;
            }
            result += val * mult;
            mult /= base;
        }  
    }
    else {
        u64 mult = pow_f32(base,(str.length - 1));
        for (u64 idx = 0; idx < str.length; ++idx) {
            result += (str.buffer[idx] - (u8)'0') * mult;
            mult /= base;
        } 
    }
    return(result);
}

#pragma comment(lib, "User32")
#pragma comment(lib, "Gdi32")
#pragma comment(lib, "Userenv")
#pragma comment(lib, "Kernel32")
#pragma  comment(lib, "cabinet")

#ifndef NOMINMAX
#define NOMINMAX            1
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN 1
#endif 
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        1
#endif
#pragma warning(push, 0)
#include <windows.h>
#include <windowsx.h>
#include <timeapi.h>
#include <userenv.h>
#include <fileapi.h>
#include <intrin.h> // _BitScanForward, __cpuid
#include <compressapi.h>
#pragma warning(pop)
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN
#undef WIN32_MEAN_AND_LEAN
#undef VC_EXTRALEAN
#undef min
#undef max    

#include <stdarg.h>
#include <stdio.h>

UNION(CPUID_Data) {
    struct {s32 eax, ebx, ecx, edx;};
    s32 v[4];
};

static DWORD
win32_count_set_bits(ULONG_PTR bitMask) {
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;

    for (DWORD i = 0; i <= LSHIFT; ++i) {
        bitSetCount += ((bitMask & bitTest) ? 1 : 0);
        bitTest /= 2;
    }
    return(bitSetCount);
}

FUNCTION void 
os_get_cpu_info(CPU_Info *info) {
    CPUID_Data cpuInfo;

    __cpuid((int*)cpuInfo.v, 0);
    s32 nCpuIds = cpuInfo.eax;

    char vendorID[13] = {0};
    *(s32*)(vendorID+0) = cpuInfo.ebx;
    *(s32*)(vendorID+4) = cpuInfo.edx;
    *(s32*)(vendorID+8) = cpuInfo.ecx;

    info->vendor = CPU_VENDOR_UNKNOWN;
    if      (cstr_compare(vendorID,(char*)"GenuineIntel")) { info->vendor = CPU_VENDOR_INTEL;}
    else if (cstr_compare(vendorID,(char*)"AuthenticAMD")) { info->vendor = CPU_VENDOR_AMD;}

    if (nCpuIds >= 1) {
        __cpuidex((int*)cpuInfo.v, 1, 0);
    }

    info->flags |= ((cpuInfo.edx & (1 << 25)) ? CPU_FLAG_SSE    : 0);
    info->flags |= ((cpuInfo.edx & (1 << 26)) ? CPU_FLAG_SSE2   : 0);
    info->flags |= ((cpuInfo.ecx & (1 << 0))  ? CPU_FLAG_SSE3   : 0);
    info->flags |= ((cpuInfo.ecx & (1 << 9))  ? CPU_FLAG_SSSE3  : 0);
    info->flags |= ((cpuInfo.ecx & (1 << 19)) ? CPU_FLAG_SSE4P1 : 0);
    info->flags |= ((cpuInfo.ecx & (1 << 20)) ? CPU_FLAG_SSE4P2 : 0);
    info->flags |= ((cpuInfo.ecx & (1 << 28)) ? CPU_FLAG_AVX    : 0);
    info->flags |= ((cpuInfo.ecx & (1 << 12)) ? CPU_FLAG_FMA    : 0);

    if (nCpuIds >= 7) {
        __cpuidex((int*)cpuInfo.v, 7, 0);
    }

    info->flags |= ((cpuInfo.ebx & (1 << 5))  ? CPU_FLAG_AVX2     : 0);
    info->flags |= ((cpuInfo.ebx & (1 << 16)) ? CPU_FLAG_AVX512F  : 0);
    info->flags |= ((cpuInfo.ebx & (1 << 26)) ? CPU_FLAG_AVX512PF : 0);
    info->flags |= ((cpuInfo.ebx & (1 << 27)) ? CPU_FLAG_AVX512ER : 0);
    info->flags |= ((cpuInfo.ebx & (1 << 28)) ? CPU_FLAG_AVX512CD : 0);

    // Get physical and logical core count.
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr    = NULL;
    DWORD bufferSize            = 0;
    DWORD byteOffset            = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount         = 0;
    DWORD processorCoreCount    = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    PCACHE_DESCRIPTOR cache;

    BOOL done = 0;
    while (!done) {
        DWORD rc = GetLogicalProcessorInformation(buffer, &bufferSize);
        if (rc == FALSE) {
            if (buffer) {
                VirtualFree(buffer, 0, MEM_RELEASE);
            }
            buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)VirtualAlloc(0, bufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (buffer == NULL) {
                // ERROR.
            }
        }
        else {
            done = TRUE;
        }
    }

    ptr = buffer;
    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= bufferSize) {
        switch (ptr->Relationship) {
            case RelationNumaNode: {
                numaNodeCount++;
            } break;
            case RelationProcessorCore: {
                processorCoreCount++;
                logicalProcessorCount += win32_count_set_bits(ptr->ProcessorMask);
            } break;
            case RelationCache: {
                cache = &ptr->Cache;
                if      (cache->Level == 1) { processorL1CacheCount++;}
                else if (cache->Level == 2) { processorL2CacheCount++;}
                else if (cache->Level == 3) { processorL3CacheCount++;}
            } break;
            case RelationProcessorPackage: {
                processorPackageCount++;
            } break;
            default: {
                // ERROR.
            } break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }
    VirtualFree(buffer, 0, MEM_RELEASE);
    info->physicalCoreCount = processorCoreCount;
    info->logicalCoreCount  = logicalProcessorCount;
}

FUNCTION void 
os_init(void) {
    if (!os_info.isInitialized) {
        // Get cpu info
        os_get_cpu_info(&os_info.cpu);

        // Get Virtual Memory Page Size
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        os_info.virtualMemoryPageSize = sysInfo.dwPageSize;

        // Get Monitor Refresh Rate
        DEVMODEA devmode = {0};
        EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &devmode);
        os_info.displayRefreshRateHz  = devmode.dmDisplayFrequency;
        os_info.displayRefreshRateSec = 1.0f / (f32)os_info.displayRefreshRateHz; 

        // Permanant Memory Arena
        os_arena = mem_arena_init(MEM_DEFAULT_RESERVE_SIZE);

        os_info.binaryPath  = os_get_binary_path(&os_arena);
        os_info.userPath    = os_get_user_data_path(&os_arena);
        os_info.tempPath    = os_get_temp_data_path(&os_arena);
        os_info.workingPath = os_get_working_path(&os_arena);

        // see pcg rand number generator
        // pcg_rand_seed(42u, 54u);

        os_info.isInitialized = true;
    }
}

// BASIC MEMORY OPS
FUNCTION void* 
os_mem_reserve(u64 sizeInBytes) {
    void *result = VirtualAlloc(0, sizeInBytes, MEM_RESERVE, PAGE_NOACCESS);
    return(result);
}

FUNCTION void 
os_mem_commit(void *memory, u64 sizeInBytes) {
    VirtualAlloc(memory, sizeInBytes, MEM_COMMIT, PAGE_READWRITE);
}

FUNCTION void 
os_mem_decommit(void *memory, u64 sizeInBytes) {
    VirtualFree(memory, sizeInBytes, MEM_DECOMMIT);
}

FUNCTION void 
os_mem_release(void *memory) {
    VirtualFree(memory, 0, MEM_RELEASE);
}

void 
os_mem_zero(void *memory, u64 sizeInBytes) {
    memory = SecureZeroMemory(memory, sizeInBytes);
}

void 
os_mem_copy(void *dest, void *src, u64 sizeInBytes) {
    CopyMemory(dest, src, sizeInBytes);
}

//------------------------------------------------------------------------
// PATH QUERIES
//------------------------------------------------------------------------
FUNCTION String8 
os_get_binary_path(MemoryArena *arena) {
    String8 result = {0};
    MemoryArena scratch = mem_arena_get_scratch();

    DWORD capacity = 2048;
    u16  *buffer   = NULL;
    DWORD size     = 0;
    for (u64 attempts = 0; attempts < 4; ++attempts, capacity *= 4) {
        buffer = mem_arena_push(&scratch, capacity, u16);
        size   = GetModuleFileNameW(0,  (LPWSTR)buffer, capacity);
        if (size == capacity && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            buffer = NULL;
            size   = 0;
        }
        else {
            break;
        }
    }
    
    if (buffer) {
        String8 fullPath = string8_from_string16(&scratch, string16(buffer, size));
        String8 binaryPath = string8_chop_last_slash(fullPath);
        result = string8_copy(arena, binaryPath);
    }

    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION String8
os_get_user_data_path(MemoryArena *arena) {
    String8 result = {0};
    MemoryArena scratch = mem_arena_get_scratch();

    HANDLE token = GetCurrentProcessToken();
    u16 *buffer = NULL;
    DWORD size    = 0;
    GetUserProfileDirectoryW(token, NULL, &size);
    if (size > 0) {
        buffer = mem_arena_push(&scratch, size, u16);
        GetUserProfileDirectoryW(token, (LPWSTR)buffer, &size);
    }

    if (buffer) {
        String8 fullPath = string8_from_string16(&scratch, string16(buffer, size));
        result = string8_copy(arena, fullPath);
    }

    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION String8 
os_get_temp_data_path(MemoryArena *arena) {
    String8 result = {0};
    MemoryArena scratch = mem_arena_get_scratch();

    u16 *buffer = 0;
    DWORD size  = GetTempPathW(0, NULL);
    if (size > 0) {
        buffer = mem_arena_push(&scratch, size, u16);
        size = GetTempPathW(size, (LPWSTR)buffer);
    }

    if (buffer) {
        String8 fullPath = string8_from_string16(&scratch, string16(buffer, size - 1));
        result = string8_copy(arena, fullPath);
    }

    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION String8 
os_get_working_path(MemoryArena *arena) {
    String8 result = {0};
    MemoryArena scratch = mem_arena_get_scratch();

    DWORD size  = GetCurrentDirectoryW(0, NULL);
    u16 *buffer = mem_arena_push(&scratch, size, u16);

    if (GetCurrentDirectoryW(size, (LPWSTR)buffer) == (size-1)) {
        result = string8_from_string16(arena, string16(buffer, size-1));
    }

    mem_arena_release_scratch(&scratch);
    return(result);
}

//---------------------------------------------------------------------------
// TIME
//---------------------------------------------------------------------------
FUNCTION OS_DateTime 
win32_datetime_from_systemtime(SYSTEMTIME *time) {
    OS_DateTime result    = {0};
    result.year        = time->wYear;
    result.month       = time->wMonth;
    result.day         = time->wDay;
    result.hour        = time->wHour;
    result.minute      = time->wMinute;
    result.second      = time->wSecond;
    result.millisecond = time->wMilliseconds;
    return(result);
}

FUNCTION OS_DateTime 
win32_datetime_from_filetime(FILETIME *fileTime) {
    SYSTEMTIME sysTime = {0};
    FileTimeToSystemTime(fileTime, &sysTime);
    OS_DateTime result = win32_datetime_from_systemtime(&sysTime);
    return(result);
}

FUNCTION OS_DateTime 
os_get_datetime_UTC(void) {
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);
    OS_DateTime result = win32_datetime_from_systemtime(&systemTime);
    return(result);
}

FUNCTION OS_DateTime 
os_get_datetime_local(void) {
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    OS_DateTime result = win32_datetime_from_systemtime(&systemTime);
    return(result);
}

//---------------------------------------------------------------------------
// COPY TO/FROM CLIPBOARD
//---------------------------------------------------------------------------
FUNCTION void 
os_copy_string8_to_clipboard(String8  str) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 str16 = string16_from_string8(&scratch, str);
    os_copy_string16_to_clipboard(str16); 
    mem_arena_release_scratch(&scratch);
}

FUNCTION void 
os_copy_string16_to_clipboard(String16 str) {
    if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        if(OpenClipboard(NULL)) {
            EmptyClipboard();
            HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, (str.length + 1) * sizeof(u16));
            if (hMem) {
                WCHAR *dst = (WCHAR *)GlobalLock(hMem);
                if (dst) {
                    os_mem_copy(dst, str.buffer, str.size);
                    dst[str.length] = 0;
                    SetClipboardData(CF_UNICODETEXT, hMem);
                }
                GlobalUnlock(hMem);
            }
            CloseClipboard();
        }
    }
}

FUNCTION void 
os_copy_string32_to_clipboard(String32 str) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 str16 = string16_from_string32(&scratch, str);
    os_copy_string16_to_clipboard(str16);
    mem_arena_release_scratch(&scratch);    
}

FUNCTION String8 
os_copy_string8_from_clipboard(MemoryArena *arena) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 str16 = os_copy_string16_from_clipboard(&scratch);
    String8 result = string8_from_string16(arena, str16);
    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION String16 
os_copy_string16_from_clipboard(MemoryArena *arena) {
    String16 result = {0};
    if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        if(OpenClipboard(NULL)) {
            HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
            if (hMem) {
                WCHAR *strData = (WCHAR *)GlobalLock(hMem);
                if (strData) {
                    result = string16_copy(arena, string16((u16*)strData, wstr_length(strData)));
                }
                GlobalUnlock(hMem);
            } 
            CloseClipboard();
        }
    }
    return(result);
}

FUNCTION String32 
os_copy_string32_from_clipboard(MemoryArena *arena) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 str16  = os_copy_string16_from_clipboard(&scratch);
    String32 result = string32_from_string16(arena, str16);
    mem_arena_release_scratch(&scratch);
    return(result);
}

//------------------------------------------------------------------------
// BASIC FILE OPS
//------------------------------------------------------------------------
FUNCTION OS_Handle 
os_file_open(String8 fileName, OS_FileMode fileMode) {
    OS_Handle result = {0};

    MemoryArena scratch = mem_arena_get_scratch();
    wchar_t *fileNameW  = wstr_from_string8(&scratch, fileName);

    switch (fileMode) {
        case OS_FILE_READ: {
            result = (u64)CreateFileW(fileNameW, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        } break;
        case OS_FILE_WRITE: {
            result = (u64)CreateFileW(fileNameW, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        } break;
    }

    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION void 
os_file_close(OS_Handle fileID) {
    HANDLE handle = (HANDLE)fileID;
    CloseHandle(handle);
}

FUNCTION u64 
os_file_get_size(OS_Handle fileID) {
    u64 result = 0;
    HANDLE handle = (HANDLE)fileID;

    LARGE_INTEGER fileSizeInBytes; 
    if (GetFileSizeEx(handle, &fileSizeInBytes)) {
        result = fileSizeInBytes.QuadPart;
    }
    return(result);
}

FUNCTION u64      
os_file_op(OS_Handle fileID, void *data, u64 size, OS_FileMode fileMode) {
    u64 amountOfDataRW = 0;
    if (data && size > 0) {
        while (amountOfDataRW < size) {
            u64 amountToRW = size - amountOfDataRW;
            DWORD amountToRW32 = (amountToRW > UINT32_MAX) ? (DWORD)UINT32_MAX : (DWORD)amountToRW;
            DWORD amountActuallyRW = 0;
            switch(fileMode) {
                case OS_FILE_READ:  { ReadFile( (HANDLE)fileID, (u8*)data + amountOfDataRW, amountToRW32, &amountActuallyRW, 0 ); }; break;
                case OS_FILE_WRITE: { WriteFile((HANDLE)fileID, (u8*)data + amountOfDataRW, amountToRW32, &amountActuallyRW, 0 ); }; break;
            }
            if (amountActuallyRW == 0) {
                DWORD error = GetLastError();
                DEBUG_BREAK();
            }
            amountOfDataRW += amountActuallyRW;
        }
    }
    return(amountOfDataRW);
}

FUNCTION void 
os_file_read_block(OS_Handle fileID, void *blockData, u64 blockSize) {
    os_file_op(fileID, blockData, blockSize, OS_FILE_READ);
}

FUNCTION void 
os_file_write_block(OS_Handle fileID, void *blockData, u64 blockSize) {
    os_file_op(fileID, blockData, blockSize, OS_FILE_WRITE);
}

FUNCTION String8 
os_file_read_all(MemoryArena *arena, String8 fileName) {
    String8 result = {0};

    // Open File
    OS_Handle fileID = os_file_open(fileName, OS_FILE_READ);
    if (!fileID) {
        return(result);
    }
        
    // Get File Size
    u64 fileSizeInBytes = os_file_get_size(fileID);
    if (fileSizeInBytes == 0) {
        return(result);
    }

    // Allocate Data
    result.buffer = mem_arena_push(arena, fileSizeInBytes + 1, u8);
    if (result.buffer == NULL) {
        return(result);
    }
    result.length = fileSizeInBytes;

    // Read entire file
    os_file_read_block(fileID, result.buffer, result.length);
    result.buffer[result.length] = '\0';

    os_file_close(fileID);

    return(result);
}

FUNCTION b32 
os_file_delete(String8 fileName) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 fileNameW = string16_from_string8(&scratch, fileName);
    b32 result = DeleteFileW((WCHAR*)fileNameW.buffer);
    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION b32 
os_file_rename(String8 fileNameOld, String8 fileNameNew) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 fileNameOldW = string16_from_string8(&scratch, fileNameOld);
    String16 fileNameNewW = string16_from_string8(&scratch, fileNameNew);
    b32 result = MoveFileW((WCHAR*)fileNameOldW.buffer, (WCHAR*)fileNameNewW.buffer);
    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION b32 
os_file_create_directory(String8 path) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 pathW = string16_from_string8(&scratch, path);
    b32 result = CreateDirectoryW((WCHAR*)pathW.buffer, 0);
    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION b32 
os_file_delete_directory(String8 path) {
    MemoryArena scratch = mem_arena_get_scratch();
    String16 pathW = string16_from_string8(&scratch, path);
    b32 result = RemoveDirectoryW((WCHAR*)pathW.buffer);
    mem_arena_release_scratch(&scratch);
    return(result);
}

//------------------------------------------------------------------------
// FILE ITERATION
//------------------------------------------------------------------------
STRUCT(W32_FileIter) {
    HANDLE           handle;
    WIN32_FIND_DATAW data;
    b32              done;
};


FUNCTION u32 
win32_prop_flags_from_attribs(DWORD attribs) {
    u32 result = 0;
    if (attribs & FILE_ATTRIBUTE_DIRECTORY) {
        result |= FILE_PROPERTY_IS_FOLDER;
    }
    return(result);
}

FUNCTION u32 
win32_access_flags_from_attribs(DWORD attribs) {
    u32 result = DATA_ACCESS_READ | DATA_ACCESS_EXECUTE;
    if (!(attribs & FILE_ATTRIBUTE_READONLY)) {
        result |= DATA_ACCESS_WRITE;
    }
    return(result);
}

FUNCTION OS_FileIter 
os_file_iter_start(String8 path) {
    OS_FileIter result = {0};
    W32_FileIter *iter = (W32_FileIter*)&result;

    MemoryArena scratch = mem_arena_get_scratch();
    //String8 fullPath = string8_concat(&scratch, path, string8_from_cstr("\\*"));
    wchar_t *pathW = wstr_from_string8(&scratch, path);
    iter->handle = FindFirstFileW(pathW, &iter->data);

    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION b32 
os_file_iter_next(MemoryArena *arena, OS_FileIter *fileIter, String8 *fileNameOut, OS_FileProperties *properties) {
    b32 result = false;
    W32_FileIter *iter = (W32_FileIter*)fileIter;

    if (iter->handle != 0 && iter->handle != INVALID_HANDLE_VALUE) {
        while (!iter->done) {
            WCHAR *fileName = iter->data.cFileName;
            b32 isDot = (fileName[0] == '.' && 
                         fileName[1] == 0);
            b32 isDotDot = (fileName[0] == '.' && 
                            fileName[1] == '.' && 
                            fileName[2] == 0); 
        

            WIN32_FIND_DATAW data = {0};

            b32 isValidFile = !isDot && !isDotDot;
            if (isValidFile) {
                os_mem_copy(&data, &iter->data, sizeof(data));
            }

            // Increment the iterator
            if (!FindNextFileW(iter->handle, &iter->data)) {
                iter->done = true;
            }

            if (isValidFile) {
                if (fileNameOut) {
                    *fileNameOut = string8_from_string16(arena, string16_from_wstr(data.cFileName));
                }
                if (properties) {
                    properties->sizeInBytes       = ((u64)data.nFileSizeHigh << 32) | (u64)data.nFileSizeLow;
                    properties->creationTime      = win32_datetime_from_filetime(&data.ftCreationTime);
                    properties->lastAccessTime    = win32_datetime_from_filetime(&data.ftLastAccessTime);
                    properties->lastWriteTime     = win32_datetime_from_filetime(&data.ftLastWriteTime);
                    properties->filePropertyFlags = win32_prop_flags_from_attribs(data.dwFileAttributes);
                    properties->dataAccessFlags   = win32_access_flags_from_attribs(data.dwFileAttributes);
                }
                result = true;
                break;
            }
        }
    }

    return(result);
}

FUNCTION void 
os_file_iter_end(OS_FileIter *fileIter) {
    W32_FileIter *iter = (W32_FileIter*)fileIter;
    if (iter->handle != 0 && iter->handle != INVALID_HANDLE_VALUE) {
        FindClose(iter->handle);
    }
}

FUNCTION String8List 
os_get_filenames_in_dir(MemoryArena *arena, String8 dir) {
    String8List result = {0};

    OS_FileIter iter = os_file_iter_start(dir);
    String8 fileName = {0};
    while (os_file_iter_next(arena, &iter, &fileName, NULL)) {
        string8_list_push(arena, &result, fileName);
    }
    os_file_iter_end(&iter);
    return(result);
}

//------------------------------------------------------------------------
// COMPRESSION
//------------------------------------------------------------------------
FUNCTION String8 
os_zip(MemoryArena *arena, String8 uncompressed) {
    String8 result = {0};

    COMPRESSOR_HANDLE handle;
    CreateCompressor(COMPRESS_ALGORITHM_MSZIP, 0, &handle);

    // NOTE(tim): Call Compress with result.size == 0, to get the size of uncompressed data.
    size_t compressedDataSize = 0;
    Compress(handle, uncompressed.buffer, uncompressed.size, result.buffer, result.size, &compressedDataSize);

    result.buffer = mem_arena_push(arena, compressedDataSize, uint8_t);
    result.length = compressedDataSize;
    result.size   = compressedDataSize;
    Compress(handle, uncompressed.buffer, uncompressed.size, result.buffer, result.size, &compressedDataSize);

    return(result);
}

FUNCTION String8 
os_unzip(MemoryArena *arena, String8 compressed) {
    String8 result = {0};

    DECOMPRESSOR_HANDLE handle;
    CreateDecompressor(COMPRESS_ALGORITHM_MSZIP, 0, &handle);

    // NOTE(tim): Call Decompress with result.size == 0, to get the size of uncompressed data.
    size_t uncompressedDataSize = 0;
    Decompress(handle, compressed.buffer, compressed.size, result.buffer, result.size, &uncompressedDataSize);

    result.buffer = mem_arena_push(arena, uncompressedDataSize, uint8_t);
    result.length = uncompressedDataSize;
    result.size   = uncompressedDataSize;
    Decompress(handle, compressed.buffer, compressed.size, result.buffer, result.size, &uncompressedDataSize);

    return(result);
}

//------------------------------------------------------------------------
// LOADING LIBRARIES
//------------------------------------------------------------------------
FUNCTION OS_Handle 
os_lib_load(String8 fileName) {
    OS_Handle result = {0};
    MemoryArena scratch = mem_arena_get_scratch();
    String16 fileNameW = string16_from_string8(&scratch, fileName);
    result = (u64)(LoadLibraryW((WCHAR*)fileNameW.buffer));
    DWORD error = GetLastError();
    mem_arena_release_scratch(&scratch);
    return(result);
}

FUNCTION VoidFn* 
os_lib_get_proc(OS_Handle lib, char *procName) {
    HMODULE module = (HMODULE)(lib);
    VoidFn* result = (VoidFn*)(GetProcAddress(module,procName));
    return(result);
}

FUNCTION void 
os_lib_release(OS_Handle lib) {
    HMODULE module = (HMODULE)(lib);
    FreeLibrary(module);
}

//------------------------------------------------------------------------
// THREADING
//------------------------------------------------------------------------
FUNCTION OS_Handle 
os_create_thread(OS_ThreadProc *proc, void *data) {
    OS_Handle result = {0};

    DWORD id;
    HANDLE threadHandle = CreateThread(0, 0, proc, data, 0, &id);
    result = (u64)threadHandle;

    return(result);
}

FUNCTION void      
os_wait_for_one_thread_to_finish(OS_Handle thread) {
    WaitForSingleObject((HANDLE)thread,INFINITE);
}

FUNCTION void      
os_wait_for_all_threads_to_finish(OS_Handle *threads, u64 nThreads) {
    WaitForMultipleObjects(nThreads, (HANDLE*)threads, true, INFINITE);
}

FUNCTION s32 
os_interlocked_increment(s32 volatile *value) {
    return(InterlockedIncrement((LONG volatile*)value));
}

FUNCTION void 
os_close_thread_handle(OS_Handle handle) {
    CloseHandle((HANDLE)handle);
}

FUNCTION OS_Handle os_create_mutex(void) {
    OS_Handle result = (OS_Handle)CreateMutex(NULL, FALSE, NULL);
    return(result);
}

FUNCTION void os_release_mutex(OS_Handle mutex) {
    ReleaseMutex((HANDLE)mutex);
}

FUNCTION void os_request_mutex(OS_Handle mutex) {
    WaitForSingleObject((HANDLE)mutex, INFINITE);
}

FUNCTION void os_close_mutex(OS_Handle mutex) {
    CloseHandle((HANDLE)mutex);
}

//------------------------------------------------------------------------
// TIMING
//------------------------------------------------------------------------
FUNCTION void 
os_sleep_ms(u64 time) {
    Sleep(time);
}
