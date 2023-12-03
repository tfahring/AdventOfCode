static void 
day3_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day3.dat"));

    u64 width = 0;
    for (u64 idx = 0; idx < file.length; ++idx) {
        if (file.buffer[idx] == '\n') {
            width = idx+1;
            break;
        }
    }
    u64 height = file.length / width + 1;

    u64 bufferedWidth  = width + 2;
    u64 bufferedHeight = height + 2;

    u8 *bufferedFile = mem_arena_push_and_zero(arena, bufferedWidth * bufferedHeight, u8);
    for (u64 rowIdx = 1; rowIdx < (bufferedHeight-1); ++rowIdx) {
        os_mem_copy(&bufferedFile[rowIdx*bufferedWidth+1], &file.buffer[(rowIdx-1)*width], width*sizeof(u8));
    }

    bool *isTouchingSymbol = mem_arena_push_and_zero(arena, bufferedWidth * bufferedHeight, bool);
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            u8 c = bufferedFile[row*bufferedWidth + col];
            if (!(is_numeric(c) || c == '.' || c == 0 || c == '\n' || c == '\0')) {
                isTouchingSymbol[(row-1)*bufferedWidth + (col-1)] = true;
                isTouchingSymbol[(row-1)*bufferedWidth + (col  )] = true;
                isTouchingSymbol[(row-1)*bufferedWidth + (col+1)] = true;

                isTouchingSymbol[(row  )*bufferedWidth + (col-1)] = true;
                isTouchingSymbol[(row  )*bufferedWidth + (col+1)] = true;

                isTouchingSymbol[(row+1)*bufferedWidth + (col-1)] = true;
                isTouchingSymbol[(row+1)*bufferedWidth + (col  )] = true;
                isTouchingSymbol[(row+1)*bufferedWidth + (col+1)] = true;
            }
        }
    }

    u64 sum = 0;
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            u8 c = bufferedFile[row*bufferedWidth + col];
            if (is_numeric(c)){
                bool isTouching = false;
                u64  length     = 0;
                while (is_numeric(bufferedFile[row*bufferedWidth + (col+length)])) {
                    isTouching = isTouching || isTouchingSymbol[row*bufferedWidth + (col+length)];
                    length++;
                }
                u64 partNumber = string_to_int64(&bufferedFile[row*bufferedWidth + col], length);
                sum += (isTouching) ? partNumber : 0;
                col += length;
            }
        }
    }

    printf("%lld\n", sum);
}

static void 
day3_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day3.dat"));

    u64 width = 0;
    for (u64 idx = 0; idx < file.length; ++idx) {
        if (file.buffer[idx] == '\n') {
            width = idx+1;
            break;
        }
    }
    u64 height = file.length / width + 1;

    u64 bufferedWidth  = width + 2;
    u64 bufferedHeight = height + 2;

    u8 *bufferedFile = mem_arena_push_and_zero(arena, bufferedWidth * bufferedHeight, u8);
    for (u64 rowIdx = 1; rowIdx < (bufferedHeight-1); ++rowIdx) {
        os_mem_copy(&bufferedFile[rowIdx*bufferedWidth+1], &file.buffer[(rowIdx-1)*width], width*sizeof(u8));
    }

    bool *isTouchingSymbol = mem_arena_push_and_zero(arena, bufferedWidth * bufferedHeight, bool);
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            u8 c = bufferedFile[row*bufferedWidth + col];
            if (!(is_numeric(c) || c == '.' || c == 0 || c == '\n' || c == '\0')) {
                isTouchingSymbol[(row-1)*bufferedWidth + (col-1)] = true;
                isTouchingSymbol[(row-1)*bufferedWidth + (col  )] = true;
                isTouchingSymbol[(row-1)*bufferedWidth + (col+1)] = true;

                isTouchingSymbol[(row  )*bufferedWidth + (col-1)] = true;
                isTouchingSymbol[(row  )*bufferedWidth + (col+1)] = true;

                isTouchingSymbol[(row+1)*bufferedWidth + (col-1)] = true;
                isTouchingSymbol[(row+1)*bufferedWidth + (col  )] = true;
                isTouchingSymbol[(row+1)*bufferedWidth + (col+1)] = true;
            }
        }
    }

    u64 *partNumbers = mem_arena_push_and_zero(arena, bufferedWidth * bufferedHeight, u64);
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            u8 c = bufferedFile[row*bufferedWidth + col];
            if (is_numeric(c)){
                bool isTouching = false;
                u64  length     = 0;
                while (is_numeric(bufferedFile[row*bufferedWidth + (col+length)])) {
                    isTouching = isTouching || isTouchingSymbol[row*bufferedWidth + (col+length)];
                    length++;
                }
                u64 partNumber = string_to_int64(&bufferedFile[row*bufferedWidth + col], length);

                if (isTouching) {
                    for (u64 idx = 0; idx < length; ++idx) {
                        partNumbers[row*bufferedWidth + col + idx] = partNumber;
                    }
                }
                col += length;
            }
        }
    }

    u64 sum = 0;
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            u8 c = bufferedFile[row*bufferedWidth + col];
            if (c == '*') {
                u64 neighborIndices[8] = {(row-1)*bufferedWidth + (col-1),
                                          (row-1)*bufferedWidth + (col  ),
                                          (row-1)*bufferedWidth + (col+1),
                                          (row  )*bufferedWidth + (col-1),
                                          (row  )*bufferedWidth + (col+1),
                                          (row+1)*bufferedWidth + (col-1),
                                          (row+1)*bufferedWidth + (col  ),
                                          (row+1)*bufferedWidth + (col+1)};

                u64 nPartsTouchingGears = 0;
                u64 partNumbersTouchingGear[2] = {0};
                bool tooManyPartTouchingGear = false;
                for (u64 idx = 0; idx < 8; ++idx) {
                    if (partNumbers[neighborIndices[idx]] != 0 && 
                      !(partNumbers[neighborIndices[idx]] == partNumbersTouchingGear[0] ||
                        partNumbers[neighborIndices[idx]] == partNumbersTouchingGear[1])) 
                    {
                        if (nPartsTouchingGears == 2) {
                            tooManyPartTouchingGear = true;
                        }
                        partNumbersTouchingGear[nPartsTouchingGears++] = partNumbers[neighborIndices[idx]];
                    }

                    if (tooManyPartTouchingGear) {
                        break;
                    }
                }

                if (nPartsTouchingGears == 2) {
                    u64 gearRatio = partNumbersTouchingGear[0] * partNumbersTouchingGear[1];
                    sum += gearRatio;
                }
            }
        }
    }

    printf("%lld\n", sum);

}