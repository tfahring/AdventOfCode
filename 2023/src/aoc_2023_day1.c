
static void
day1_part1(MemoryArena *arena) {
    String8 file1 = os_file_read_all(arena, string8_from_cstr("day1.dat"));
    u8 *ptr = file1.buffer;
    u64 sum    = 0;
    u64 digit1 = 0;
    u64 digit2 = 0;
    bool parsingDigit1 = true;
    while((ptr - file1.buffer) <= file1.length) {
        if (*ptr == '\n' || *ptr == '\0') {
            sum += digit1*10 + digit2;
            parsingDigit1 = true;
        }

        s64 num = -1;
        if (*ptr >= '0' && *ptr <= '9') { num = *ptr - '0'; }

        if (num >= 0) {
            if (parsingDigit1)  {
                digit1 = num;
                digit2 = num;
                parsingDigit1 = false;
            }
            else {
                digit2 = num;
            }
        }
        ptr++;
    }

    printf("%lld\n",sum);
}

static void
day1_part2(MemoryArena *arena) {
    String8 file1 = os_file_read_all(arena, string8_from_cstr("day1.dat"));
    u8 *ptr = file1.buffer;
    u64 sum    = 0;
    u64 digit1 = 0;
    u64 digit2 = 0;
    bool parsingDigit1 = true;
    while((ptr - file1.buffer) <= file1.length) {
        if (*ptr == '\n' || *ptr == '\0') {
            sum += digit1*10 + digit2;
            parsingDigit1 = true;
        }

        s64 num = -1;
        if (*ptr >= '0' && *ptr <= '9') { num = *ptr - '0'; }
        else if (string8_compare(string8(ptr,4), string8_from_cstr("zero")))  { num = 0; }
        else if (string8_compare(string8(ptr,3), string8_from_cstr("one")))   { num = 1; }
        else if (string8_compare(string8(ptr,3), string8_from_cstr("two")))   { num = 2; }
        else if (string8_compare(string8(ptr,5), string8_from_cstr("three"))) { num = 3; }
        else if (string8_compare(string8(ptr,4), string8_from_cstr("four")))  { num = 4; }
        else if (string8_compare(string8(ptr,4), string8_from_cstr("five")))  { num = 5; }
        else if (string8_compare(string8(ptr,3), string8_from_cstr("six")))   { num = 6; }
        else if (string8_compare(string8(ptr,5), string8_from_cstr("seven"))) { num = 7; }
        else if (string8_compare(string8(ptr,5), string8_from_cstr("eight"))) { num = 8; }
        else if (string8_compare(string8(ptr,4), string8_from_cstr("nine")))  { num = 9; }

        if (num >= 0) {
            if (parsingDigit1)  {
                digit1 = num;
                digit2 = num;
                parsingDigit1 = false;
            }
            else {
                digit2 = num;
            }
        }
        ptr++;
    }

    printf("%lld\n",sum);
}
