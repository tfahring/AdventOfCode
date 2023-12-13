static void 
day13_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day13.dat"));
    u8 newLine[1] = {'\n'};
    // u8 space[1]   = {' '};
    // u8 comma[1]   = {','};
    // String8List lines = string8_split(arena, file, newLine, 1);

    String8List mirrors = {0};
    u64 start = 0;
    for (u64 idx = 0; idx < (file.length-1); ++idx) {
        if (file.buffer[idx] == '\n' && file.buffer[idx+1] == '\n') {
            String8 mirror = string8(&file.buffer[start], idx-start);
            string8_list_push(arena, &mirrors, mirror);
            start = idx += 2;
        }
    }
    String8 mirror = string8(&file.buffer[start], file.length-start);
    string8_list_push(arena, &mirrors, mirror);

    u64 sum = 0;
    for (String8ListNode *node = mirrors.first; node; node = node->next) {
        u64 usedBytes = arena->usedBytes;
        String8List lines = string8_split(arena, node->string, newLine, 1);

        s64 width  = lines.first->string.length;
        s64 height = lines.nodeCount;
        u8 *data   = mem_arena_push(arena, width*height, u8);

        String8ListNode *line = lines.first;
        for (s64 row = 0; row < height; ++row) {
            os_mem_copy(&data[row*width], line->string.buffer, width);
            line = line->next;
        }

        bool isVerticalMirror = false;
        s64 nColumnsToLeft = 0;
        for (s64 pivotIdx = 1; pivotIdx < width; ++pivotIdx) {

            s64 start = 1;
            s64 end = MIN(pivotIdx, width - pivotIdx);

            bool isMirror = true;
            for (s64 idx = start; idx <= end; ++idx) {
                for (s64 row = 0; row < height; ++row) {
                    if (data[row*width + pivotIdx - idx] != data[row*width + pivotIdx + idx - 1]) {
                        isMirror = false;
                        goto next;
                    }
                }
            }
next:
            if (isMirror) {
                isVerticalMirror = true;
                nColumnsToLeft = pivotIdx;
                break;
            }
        }

        bool isHorizontalMirror = false;
        s64 nRowsAbove = 0;
        if (!isVerticalMirror){
            for (s64 pivotIdx = 1; pivotIdx < height; ++pivotIdx) {

                s64 start = 1;
                s64 end = MIN(pivotIdx, height - pivotIdx);

                bool isMirror = true;
                for (s64 idx = start; idx <= end; ++idx) {
                    for (s64 col = 0; col < width; ++col) {
                        if (data[(pivotIdx - idx)*width + col] != data[(pivotIdx+idx-1)*width + col]) {
                            isMirror = false;
                            goto next2;
                        }
                    }
                }
next2:
                if (isMirror) {
                    isHorizontalMirror = true;
                    nRowsAbove = pivotIdx;
                    break;
                }
            }
        }
        sum += nColumnsToLeft + 100 * nRowsAbove;
        arena->usedBytes = usedBytes;
    }

    printf("%llu\n",sum);
    int debug = 1;
}

static void 
day13_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day13.dat"));
    u8 newLine[1] = {'\n'};
    // u8 space[1]   = {' '};
    // u8 comma[1]   = {','};
    // String8List lines = string8_split(arena, file, newLine, 1);

    String8List mirrors = {0};
    u64 start = 0;
    for (u64 idx = 0; idx < (file.length-1); ++idx) {
        if (file.buffer[idx] == '\n' && file.buffer[idx+1] == '\n') {
            String8 mirror = string8(&file.buffer[start], idx-start);
            string8_list_push(arena, &mirrors, mirror);
            start = idx += 2;
        }
    }
    String8 mirror = string8(&file.buffer[start], file.length-start);
    string8_list_push(arena, &mirrors, mirror);

    u64 sum = 0;
    for (String8ListNode *node = mirrors.first; node; node = node->next) {
        u64 usedBytes = arena->usedBytes;
        String8List lines = string8_split(arena, node->string, newLine, 1);

        s64 width  = lines.first->string.length;
        s64 height = lines.nodeCount;
        u8 *data   = mem_arena_push(arena, width*height, u8);

        String8ListNode *line = lines.first;
        for (s64 row = 0; row < height; ++row) {
            os_mem_copy(&data[row*width], line->string.buffer, width);
            line = line->next;
        }

        bool isVerticalMirror = false;
        s64 nColumnsToLeft = 0;
        for (s64 pivotIdx = 1; pivotIdx < width; ++pivotIdx) {

            s64 start = 1;
            s64 end = MIN(pivotIdx, width - pivotIdx);

            u64 smudgeCount = 0;
            for (s64 idx = start; idx <= end; ++idx) {
                for (s64 row = 0; row < height; ++row) {
                    if (data[row*width + pivotIdx - idx] != data[row*width + pivotIdx + idx - 1]) {
                        smudgeCount++;
                    }
                }
            }

            if (smudgeCount == 1) {
                isVerticalMirror = true;
                nColumnsToLeft = pivotIdx;
                break;
            }
        }

        bool isHorizontalMirror = false;
        s64 nRowsAbove = 0;
        if (!isVerticalMirror){
            for (s64 pivotIdx = 1; pivotIdx < height; ++pivotIdx) {

                s64 start = 1;
                s64 end = MIN(pivotIdx, height - pivotIdx);

                u64 smudgeCount = 0;
                for (s64 idx = start; idx <= end; ++idx) {
                    for (s64 col = 0; col < width; ++col) {
                        if (data[(pivotIdx - idx)*width + col] != data[(pivotIdx+idx-1)*width + col]) {
                            smudgeCount++;
                        }
                    }
                }
                if (smudgeCount == 1) {
                    isHorizontalMirror = true;
                    nRowsAbove = pivotIdx;
                    break;
                }
            }
        }
        sum += nColumnsToLeft + 100 * nRowsAbove;
        arena->usedBytes = usedBytes;
    }

    printf("%llu\n",sum);
    int debug = 1;
}