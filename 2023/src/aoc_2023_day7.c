
ENUM(HandType) {
    HIGH_CARD = 0,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    FIVE_OF_A_KIND,
    HAND_TYPE_COUNT,
};

STRUCT(Hand) {
    HandType type;
    u16 cards[5];
    u64 bid;
};

static u16
getCardStrength(u8 card) {
    switch (card) {
        case '2': return(2);
        case '3': return(3);
        case '4': return(4);
        case '5': return(5);
        case '6': return(6);
        case '7': return(7);
        case '8': return(8);
        case '9': return(9);
        case 'T': return(10);
        case 'J': return(11);
        case 'Q': return(12);
        case 'K': return(13);
        case 'A': return(14);
    }
    return(0);
}

static Hand 
getHandType(String8 hand) {
    Hand result = {.type = HIGH_CARD};

    for (u64 i = 0; i < 5; ++i) {
        result.cards[i] = getCardStrength(hand.buffer[i]);
    }

    u8 nOccurances[13] = {0};
    for (u64 i = 0; i < 5; ++i) {
        nOccurances[result.cards[i]-2]++;
    }

    u8 nPairs  = 0;
    u8 nTrips  = 0;
    u8 nQuads  = 0;
    u8 nQuints = 0;

    for (u64 i = 0; i < 13; ++i) {
        switch(nOccurances[i]) {
            case 2: {nPairs++;} break;
            case 3: {nTrips++;} break;
            case 4: {nQuads++;} break;
            case 5: {nQuints++;} break;
        }
    }

    if      (nPairs == 1 && nTrips == 0) { result.type = ONE_PAIR; }
    else if (nPairs == 2)                { result.type = TWO_PAIR; }
    else if (nTrips == 1 && nPairs == 0) { result.type = THREE_OF_A_KIND; }
    else if (nTrips == 1 && nPairs == 1) { result.type = FULL_HOUSE; }
    else if (nQuads == 1)                { result.type = FOUR_OF_A_KIND; }
    else if (nQuints == 1)               { result.type = FIVE_OF_A_KIND; }

    return(result);
}

typedef int64_t (*qsort_cmp_fn)(void*,void*);
static void _swap(void *v1, void *v2, int64_t sizeOfElement)
{
    char tmp[32];
    os_mem_copy(tmp, v1, sizeOfElement);
    os_mem_copy(v1, v2, sizeOfElement);
    os_mem_copy(v2, tmp, sizeOfElement);
}

FUNCTION void quicksort(void *v, int64_t sizeOfElement, int64_t left, int64_t right, qsort_cmp_fn fn)
{
    if (left >= right)
        return;

    int64_t mid = (left + right) / 2;

    void *vL = ((char*)v + (left * sizeOfElement));
    void *vR = ((char*)v + (mid * sizeOfElement));
    _swap(vL,vR,sizeOfElement);
    int64_t last = left;
    void *v3 = NULL;
    for (int64_t idx = left + 1; idx <= right; ++idx) {
        void *vt = ((char*)v + (idx *sizeOfElement));
        if (fn(vL,vt) > 0) {
            ++last;
            v3 = ((char*)v + (last*sizeOfElement));
            _swap(vt,v3,sizeOfElement);
        }
    }
    v3 = ((char*)v + (last*sizeOfElement));
    _swap(vL,v3,sizeOfElement);
    quicksort(v, sizeOfElement, left, last-1, fn);
    quicksort(v, sizeOfElement, last+1, right, fn);
}


static int64_t 
handCmp(void *h0, void *h1) {
    Hand *hand0 = (Hand*)h0;
    Hand *hand1 = (Hand*)h1;

    if (hand0->type < hand1->type) {
        return(-1);
    }
    else if (hand0->type > hand1->type) {
        return(1);
    }
    else {
        for (u64 idx = 0; idx < 5; ++idx) {
            if (hand0->cards[idx] < hand1->cards[idx]) {
                return(-1);
            }
            else if (hand0->cards[idx] > hand1->cards[idx]) {
                return(1);
            }
        }
    }
    return(0);
}


static void 
day7_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day7.dat"));

    u8 roundSplitChar[1] = {'\n'};
    u8 handSplitChar[1]  = {' '};

    String8List rounds = string8_split(arena, file, roundSplitChar, 1);

    u64 nHands = rounds.nodeCount;
    Hand *hands = mem_arena_push(arena, nHands, Hand);
    u64 idx = 0;
    for (String8ListNode *node = rounds.first; node; node = node->next) {
        String8List handAndBid = string8_split(arena, node->string, handSplitChar, 1);

        hands[idx]     = getHandType(handAndBid.first->string);
        hands[idx].bid = string_to_int64(handAndBid.last->string.buffer, handAndBid.last->string.length);
        idx++;
    }

    // Sort the hands
    quicksort(hands, sizeof(Hand), 0, nHands-1, handCmp);

    u64 score = 0;
    for (u64 idx = 0; idx < nHands; ++idx) {
        score += hands[idx].bid * (idx+1);
    }

    printf("%llu\n", score);
}   


static u16
getCardStrength2(u8 card) {
    switch (card) {
        case 'J': return(1);
        case '2': return(2);
        case '3': return(3);
        case '4': return(4);
        case '5': return(5);
        case '6': return(6);
        case '7': return(7);
        case '8': return(8);
        case '9': return(9);
        case 'T': return(10);
        case 'Q': return(11);
        case 'K': return(12);
        case 'A': return(13);
    }
    return(0);
}

static Hand 
getHandType2(String8 hand) {
    Hand result = {.type = HIGH_CARD};

    for (u64 i = 0; i < 5; ++i) {
        result.cards[i] = getCardStrength2(hand.buffer[i]);
    }

    u8 nOccurances[13] = {0};
    for (u64 i = 0; i < 5; ++i) {
        nOccurances[result.cards[i]-1]++;
    }

    u8 nPairs  = 0;
    u8 nTrips  = 0;
    u8 nQuads  = 0;
    u8 nQuints = 0;
    u8 nJokers = nOccurances[0];
    for (u64 i = 1; i < 13; ++i) {
        switch(nOccurances[i]) {
            case 2: {nPairs++;} break;
            case 3: {nTrips++;} break;
            case 4: {nQuads++;} break;
            case 5: {nQuints++;} break;
        }
    }

    if      (nPairs == 1 && nTrips == 0) { result.type = ONE_PAIR; }
    else if (nPairs == 2)                { result.type = TWO_PAIR; }
    else if (nTrips == 1 && nPairs == 0) { result.type = THREE_OF_A_KIND; }
    else if (nTrips == 1 && nPairs == 1) { result.type = FULL_HOUSE; }
    else if (nQuads == 1)                { result.type = FOUR_OF_A_KIND; }
    else if (nQuints == 1)               { result.type = FIVE_OF_A_KIND; }

    switch(result.type) {
        case HIGH_CARD: {
            if      (nJokers == 1) { result.type = ONE_PAIR; }
            else if (nJokers == 2) { result.type = THREE_OF_A_KIND; }
            else if (nJokers == 3) { result.type = FOUR_OF_A_KIND; }
            else if (nJokers == 4) { result.type = FIVE_OF_A_KIND; }
        } break;
        case ONE_PAIR: {
            if      (nJokers == 1) { result.type = THREE_OF_A_KIND; }
            else if (nJokers == 2) { result.type = FOUR_OF_A_KIND; }
            else if (nJokers == 3) { result.type = FIVE_OF_A_KIND; }
        } break;
        case TWO_PAIR: {
            if (nJokers == 1) { result.type = FULL_HOUSE; }
        } break;
        case THREE_OF_A_KIND: {
            if      (nJokers == 1) { result.type = FOUR_OF_A_KIND; }
            else if (nJokers == 2) { result.type = FIVE_OF_A_KIND; }
        } break;
        case FOUR_OF_A_KIND: {
            if (nJokers == 1) { result.type = FIVE_OF_A_KIND; }
        } break;
    }

    if (nJokers == 5) {
        result.type = FIVE_OF_A_KIND;
    }

    return(result);
}

static void 
day7_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day7.dat"));

    u8 roundSplitChar[1] = {'\n'};
    u8 handSplitChar[1]  = {' '};

    String8List rounds = string8_split(arena, file, roundSplitChar, 1);

    u64 nHands = rounds.nodeCount;
    Hand *hands = mem_arena_push(arena, nHands, Hand);
    u64 idx = 0;
    for (String8ListNode *node = rounds.first; node; node = node->next) {
        String8List handAndBid = string8_split(arena, node->string, handSplitChar, 1);

        hands[idx]     = getHandType2(handAndBid.first->string);
        hands[idx].bid = string_to_int64(handAndBid.last->string.buffer, handAndBid.last->string.length);
        idx++;
    }

    // Sort the hands
    quicksort(hands, sizeof(Hand), 0, nHands-1, handCmp);

    u64 score = 0;
    for (u64 idx = 0; idx < nHands; ++idx) {
        score += hands[idx].bid * (idx+1);
    }

    printf("%llu\n", score);
}