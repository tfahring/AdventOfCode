#include "aoc_lib.h"

GLOBAL       MemoryArena          os_arena;
GLOBAL       OS_Info              os_info;
THREAD_LOCAL MEM_ScratchArenaPool scratchArenaPool;

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#include "aoc_lib.c"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "aoc_2023_day1.c"
#include "aoc_2023_day2.c"
#include "aoc_2023_day3.c"
#include "aoc_2023_day4.c"
#include "aoc_2023_day5.c"
#include "aoc_2023_day6.c"
#include "aoc_2023_day7.c"
#include "aoc_2023_day8.c"
#include "aoc_2023_day9.c"
#include "aoc_2023_day10.c"
#include "aoc_2023_day11.c"
#include "aoc_2023_day12.c"
#include "aoc_2023_day13.c"

int main(int argc, char *argv[]) {
    os_init();
    MemoryArena arena = mem_arena_init(GIGABYTES(1));

    // day1_part1(&arena);
    // day1_part2(&arena);

    // day2_part1(&arena);
    // day2_part2(&arena);

    // day3_part1(&arena);
    // day3_part2(&arena);

    // day4_part1(&arena);
    // day4_part2(&arena);

    // day5_part1(&arena);
    // day5_part2(&arena);

    // day6_part1(&arena);
    // day6_part2(&arena);

    // day7_part1(&arena);
    // day7_part2(&arena);

    // day8_part1(&arena);
    // day8_part2(&arena);

    // day9_part1(&arena);
    // day9_part2(&arena);

    // day10_part1(&arena);
    // day10_part2(&arena);

    // day11_part1(&arena);
    // day11_part2(&arena);

    // day12_part1(&arena);
    // day12_part2(&arena);

    day13_part1(&arena);
    day13_part2(&arena);
}