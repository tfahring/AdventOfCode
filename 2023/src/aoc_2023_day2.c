static void 
day2_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day2.dat"));
    u8 *ptr = file.buffer;

    u64 maxRedCubes   = 12;
    u64 maxGreenCubes = 13;
    u64 maxBlueCubes  = 14;

    u64 id = 0;
    u64 nRedCubes   = 0;
    u64 nGreenCubes = 0;
    u64 nBlueCubes  = 0;
    u64 sumOfPossibleGames = 0;

    bool invalidSetFound = false;
    while((ptr - file.buffer) <= file.length) {
        while(is_whitespace(*ptr)) {
            ptr++;
        }

        if (is_alpha(*ptr)) {
            ptr += 5; // Eat "Game "

            u64 length = 0;
            while (ptr[length] != ':') {
                length++;
            }
            id = string_to_int64(ptr, length);

            ptr += (length+1);
        }
        else if (is_numeric(*ptr)) {
            u64 length = 0;
            while (ptr[length] != ' ') {
                length++;
            }
            u64 num = string_to_int64(ptr, length);
            ptr += (length+1);

            if      (string8_compare(string8(ptr,3), string8_from_cstr("red")))   { nRedCubes   += num; ptr += 3;}
            else if (string8_compare(string8(ptr,5), string8_from_cstr("green"))) { nGreenCubes += num; ptr += 5;}
            else if (string8_compare(string8(ptr,4), string8_from_cstr("blue")))  { nBlueCubes  += num; ptr += 4;}
        }
        else if (*ptr == ',') {
            ptr++; // eat comma.
        }
        else if (*ptr == ';') {
            ptr++; // eat semicolon
            if (nRedCubes   > maxRedCubes || 
                nGreenCubes > maxGreenCubes || 
                nBlueCubes  > maxBlueCubes) {
                invalidSetFound = true;
            }

            nRedCubes   = 0;
            nGreenCubes = 0;
            nBlueCubes  = 0;
        }
        else if (*ptr == '\n' || *ptr == '\0') {
            if (nRedCubes   > maxRedCubes || 
                nGreenCubes > maxGreenCubes || 
                nBlueCubes  > maxBlueCubes) {
                invalidSetFound = true;
            }

            nRedCubes   = 0;
            nGreenCubes = 0;
            nBlueCubes  = 0;

            sumOfPossibleGames += (!invalidSetFound) ? id : 0;
            invalidSetFound = false;
            ptr++;
        } 
    }
    printf("%lld\n",sumOfPossibleGames);
}

static void 
day2_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day2.dat"));
    u8 *ptr = file.buffer;

    u64 maxRedCubes   = 12;
    u64 maxGreenCubes = 13;
    u64 maxBlueCubes  = 14;

    u64 id = 0;
    u64 nRedCubes   = 0;
    u64 nGreenCubes = 0;
    u64 nBlueCubes  = 0;

    u64 minNumRedCubes   = 0;
    u64 minNumGreenCubes = 0;
    u64 minNumBlueCubes  = 0;

    u64 sumOfGamePower = 0;
    while((ptr - file.buffer) <= file.length) {
        while(is_whitespace(*ptr)) {
            ptr++;
        }

        if (is_alpha(*ptr)) {
            ptr += 5; // Eat "Game "

            u64 length = 0;
            while (ptr[length] != ':') {
                length++;
            }
            id = string_to_int64(ptr, length);

            ptr += (length+1);
        }
        else if (is_numeric(*ptr)) {
            u64 length = 0;
            while (ptr[length] != ' ') {
                length++;
            }
            u64 num = string_to_int64(ptr, length);
            ptr += (length+1);

            if      (string8_compare(string8(ptr,3), string8_from_cstr("red")))   { nRedCubes   += num; ptr += 3;}
            else if (string8_compare(string8(ptr,5), string8_from_cstr("green"))) { nGreenCubes += num; ptr += 5;}
            else if (string8_compare(string8(ptr,4), string8_from_cstr("blue")))  { nBlueCubes  += num; ptr += 4;}
        }
        else if (*ptr == ',') {
            ptr++; // eat comma.
        }
        else if (*ptr == ';') {
            ptr++; // eat semicolon
            minNumRedCubes   = MAX(minNumRedCubes, nRedCubes);
            minNumGreenCubes = MAX(minNumGreenCubes, nGreenCubes);
            minNumBlueCubes  = MAX(minNumBlueCubes, nBlueCubes);

            nRedCubes   = 0;
            nGreenCubes = 0;
            nBlueCubes  = 0;
        }
        else if (*ptr == '\n' || *ptr == '\0') {
            ptr++;
            minNumRedCubes   = MAX(minNumRedCubes, nRedCubes);
            minNumGreenCubes = MAX(minNumGreenCubes, nGreenCubes);
            minNumBlueCubes  = MAX(minNumBlueCubes, nBlueCubes);
            u64 power = minNumRedCubes*minNumGreenCubes*minNumBlueCubes;
            sumOfGamePower += power; 

            minNumRedCubes   = 0;
            minNumGreenCubes = 0;
            minNumBlueCubes  = 0;

            nRedCubes   = 0;
            nGreenCubes = 0;
            nBlueCubes  = 0;
        }
    }
    printf("%lld\n",sumOfGamePower);
}