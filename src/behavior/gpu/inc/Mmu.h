#pragma once

struct PteEntry {
    uint64_t    valid               : 1; // 0
    uint64_t    system              : 1; // 1
    uint64_t    snooped             : 1; // 2
    uint64_t    executable          : 1; // 3
    uint64_t    readable            : 1; // 4
    uint64_t    writeable           : 1; // 5
    uint64_t    fragment            : 4; // 9:6
    uint64_t    transfurther        : 1; // 10
    uint64_t    reserved_0          : 1; // 11
    uint64_t    address             : 37; // 48: 12
    uint64_t    reserved_1          : 3; // 51:49
    uint64_t    resident            : 1; // 52
    uint64_t    reserved_2          : 1; // 53
    uint64_t    pagesize            : 1; // 54
    uint64_t    level               : 3; // 57:55
    uint64_t    reserved_3          : 6; // 63:58
};

struct PdeEntry {
    uint64_t    valid               : 1; // 0
    uint64_t    system              : 1; // 1
    uint64_t    snooped             : 1; // 2
    uint64_t    executable          : 1; // 3
    uint64_t    readable            : 1; // 4
    uint64_t    writeable           : 1; // 5
    uint64_t    fragment            : 4; // 9:6
    uint64_t    transfurther        : 1; // 10
    uint64_t    reserved_0          : 1; // 11
    uint64_t    address             : 37; // 48: 12
    uint64_t    reserved_1          : 3; // 51:49
    uint64_t    resident            : 1; // 52
    uint64_t    pte                 : 1; // 53
    uint64_t    pagesize            : 1; // 54
    uint64_t    level               : 3; // 57:55
    uint64_t    reserved_3          : 6; // 63:58
};

typedef union {
    PteEntry    info;
    uint64_t    data;
} PTE;


typedef union {
    PdeEntry    info;
    uint64_t    data;
} PDE;

enum class PteLevel : uint32_t {
    LEVEL_PTE   = 0,
    LEVEL_PDE   = 1,
    LEVEL_PD1   = 2,
    LEVEL_PD0   = 3
};

#define PAGE_4KB_BIT_NUM            (12)
#define PAGE_64KB_BIT_NUM           (16)
#define PD0_ADDR_BIT_NUM            (10)
#define PD1_ADDR_BIT_NUM            (9)
#define PDE_ADDR_BIT_NUM            (9)
#define PTE_ADDR_BIT_NUM            (9)
#define ART_ADDR_BIT_NUM            (9)
#define VA_WIDTH                    (48)
#define PA_WIDTH                    (40)


struct PageFault {
    uint64_t    miss               : 1; // 0
};


// size in bytes
#define CACHELINE_SIZE      128
#define PAGE_4KB_SIZE           (1 << PAGE_4KB_BIT_NUM)


