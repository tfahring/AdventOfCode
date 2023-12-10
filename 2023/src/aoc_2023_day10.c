typedef enum {
    NONE  = 0,
    NORTH,
    SOUTH,
    EAST,
    WEST,
} Direction;

typedef enum {
    NO_CONNECTION = 0,
    NORTH_AND_SOUTH = '|',
    EAST_AND_WEST   = '-',
    NORTH_AND_EAST  = 'L',
    NORTH_AND_WEST  = 'J',
    SOUTH_AND_WEST  = '7',
    SOUTH_AND_EAST  = 'F',
} Connection;

static Direction
get_next_pos(Direction directionOfTravel, u8 nextPipe, u64 *currentRow, u64 *currentCol, u64 *nextRow, u64 *nextCol) {
    switch(directionOfTravel) {
        case NORTH: {
            switch (nextPipe) {
                case NORTH_AND_SOUTH: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    (*nextRow) -= 1;
                    // (*nextCol) += 0;
                    return(NORTH);
                } break;
                case SOUTH_AND_WEST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    // (*nextRow) += 0;
                    (*nextCol) -= 1;
                    return(WEST);
                } break;
                case SOUTH_AND_EAST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    // (*nextRow) += 0;
                    (*nextCol) += 1;
                    return(EAST);
                } break;
            }
        } break;
        case SOUTH: {
            switch (nextPipe) {
                case NORTH_AND_SOUTH: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    (*nextRow) += 1;
                    // (*nextCol) += 0;
                    return(SOUTH);
                } break;
                case NORTH_AND_WEST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    // (*nextRow) += 0;
                    (*nextCol) -= 1;
                    return(WEST);
                } break;
                case NORTH_AND_EAST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    // (*nextRow) += 0;
                    (*nextCol) += 1;
                    return(EAST);
                } break;
            }
        } break;
        case EAST: {
            switch (nextPipe) {
                case EAST_AND_WEST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    // (*nextRow) += 0;
                    (*nextCol) += 1;
                    return(EAST);
                } break;
                case NORTH_AND_WEST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    (*nextRow) -= 1;
                    // (*nextCol) += 0;
                    return(NORTH);
                } break;
                case SOUTH_AND_WEST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    (*nextRow) += 1;
                    // (*nextCol) += 0;
                    return(SOUTH);
                } break;
            }
        } break;
        case WEST: {
            switch (nextPipe) {
                case EAST_AND_WEST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    // (*nextRow) += 0;
                    (*nextCol) -= 1;
                    return(WEST);
                } break;
                case NORTH_AND_EAST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    (*nextRow) -= 1;
                    // (*nextCol) += 0;
                    return(NORTH);
                } break;
                case SOUTH_AND_EAST: {
                    (*currentRow) = (*nextRow);
                    (*currentCol) = (*nextCol);
                    (*nextRow) += 1;
                    // (*nextCol) += 0;
                    return(SOUTH);
                } break;
            }
        } break;
    }
    return(NONE);
}


static void 
day10_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day10.dat"));

    u64 width  = 0;
    for (u64 idx = 0; idx < file.length; ++idx) {
        if (file.buffer[idx] ==  '\n') {
            width = idx;
            break;
        }
    }
    u64 height = file.length / width;

    u8 *data = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u8);
    u64 *N = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u64);
    u64 *S = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u64);
    u64 *E = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u64);
    u64 *W = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u64);
    u64 bufferedWidth  = width + 2;
    u64 bufferedHeight = height + 2;

    for (u64 idx = 0; idx < height; ++idx) {
        os_mem_copy(&data[(idx+1)*bufferedWidth+1], &file.buffer[idx*(width+1)], width);
    }

    u64 sRow = 0;
    u64 sCol = 0;
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            if (data[row*bufferedWidth+col] == 'S') {
                sRow = row;
                sCol = col;
            }
        }
    }

   
   { 
        // NORTH
        u64 currentRow = sRow;
        u64 currentCol = sCol;
        u64 nextRow    = sRow - 1;
        u64 nextCol    = sCol;
        u8  nextC      = data[nextRow*bufferedWidth + nextCol];

        Direction dir = get_next_pos(NORTH, nextC, &currentRow, &currentCol, &nextRow, &nextCol);

        if (dir) {
            u64 count = 1;
            N[currentRow*bufferedWidth + currentCol] = count++;
            nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            while (nextC != 'S') {
                dir = get_next_pos(dir, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
                N[currentRow*bufferedWidth + currentCol] = count++;
                nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            }
        }
    }

    { 
        // SOUTH
        u64 currentRow = sRow;
        u64 currentCol = sCol;
        u64 nextRow    = sRow + 1;
        u64 nextCol    = sCol;
        u8  nextC      = data[nextRow*bufferedWidth + nextCol];

        Direction dir = get_next_pos(SOUTH, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
        if (dir) {
            u64 count = 1;
            S[currentRow*bufferedWidth + currentCol] = count++;
            nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            while (nextC != 'S') {
                dir = get_next_pos(dir, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
                S[currentRow*bufferedWidth + currentCol] = count++;
                nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            }
        }
    }

    { 
        // EAST
        u64 currentRow = sRow;
        u64 currentCol = sCol;
        u64 nextRow    = sRow;
        u64 nextCol    = sCol + 1;
        u8  nextC      = data[nextRow*bufferedWidth + nextCol];

        Direction dir = get_next_pos(EAST, nextC, &currentRow, &currentCol, &nextRow, &nextCol);

        if (dir) {
            u64 count = 1;
            E[currentRow*bufferedWidth + currentCol] = count++;
            nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            while (nextC != 'S') {
                dir = get_next_pos(dir, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
                E[currentRow*bufferedWidth + currentCol] = count++;
                nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            }
        }
    }

    { 
        // WEST
        u64 currentRow = sRow;
        u64 currentCol = sCol;
        u64 nextRow    = sRow;
        u64 nextCol    = sCol - 1;
        u8  nextC      = data[nextRow*bufferedWidth + nextCol];

        Direction dir = get_next_pos(WEST, nextC, &currentRow, &currentCol, &nextRow, &nextCol);

        if (dir) {
            u64 count = 1;
            W[currentRow*bufferedWidth + currentCol] = count++;
            nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            while (nextC != 'S') {
                dir = get_next_pos(dir, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
                W[currentRow*bufferedWidth + currentCol] = count++;
                nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            }
        }
    }

    u64 maxDistAlongPipe = 0;
    for (u64 idx = 0; idx < (bufferedWidth*bufferedHeight); ++idx) {
        u64 dist = 0xFFFFFFFFFFFFFFFF;
        dist = (N[idx] == 0) ? dist : MIN(dist, N[idx]);
        dist = (S[idx] == 0) ? dist : MIN(dist, S[idx]);
        dist = (E[idx] == 0) ? dist : MIN(dist, E[idx]);
        dist = (W[idx] == 0) ? dist : MIN(dist, W[idx]);

        maxDistAlongPipe = (dist == 0xFFFFFFFFFFFFFFFF) ? maxDistAlongPipe : MAX(maxDistAlongPipe, dist);
    }

    printf("%llu\n", maxDistAlongPipe);
}


static void 
day10_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day10.dat"));

    u64 width  = 0;
    for (u64 idx = 0; idx < file.length; ++idx) {
        if (file.buffer[idx] ==  '\n') {
            width = idx;
            break;
        }
    }
    u64 height = file.length / width;

    u8 *data = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u8);
    u64 *N = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u64);
    u64 *S = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u64);
    u64 *E = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u64);
    u64 *W = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u64);
    u64 bufferedWidth  = width + 2;
    u64 bufferedHeight = height + 2;

    for (u64 idx = 0; idx < height; ++idx) {
        os_mem_copy(&data[(idx+1)*bufferedWidth+1], &file.buffer[idx*(width+1)], width);
    }

    u64 sRow = 0;
    u64 sCol = 0;
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            if (data[row*bufferedWidth+col] == 'S') {
                sRow = row;
                sCol = col;
            }
        }
    }

   bool northWasConnected = false;
   bool southWasConnected = false;
   bool eastWasConnected = false;
   bool westWasConnected = false;
   { 
        // NORTH
        u64 currentRow = sRow;
        u64 currentCol = sCol;
        u64 nextRow    = sRow - 1;
        u64 nextCol    = sCol;
        u8  nextC      = data[nextRow*bufferedWidth + nextCol];

        Direction dir = get_next_pos(NORTH, nextC, &currentRow, &currentCol, &nextRow, &nextCol);

        if (dir) {
            u64 count = 1;
            N[currentRow*bufferedWidth + currentCol] = count++;
            nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            while (nextC != 'S') {
                dir = get_next_pos(dir, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
                N[currentRow*bufferedWidth + currentCol] = count++;
                nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            }
            northWasConnected = true;
        }
    }

    { 
        // SOUTH
        u64 currentRow = sRow;
        u64 currentCol = sCol;
        u64 nextRow    = sRow + 1;
        u64 nextCol    = sCol;
        u8  nextC      = data[nextRow*bufferedWidth + nextCol];

        Direction dir = get_next_pos(SOUTH, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
        if (dir) {
            u64 count = 1;
            S[currentRow*bufferedWidth + currentCol] = count++;
            nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            while (nextC != 'S') {
                dir = get_next_pos(dir, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
                S[currentRow*bufferedWidth + currentCol] = count++;
                nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            }
            southWasConnected = true;
        }
    }

    { 
        // EAST
        u64 currentRow = sRow;
        u64 currentCol = sCol;
        u64 nextRow    = sRow;
        u64 nextCol    = sCol + 1;
        u8  nextC      = data[nextRow*bufferedWidth + nextCol];

        Direction dir = get_next_pos(EAST, nextC, &currentRow, &currentCol, &nextRow, &nextCol);

        if (dir) {
            u64 count = 1;
            E[currentRow*bufferedWidth + currentCol] = count++;
            nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            while (nextC != 'S') {
                dir = get_next_pos(dir, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
                E[currentRow*bufferedWidth + currentCol] = count++;
                nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            }
            eastWasConnected = true;
        }
    }

    { 
        // WEST
        u64 currentRow = sRow;
        u64 currentCol = sCol;
        u64 nextRow    = sRow;
        u64 nextCol    = sCol - 1;
        u8  nextC      = data[nextRow*bufferedWidth + nextCol];

        Direction dir = get_next_pos(WEST, nextC, &currentRow, &currentCol, &nextRow, &nextCol);

        if (dir) {
            u64 count = 1;
            W[currentRow*bufferedWidth + currentCol] = count++;
            nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            while (nextC != 'S') {
                dir = get_next_pos(dir, nextC, &currentRow, &currentCol, &nextRow, &nextCol);
                W[currentRow*bufferedWidth + currentCol] = count++;
                nextC  = data[(nextRow)*bufferedWidth + (nextCol)];
            }
            westWasConnected = true;
        }
    }

    u64 *map = NULL;
    if      (northWasConnected) { map = N;}
    else if (southWasConnected) { map = S;}
    else if (eastWasConnected)  { map = E;}
    else if (westWasConnected)  { map = W;}

    map[sRow*bufferedWidth + sCol] = 1;

    u64 nInsideTiles = 0;
    for (s64 row = 1; row < (bufferedHeight - 1); ++row) {
        for (s64 col = 1; col < (bufferedWidth - 1); ++col) {
            if (map[row * bufferedWidth + col] == 0) {
                nInsideTiles++;
            }
            else {
            }
        }
    }

    for (s64 row = 1; row < (bufferedHeight-1); ++row) {
        for (s64 col = 1; col < (bufferedWidth-1); ++col) {
            if (map[row*bufferedWidth + col] == 0) {

                u64 crossCountN = 0;
                u64 crossCountS = 0;
                u64 crossCountE = 0;
                u64 crossCountW = 0;
                // North
                {
                    Connection prev = NO_CONNECTION;
                    s64 idx = -1;
                    while (row + idx >= 0) {
                        
                        if ( map[(row+idx)*bufferedWidth + col] && 
                            data[(row+idx)*bufferedWidth + col] != NORTH_AND_SOUTH) 
                        {
                            Direction dir = data[(row+idx)*bufferedWidth + col];
                            switch(dir) {
                                case EAST_AND_WEST: {
                                    crossCountN++;
                                    prev = NO_CONNECTION;
                                } break;
                                case NORTH_AND_WEST: 
                                case NORTH_AND_EAST: {
                                    prev = dir;
                                } break;
                                case SOUTH_AND_WEST: {
                                    if (prev == NORTH_AND_EAST) {
                                        crossCountN++;
                                    }
                                    prev = NO_CONNECTION;
                                } break;
                                case SOUTH_AND_EAST: {
                                    if (prev == NORTH_AND_WEST) {
                                        crossCountN++;
                                    }
                                    prev = NO_CONNECTION;
                                } break;
                            }
                        }
                        idx--;
                    }
                }

                // South
                {
                    Connection prev = NO_CONNECTION;
                    s64 idx = 1;
                    while (row + idx < bufferedHeight) {
                        
                        if ( map[(row+idx)*bufferedWidth + col] && 
                            data[(row+idx)*bufferedWidth + col] != NORTH_AND_SOUTH) 
                        {
                            Direction dir = data[(row+idx)*bufferedWidth + col];
                            switch(dir) {
                                case EAST_AND_WEST: {
                                    crossCountS++;
                                    prev = NO_CONNECTION;
                                } break;
                                case SOUTH_AND_WEST: 
                                case SOUTH_AND_EAST: {
                                    prev = dir;
                                } break;
                                case NORTH_AND_WEST: {
                                    if (prev == SOUTH_AND_EAST) {
                                        crossCountS++;
                                    }
                                    prev = NO_CONNECTION;
                                } break;
                                case NORTH_AND_EAST: {
                                    if (prev == SOUTH_AND_WEST) {
                                        crossCountS++;
                                    }
                                    prev = NO_CONNECTION;
                                } break;
                            }
                        }
                        idx++;
                    }
                }

                // East
                {
                    Connection prev = NO_CONNECTION;
                    s64 idx = 1;
                    while (col + idx < bufferedWidth) {
                        
                        if ( map[row*bufferedWidth + col+idx] && 
                            data[row*bufferedWidth + col+idx] != EAST_AND_WEST) 
                        {
                            Direction dir = data[row*bufferedWidth + col+idx];
                            switch(dir) {
                                case NORTH_AND_SOUTH: {
                                    crossCountE++;
                                    prev = NO_CONNECTION;
                                } break;
                                case SOUTH_AND_EAST: 
                                case NORTH_AND_EAST: {
                                    prev = dir;
                                } break;
                                case NORTH_AND_WEST: {
                                    if (prev == SOUTH_AND_EAST) {
                                        crossCountE++;
                                    }
                                    prev = NO_CONNECTION;
                                } break;
                                case SOUTH_AND_WEST: {
                                    if (prev == NORTH_AND_EAST) {
                                        crossCountE++;
                                    }
                                    prev = NO_CONNECTION;
                                } break;
                            }
                        }
                        idx++;
                    }
                }

                // West
                {
                    Connection prev = NO_CONNECTION;
                    s64 idx = -1;
                    while (col + idx >= 0) {
                        
                        if ( map[row*bufferedWidth + col+idx] && 
                            data[row*bufferedWidth + col+idx] != EAST_AND_WEST) 
                        {
                            Direction dir = data[row*bufferedWidth + col+idx];
                            switch(dir) {
                                case NORTH_AND_SOUTH: {
                                    crossCountW++;
                                    prev = NO_CONNECTION;
                                } break;
                                case SOUTH_AND_WEST: 
                                case NORTH_AND_WEST: {
                                    prev = dir;
                                } break;
                                case NORTH_AND_EAST: {
                                    if (prev == SOUTH_AND_WEST) {
                                        crossCountW++;
                                    }
                                    prev = NO_CONNECTION;
                                } break;
                                case SOUTH_AND_EAST: {
                                    if (prev == NORTH_AND_WEST) {
                                        crossCountW++;
                                    }
                                    prev = NO_CONNECTION;
                                } break;
                            }
                        }
                        idx--;
                    }
                }

                bool areAnyZero = crossCountN == 0 || crossCountS == 0 || crossCountE == 0 || crossCountW == 0;
                bool areAnyOdd = (crossCountN % 2);
                if (areAnyZero || !areAnyOdd) {
                    nInsideTiles--;
                }
            }
        }
    }

    printf("%llu\n", nInsideTiles);
    int debug = 1;

}