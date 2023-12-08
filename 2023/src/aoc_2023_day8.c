
STRUCT(InstNode) {
    InstNode *prev;
    InstNode *next;
    InstNode *left;
    InstNode *right;

    u64 key;
    u8 str[4];
};

STRUCT(InstList) {
    InstNode *first;
    InstNode *last;
    u64 nodeCount;
};

static u64 
hash_key(String8 key) {
    u64 fnv64Prime = 0x00000100000001b3ull;
    u64 result  = 0xcbf29ce484222325ull;

    for (u64 idx = 0; idx < key.length; ++idx) {
        result ^= (u64)key.buffer[idx];
        result *= fnv64Prime;
    }
    return(result);
}

static void 
inst_push_ex(InstList *list, u64 key, u8 *str, InstNode *node) {
    node->key = key;
    os_mem_copy(node->str, str, 3);
    DLL_PUSH_BACK(list->first, list->last, node);
    list->nodeCount++;
}

static void 
inst_push(MemoryArena *arena, InstList *list, u64 key, u8 *str) {
    InstNode *node = mem_arena_push(arena, 1, InstNode);
    inst_push_ex(list, key, str, node);
}

static InstNode*
inst_node_in_list(InstList *list, u64 hash) {
    InstNode* result = NULL;
    for (InstNode *node = list->first; node; node = node->next) {
        if (node->key == hash) {
            result = node;
            break;
        }
    }
    return(result);
}

static void 
day8_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day8.dat"));

    u8 lineSplitChar[1] = {'\n'};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    String8 instructionOrder = lines.first->string;

    lines.first = lines.first->next;
    lines.first->prev = NULL;

    InstList list = {0};
    for (String8ListNode *node = lines.first; node; node = node->next) {
        u8* line = node->string.buffer;
        u64 nodeHash = hash_key(string8(&line[0],3));
        inst_push(arena, &list, nodeHash, &line[0]);
    }

    for (String8ListNode *node = lines.first; node; node = node->next) {
        u8* line = node->string.buffer;

        u64 centerHash = hash_key(string8(&line[0],3));
        u64 leftHash   = hash_key(string8(&line[7],3));
        u64 rightHash  = hash_key(string8(&line[12],3));

        InstNode *center = inst_node_in_list(&list, centerHash);
        InstNode *left   = inst_node_in_list(&list, leftHash);
        InstNode *right  = inst_node_in_list(&list, rightHash);

        center->left  = left;
        center->right = right;
    }

    bool zzzFound = false;
    u64 zzzHash   = hash_key(string8_from_cstr("ZZZ"));

    u64 idx   = 0;
    u64 count = 0;

    u64 aaaHash = hash_key(string8_from_cstr("AAA"));
    InstNode *node = inst_node_in_list(&list, aaaHash);
    while(!zzzFound) {
        // printf("%llu\n", count);
        if (idx == instructionOrder.length) {
            idx = 0;
        }

        u8 dir = instructionOrder.buffer[idx];

        switch (dir) {
            case 'L': {
                node = node->left;
            } break;
            case 'R': {
                node = node->right;
            } break;
        }
        count++;
        idx++;

        if (node->key == zzzHash) {
            zzzFound = true;
        }
    }
    printf("%llu\n", count);
}

static void 
day8_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day8.dat"));

    u8 lineSplitChar[1] = {'\n'};
    String8List lines = string8_split(arena, file, lineSplitChar, 1);

    String8 instructionOrder = lines.first->string;

    lines.first = lines.first->next;
    lines.first->prev = NULL;

    InstList list = {0};
    for (String8ListNode *node = lines.first; node; node = node->next) {
        u8* line = node->string.buffer;
        u64 nodeHash = hash_key(string8(&line[0],3));
        inst_push(arena, &list, nodeHash, &line[0]);
    }

    for (String8ListNode *node = lines.first; node; node = node->next) {
        u8* line = node->string.buffer;

        u64 centerHash = hash_key(string8(&line[0],3));
        u64 leftHash   = hash_key(string8(&line[7],3));
        u64 rightHash  = hash_key(string8(&line[12],3));

        InstNode *center = inst_node_in_list(&list, centerHash);
        InstNode *left   = inst_node_in_list(&list, leftHash);
        InstNode *right  = inst_node_in_list(&list, rightHash);

        center->left  = left;
        center->right = right;
    }

    InstNode **nodesThatEndInA = mem_arena_push(arena, list.nodeCount, InstNode*);
    u64 nodesThandEndInACount = 0;
    for (InstNode *node = list.first; node; node = node->next) {
        if (node->str[2] == 'A') {
            nodesThatEndInA[nodesThandEndInACount++] = node;
        }
    }

    u64 idx   = 0;
    u64 count = 0;
    u64 *cycleCount = mem_arena_push(arena, nodesThandEndInACount, u64);
    bool allCycleCountsFound = false;
    while(!allCycleCountsFound) {
        if (idx == instructionOrder.length) {
            idx = 0;
        }
        u8 dir = instructionOrder.buffer[idx];

        for (u64 nodeIdx = 0; nodeIdx < nodesThandEndInACount; ++nodeIdx) {
            switch (dir) {
                case 'L': { nodesThatEndInA[nodeIdx] = nodesThatEndInA[nodeIdx]->left; } break;
                case 'R': { nodesThatEndInA[nodeIdx] = nodesThatEndInA[nodeIdx]->right; } break;
            }
        }
        
        count++;
        idx++;

        for (u64 nodeIdx = 0; nodeIdx < nodesThandEndInACount; ++nodeIdx) {
            if (cycleCount[nodeIdx] == 0 && nodesThatEndInA[nodeIdx]->str[2] == 'Z') {
                cycleCount[nodeIdx] = count;
                break;
            }
        }

        allCycleCountsFound = true;
        for (u64 nodeIdx = 0; nodeIdx < nodesThandEndInACount; ++nodeIdx) {
            if (cycleCount[nodeIdx] == 0) {
                allCycleCountsFound = false;
            }
        }
    }

    // find least common multiple of cycleCounts.
}