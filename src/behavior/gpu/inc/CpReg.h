#pragma once

union reg_CP_GLB_CTRL {
    struct {
        uint32_t        enable              : 1;
        uint32_t        stop             : 1;
        uint32_t        reserved            : 30;
    } bits;
    uint32_t            val;
};

union reg_CP_GLB_STATUS {
    struct {
        uint32_t        busy                : 1;
        uint32_t        active              : 1;
        uint32_t        reserved            : 31;
    } bits;
    uint32_t            val;
};

union reg_CP_QUEUE_CTRL {
    struct {
        uint32_t        enable              : 1;
        uint32_t        rst                 : 1;
        uint32_t        priority             : 3;
        uint32_t        reserved            : 27;
    } bits;
    uint32_t            val;
};

union reg_CP_QUEUE_RB_BASE {
    struct {
        uint32_t        lo;
        uint32_t        hi;
    } bits;
    uint64_t            val;
};

union reg_CP_QUEUE_RB_SIZE {
    struct {
        uint32_t        val;
    } bits;
    uint32_t            val;
};

union reg_CP_QUEUE_RB_RPTR {
    struct {
        uint32_t        lo;
        uint32_t        hi;
    } bits;
    uint64_t            val;
};

union reg_CP_QUEUE_RB_WPTR {
    struct {
        uint32_t        lo;
        uint32_t        hi;
    } bits;
    uint64_t            val;
};

union reg_CP_QUEUE_DOORBELL_BASE {
    struct {
        uint32_t        lo;
        uint32_t        hi;
    } bits;
    uint64_t            val;
};

union reg_CP_QUEUE_DOORBELL_OFFSET {
    struct {
        uint32_t        val;
    } bits;
    uint32_t            val;
};
