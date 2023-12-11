static void 
day11_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day11.dat"));

    u64 width  = 0;
    for (u64 idx = 0; idx < file.length; ++idx) {
        if (file.buffer[idx] ==  '\n') {
            width = idx;
            break;
        }
    }
    u64 height = file.length / width;

    u8 *image = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u8);
    u64 bufferedWidth  = width + 2;
    u64 bufferedHeight = height + 2;

    for (u64 idx = 0; idx < height; ++idx) {
        os_mem_copy(&image[(idx+1)*bufferedWidth+1], &file.buffer[idx*(width+1)], width);
    }

    u64 *vacantColumnIdx = mem_arena_push(arena, bufferedWidth, u64);
    u64 nVacantColumns = 0;
    for (u64 col = 1; col < (bufferedWidth-1); ++col) {
        bool wasGalaxyFound = false;
        for (u64 row = 1; row < (bufferedHeight-1); ++row) {
            if (image[row*bufferedWidth + col] == '#') {
                wasGalaxyFound = true;
                break;
            }
        }

        if (!wasGalaxyFound) {
            vacantColumnIdx[nVacantColumns++] = col;
        }
    }

    u64 *vacantRowIdx = mem_arena_push(arena, bufferedHeight, u64);
    u64 nVacantRows = 0;
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        bool wasGalaxyFound = false;
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            if (image[row*bufferedWidth + col] == '#') {
                wasGalaxyFound = true;
                break;
            }
        }

        if (!wasGalaxyFound) {
           vacantRowIdx[nVacantRows++] = row;
        }
    }

    u64 expandedWidth = bufferedWidth + nVacantColumns;
    u64 expandedHeight = bufferedHeight + nVacantRows;
    u8 *expandedImage = mem_arena_push_and_zero(arena, expandedWidth * expandedHeight, u8);

    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        u64 nVacantColumnsAdded = 0;
        u64 vacantColIdx = 0;
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            if (col == vacantColumnIdx[vacantColIdx]) {
                expandedImage[row*expandedWidth+col+nVacantColumnsAdded++] = '.';
                expandedImage[row*expandedWidth+col+nVacantColumnsAdded] = '.';
                vacantColIdx++;
            }
            else {
                expandedImage[row*expandedWidth+col+nVacantColumnsAdded] = image[row*bufferedWidth+col];
            }
        }
    }

    u64 nVacantRowsAdded = 0;
    for (u64 rowIdx = 0; rowIdx < nVacantRows; ++rowIdx) {
        u64 row = vacantRowIdx[nVacantRows-rowIdx-1];
        u64 copyStart  =  row * expandedWidth;
        u64 destStart  = (row+1) * expandedWidth;
        u64 copyLength = (bufferedHeight - row+1+rowIdx) * expandedWidth;
        os_mem_copy(&expandedImage[destStart],&expandedImage[copyStart],copyLength);

        for (u64 col = 1; col < (expandedWidth-1); ++col) {
            expandedImage[row*expandedWidth+col]  = '.';
        }
    }

    u64 *galaxyIdx = mem_arena_push(arena, expandedWidth*expandedHeight, u64);
    u64 nGalaxies = 0;
    for (u64 idx = 0; idx < expandedWidth*expandedHeight; ++idx) {
        if (expandedImage[idx] == '#') {
            galaxyIdx[nGalaxies++] = idx;
        }
    }

    u64 sumDist = 0;
    for (u64 idx = 0; idx < nGalaxies; ++idx) {
        u64 row0 = galaxyIdx[idx] / expandedWidth;
        u64 col0 = galaxyIdx[idx] - row0 * expandedWidth;
        for (u64 idx2 = (idx+1); idx2 < nGalaxies; ++idx2) {
            u64 row1 = galaxyIdx[idx2] / expandedWidth;
            u64 col1 = galaxyIdx[idx2] - row1 * expandedWidth;

            sumDist += abs(row1-row0) + abs(col1-col0);
        }
    }
    printf("%llu\n",sumDist);
}

static void 
day11_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day11.dat"));

    u64 expansionFactor = 1000000-1;

    u64 width  = 0;
    for (u64 idx = 0; idx < file.length; ++idx) {
        if (file.buffer[idx] ==  '\n') {
            width = idx;
            break;
        }
    }
    u64 height = file.length / width;

    u8 *image = mem_arena_push_and_zero(arena, (width + 2) * (height + 2), u8);
    u64 bufferedWidth  = width + 2;
    u64 bufferedHeight = height + 2;

    for (u64 idx = 0; idx < height; ++idx) {
        os_mem_copy(&image[(idx+1)*bufferedWidth+1], &file.buffer[idx*(width+1)], width);
    }

    u64 *vacantColumnIdx = mem_arena_push(arena, bufferedWidth, u64);
    u64 nVacantColumns = 0;
    for (u64 col = 1; col < (bufferedWidth-1); ++col) {
        bool wasGalaxyFound = false;
        for (u64 row = 1; row < (bufferedHeight-1); ++row) {
            if (image[row*bufferedWidth + col] == '#') {
                wasGalaxyFound = true;
                break;
            }
        }

        if (!wasGalaxyFound) {
            vacantColumnIdx[nVacantColumns++] = col;
        }
    }

    u64 *vacantRowIdx = mem_arena_push(arena, bufferedHeight, u64);
    u64 nVacantRows = 0;
    for (u64 row = 1; row < (bufferedHeight-1); ++row) {
        bool wasGalaxyFound = false;
        for (u64 col = 1; col < (bufferedWidth-1); ++col) {
            if (image[row*bufferedWidth + col] == '#') {
                wasGalaxyFound = true;
                break;
            }
        }

        if (!wasGalaxyFound) {
           vacantRowIdx[nVacantRows++] = row;
        }
    }

    u64 *galaxyIdx = mem_arena_push(arena, bufferedWidth*bufferedHeight, u64);
    u64 nGalaxies = 0;
    for (u64 idx = 0; idx < bufferedWidth*bufferedHeight; ++idx) {
        if (image[idx] == '#') {
            galaxyIdx[nGalaxies++] = idx;
        }
    }

    u64 sumDist = 0;
    for (u64 idx = 0; idx < nGalaxies; ++idx) {
        u64 row0 = galaxyIdx[idx] / bufferedWidth;
        u64 col0 = galaxyIdx[idx] - row0 * bufferedWidth;
        for (u64 idx2 = (idx+1); idx2 < nGalaxies; ++idx2) {
            u64 row1 = galaxyIdx[idx2] / bufferedWidth;
            u64 col1 = galaxyIdx[idx2] - row1 * bufferedWidth;

            u64 sumRows = abs(row1 - row0);
            for (u64 i = MIN(row0,row1); i < MAX(row0,row1); ++i) {
                for (u64 j = 0; j < nVacantRows; ++j) {
                    if (i == vacantRowIdx[j]) {
                        sumRows += expansionFactor;
                    }
                }
            }

            u64 sumCols = abs(col1 - col0);
            for (u64 i = MIN(col0,col1); i < MAX(col0,col1); ++i) {
                for (u64 j = 0; j < nVacantColumns; ++j) {
                    if (i == vacantColumnIdx[j]) {
                        sumCols += expansionFactor;
                    }
                }
            }
            sumDist += sumRows + sumCols;
        }
    }
    
    printf("%llu\n",sumDist);
}