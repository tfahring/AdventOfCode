static u64 
process_card(String8 line) {
    MemoryArena scratch = mem_arena_get_scratch();

    u8 cardSplitChar[2] = {':','|'};
    u8 numSplitChar[1]  = {' '};
    String8List card    = string8_split(&scratch, line, cardSplitChar, 2);

    String8 cardID               = card.first->string;
    String8 winningNumbersString = card.first->next->string;
    String8 myNumbersString      = card.last->string;

    String8List winningNumbersStringList = string8_split(&scratch, winningNumbersString, numSplitChar, 1);
    String8List myNumbersStringList      = string8_split(&scratch, myNumbersString, numSplitChar, 1);

    u64 *winningNumbers = mem_arena_push(&scratch, winningNumbersStringList.nodeCount, u64);
    u64 *myNumbers      = mem_arena_push(&scratch, myNumbersStringList.nodeCount, u64);

    u64 idx = 0;
    for (String8ListNode *node = winningNumbersStringList.first; node; node = node->next) {
        winningNumbers[idx++] = string_to_int64(node->string.buffer, node->string.length);
    }

    idx = 0;
    for (String8ListNode *node = myNumbersStringList.first; node; node = node->next) {
        myNumbers[idx++] = string_to_int64(node->string.buffer, node->string.length);
    }

    u64 cardSum = 0;
    for (u64 myNumIdx = 0; myNumIdx < myNumbersStringList.nodeCount; ++myNumIdx) {
        for (u64 winNumIdx = 0; winNumIdx < winningNumbersStringList.nodeCount; ++winNumIdx) {
            if (myNumbers[myNumIdx] == winningNumbers[winNumIdx]) {
                cardSum++;
            }
        }
    }
    u64 result = cardSum; //(1LL << (cardSum-1));

    mem_arena_release_scratch(&scratch);
    return(result);
}

static void 
day4_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day4.dat"));
    u8 lineSplitChar[1] = {'\n'};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    u64 sum = 0;
    for (String8ListNode *line = lines.first; line; line = line->next) {
        u64 cardSum = process_card(line->string);
        sum += (1LL << (cardSum-1));
    }

    printf("%lld\n", sum);
}

static void 
day4_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day4.dat"));
    u8 lineSplitChar[1] = {'\n'};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    u64 *nMatchingNumbers = mem_arena_push_and_zero(arena, lines.nodeCount, u64);
    u64 idx = 0;
    for (String8ListNode *line = lines.first; line; line = line->next) {
        nMatchingNumbers[idx++] = process_card(line->string);
    }

    u64 *nCopies = mem_arena_push_and_zero(arena, lines.nodeCount, u64);
    for (u64 idx = 0; idx < lines.nodeCount; ++idx) {
        nCopies[idx] = 1.0f;
    }

    for (u64 idx = 0; idx < lines.nodeCount; ++idx) {
        for (u64 copyIdx = 0; copyIdx < nCopies[idx]; ++copyIdx) {
            for (u64 i = 1; i <= nMatchingNumbers[idx]; ++i) { 
                nCopies[idx+i]++;
            }
        }
    }

    u64 sum = 0;
    for (u64 idx = 0; idx < lines.nodeCount; ++idx) {
        sum += nCopies[idx];
    }

    printf("%lld\n", sum);
}