
FLAGS(DirLightProp) {
    DIR_LIGHT_PROP_NORTH = (1 << 0),
    DIR_LIGHT_PROP_SOUTH = (1 << 1),
    DIR_LIGHT_PROP_EAST  = (1 << 2),
    DIR_LIGHT_PROP_WEST  = (1 << 3),
};

static void propagate_north(u8 *data, s64 startRow, s64 startCol, u64 width, u64 height, u16 *energized);
static void propagate_south(u8 *data, s64 startRow, s64 startCol, u64 width, u64 height, u16 *energized);
static void propagate_east (u8 *data, s64 startRow, s64 startCol, u64 width, u64 height, u16 *energized);
static void propagate_west (u8 *data, s64 startRow, s64 startCol, u64 width, u64 height, u16 *energized);

static void
propagate_north(u8 *data, s64 startRow, s64 startCol, u64 width, u64 height, u16 *energized) {
    for (s64 row = startRow-1; row >= 0; --row) {
        u64 idx = row*width + startCol; 

        if  (energized[idx] & DIR_LIGHT_PROP_NORTH) {
            return;
        }

        energized[idx] |= DIR_LIGHT_PROP_NORTH;
        switch(data[idx]) {
            case '-': {
                propagate_east(data, row, startCol, width, height, energized);
                propagate_west(data, row, startCol, width, height, energized);
                return;
            } break;
            case '\\': {
                propagate_west(data, row, startCol, width, height, energized);
                return;
            } break;
            case '/': {
                propagate_east(data, row, startCol, width, height, energized);
                return;
            } break;
        }
    }
}

static void
propagate_south(u8 *data, s64 startRow, s64 startCol, u64 width, u64 height, u16 *energized) {
    for (s64 row = startRow+1; row < height; ++row) {
        u64 idx = row*width + startCol; 

        if  (energized[idx] & DIR_LIGHT_PROP_SOUTH) {
            return;
        }

        energized[idx] |= DIR_LIGHT_PROP_SOUTH;
        switch(data[idx]) {
            case '-': {
                propagate_east(data, row, startCol, width, height, energized);
                propagate_west(data, row, startCol, width, height, energized);
                return;
            } break;
            case '\\': {
                propagate_east(data, row, startCol, width, height, energized);
                return;
            } break;
            case '/': {
                propagate_west(data, row, startCol, width, height, energized);
                return;
            } break;
        }
    }
}

static void
propagate_east(u8 *data, s64 startRow, s64 startCol, u64 width, u64 height, u16 *energized) {
    for (s64 col = startCol+1; col < width; ++col) {
        u64 idx = startRow*width + col; 

        if  (energized[idx] & DIR_LIGHT_PROP_EAST) {
            return;
        }

        energized[idx] |= DIR_LIGHT_PROP_EAST;
        switch(data[idx]) {
            case '|': {
                propagate_north(data, startRow, col, width, height, energized);
                propagate_south(data, startRow, col, width, height, energized);
                return;
            } break;
            case '\\': {
                propagate_south(data, startRow, col, width, height, energized);
                return;
            } break;
            case '/': {
                propagate_north(data, startRow, col, width, height, energized);
                return;
            } break;
        }
    }
}

static void
propagate_west(u8 *data, s64 startRow, s64 startCol, u64 width, u64 height, u16 *energized) {
    for (s64 col = startCol-1; col >= 0; --col) {
        u64 idx = startRow*width + col;

        if  (energized[idx] & DIR_LIGHT_PROP_WEST) {
            return;
        }

        energized[idx] |= DIR_LIGHT_PROP_WEST;
        switch(data[idx]) {
            case '|': {
                propagate_north(data, startRow, col, width, height, energized);
                propagate_south(data, startRow, col, width, height, energized);
                return;
            } break;
            case '\\': {
                propagate_north(data, startRow, col, width, height, energized);
                return;
            } break;
            case '/': {
                propagate_south(data, startRow, col, width, height, energized);
                return;
            } break;
        }
    }
}

static u64 
num_tiles_energized(u16 *energized, u64 width, u64 height) {
    u64 result = 0;
    for (u64 idx = 0; idx < width * height; ++idx) {
        result += (energized[idx] > 0);
    }
    return(result);
}

static void 
day16_part1(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day16.dat"));
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

    u16 *energized = mem_arena_push_and_zero(arena, width*height, u16);
    propagate_east(data, 0, -1, width, height, energized);

    u64 sum = num_tiles_energized(energized, width, height);
    printf("%llu\n",sum);
}

static void 
day16_part2(MemoryArena *arena) {
    String8 file = os_file_read_all(arena, string8_from_cstr("day16.dat"));
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

    u16 *energized = mem_arena_push(arena, width*height, u16);

    u64 maxNumTilesEnergized = 0;
    for (u64 row = 0; row < height; ++row) {
        os_mem_zero(energized, width*height*sizeof(u16));
        propagate_east(data, row, -1, width, height, energized);
        u64 nTileEnergizedEast = num_tiles_energized(energized, width, height);
        maxNumTilesEnergized = MAX(maxNumTilesEnergized, nTileEnergizedEast);

        os_mem_zero(energized, width*height*sizeof(u16));
        propagate_west(data, row, width, width, height, energized);
        u64 nTileEnergizedWest = num_tiles_energized(energized, width, height);
        maxNumTilesEnergized = MAX(maxNumTilesEnergized, nTileEnergizedWest);
    }

    for (u64 col = 0; col < width; ++col) {
        os_mem_zero(energized, width*height*sizeof(u16));
        propagate_south(data, -1, col, width, height, energized);
        u64 nTileEnergizedSouth = num_tiles_energized(energized, width, height);
        maxNumTilesEnergized = MAX(maxNumTilesEnergized, nTileEnergizedSouth);

        os_mem_zero(energized, width*height*sizeof(u16));
        propagate_north(data, height, col, width, height, energized);
        u64 nTileEnergizedNorth = num_tiles_energized(energized, width, height);
        maxNumTilesEnergized = MAX(maxNumTilesEnergized, nTileEnergizedNorth);
    }

    printf("%llu\n",maxNumTilesEnergized);
}