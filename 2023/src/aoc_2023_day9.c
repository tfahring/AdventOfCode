static void 
day9_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day9.dat"));

    u8 lineSplitChar[1] = {'\n'};
    u8 seqSplitChar[1] = {' '};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    u64 sum = 0;
    for (String8ListNode *line = lines.first; line; line = line->next) {
        u64 usedBytes = arena->usedBytes;
        String8List seqList = string8_split(arena, line->string, seqSplitChar, 1);
        u64 seqLength = seqList.nodeCount;
        s64 *sequence = mem_arena_push_and_zero(arena, seqLength*seqLength, s64);
        u64 idx = 0;
        for (String8ListNode *seqStr = seqList.first; seqStr; seqStr = seqStr->next) {
            sequence[idx++] = string_to_int64(seqStr->string.buffer, seqStr->string.length);
        }

        u64 row = 0;
        bool isAllZeros = false;
        while (!isAllZeros) {
            row++;
            isAllZeros = true;
            for (u64 idx = 0; idx < (seqLength-row); ++idx) {
                sequence[row*seqLength + idx] = sequence[(row-1)*seqLength + (idx+1)] - sequence[(row-1)*seqLength + (idx)];
                if (sequence[row*seqLength + idx] != 0) {
                    isAllZeros = false;
                }
            }
        }

        u64 value = 0;
        for (s64 idx = (row-1); idx >= 0; --idx) {
            value += sequence[idx*seqLength + (seqLength-idx-1)];
        }
        sum += value;

        arena->usedBytes = usedBytes;
    }

    printf("%llu\n", sum);
}

static void 
day9_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day9.dat"));

    u8 lineSplitChar[1] = {'\n'};
    u8 seqSplitChar[1] = {' '};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    s64 sum = 0;
    for (String8ListNode *line = lines.first; line; line = line->next) {
        u64 usedBytes = arena->usedBytes;
        String8List seqList = string8_split(arena, line->string, seqSplitChar, 1);
        u64 seqLength = seqList.nodeCount;
        s64 *sequence = mem_arena_push_and_zero(arena, seqLength*seqLength, s64);
        u64 idx = 0;
        for (String8ListNode *seqStr = seqList.first; seqStr; seqStr = seqStr->next) {
            sequence[idx++] = string_to_int64(seqStr->string.buffer, seqStr->string.length);
        }

        u64 row = 0;
        bool isAllZeros = false;
        while (!isAllZeros) {
            row++;
            isAllZeros = true;
            for (u64 idx = 0; idx < (seqLength-row); ++idx) {
                sequence[row*seqLength + idx] = sequence[(row-1)*seqLength + (idx+1)] - sequence[(row-1)*seqLength + (idx)];
                if (sequence[row*seqLength + idx] != 0) {
                    isAllZeros = false;
                }
            }
        }

        s64 value = 0;
        for (s64 idx = (row-1); idx >= 0; --idx) {
            value = sequence[idx*seqLength + (0)] - value;
        }
        sum += value;

        arena->usedBytes = usedBytes;
    }

    printf("%lld\n", sum);
}