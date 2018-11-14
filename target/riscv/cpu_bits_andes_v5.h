/* Andes RISC-V V5 ISA Extension constants */

enum csr_mie_t {
    MIE_USIE       = 0,
    MIE_SSIE       = 1,
    MIE_MSIE       = 3,
    MIE_UTIE       = 4,
    MIE_STIE       = 5,
    MIE_MTIE       = 7,
    MIE_UEIE       = 8,
    MIE_SEIE       = 9,
    MIE_MEIE       = 11,
    MIE_IMECCI     = 16,
    MIE_BWEI       = 17,
    MIE_PMOVI      = 18,
};

#define MIP_IMECCI  (1 << MIE_IMECCI)
#define MIP_BWEI    (1 << MIE_BWEI)
#define MIP_PMOVI   (1 << MIE_PMOVI)

/* xSIE:        software interrupt
 * xTIE:        timer interrupt
 * xEIE:        external interrupt
 * IMECCI:      imprecise parity/ECC error interrupt
 * BWEI:        bus write transaction error interrupt
 * PMOVE:       performance counter interrupt
 */

enum csr_micm_cfg_t {
    MMSC_CFG_ISET       = 0,
    MMSC_CFG_IWAY       = 3,
    MMSC_CFG_ISZ        = 6,
    MMSC_CFG_ILCK       = 9,
    MMSC_CFG_IC_ECC     = 10,
    MMSC_CFG_ILMB       = 12,
    MMSC_CFG_ILMSZ      = 15,
    MMSC_CFG_IULM_2BANK = 20,
    MMSC_CFG_ILM_ECC    = 21,
};

/* ISET:        Icache sets per way (# of cache lines)
 * IWAY:        Icache ways
 * ISZ:         Icache block (line) size
 * ILCK:        Icache locking support
 * IC_ECC:      Icache Parity support
 * ILMB:        # of Instruction Local Memory Base registers
 * ILMSZ:       ILM size
 * IULM_2BANK:  Unified Local Memory has 2 banks or not
 * ILM_ECC:     ILM Parity support
 */

enum csr_mdcm_cfg_t {
    MMSC_CFG_DSET       = 0,
    MMSC_CFG_DWAY       = 3,
    MMSC_CFG_DSZ        = 6,
    MMSC_CFG_DLCK       = 9,
    MMSC_CFG_DC_ECC     = 10,
    MMSC_CFG_DLMB       = 12,
    MMSC_CFG_DLMSZ      = 15,
    MMSC_CFG_DULM_2BANK = 20,
    MMSC_CFG_DLM_ECC    = 21,
};

/* DSET:        Dcache sets per way (# of cache lines)
 * DWAY:        Dcache ways
 * DSZ:         Dcache block (line) size
 * DLCK:        Dcache locking support
 * DC_ECC:      Dcache Parity support
 * DLMB:        # of Data Local Memory Base registers
 * DLMSZ:       DLM size
 * DULM_2BANK:  Unified Local Memory has 2 banks or not
 * DLM_ECC:     DLM Parity support
 */

enum csr_mmsc_cfg_t {
    MMSC_CFG_ECC        = 0,
    MMSC_CFG_TLB_ECC    = 1,
    MMSC_CFG_ECD        = 3,
    MMSC_CFG_PFT        = 4,
    MMSC_CFG_HSP        = 5,
    MMSC_CFG_ACE        = 6,
    MMSC_CFG_VPLIC      = 12,
    MMSC_CFG_EV5PE      = 13,
    MMSC_CFG_LMSLVP     = 14,
    MMSC_CFG_PMNDS      = 15,
    MMSC_CFG_CCTLCSR    = 16,
    MMSC_CFG_EFHW       = 17,
    MMSC_CFG_VCCTL      = 18,
};

/* ECC:     global parity support
 * TLB_ECC: TLB parity support
 * ECD:     CodeDense extension support
 * PFT:     PowerBrake (Performance Throttling) support
 * HSP:     HW stack protection and recording support
 * ACE:     Andes Custom Extension support
 * VPLIC:   vectored PLIC mode support
 * EV5PE:   V5 performance extension support
 * LMSLVP:  local memory slave port support
 * PMNDS:   Andes-enhanced performance monitoring support
 * CCTLCSR: CCTL CSRs support
 * EFHW:    FLHW/FSHW instruction support
 * VCCTL:   CCTL version
 */

enum csr_mmisc_ctl_t {
    MMISC_CTL_ACE       = 0,
    MMISC_CTL_VEC_PLIC  = 1,
    MMISC_CTL_RVCOMPM   = 2,
    MMISC_CTL_BRPE      = 3,
    MMISC_CTL_ACES      = 4,
    MMISC_CTL_UNA       = 6,
};

/* ACE:         Andes Custom Extension enable
 * VEC_PLIC:    Vectored external PLIC interrupt enable
 * RVCOMPM:     RV compatibility mode enable
 * BRPE:        branch target buffer and return address stack enable
 * ACES:        ACE extension context status
 * UNA:         address misaligned exception disable
 */
