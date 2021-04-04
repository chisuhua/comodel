#pragma once


#define CP_REG_SIZE                  (1000)
#define mmCP_REG_ADDR                (1024)

#define CP_GLB_REG_SIZE              (100)
#define mmCP_GLB_REG_ADDR(offset)    (mmCP_REG_ADDR + (0x4 * offset))
#define CP_GLB_CTRL                  (0)
#define CP_GLB_STATUS                (1)

#define CP_MAX_QUEUE_NUM             (8)

#define CP_QUEUE_REG_SIZE            (80)
#define mmCP_QUEUE_REG_ADDR(i, offset) mmCP_GLB_REG_ADDR(CP_GLB_REG_SIZE) + (i * CP_QUEUE_REG_SIZE) + offset * 0x4

#define CP_QUEUE_REG_NUM            (11)

#define CP_QUEUE_CTRL               (0)
#define CP_QUEUE_RB_BASE_HI         (1)
#define CP_QUEUE_RB_BASE_LO         (2)
#define CP_QUEUE_RB_SIZE            (3)
#define CP_QUEUE_RB_RPTR_HI         (4)
#define CP_QUEUE_RB_RPTR_LO         (5)
#define CP_QUEUE_RB_WPTR_HI         (6)
#define CP_QUEUE_RB_WPTR_LO         (7)
#define CP_QUEUE_DOORBELL_BASE_HI   (8)
#define CP_QUEUE_DOORBELL_BASE_LO   (9)
#define CP_QUEUE_DOORBELL_OFFSET    (10)

#define CP_QUEUE_SCRATCH_BACKING_ADDR_HI    11
#define CP_QUEUE_SCRATCH_BACKING_ADDR_LO    12
#define CP_QUEUE_SCRATCH_BACKING_SIZE       13



#define MMU_REG_SIZE                  (1000)
#define mmMMU_REG_ADDR                (4096)

#define MMU_GLB_REG_SIZE              (100)
#define mmMMU_GLB_REG_ADDR(offset)    (mmMMU_REG_ADDR + (0x4 * offset))

#define MMU_CFG                      (0)
#define MMU_STATUS                    (1)
#define MMU_TLB_CFG			          (2)
// #define MMU_TLB_CFG			          (3)
#define MMU_INV_CFG			          (4)
#define MMU_INV_STATUS			      (5)
#define MMU_DEV_PA_RANGE_BASE_HI	  (6)
#define MMU_DEV_PA_RANGE_BASE_LO	  (7)
#define MMU_DEV_PA_RANGE_TOP_HI		  (8)
#define MMU_DEV_PA_RANGE_TOP_LO		  (9)
#define MMU_SYS_PA_RANGE_BASE_HI	  (10)
#define MMU_SYS_PA_RANGE_BASE_LO	  (11)
#define MMU_SYS_PA_RANGE_TOP_HI		  (12)
#define MMU_SYS_PA_RANGE_TOP_LO		  (13)
#define MMU_SVM_RANGE_BASE_HI		  (14)
#define MMU_SVM_RANGE_BASE_LO		  (15)
#define MMU_SVM_RANGE_TOP_HI		  (16)
#define MMU_SVM_RANGE_TOP_LO		  (17)
#define MMU_PT_CFG			          (18)
#define MMU_PT_BASE_HI			      (19)
#define MMU_PT_BASE_LO			      (20)
#define MMU_PT_TOP_HI			      (21)
#define MMU_PT_TOP_LO			      (22)
#define MMU_PWC_CFG			          (23)


