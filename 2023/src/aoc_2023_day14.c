static void 
day14_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day14.dat"));
    u8 newLine[1] = {'\n'};

    String8List lines = string8_split(arena, file, newLine, 1);

    s64 width  = lines.first->string.length;
    s64 height = lines.nodeCount;
    u8 *data   = mem_arena_push(arena, width*height, u8);

    String8ListNode *line = lines.first;
    for (s64 row = 0; row < height; ++row) {
        os_mem_copy(&data[row*width], line->string.buffer, width);
        line = line->next;
    }

    // NORTH
    for (s64 row = 1; row < height; ++row) {
        for (s64 col = 0; col < width; ++col) {
            if (data[row*width+col] == 'O') {
                u64 rowToMoveTo = row;
                for (s64 row2 = (row-1); row2 >= 0; --row2) {
                    if (data[row2*width+col] != '.') {
                        break;
                    }
                    rowToMoveTo--;
                }
                if (rowToMoveTo != row) {
                    data[row*width+col] = '.';
                    data[rowToMoveTo*width+col] = 'O';
                }
            }
        }
    }

    u64 sum = 0;
    for (s64 col = 0; col < width; ++col) {
        u64 colSum = 0;
        for (s64 row = 0; row < height; ++row) {
            if (data[row*width+col] == 'O') {
                colSum += (height-row);
            }
        }
        sum += colSum;
    }
    printf("%llu\n",sum);
}

static void 
day14_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day14.dat"));
    u8 newLine[1] = {'\n'};

    String8List lines = string8_split(arena, file, newLine, 1);

    s64 width  = lines.first->string.length;
    s64 height = lines.nodeCount;
    u8 *data   = mem_arena_push(arena, width*height, u8);

    String8ListNode *line = lines.first;
    for (s64 row = 0; row < height; ++row) {
        os_mem_copy(&data[row*width], line->string.buffer, width);
        line = line->next;
    }

    for (s64 row = 0; row < height; ++row) {
        printf("%.*s\n", (int)width, &data[row*width]);
    }
    printf("\n");

    u64 nCycles = 10000;

    FILE *fileID;
    fopen_s(&fileID, "data14_pattern.dat", "wt");
    for (u64 cycles = 0; cycles < nCycles; ++cycles) {

        // NORTH
        for (s64 row = 1; row < height; ++row) {
            for (s64 col = 0; col < width; ++col) {
                if (data[row*width+col] == 'O') {
                    u64 rowToMoveTo = row;
                    for (s64 row2 = (row-1); row2 >= 0; --row2) {
                        if (data[row2*width+col] != '.') {
                            break;
                        }
                        rowToMoveTo--;
                    }
                    if (rowToMoveTo != row) {
                        data[row*width+col] = '.';
                        data[rowToMoveTo*width+col] = 'O';
                    }
                }
            }
        }

        // WEST
        for (s64 col = 1; col < width; ++col) {
            for (s64 row = 0; row < height; ++row) {
                if (data[row*width+col] == 'O') {
                    s64 colToMoveTo = col;
                    for (s64 col2 = (col-1); col2 >= 0; --col2) {
                        if (data[row*width+col2] != '.') {
                            break;
                        }
                        colToMoveTo--;
                    }
                    if (colToMoveTo != col) {
                        data[row*width+col] = '.';
                        data[row*width+colToMoveTo] = 'O';
                    }
                }
            }
        }

        // SOUTH
        for (s64 row = (height-2); row >= 0; --row) {
            for (s64 col = 0; col < width; ++col) {
                if (data[row*width+col] == 'O') {
                    s64 rowToMoveTo = row;
                    for (s64 row2 = (row+1); row2 < height; ++row2) {
                        if (data[row2*width+col] != '.') {
                            break;
                        }
                        rowToMoveTo++;
                    }
                    if (rowToMoveTo != row) {
                        data[row*width+col] = '.';
                        data[rowToMoveTo*width+col] = 'O';
                    }
                }
            }
        }

        // EAST
        for (s64 col = (width-2); col >= 0; --col) {
            for (s64 row = 0; row < height; ++row) {
                if (data[row*width+col] == 'O') {
                    s64 colToMoveTo = col;
                    for (s64 col2 = (col+1); col2 < width; ++col2) {
                        if (data[row*width+col2] != '.') {
                            break;
                        }
                        colToMoveTo++;
                    }
                    if (colToMoveTo != col) {
                        data[row*width+col] = '.';
                        data[row*width+colToMoveTo] = 'O';
                    }
                }
            }
        }

        u64 sum = 0;
        for (s64 col = 0; col < width; ++col) {
            u64 colSum = 0;
            for (s64 row = 0; row < height; ++row) {
                if (data[row*width+col] == 'O') {
                    colSum += (height-row);
                }
            }
            sum += colSum;
        }
        fprintf(fileID, "%llu, %llu\n",cycles, sum);
    }

    fclose(fileID);

    // used external program to figure out the pattern.

    
}