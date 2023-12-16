
static u64
hash(String8 str) {
    u64 result = 0;
    for (u64 idx = 0; idx < str.length; ++idx) {
        if (str.buffer[idx] != '\n') {
            result += (u64)str.buffer[idx];
            result *= 17;
            result = result % 256;
        }
    }
    return(result);
}

STRUCT(BoxNode) {
    BoxNode *prev;
    BoxNode *next;

    String8 label;
    u8 focalLength;
};


STRUCT(Box) {
    BoxNode *first;
    BoxNode *last;
    u64 nodeCount;
};


static void 
day15_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day15.dat"));
    u8 comma[1]   = {','};

    String8List sequence = string8_split(arena, file, comma, 1);
    u64 sum = 0;
    for (String8ListNode *node = sequence.first; node; node = node->next) {
        sum += hash(node->string);
    }

    printf("%llu\n",sum);
}

static void 
day15_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day15.dat"));
    u8 comma[1]   = {','};

    Box *boxes = mem_arena_push(arena, 256, Box);

    String8List sequence = string8_split(arena, file, comma, 1);
    for (String8ListNode *node = sequence.first; node; node = node->next) {

        String8 label = {
            .buffer = node->string.buffer,
        };

        u8 operation = 0;
        for (u64 idx = 0; idx < node->string.length; ++idx) {
            if (node->string.buffer[idx] == '-' || node->string.buffer[idx] == '=') {
                label.length = idx;
                operation = node->string.buffer[idx];
                break;
            }
        }

        u64 boxIdx = hash(label);
        Box *box = &boxes[boxIdx];

        switch (operation) {
            case '-': {
                
                for (BoxNode *boxNode = box->first; boxNode; boxNode = boxNode->next) {
                    if (string8_compare(boxNode->label, label)) {
                        DLL_REMOVE(box->first, box->last, boxNode);
                        box->nodeCount--;
                        break;
                    }
                }
            } break;
            case '=': {
                u8 focalLength = node->string.buffer[node->string.length-1] - '0';

                bool boxFound = false;
                for (BoxNode *boxNode = box->first; boxNode; boxNode = boxNode->next) {
                    if (string8_compare(boxNode->label, label)) {
                        boxNode->focalLength = focalLength;
                        boxFound = true;
                        break;
                    }
                }

                if (!boxFound) {
                    BoxNode *boxNode = mem_arena_push(arena, 1, BoxNode);
                    boxNode->label = label;
                    boxNode->focalLength = focalLength;
                    DLL_PUSH_BACK(box->first, box->last, boxNode);
                    box->nodeCount++;
                }

            } break;
        }
    }

    u64 sum = 0;
    for (u64 idx = 0; idx < 256; ++idx) {
        Box box = boxes[idx];
        if (box.nodeCount) {
            u64 slot = 1;
            for (BoxNode *boxNode = box.first; boxNode; boxNode = boxNode->next) {
                sum += (idx+1) * slot * boxNode->focalLength;
                slot++;
            }
        }
    }

    printf("%llu\n",sum);
}