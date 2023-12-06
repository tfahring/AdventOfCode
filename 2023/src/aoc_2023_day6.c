static void 
day6_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day6.dat"));

    u8 lineSplitChar[1] = {'\n'};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    u8 numbersSplitChar[1] = {' '};
    String8List times = string8_split(arena, lines.first->string, numbersSplitChar, 1);
    String8List distance = string8_split(arena, lines.first->next->string, numbersSplitChar, 1);

    u64 mul = 1;
    u64 nRaces = times.nodeCount - 1;
    String8ListNode *timeNode = times.first->next;
    String8ListNode *distNode = distance.first->next;
    for (u64 raceIdx = 0; raceIdx < nRaces; ++raceIdx) {
        u64 timeForRace  = string_to_int64(timeNode->string.buffer, timeNode->string.length);
        u64 distToBeat   = string_to_int64(distNode->string.buffer, distNode->string.length);
        u64 nWinningWays = 0;
        for (u64 time = 0; time < timeForRace; ++time) {
            u64 distTraveled = (timeForRace - time) * time;
            if (distTraveled > distToBeat) {
                nWinningWays++;
            }
        }

        mul *= nWinningWays;
        timeNode = timeNode->next;
        distNode = distNode->next;
    }

    printf("%llu\n", mul);
}

static void 
day6_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day6.dat"));

    u8 lineSplitChar[1] = {'\n'};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    u8 numbersSplitChar[1] = {' '};
    String8List times = string8_split(arena, lines.first->string, numbersSplitChar, 1);
    String8List distance = string8_split(arena, lines.first->next->string, numbersSplitChar, 1);

    String8 time = {0};
    for (String8ListNode *timeNode = times.first->next; timeNode; timeNode = timeNode->next) {
        time = string8_concat(arena, time, timeNode->string);
    }

    String8 dist = {0};
    for (String8ListNode *distNode = distance.first->next; distNode; distNode = distNode->next) {
        dist = string8_concat(arena, dist, distNode->string);
    }

    u64 timeForRace  = string_to_int64(time.buffer, time.length);
    u64 distToBeat   = string_to_int64(dist.buffer, dist.length);
    u64 nWinningWays = 0;
    for (u64 time = 0; time < timeForRace; ++time) {
        u64 distTraveled = (timeForRace - time) * time;
        if (distTraveled > distToBeat) {
            nWinningWays++;
        }
    }

    printf("%llu\n", nWinningWays);
}