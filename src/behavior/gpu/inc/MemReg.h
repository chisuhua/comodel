#pragma once


union reg_MMU_CFG {
    struct {
        uint32_t        fragment_en             : 1;
        uint32_t        read_only_check_en      : 1;
        uint32_t        write_only_check_en     : 1;
        uint32_t        exec_only_check_en      : 1;
        uint32_t        inst_tlb_en             : 1;
    } bits;
    uint32_t            val;
};

union reg_MMU_STATUS {
    struct {
        uint32_t        busy                    : 1;
        uint32_t        rsvd                    : 31;
    } bits;
    uint32_t            val;
};

union reg_MMU_TLB_CFG {
    struct {
        uint32_t        pte_cache_en            : 1;
        uint32_t        pte_cache_size_sel      : 2;
        uint32_t        replace_type            : 1;
        uint32_t        inst_cache_size         : 1;
        uint32_t        rsvd                    : 1;
    } bits;
    uint32_t            val;
};


union reg_MMU_INV_CFG {
    struct {
        uint32_t        vmid_inv_en             : 1;
        uint32_t        range_inv_en            : 1;
        uint32_t        rsvd                    : 30;
    } bits;
    uint32_t            val;
};

union reg_MMU_INV_STATUS {
    struct {
        uint32_t        inv_done                : 1;
        uint32_t        exception_status        : 3;
        uint32_t        rsvd                    : 30;
    } bits;
    uint32_t            val;
};

union reg_MMU_DEV_PA_RANGE_BASE {
    struct {
        uint32_t        lo                  : 32;
        uint32_t        hi                  : 32;
    } bits;
    uint64_t            val;
};


union reg_MMU_DEV_PA_RANGE_TOP {
    struct {
        uint32_t        lo                  :32;
        uint32_t        hi                  :32;
    } bits;
    uint64_t            val;
};


union reg_MMU_SYS_PA_RANGE_BASE {
    struct {
        uint32_t        lo                  :32;
        uint32_t        hi                  :32;
    } bits;
    uint64_t            val;
};


union reg_MMU_SYS_PA_RANGE_TOP {
    struct {
        uint32_t        lo                  :32;
        uint32_t        hi                  :32;
    } bits;
    uint64_t            val;
};


union reg_MMU_SVM_RANGE_BASE {
    struct {
        uint32_t        lo                  :32;
        uint32_t        hi                  :32;
    } bits;
    uint64_t            val;
};

union reg_MMU_SVM_RANGE_TOP {
    struct {
        uint32_t        lo                  :32;
        uint32_t        hi                  :32;
    } bits;
    uint64_t            val;
};


union reg_MMU_PT_CFG {
    struct {
        uint32_t        physical_mode           : 1;
        uint32_t        pte_bit                 : 6;
        uint32_t        pde_bit                 : 5;
        uint32_t        pd0_bit                 : 5;
        uint32_t        pd1_bit                 : 5;
        uint32_t        rsvd                    : 10;
    } bits;
    uint32_t            val;
};

union reg_MMU_PT_BASE {
    struct {
        uint32_t        lo                  :32;
        uint32_t        hi                  :32;
    } bits;
    uint64_t            val;
};


union reg_MMU_PT_TOP {
    struct {
        uint32_t        lo                  :32;
        uint32_t        hi                  :32;
    } bits;
    uint64_t            val;
};


union reg_MMU_PWC_CFG {
    struct {
        uint32_t        pde_cache_en                :1;
        uint32_t        pd1_cache_en                :1;
        uint32_t        pd0_cache_en                :1;
        uint32_t        replace_type                :2;
        uint32_t        rsved_32_5                  :27;
    } bits;
    uint32_t            val;
};
/*
struct PageTablePte {
    uint64_t valid              : 1; //0
    uint64_t system             : 1; //1
    uint64_t reserved_0         : 1; //2
    uint64_t snooped            : 1; //3
    uint64_t executable         : 1; //4
    uint64_t readable           : 1; //5
    uint64_t writeable          : 1; //6
    uint64_t fragment           : 4; //10:7
    uint64_t reserved_1         : 1; //11
    uint64_t address            : 37; //48:12
    uint64_t reserved_2         : 3; //51:48
    uint64_t resident           : 1; //52
    uint64_t reserved_3         : 1; //53
    uint64_t pagesize           : 1; //54
    uint64_t level              : 3; //57:55
    uint64_t reserved_4         : 6; //63:58
};

struct PageTablePde {
    uint64_t valid              : 1; //0
    uint64_t system             : 1; //1
    uint64_t reserved_0         : 1; //2
    uint64_t snooped            : 1; //3
    uint64_t executable         : 1; //4
    uint64_t readable           : 1; //5
    uint64_t writeable          : 1; //6
    uint64_t fragment           : 4; //10:7
    uint64_t reserved_1         : 1; //11
    uint64_t address            : 37; //48:12
    uint64_t reserved_2         : 3; //51:48
    uint64_t resident           : 1; //52
    uint64_t pte                : 1; //53
    uint64_t pagesize           : 1; //54
    uint64_t level              : 3; //57:55
    uint64_t reserved_4         : 6; //63:58
};

// both PTE and PDE are 64bit width
typedef union {
    PageTablePte info;
    uint64_t     data;
} PTE;

typedef union {
    PageTablePde info;
    uint64_t     data;
} PDE;
*/

// #define PageSize 4096
// #define PageBits 12
// #define PteBits 9
// #define PdeBits 9
// #define Pd1Bits 9
// #define Pd0Bits 9
