
STRUCT(MapNode) {
    MapNode *prev;
    MapNode *next;

    u64 key;
    u64 val;
    u64 len;
};

STRUCT(Map) {
    MapNode *first;
    MapNode *last;
    u64 nodeCount;
};


static void 
map_push_ex(Map *list, u64 key, u64 val, u64 len, MapNode *node) {
    node->key = key;
    node->val = val;
    node->len = len;
    DLL_PUSH_BACK(list->first, list->last, node);
    list->nodeCount++;
}

static void 
map_push(MemoryArena *arena, Map *list, u64 key, u64 val, u64 len) {
    MapNode *node = mem_arena_push(arena, 1, MapNode);
    map_push_ex(list, key, val, len, node);
}

static u64
map_get_value(Map map, u64 key) {
    u64 result = key;
    for (MapNode *node = map.first; node; node = node->next) {
        if (key >= node->key && key < (node->key + node->len)) {
            u64 diff = key - node->key;
            result = node->val + diff;
            break;
        }
    }
    return(result);
}

static String8ListNode*
map_fill(MemoryArena *arena, Map *map, String8ListNode *node) {
    u8 seedListSplitChar[1] = {' '};
    while(node && is_numeric(node->string.buffer[0])) {
        String8List strings = string8_split(arena, node->string, seedListSplitChar, 1);

        String8 dstString = strings.first->string;
        u64 dstIdx = string_to_int64(dstString.buffer, dstString.length);

        String8 srcString = strings.first->next->string;
        u64 srcIdx = string_to_int64(srcString.buffer, srcString.length);

        String8 lenString = strings.first->next->next->string;
        u64 len = string_to_int64(lenString.buffer, lenString.length);

        map_push(arena, map, srcIdx, dstIdx, len);
        node = node->next;
    }
    if (node) {
        node = node->next;
    }
    return(node);
}


static void 
day5_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day5.dat"));

    u8 lineSplitChar[1] = {'\n'};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    u8 seedListSplitChar[1] = {' '};
    String8List seedsStrings = string8_split(arena, lines.first->string, seedListSplitChar, 1);

    u64 nSeeds = seedsStrings.nodeCount - 1;
    u64 *seeds = mem_arena_push(arena, nSeeds, u64);
    u64 idx = 0;
    for (String8ListNode *node = seedsStrings.first->next; node; node = node->next) {
        seeds[idx++] = string_to_int64(node->string.buffer, node->string.length);
    }

    String8ListNode *node = lines.first->next->next;

    Map seedsToSoil = {0};
    node = map_fill(arena, &seedsToSoil, node);
    
    Map soilToFertilizer = {0};
    node = map_fill(arena, &soilToFertilizer, node);

    Map fertilizerToWater = {0};
    node = map_fill(arena, &fertilizerToWater, node);

    Map waterToLight = {0};
    node = map_fill(arena, &waterToLight, node);

    Map lightToTemperature = {0};
    node = map_fill(arena, &lightToTemperature, node);

    Map temperatureToHumidity = {0};
    node = map_fill(arena, &temperatureToHumidity, node);

    Map humidityToLocation = {0};
    node = map_fill(arena, &humidityToLocation, node);

    u64 minLocNum = 0xFFFFFFFFFFFFFFFF;
    for (u64 idx = 0; idx < nSeeds; ++idx) {
        u64 seed  = seeds[idx];
        u64 soil  = map_get_value(seedsToSoil, seed);
        u64 fert  = map_get_value(soilToFertilizer, soil);
        u64 water = map_get_value(fertilizerToWater, fert);
        u64 light = map_get_value(waterToLight, water);
        u64 temp  = map_get_value(lightToTemperature, light);
        u64 humid = map_get_value(temperatureToHumidity, temp);
        u64 loc   = map_get_value(humidityToLocation, humid);

        minLocNum = MIN(minLocNum, loc);
    }
    printf("%llu\n", minLocNum);
}

static void 
day5_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day5.dat"));

    u8 lineSplitChar[1] = {'\n'};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    u8 seedListSplitChar[1] = {' '};
    String8List seedsStrings = string8_split(arena, lines.first->string, seedListSplitChar, 1);

    u64 nSeedPairs = (seedsStrings.nodeCount - 1)/2;
    u64 *seedStart = mem_arena_push(arena, nSeedPairs, u64);
    u64 *seedLen   = mem_arena_push(arena, nSeedPairs, u64);

    u64 idx = 0;
    String8ListNode *seedNode = seedsStrings.first->next;
    while (seedNode) {
        seedStart[idx] += string_to_int64(seedNode->string.buffer, seedNode->string.length);
        seedNode = seedNode->next;
        seedLen[idx++] += string_to_int64(seedNode->string.buffer, seedNode->string.length);
        seedNode = seedNode->next;
    }

    String8ListNode *node = lines.first->next->next;
    Map seedsToSoil = {0};
    node = map_fill(arena, &seedsToSoil, node);
    
    Map soilToFertilizer = {0};
    node = map_fill(arena, &soilToFertilizer, node);

    Map fertilizerToWater = {0};
    node = map_fill(arena, &fertilizerToWater, node);

    Map waterToLight = {0};
    node = map_fill(arena, &waterToLight, node);

    Map lightToTemperature = {0};
    node = map_fill(arena, &lightToTemperature, node);

    Map temperatureToHumidity = {0};
    node = map_fill(arena, &temperatureToHumidity, node);

    Map humidityToLocation = {0};
    node = map_fill(arena, &humidityToLocation, node);

    u64 minLocNum = 0xFFFFFFFFFFFFFFFF;
    for (u64 idx  = 0; idx < nSeedPairs; ++idx) {
        printf("%llu, %llu, %llu\n",idx, seedLen[idx], minLocNum);
        for (u64 seedIdx = 0; seedIdx < seedLen[idx]; ++seedIdx) {
            u64 seed = seedStart[idx] + seedIdx;
            u64 soil  = map_get_value(seedsToSoil, seed);
            u64 fert  = map_get_value(soilToFertilizer, soil);
            u64 water = map_get_value(fertilizerToWater, fert);
            u64 light = map_get_value(waterToLight, water);
            u64 temp  = map_get_value(lightToTemperature, light);
            u64 humid = map_get_value(temperatureToHumidity, temp);
            u64 loc   = map_get_value(humidityToLocation, humid);
            minLocNum = MIN(minLocNum, loc);
        }
    }
    printf("%lld\n", minLocNum);
}