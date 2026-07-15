/* Default linker script, for normal executables */
OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)

__TRICORE_DERIVATE_MEMORY_MAP__ = 0x390;

LCF_CSA0_SIZE = 12k;
LCF_USTACK0_SIZE = 10k;
LCF_ISTACK0_SIZE = 1k;

LCF_CSA1_SIZE = 8k;
LCF_USTACK1_SIZE = 2k;
LCF_ISTACK1_SIZE = 1k;

LCF_CSA2_SIZE = 8k;
LCF_USTACK2_SIZE = 2k;
LCF_ISTACK2_SIZE = 1k;

LCF_CSA3_SIZE = 8k;
LCF_USTACK3_SIZE = 2k;
LCF_ISTACK3_SIZE = 1k;

LCF_CSA4_SIZE = 8k;
LCF_USTACK4_SIZE = 2k;
LCF_ISTACK4_SIZE = 1k;

LCF_CSA5_SIZE = 8k;
LCF_USTACK5_SIZE = 2k;
LCF_ISTACK5_SIZE = 1k;

LCF_HEAP_SIZE = 4k;

LCF_DSPR5_START = 0x10000000;
LCF_DSPR5_SIZE = 96k;

LCF_DSPR4_START = 0x30000000;
LCF_DSPR4_SIZE = 96k;

LCF_DSPR3_START = 0x40000000;
LCF_DSPR3_SIZE = 96k;

LCF_DSPR2_START = 0x50000000;
LCF_DSPR2_SIZE = 96k;

LCF_DSPR1_START = 0x60000000;
LCF_DSPR1_SIZE = 240k;

LCF_DSPR0_START = 0x70000000;
LCF_DSPR0_SIZE = 240k;

LCF_CSA5_OFFSET =  (LCF_DSPR5_SIZE - 1k - LCF_CSA5_SIZE);
LCF_ISTACK5_OFFSET =  (LCF_CSA5_OFFSET - 256 - LCF_ISTACK5_SIZE);
LCF_USTACK5_OFFSET =  (LCF_ISTACK5_OFFSET - 256 - LCF_USTACK5_SIZE);

LCF_CSA4_OFFSET =  (LCF_DSPR4_SIZE - 1k - LCF_CSA4_SIZE);
LCF_ISTACK4_OFFSET =  (LCF_CSA4_OFFSET - 256 - LCF_ISTACK4_SIZE);
LCF_USTACK4_OFFSET =  (LCF_ISTACK4_OFFSET - 256 - LCF_USTACK4_SIZE);

LCF_CSA3_OFFSET =  (LCF_DSPR3_SIZE - 1k - LCF_CSA3_SIZE);
LCF_ISTACK3_OFFSET =  (LCF_CSA3_OFFSET - 256 - LCF_ISTACK3_SIZE);
LCF_USTACK3_OFFSET =  (LCF_ISTACK3_OFFSET - 256 - LCF_USTACK3_SIZE);

LCF_CSA2_OFFSET  =  (LCF_DSPR2_SIZE - 1k - LCF_CSA2_SIZE);
LCF_ISTACK2_OFFSET =  (LCF_CSA2_OFFSET - 256 - LCF_ISTACK2_SIZE);
LCF_USTACK2_OFFSET =  (LCF_ISTACK2_OFFSET - 256 - LCF_USTACK2_SIZE);

LCF_CSA1_OFFSET  =  (LCF_DSPR1_SIZE - 1k - LCF_CSA1_SIZE);
LCF_ISTACK1_OFFSET =  (LCF_CSA1_OFFSET - 256 - LCF_ISTACK1_SIZE);
LCF_USTACK1_OFFSET =  (LCF_ISTACK1_OFFSET - 256 - LCF_USTACK1_SIZE);

LCF_CSA0_OFFSET  =  (LCF_DSPR0_SIZE - 1k - LCF_CSA0_SIZE);
LCF_ISTACK0_OFFSET =  (LCF_CSA0_OFFSET - 256 - LCF_ISTACK0_SIZE);
LCF_USTACK0_OFFSET =  (LCF_ISTACK0_OFFSET - 256 - LCF_USTACK0_SIZE);

LCF_HEAP0_OFFSET =  (LCF_USTACK0_OFFSET - LCF_HEAP_SIZE);
LCF_HEAP1_OFFSET =  (LCF_USTACK1_OFFSET - LCF_HEAP_SIZE);
LCF_HEAP2_OFFSET =  (LCF_USTACK2_OFFSET - LCF_HEAP_SIZE);
LCF_HEAP3_OFFSET =  (LCF_USTACK3_OFFSET - LCF_HEAP_SIZE);
LCF_HEAP4_OFFSET =  (LCF_USTACK4_OFFSET - LCF_HEAP_SIZE);
LCF_HEAP5_OFFSET =  (LCF_USTACK5_OFFSET - LCF_HEAP_SIZE);

LCF_INTVEC_START = 0x8020E000;
/*LCF_INTVEC1_START = 0x805FE000;*/
/*LCF_INTVEC2_START = 0x808FE000;*/
/*LCF_INTVEC3_START = 0x80BFE000;*/
/*LCF_INTVEC4_START = 0x80EFE000;*/
/*LCF_INTVEC5_START = 0x80FFE000;*/


/*__INTTAB_CPU0 = LCF_INTVEC0_START;*/
/*__INTTAB_CPU1 = LCF_INTVEC0_START;*/
/*__INTTAB_CPU2 = LCF_INTVEC0_START;*/
/*__INTTAB_CPU3 = LCF_INTVEC0_START;*/
/*__INTTAB_CPU4 = LCF_INTVEC0_START;*/
/*__INTTAB_CPU5 = LCF_INTVEC0_START;*/


LCF_TRAPVEC_START = 0x80000100;
/*LCF_TRAPVEC1_START = 0x80300000;*/
/*LCF_TRAPVEC2_START = 0x80600000;*/
/*LCF_TRAPVEC3_START = 0x80900000;*/
/*LCF_TRAPVEC4_START = 0x80C00000;*/
/*LCF_TRAPVEC5_START = 0x80F00000;*/


LCF_STARTPTR_CPU0 = 0x80000200;
LCF_STARTPTR_CPU1 = 0x80300100;
LCF_STARTPTR_CPU2 = 0x80600100;
LCF_STARTPTR_CPU3 = 0x80900100;
LCF_STARTPTR_CPU4 = 0x80C00100;
LCF_STARTPTR_CPU5 = 0x80F00100;

CALIBRATION_AREA = 0x80180000;
CALIBRATION_SIZE = 0x00004000;

RESET = 0xA0000000;

BRIDGE = 0xF0040008; 
MEMORY
{
  dsram5_local (w!xp): org = 0xd0000000, len = 96K
  dsram5 (w!xp): org = 0x10000000, len = 96K
  psram5 (w!xp): org = 0x10100000, len = 64K
  
  dsram4_local (w!xp): org = 0xd0000000, len = 96K
  dsram4 (w!xp): org = 0x30000000, len = 96K
  psram4 (w!xp): org = 0x30100000, len = 64K
  
  dsram3_local (w!xp): org = 0xd0000000, len = 96K
  dsram3 (w!xp): org = 0x40000000, len = 96K
  psram3 (w!xp): org = 0x40100000, len = 64K

  dsram2_local (w!xp): org = 0xd0000000, len = 96K
  dsram2 (w!xp): org = 0x50000000, len = 96K
  psram2 (w!xp): org = 0x50100000, len = 64K
  
  dsram1_local (w!xp): org = 0xd0000000, len = 240K
  dsram1 (w!xp): org = 0x60000000, len = 240K
  psram1 (w!xp): org = 0x60100000, len = 64K
  
  dsram0_local (w!xp): org = 0xd0000000, len = 240K
  dsram0 (w!xp): org = 0x70000000, len = (240K-16K)
  psram0 (w!xp): org = 0x70100000, len = 64K

  calib_ram (w!xp): org = 0x70030000, len = 16K

  psram_local (w!xp): org = 0xc0000000, len = 64K
  
  pfls0_a (rx!p): org = 0x80000000, len = 0x10000
  pfls0_hsm (r): org = 0x80010000, len = 0x28000
  pfls0_b (rx!p): org = 0x80038000, len = 3M - 38000

  pfls0_nc_a (rx!p): org = 0xa0000000, len = 3M
  pfls0_nc_hsm (rx!p): org = 0xa0010000, len = 0x28000
  pfls0_nc_b (rx!p): org = 0xa0038000, len = 3M - 38000

  calib_mem (rx!p): org = 0x8001C000, len = 16K
  
  pfls1 (rx!p): org = 0x80300000, len = 3M
  pfls1_nc (rx!p): org = 0xa0300000, len = 3M
  
  pfls2 (rx!p): org = 0x80600000, len = 3M
  pfls2_nc (rx!p): org = 0xa0600000, len = 3M
  
  pfls3 (rx!p): org = 0x80900000, len = 3M
  pfls3_nc (rx!p): org = 0xa0900000, len = 3M
  
  pfls4 (rx!p): org = 0x80c00000, len = 3M
  pfls4_nc (rx!p): org = 0xa0c00000, len = 3M
  
  pfls5 (rx!p): org = 0x80f00000, len = 1M
  pfls5_nc (rx!p): org = 0xa0f00000, len = 1M
  
  dfls0 (rx!p): org = 0xaf000000, len = 1M
  
  ucb (rx!p): org = 0xaf400000, len = 24K
  
  cpu0_dlmu (w!xp): org = 0x90000000, len = 64K
  cpu0_dlmu_nc (w!xp): org = 0xb0000000, len = 64K
  
  cpu1_dlmu (w!xp): org = 0x90010000, len = 64K
  cpu1_dlmu_nc (w!xp): org = 0xb0010000, len = 64K
  
  cpu2_dlmu (w!xp): org = 0x90020000, len = 64K
  cpu2_dlmu_nc (w!xp): org = 0xb0020000, len = 64K
  
  cpu3_dlmu (w!xp): org = 0x90030000, len = 64K
  cpu3_dlmu_nc (w!xp): org = 0xb0030000, len = 64K
  
  lmuram (w!xp): org = 0x90040000, len = 768K
  lmuram_nc (w!xp): org = 0xb0040000, len = 768K
  
  cpu4_dlmu (w!xp): org = 0x90100000, len = 64K
  cpu4_dlmu_nc (w!xp): org = 0xb0100000, len = 64K
  
  cpu5_dlmu (w!xp): org = 0x90110000, len = 64K
  cpu5_dlmu_nc (w!xp): org = 0xb0110000, len = 64K
  
  edmem (w!xp): org = 0x99000000, len = 4M
  edmem_nc (w!xp): org = 0xb9000000, len = 4M
}  

/* map local memory address to a global address */ 
REGION_MAP( CPU0 , ORIGIN(dsram0_local), LENGTH(dsram0_local), ORIGIN(dsram0))
REGION_MAP( CPU1 , ORIGIN(dsram1_local), LENGTH(dsram1_local), ORIGIN(dsram1))
REGION_MAP( CPU2 , ORIGIN(dsram2_local), LENGTH(dsram2_local), ORIGIN(dsram2))
REGION_MAP( CPU3 , ORIGIN(dsram3_local), LENGTH(dsram3_local), ORIGIN(dsram3))
REGION_MAP( CPU4 , ORIGIN(dsram4_local), LENGTH(dsram4_local), ORIGIN(dsram4))
REGION_MAP( CPU5 , ORIGIN(dsram5_local), LENGTH(dsram5_local), ORIGIN(dsram5))

/*Un comment one of the below statement groups to enable CpuX DMI RAM to hold global variables*/

REGION_ALIAS( default_ram , dsram0)
REGION_ALIAS( default_rom , pfls0_b)

/*
REGION_ALIAS( default_ram , dsram1)
REGION_ALIAS( default_rom , pfls1)
*/
/*
REGION_ALIAS( default_ram , dsram2)
REGION_ALIAS( default_rom , pfls2)
*/
/*
REGION_ALIAS( default_ram , dsram3)
REGION_ALIAS( default_rom , pfls3)
*/
/*
REGION_ALIAS( default_ram , dsram4)
REGION_ALIAS( default_rom , pfls4)
*/
/*
REGION_ALIAS( default_ram , dsram5)
REGION_ALIAS( default_rom , pfls5)
*/

/*Sections located at absolute fixed address*/  
  SECTIONS
  {
    .hsm_core (LCF_TRAPVEC_START) : 
    { 

    } > pfls0_hsm = 0x00
  }

  /*Fixed memory Allocations for stack memory and CSA*/
  CORE_ID = CPU5 ;
  SECTIONS
  {
    CORE_SEC(.ustack) (LCF_DSPR5_START + LCF_USTACK5_OFFSET):
    { PROVIDE(__USTACK5_END = .);  . = . + LCF_USTACK5_SIZE;  PROVIDE(__USTACK5 = .); }
    
    CORE_SEC(.istack) (LCF_DSPR5_START + LCF_ISTACK5_OFFSET):
    { PROVIDE(__ISTACK5_END = .);  . = . + LCF_ISTACK5_SIZE;  PROVIDE(__ISTACK5 = .); }
    
    CORE_SEC(.csa) (LCF_DSPR5_START + LCF_CSA5_OFFSET):
    { PROVIDE(__CSA5_END = .);  . = . + LCF_CSA5_SIZE;  PROVIDE(__CSA5 = .); }
  }
  
  CORE_ID = CPU4 ;
  SECTIONS
  {
    CORE_SEC(.ustack) (LCF_DSPR4_START + LCF_USTACK4_OFFSET):
    { PROVIDE(__USTACK4_END = .);  . = . + LCF_USTACK4_SIZE;  PROVIDE(__USTACK4 = .); }
    
    CORE_SEC(.istack) (LCF_DSPR4_START + LCF_ISTACK4_OFFSET):
    { PROVIDE(__ISTACK4_END = .);  . = . + LCF_ISTACK4_SIZE;  PROVIDE(__ISTACK4 = .); }
    
    CORE_SEC(.csa) (LCF_DSPR4_START + LCF_CSA4_OFFSET):
    { PROVIDE(__CSA4_END = .);  . = . + LCF_CSA4_SIZE;  PROVIDE(__CSA4 = .); }
  }
  
  CORE_ID = CPU3 ;
  SECTIONS
  {
    CORE_SEC(.ustack) (LCF_DSPR3_START + LCF_USTACK3_OFFSET):
    { PROVIDE(__USTACK3_END = .);  . = . + LCF_USTACK3_SIZE;  PROVIDE(__USTACK3 = .); }
    
    CORE_SEC(.istack) (LCF_DSPR3_START + LCF_ISTACK3_OFFSET):
    { PROVIDE(__ISTACK3_END = .);  . = . + LCF_ISTACK3_SIZE;  PROVIDE(__ISTACK3 = .); }
    
    CORE_SEC(.csa) (LCF_DSPR3_START + LCF_CSA3_OFFSET):
    { PROVIDE(__CSA3_END = .);  . = . + LCF_CSA3_SIZE;  PROVIDE(__CSA3 = .); }
  }
  
  CORE_ID = CPU2 ;
  SECTIONS
  {
    CORE_SEC(.ustack) (LCF_DSPR2_START + LCF_USTACK2_OFFSET):
    { PROVIDE(__USTACK2_END = .);  . = . + LCF_USTACK2_SIZE;  PROVIDE(__USTACK2 = .); }
    
    CORE_SEC(.istack) (LCF_DSPR2_START + LCF_ISTACK2_OFFSET):
    { PROVIDE(__ISTACK2_END = .);  . = . + LCF_ISTACK2_SIZE;  PROVIDE(__ISTACK2 = .); }
    
    CORE_SEC(.csa) (LCF_DSPR2_START + LCF_CSA2_OFFSET):
    { PROVIDE(__CSA2_END = .);  . = . + LCF_CSA2_SIZE;  PROVIDE(__CSA2 = .); }
  }
  
  CORE_ID = CPU1 ;
  SECTIONS
  {
    CORE_SEC(.ustack) (LCF_DSPR1_START + LCF_USTACK1_OFFSET):
    { PROVIDE(__USTACK1_END = .);  . = . + LCF_USTACK1_SIZE;  PROVIDE(__USTACK1 = .); }
    
    CORE_SEC(.istack) (LCF_DSPR1_START + LCF_ISTACK1_OFFSET):
    { PROVIDE(__ISTACK1_END = .);  . = . + LCF_ISTACK1_SIZE;  PROVIDE(__ISTACK1 = .); }
    
    CORE_SEC(.csa) (LCF_DSPR1_START + LCF_CSA1_OFFSET):
    { PROVIDE(__CSA1_END = .);  . = . + LCF_CSA1_SIZE;  PROVIDE(__CSA1 = .); }
  }
  
  CORE_ID = CPU0 ;
  SECTIONS
  {
    CORE_SEC(.ustack) (LCF_DSPR0_START + LCF_USTACK0_OFFSET):
    { PROVIDE(__USTACK0_END = .);  . = . + LCF_USTACK0_SIZE;  PROVIDE(__USTACK0 = .); }
    
    CORE_SEC(.istack) (LCF_DSPR0_START + LCF_ISTACK0_OFFSET):
    { PROVIDE(__ISTACK0_END = .);  . = . + LCF_ISTACK0_SIZE;  PROVIDE(__ISTACK0 = .); }
    
    CORE_SEC(.csa) (LCF_DSPR0_START + LCF_CSA0_OFFSET):
    { PROVIDE(__CSA0_END = .);  . = . + LCF_CSA0_SIZE;  PROVIDE(__CSA0 = .); }
  }
  
  /*Fixed memory Allocations for _START*/
  CORE_ID = GLOBAL ;
  SECTIONS
  {
    .startup (0xA0000000) : FLAGS(rxl) { KEEP (*(.start)); } > pfls0_nc_a =0x800  
    .interface_const (0x80000020) : { __IF_CONST = .; KEEP (*(.interface_const)); } > pfls0_a
  }
  
  /*Fixed memory Allocations for Trap Vector Table*/
  CORE_ID = GLOBAL ;
  SECTIONS
  {
    .os_interrupt_code.ostraps (LCF_TRAPVEC_START) : 
    { 
    PROVIDE(__TRAPTAB_CPU0 = .);
    PROVIDE(__TRAPTAB_CPU1 = .);
    PROVIDE(__TRAPTAB_CPU2 = .);
    PROVIDE(__TRAPTAB_CPU3 = .);
    PROVIDE(__TRAPTAB_CPU4 = .);
    PROVIDE(__TRAPTAB_CPU5 = .);   
    KEEP (*(.os_interrupt_code.ostraps)); 
    } > pfls0_a
  }

  /*Fixed memory Allocations for Start up code*/
  CORE_ID = GLOBAL ;
  SECTIONS
  {
    .startptr_tc0 (LCF_STARTPTR_CPU0) : { KEEP (*(.startptr_cpu0)); } > pfls0_a
    .startptr_tc1 (LCF_STARTPTR_CPU1) : { KEEP (*(.startptr_cpu1)); } > pfls1
    .startptr_tc2 (LCF_STARTPTR_CPU2) : { KEEP (*(.startptr_cpu2)); } > pfls2
    .startptr_tc3 (LCF_STARTPTR_CPU3) : { KEEP (*(.startptr_cpu3)); } > pfls3
    .startptr_tc4 (LCF_STARTPTR_CPU4) : { KEEP (*(.startptr_cpu4)); } > pfls4
    .startptr_tc5 (LCF_STARTPTR_CPU5) : { KEEP (*(.startptr_cpu5)); } > pfls5
    PROVIDE(__STARTPTR_CPU0 = LCF_STARTPTR_CPU0); 
    PROVIDE(__STARTPTR_CPU1 = LCF_STARTPTR_CPU1); 
    PROVIDE(__STARTPTR_CPU2 = LCF_STARTPTR_CPU2); 
    PROVIDE(__STARTPTR_CPU3 = LCF_STARTPTR_CPU3); 
    PROVIDE(__STARTPTR_CPU4 = LCF_STARTPTR_CPU4); 
    PROVIDE(__STARTPTR_CPU5 = LCF_STARTPTR_CPU5);
    PROVIDE(__ENABLE_INDIVIDUAL_C_INIT_CPU0 = 0); /* Not used */
    PROVIDE(__ENABLE_INDIVIDUAL_C_INIT_CPU1 = 0);
    PROVIDE(__ENABLE_INDIVIDUAL_C_INIT_CPU2 = 0);
    PROVIDE(__ENABLE_INDIVIDUAL_C_INIT_CPU3 = 0);
    PROVIDE(__ENABLE_INDIVIDUAL_C_INIT_CPU4 = 0);
    PROVIDE(__ENABLE_INDIVIDUAL_C_INIT_CPU5 = 0);
  }
    
  /*Fixed memory Allocations for Interrupt Vector Table*/
  SECTIONS
  {
    /*CPU0 Interrupt Vector Table*/
    Os_InterruptVectorTable0 (LCF_INTVEC_START) : { . = ALIGN(4) ;  PROVIDE(__INTTAB_CPU0 = .); KEEP (*(.os_interrupt_code.osinterrupts)); }
  }
  SECTIONS
  {
    /*CPU1 Interrupt Vector Table*/
    Os_InterruptVectorTable1 (LCF_INTVEC_START) : { . = ALIGN(4) ;  PROVIDE(__INTTAB_CPU1 = .); KEEP (*(.os_interrupt_code.osinterrupts)); }
  }
  SECTIONS
  {
    /*CPU2 Interrupt Vector Table*/
    Os_InterruptVectorTable2 (LCF_INTVEC_START) : { . = ALIGN(4) ;  PROVIDE(__INTTAB_CPU2 = .); KEEP (*(.os_interrupt_code.osinterrupts)); }
  }
  SECTIONS
  {
    /*CPU3 Interrupt Vector Table*/
    Os_InterruptVectorTable3 (LCF_INTVEC_START) : { . = ALIGN(4) ;  PROVIDE(__INTTAB_CPU3 = .); KEEP (*(.os_interrupt_code.osinterrupts)); }
  }
  SECTIONS
  {
    /*CPU4 Interrupt Vector Table*/
    Os_InterruptVectorTable4 (LCF_INTVEC_START) : { . = ALIGN(4) ;  PROVIDE(__INTTAB_CPU4 = .); KEEP (*(.os_interrupt_code.osinterrupts)); }
  }
  SECTIONS
  {
    /*CPU5 Interrupt Vector Table*/
    Os_InterruptVectorTable5 (LCF_INTVEC_START) : { . = ALIGN(4) ;  PROVIDE(__INTTAB_CPU5 = .); KEEP (*(.os_interrupt_code.osinterrupts)); }
  }

  
  /*Fixed memory Allocations for BMHD*/
  CORE_ID = GLOBAL;
  SECTIONS
  {
    .bmhd_0_org (0xaf400000) : FLAGS(arl) { KEEP (*(.bmhd_0_orig.*)); } > ucb
    .bmhd_1_org (0xaf400200) : FLAGS(arl) { KEEP (*(.bmhd_1_orig.*)); } > ucb
    .bmhd_2_org (0xaf400400) : FLAGS(arl) { KEEP (*(.bmhd_2_orig.*)); } > ucb
    .bmhd_3_org (0xaf400600) : FLAGS(arl) { KEEP (*(.bmhd_3_orig.*)); } > ucb
    .ucb_reserved (0xaf400800) : FLAGS(arl) { } > ucb
    .bmhd_0_copy (0xaf401000) : FLAGS(arl) { KEEP (*(.bmhd_0_copy.*)); } > ucb
    .bmhd_1_copy (0xaf401200) : FLAGS(arl) { KEEP (*(.bmhd_1_copy.*)); } > ucb
    .bmhd_2_copy (0xaf401400) : FLAGS(arl) { KEEP (*(.bmhd_2_copy.*)); } > ucb
    .bmhd_3_copy (0xaf401600) : FLAGS(arl) { KEEP (*(.bmhd_3_copy.*)); } > ucb
  }
  
/*Near Abbsolute Addressable Data Sections*/  
  /*Near Absolute Data, selectable with patterns and user defined sections*/
  CORE_ID = CPU5;
  SECTIONS
  {  
    CORE_SEC(.zdata) (LCF_DSPR5_START): FLAGS(awzl) 
    {
      *Ifx_Ssw_Tc5.* (.zdata)
      *Cpu5_Main.* (.zdata)
      *(.zdata_cpu5)
      *(.zdata_cpu5.*)
      } > dsram5 AT> pfls0_b
      
      CORE_SEC(.zbss) (NOLOAD): FLAGS(awz)
    {
      *Ifx_Ssw_Tc5.* (.zbss)
      *Cpu5_Main.* (.zbss)
      *(.zbss_cpu5)
      *(.zbss_cpu5.*)
    } > dsram5
  }
  CORE_ID = CPU4;
  SECTIONS
  {  
    CORE_SEC(.zdata) (LCF_DSPR4_START): FLAGS(awzl) 
    {
      *Ifx_Ssw_Tc4.* (.zdata)
      *Cpu4_Main.* (.zdata)
      *(.zdata_cpu4)
      *(.zdata_cpu4.*)
      } > dsram4 AT> pfls0_b
      
      CORE_SEC(.zbss) (NOLOAD): FLAGS(awz)
    {
      *Ifx_Ssw_Tc4.* (.zbss)
      *Cpu4_Main.* (.zbss)
      *(.zbss_cpu4)
      *(.zbss_cpu4.*)
    } > dsram4
  }
  CORE_ID = CPU3;
  SECTIONS
  {  
    CORE_SEC(.zdata) (LCF_DSPR3_START): FLAGS(awzl) 
    {
      *Ifx_Ssw_Tc3.* (.zdata)
      *Cpu3_Main.* (.zdata)
      *(.zdata_cpu3)
      *(.zdata_cpu3.*)
      } > dsram3 AT> pfls0_b
      
      CORE_SEC(.zbss) (NOLOAD): FLAGS(awz)
    {
      *Ifx_Ssw_Tc3.* (.zbss)
      *Cpu3_Main.* (.zbss)
      *(.zbss_cpu3)
      *(.zbss_cpu3.*)
    } > dsram3
  }
  CORE_ID = CPU2;
  SECTIONS
  {  
    CORE_SEC(.zdata) (LCF_DSPR2_START): FLAGS(awzl) 
    {
      *Ifx_Ssw_Tc2.* (.zdata)
      *Cpu2_Main.* (.zdata)
      *(.zdata_cpu2)
      *(.zdata_cpu2.*)
      } > dsram2 AT> pfls0_b
      
      CORE_SEC(.zbss) (NOLOAD): FLAGS(awz)
    {
      *Ifx_Ssw_Tc2.* (.zbss)
      *Cpu2_Main.* (.zbss)
      *(.zbss_cpu2)
      *(.zbss_cpu2.*)
    } > dsram2
  }
  CORE_ID = CPU1;
  SECTIONS
  {  
    CORE_SEC(.zdata) (LCF_DSPR1_START): FLAGS(awzl) 
    {
      *Ifx_Ssw_Tc1.* (.zdata)
      *Cpu1_Main.* (.zdata)
      *(.zdata_cpu1)
      *(.zdata_cpu1.*)
    } > dsram1 AT> pfls0_b
    
    CORE_SEC(.zbss) (NOLOAD): FLAGS(awz)
    {
      *Ifx_Ssw_Tc1.* (.zbss)
      *Cpu1_Main.* (.zbss)
      *(.zbss_cpu1)
      *(.zbss_cpu1.*)
    } > dsram1
  }
  CORE_ID = CPU0;
  SECTIONS
  {  
    CORE_SEC(.zdata) (LCF_DSPR0_START): FLAGS(awzl) 
    {
      *Ifx_Ssw_Tc0.* (.zdata)
      *Cpu0_Main.* (.zdata)
      *(.zdata_cpu0)
      *(.zdata_cpu0.*)
    } > dsram0 AT> pfls0_b
    
    CORE_SEC(.zbss) (NOLOAD): FLAGS(awz)
    {
      *Ifx_Ssw_Tc0.* (.zbss)
      *Cpu0_Main.* (.zbss)
      *(.zbss_cpu0)
      *(.zbss_cpu0.*)
    } > dsram0
  }
  
  /*Near Absolute Data, selectable by toolchain*/
  CORE_ID = GLOBAL;
  SECTIONS
  {  
    CORE_SEC(.zdata_powerOn) : FLAGS(awzl)
    {
      *(.zdata.dsprPowerOnInit.cpu0.32bit)
      *(.zdata.dsprPowerOnInit.cpu0.16bit)
      *(.zdata.dsprPowerOnInit.cpu0.8bit)
    } > default_ram AT> pfls0_b
    
    CORE_SEC(.zdata) : FLAGS(awzl)
    {
      *(.zdata)
      *(.zdata.*)
      *(.zdata.dsprInit.cpu0.32bit)
      *(.zdata.dsprInit.cpu0.16bit)
      *(.zdata.dsprInit.cpu0.8bit)
      *(.gnu.linkonce.z.*)  
    } > default_ram AT> pfls0_b
    
    CORE_SEC(.zbss_powerOn) (NOLOAD) : FLAGS(awz)
    {
      *(.zbss.dsprPowerOnClear.cpu0.32bit)
      *(.zbss.dsprPowerOnClear.cpu0.16bit)
      *(.zbss.dsprPowerOnClear.cpu0.8bit)  
    } > default_ram
    
    CORE_SEC(.zbss_noClear) (NOLOAD) : FLAGS(awz)
    {
      *(.zbss.dsprNoInit.cpu0.32bit)
      *(.zbss.dsprNoInit.cpu0.16bit)
      *(.zbss.dsprNoInit.cpu0.8bit)
    } > default_ram
    
    CORE_SEC(.zbss) (NOLOAD) : FLAGS(awz)
    {
      *(.zbss)
      *(.zbss.dsprClearOnInit.cpu0.32bit)
      *(.zbss.dsprClearOnInit.cpu0.16bit)
      *(.zbss.dsprClearOnInit.cpu0.8bit)
      *(.zbss.*)
      *(.bbss)
      *(.bbss.*)
      *(.gnu.linkonce.zb.*)  
    } > default_ram
  }

  CORE_ID = GLOBAL;
  SECTIONS
  {  
    /*Cpu0_dlmu also is the segment start, all the near lmu data shll be located here*/
    CORE_SEC(.lmuzdata) : FLAGS(awzl)
    {
      *(.lmuzdata)
      *(.lmuzdata.*)
    } > cpu0_dlmu AT> pfls0_b
    
    CORE_SEC(.zbss) (NOLOAD) : FLAGS(awz)
    {
      *(.lmuzbss)
      *(.lmuzbss.*)
    } > cpu0_dlmu
  }
    
  /*Near Absolute Const, selectable with patterns and user defined sections*/
  CORE_ID = GLOBAL;
  SECTIONS
  {  
    CORE_SEC(.zrodata) : FLAGS(arzl)
    {
      *Ifx_Ssw_Tc?.* (.zrodata)
      *Cpu?_Main.* (.zrodata)
      *(.zrodata_cpu?)
    } > pfls0_b
  }
  
  /*Near Absolute Const, selectable by toolchain*/
  CORE_ID = GLOBAL;
  SECTIONS
  {  
    CORE_SEC(.zrodata) : FLAGS(arzl)
    {
      *(.zrodata)
      *(.zrodata.const.cpu0.32bit)
      *(.zrodata.const.cpu0.16bit)
      *(.zrodata.const.cpu0.8bit)
      *(.zrodata.config.cpu0.32bit)
      *(.zrodata.config.cpu0.16bit)
      *(.zrodata.config.cpu0.8bit)
      *(.zrodata.*)
    } > pfls0_b
  }

/*Relative A0/A1/A8/A9 Addressable Sections*/  
CORE_ID = GLOBAL;  
SECTIONS
{  
  /*Relative A0 Addressable Data, selectable with patterns and user defined sections*/
    /*Note: A0 addressable area is common, to make the functions callable in any CPU*/
  /*Relative A0 Addressable Data, selectable by toolchain*/
  CORE_SEC(.sdata) : FLAGS(awsl)
  {
    *(.sdata)
    *(.sdata.*)
  } > default_ram AT> pfls0_b  
  CORE_SEC(.sbss) (NOLOAD): FLAGS(aws)
  {
    *(.sbss)
    *(.sbss.*)
  } > default_ram
  _SMALL_DATA_ = SIZEOF(CORE_SEC(.sdata)) ? ADDR(CORE_SEC(.sdata)) + 32k : (ADDR(CORE_SEC(.sdata)) & 0xF0000000) + 32k ;
  __A0_MEM = _SMALL_DATA_;
}

CORE_ID = GLOBAL;
SECTIONS
{      
  /*Relative A1 Addressable Const, selectable with patterns and user defined sections*/
    /*Note: A1 addressable area is common, to make the functions callable in any CPU*/
  /*Relative A1 Addressable Const, selectable by toolchain*/
  CORE_SEC(.sdata2) : FLAGS(arsl)
  {
    *(.srodata)
    *(.srodata.*)
  } > default_rom
  _SMALL_DATA2_ = SIZEOF(CORE_SEC(.sdata2)) ? ADDR(CORE_SEC(.sdata2)) + 32k : (ADDR(CORE_SEC(.sdata2)) & 0xF0000000) + 32k ;
  __A1_MEM = _SMALL_DATA2_;
}

CORE_ID = GLOBAL;
SECTIONS
{    
  /*Relative A9 Addressable Data, selectable with patterns and user defined sections*/
  CORE_SEC(.sdata4) :
  {
    *(.data_a9)
    *(.data_a9.*)  
  } > lmuram AT> pfls0_b
  
  CORE_SEC(.sbss4) :
  {
    *(.bss_a9)
    *(.bss_a9.*)  
  } > lmuram
  
  _SMALL_DATA4_ = SIZEOF(CORE_SEC(.sdata4)) ? ADDR(CORE_SEC(.sdata4)) + 32k : (ADDR(CORE_SEC(.sdata4)) & 0xF0000000) + 32k ;
  __A9_MEM = _SMALL_DATA4_;

  /*Relative A8 Addressable Const, selectable with patterns and user defined sections*/
  CORE_SEC(.sdata3) : FLAGS(arsl)
  {
    *(.rodata_a8)
    *(.rodata_a8.*)
  } > default_rom
  
  _SMALL_DATA3_ = SIZEOF(CORE_SEC(.sdata3)) ? ADDR(CORE_SEC(.sdata3)) + 32k : (ADDR(CORE_SEC(.sdata3)) & 0xF0000000) + 32k ;
  __A8_MEM = _SMALL_DATA3_;
}

/*Far Data / Far Const Sections, selectable with patterns and user defined sections*/
/*Far Data Sections, selectable with patterns and user defined sections*/
CORE_ID = CPU5 ;
SECTIONS
{
  /*DSRAM5 Sections*/
  CORE_SEC(.data) : FLAGS(awl)
  {
    /* Initialized Data */
    *Ifx_Ssw_Tc5.* (.data)
    *Cpu5_Main.* (.data)
    *(InitData.Cpu5.8bit*)
    *(InitData.Cpu5.16bit*)
    *(InitData.Cpu5.32bit*)
    *(InitData.Cpu5.Unspecified*)
    *(InitData.Fast.Cpu5.8bit*)
    *(InitData.Fast.Cpu5.16bit*)
    *(InitData.Fast.Cpu5.32bit*)
    *(InitData.Fast.Cpu5.Unspecified*)	
    *(.data_cpu5*)
    . = ALIGN(4);
  } > dsram5 AT> pfls0_b
  
  CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
  {
    /* Uninitialized Data */
    *Ifx_Ssw_Tc5.* (.bss)
    *Cpu5_Main.* (.bss)
    *(ClearedData.Cpu5.8bit*)
    *(ClearedData.Cpu5.16bit*)
    *(ClearedData.Cpu5.32bit*)
    *(ClearedData.Cpu5.256bit*)
    *(ClearedData.Cpu5.Unspecified*)
    *(ClearedData.Fast.Cpu5.8bit*)
    *(ClearedData.Fast.Cpu5.16bit*)
    *(ClearedData.Fast.Cpu5.32bit*)
    *(ClearedData.Fast.Cpu5.256bit*)
    *(ClearedData.Fast.Cpu5.Unspecified*)	
    *(.bss_cpu5*)
    . = ALIGN(4);
  } > dsram5
  
  /*DLMU5 Sections*/
  CORE_SEC(.lmudata) : FLAGS(awl)
  {
    *(.lmudata_cpu5*)
  } > cpu5_dlmu AT> pfls0_b
  
  CORE_SEC(.lmubss) : FLAGS(aw)
  {
  *(.lmubss_cpu5*)  
  } > cpu5_dlmu
}
CORE_ID = CPU4 ;
SECTIONS
{
  /*DSRAM4 Sections*/
  CORE_SEC(.data) : FLAGS(awl)
  {
    /* Initialized Data */
    *Ifx_Ssw_Tc4.* (.data)
    *Cpu4_Main.* (.data)
    *(InitData.Cpu4.8bit*)
    *(InitData.Cpu4.16bit*)
    *(InitData.Cpu4.32bit*)
    *(InitData.Cpu4.Unspecified*)
    *(InitData.Fast.Cpu4.8bit*)
    *(InitData.Fast.Cpu4.16bit*)
    *(InitData.Fast.Cpu4.32bit*)
    *(InitData.Fast.Cpu4.Unspecified*)	
    *(.data_cpu4*)
    . = ALIGN(4);
  } > dsram4 AT> pfls0_b
  
  CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
  {
    /* Uninitialized Data */
    *Ifx_Ssw_Tc4.* (.bss)
    *Cpu4_Main.* (.bss)
    *(ClearedData.Cpu4.8bit*)
    *(ClearedData.Cpu4.16bit*)
    *(ClearedData.Cpu4.32bit*)
    *(ClearedData.Cpu4.256bit*)
    *(ClearedData.Cpu4.Unspecified*)
    *(ClearedData.Fast.Cpu4.8bit*)
    *(ClearedData.Fast.Cpu4.16bit*)
    *(ClearedData.Fast.Cpu4.32bit*)
    *(ClearedData.Fast.Cpu4.256bit*)
    *(ClearedData.Fast.Cpu4.Unspecified*)	
    *(.bss_cpu4*)
    . = ALIGN(4);
  } > dsram4
  
  /*DLMU4 Sections*/
  CORE_SEC(.lmudata) : FLAGS(awl)
  {
    *(.lmudata_cpu4*)
  } > cpu4_dlmu AT> pfls0_b
  
  CORE_SEC(.lmubss) : FLAGS(aw)
  {
  *(.lmubss_cpu4*)  
  } > cpu4_dlmu
}
CORE_ID = CPU3 ;
SECTIONS
{
  /*DSRAM3 Sections*/
  CORE_SEC(.data) : FLAGS(awl)
  {
    /* Initialized Data */
    *Ifx_Ssw_Tc3.* (.data)
    *Cpu3_Main.* (.data)
    *(InitData.Cpu3.8bit*)
    *(InitData.Cpu3.16bit*)
    *(InitData.Cpu3.32bit*)
    *(InitData.Cpu3.Unspecified*)
    *(InitData.Fast.Cpu3.8bit*)
    *(InitData.Fast.Cpu3.16bit*)
    *(InitData.Fast.Cpu3.32bit*)
    *(InitData.Fast.Cpu3.Unspecified*)	
    *(.data_cpu3*)
    . = ALIGN(4);
  } > dsram3 AT> pfls0_b
  
  CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
  {
    /* Uninitialized Data */
    *Ifx_Ssw_Tc3.* (.bss)
    *Cpu3_Main.* (.bss)
    *(ClearedData.Cpu3.8bit*)
    *(ClearedData.Cpu3.16bit*)
    *(ClearedData.Cpu3.32bit*)
    *(ClearedData.Cpu3.256bit*)
    *(ClearedData.Cpu3.Unspecified*)
    *(ClearedData.Fast.Cpu3.8bit*)
    *(ClearedData.Fast.Cpu3.16bit*)
    *(ClearedData.Fast.Cpu3.32bit*)
    *(ClearedData.Fast.Cpu3.256bit*)
    *(ClearedData.Fast.Cpu3.Unspecified*)	
    *(.bss_cpu3*)
    . = ALIGN(4);
  } > dsram3
  
  /*DLMU3 Sections*/
  CORE_SEC(.lmudata) : FLAGS(awl)
  {
    *(.lmudata_cpu3*)
  } > cpu3_dlmu AT> pfls0_b

  CORE_SEC(.lmubss) : FLAGS(aw)
  {
  *(.lmubss_cpu3*)  
  } > cpu3_dlmu
}
CORE_ID = CPU2 ;
SECTIONS
{
  /*DSRAM2 Sections*/
  CORE_SEC(.data) : FLAGS(awl)
  {
    /* Initialized Data */
    *Ifx_Ssw_Tc2.* (.data)
    *Cpu2_Main.* (.data)
    *(InitData.Cpu2.8bit*)
    *(InitData.Cpu2.16bit*)
    *(InitData.Cpu2.32bit*)
    *(InitData.Cpu2.Unspecified*)
    *(InitData.Fast.Cpu2.8bit*)
    *(InitData.Fast.Cpu2.16bit*)
    *(InitData.Fast.Cpu2.32bit*)
    *(InitData.Fast.Cpu2.Unspecified*)	
    *(.data_cpu2*)
    . = ALIGN(4);
  } > dsram2 AT> pfls0_b
  
  CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
  {
    /* Uninitialized Data */
    *Ifx_Ssw_Tc2.* (.bss)
    *Cpu2_Main.* (.bss)
    *(ClearedData.Cpu2.8bit*)
    *(ClearedData.Cpu2.16bit*)
    *(ClearedData.Cpu2.32bit*)
    *(ClearedData.Cpu2.256bit*)
    *(ClearedData.Cpu2.Unspecified*)
    *(ClearedData.Fast.Cpu2.8bit*)
    *(ClearedData.Fast.Cpu2.16bit*)
    *(ClearedData.Fast.Cpu2.32bit*)
    *(ClearedData.Fast.Cpu2.256bit*)
    *(ClearedData.Fast.Cpu2.Unspecified*)	
    *(.bss_cpu2*)
    . = ALIGN(4);
  } > dsram2
  
  /*DLMU2 Sections*/
  CORE_SEC(.lmudata) : FLAGS(awl)
  {
    *(.lmudata_cpu2*)
  } > cpu2_dlmu AT> pfls0_b
  
  CORE_SEC(.lmubss) : FLAGS(aw)
  {
  *(.lmubss_cpu2*)  
  } > cpu2_dlmu
}
CORE_ID = CPU1 ;
SECTIONS
{
  /*DSRAM1 Sections*/
  CORE_SEC(.data) : FLAGS(awl)
  {
    /* Initialized Data */
    *Ifx_Ssw_Tc1.* (.data)
    *Cpu1_Main.* (.data)
    *(InitData.Cpu1.8bit*)
    *(InitData.Cpu1.16bit*)
    *(InitData.Cpu1.32bit*)
    *(InitData.Cpu1.Unspecified*)
    *(InitData.Fast.Cpu1.8bit*)
    *(InitData.Fast.Cpu1.16bit*)
    *(InitData.Fast.Cpu1.32bit*)
    *(InitData.Fast.Cpu1.Unspecified*)	
    *(.data_cpu1*)
    . = ALIGN(4);
  } > dsram1 AT> pfls0_b
  
  CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
  {
    /* Uninitialized Data */
    *Ifx_Ssw_Tc1.* (.bss)
    *Cpu1_Main.* (.bss)
    *(ClearedData.Cpu1.8bit*)
    *(ClearedData.Cpu1.16bit*)
    *(ClearedData.Cpu1.32bit*)
    *(ClearedData.Cpu1.256bit*)
    *(ClearedData.Cpu1.Unspecified*)
    *(ClearedData.Fast.Cpu1.8bit*)
    *(ClearedData.Fast.Cpu1.16bit*)
    *(ClearedData.Fast.Cpu1.32bit*)
    *(ClearedData.Fast.Cpu1.256bit*)
    *(ClearedData.Fast.Cpu1.Unspecified*)	
    *(.bss_cpu1*)
    . = ALIGN(4);
  } > dsram1
  
  /*DLMU1 Sections*/
  CORE_SEC(.lmudata) : FLAGS(awl)
  {
    *(.lmudata_cpu1*)
  } > cpu1_dlmu AT> pfls0_b
  
  CORE_SEC(.lmubss) : FLAGS(aw)
  {
  *(.lmubss_cpu1*)  
  } > cpu1_dlmu
}
CORE_ID = CPU0 ;
SECTIONS
{
  /*DSRAM0 Sections*/
  CORE_SEC(.data) : FLAGS(awl)
  {
    /* Initialized Data */
    *Ifx_Ssw_Tc0.* (.data)
    *Cpu0_Main.* (.data)
    *(InitData.Cpu0.8bit*)
    *(InitData.Cpu0.16bit*)
    *(InitData.Cpu0.32bit*)
    *(InitData.Cpu0.Unspecified*)
    *(InitData.Fast.Cpu0.8bit*)
    *(InitData.Fast.Cpu0.16bit*)
    *(InitData.Fast.Cpu0.32bit*)
    *(InitData.Fast.Cpu0.Unspecified*)		
    *(InitData_*)
    *(.data_cpu0*)
    *(.hsm_shared*)
    *(.hsm_shared_ro*)
    . = ALIGN(4);
  } > dsram0 AT> pfls0_b
  
  CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
  {
    /* Uninitialized Data */
    *Ifx_Ssw_Tc0.* (.bss)
    *Cpu0_Main.* (.bss)
    *(ClearedData.Cpu0.8bit*)
    *(ClearedData.Cpu0.16bit*)
    *(ClearedData.Cpu0.32bit*)
    *(ClearedData.Cpu0.256bit*)
    *(ClearedData.Cpu0.Unspecified*)
    *(ClearedData.Fast.Cpu0.8bit*)
    *(ClearedData.Fast.Cpu0.16bit*)
    *(ClearedData.Fast.Cpu0.32bit*)
    *(ClearedData.Fast.Cpu0.256bit*)
    *(ClearedData.Fast.Cpu0.Unspecified*)	
    *(ClearedData_*)	
    *(.bss_cpu0*)	
    . = ALIGN(4);
  } > dsram0
  
  /*DLMU0 Sections*/
  CORE_SEC(.lmudata) : FLAGS(awl)
  {
    *(.lmudata_cpu0*)
     . = ALIGN(4);
  } > cpu0_dlmu AT> pfls0_b
  
  CORE_SEC(.lmubss) : FLAGS(aw)
  {
  *(.lmubss_cpu0*)  
  } > cpu0_dlmu
}

/*Far Data Sections, selectable by toolchain*/
CORE_ID = GLOBAL;
SECTIONS
{
  CORE_SEC(.bss_noClear) (NOLOAD) : FLAGS(aw)
  {
    *(.bss.farDsprNoInit.cpu0.32bit*)
    *(.bss.farDsprNoInit.cpu0.16bit*)
    *(.bss.farDsprNoInit.cpu0.8bit*)
  } > default_ram
  
  CORE_SEC(.data) : FLAGS(awl)
  {
    *(.data)
    *(.data.farDsprInit.cpu0.32bit*)
    *(.data.farDsprInit.cpu0.16bit*)
    *(.data.farDsprInit.cpu0.8bit*)
    *(.data.*)
  *(.gnu.linkonce.d.*)  
  } > default_ram AT> pfls0_b
  
  CORE_SEC(.bss) (NOLOAD) : FLAGS(aw)
  {
    *(.bss)
    *(.bss.farDsprClearOnInit.cpu0.32bit*)
    *(.bss.farDsprClearOnInit.cpu0.16bit*)
    *(.bss.farDsprClearOnInit.cpu0.8bit*)
    *(.bss.*)
    *(.gnu.linkonce.b.*)  
  } > default_ram
  
  .heap  : FLAGS(aw)
  {
  . = ALIGN(4);
  __HEAP = .;
  . += LCF_HEAP_SIZE;
  __HEAP_END = .;
  } > default_ram
  
  CORE_SEC(.lmudata) : ALIGN(4) FLAGS(awl)
  {
    /* Initialized Data */
    *(.lmudata*)
    *(.lmudata.*)  
    *(InitData.LmuNC.8bit*);
    *(InitData.LmuNC.16bit*);
    *(InitData.LmuNC.32bit*);
    *(InitData.LmuNC.256bit*);
    *(InitData.LmuNC.Unspecified*);
	*(InitData.Fast.LmuNC.8bit*);
    *(InitData.Fast.LmuNC.16bit*);
    *(InitData.Fast.LmuNC.32bit*);
    *(InitData.Fast.LmuNC.256bit*);
    *(InitData.Fast.LmuNC.Unspecified*);
  } > lmuram_nc AT> pfls0_b
  
  CORE_SEC(.lmubss) : FLAGS(aw)
  {
    /* UnInitialized Data */
    *(.lmubss*)
    *(.lmubss.*)  
    *(ClearedData.LmuNC.8bit*);
    *(ClearedData.LmuNC.16bit*);
    *(ClearedData.LmuNC.32bit*);
    *(ClearedData.LmuNC.256bit*);
    *(ClearedData.LmuNC.Unspecified*);
    *(ClearedData.Fast.LmuNC.8bit*);
    *(ClearedData.Fast.LmuNC.16bit*);
    *(ClearedData.Fast.LmuNC.32bit*);
    *(ClearedData.Fast.LmuNC.256bit*);
    *(ClearedData.Fast.LmuNC.Unspecified*);	
  } > lmuram_nc
}
/*Far Const Sections, selectable with patterns and user defined sections*/
CORE_ID = CPU0;
SECTIONS
{  
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *Ifx_Ssw_Tc0.* (.rodata)
    *Cpu0_Main.* (.rodata)
    /* Const Data */
    *(Const.Cpu0.8bit*)
    *(Const.Cpu0.16bit*)
    *(Const.Cpu0.32bit*)
    *(Const.Cpu0.Unspecified*)
	*(Const.Cpu0.256bit*)
    /* Config Data */
    *(Config.Cpu0.8bit*)
    *(Config.Cpu0.16bit*)
    *(Config.Cpu0.32bit*)
    *(Config.Cpu0.Unspecified*)
	*(Config.Cpu0.256bit*)
    /* Default */
    *(.rodata_cpu0*)	
    . = ALIGN(4);
  } > pfls0_b
}

CORE_ID = CPU1;
SECTIONS
{  
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *Ifx_Ssw_Tc1.* (.rodata)
    *Cpu1_Main.* (.rodata)
    /* Const Data */
    *(Const.Cpu1.8bit*)
    *(Const.Cpu1.16bit*)
    *(Const.Cpu1.32bit*)
    *(Const.Cpu1.Unspecified*)
	*(Const.Cpu1.256bit*)
    /* Config Data */
    *(Config.Cpu1.8bit*)
    *(Config.Cpu1.16bit*)
    *(Config.Cpu1.32bit*)
    *(Config.Cpu1.Unspecified*)
	*(Config.Cpu1.256bit*)
    /* Default */
    *(.rodata_cpu1)
    . = ALIGN(4);
  } > pfls1
}

CORE_ID = CPU2;
SECTIONS
{  
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *Ifx_Ssw_Tc2.* (.rodata)
    *Cpu2_Main.* (.rodata)
    /* Const Data */
    *(Const.Cpu2.8bit*)
    *(Const.Cpu2.16bit*)
    *(Const.Cpu2.32bit*)
    *(Const.Cpu2.Unspecified*)
    *(Const.Cpu2.256bit*)
    /* Config Data */
    *(Config.Cpu2.8bit*)
    *(Config.Cpu2.16bit*)
    *(Config.Cpu2.32bit*)
    *(Config.Cpu2.Unspecified*)
	*(Config.Cpu2.256bit*)
    /* Default */
    *(.rodata_cpu2)
    . = ALIGN(4);
  } > pfls2
}

CORE_ID = CPU3;
SECTIONS
{  
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *Ifx_Ssw_Tc3.* (.rodata)
    *Cpu3_Main.* (.rodata)
    /* Const Data */
    *(Const.Cpu3.8bit*)
    *(Const.Cpu3.16bit*)
    *(Const.Cpu3.32bit*)
    *(Const.Cpu3.Unspecified*)
	*(Const.Cpu3.256bit*)
    /* Config Data */      
    *(Config.Cpu3.8bit*)
    *(Config.Cpu3.16bit*)
    *(Config.Cpu3.32bit*)
    *(Config.Cpu3.Unspecified*)
	*(Config.Cpu3.256bit*)
    /* Default */
    *(.rodata_cpu3)
    . = ALIGN(4);
  } > pfls3
}

CORE_ID = CPU4;
SECTIONS
{  
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *Ifx_Ssw_Tc4.* (.rodata)
    *Cpu4_Main.* (.rodata)
    /* Const Data */
    *(Const.Cpu4.8bit*)
    *(Const.Cpu4.16bit*)
    *(Const.Cpu4.32bit*)
    *(Const.Cpu4.Unspecified*)
	*(Const.Cpu4.256bit*)
    /* Config Data */      
    *(Config.Cpu4.8bit*)
    *(Config.Cpu4.16bit*)
    *(Config.Cpu4.32bit*)
    *(Config.Cpu4.Unspecified*)
	*(Config.Cpu4.256bit*)
    /* Default */
    *(.rodata_cpu4)
    . = ALIGN(4);
  } > pfls4
}

CORE_ID = CPU5;
SECTIONS
{  
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *Ifx_Ssw_Tc5.* (.rodata)
    *Cpu5_Main.* (.rodata)
    /* Const Data */
    *(Const.Cpu5.8bit*)
    *(Const.Cpu5.16bit*)
    *(Const.Cpu5.32bit*)
    *(Const.Cpu5.Unspecified*)
	*(Const.Cpu5.256bit*)
    /* Config Data */      
    *(Config.Cpu5.8bit*)
    *(Config.Cpu5.16bit*)
    *(Config.Cpu5.32bit*)
    *(Config.Cpu5.Unspecified*)
	*(Config.Cpu5.256bit*)
    /* Default */
    *(.rodata_cpu5)
    . = ALIGN(4);
  } > pfls5
}

/*Far Const Sections, selectable by toolchain*/
CORE_ID = GLOBAL;
SECTIONS
{
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *(.rodata)
    *(.rodata.farConst.cpu0.32bi)
    *(.rodata.farConst.cpu0.16bit)
    *(.rodata.farConst.cpu0.8bit)
    *(.rodata.*)
  *(.gnu.linkonce.r.*)  
  /*
   * Create the clear and copy tables that tell the startup code
   * which memory areas to clear and to copy, respectively.
   */
  . = ALIGN(8) ;
    PROVIDE(__clear_table = .) ;
  LONG(0 + ADDR(.CPU5.zbss));     LONG(SIZEOF(.CPU5.zbss));
  LONG(0 + ADDR(.CPU5.bss));    LONG(SIZEOF(.CPU5.bss));
  LONG(0 + ADDR(.CPU5.lmubss));    LONG(SIZEOF(.CPU5.lmubss));
  LONG(0 + ADDR(.CPU4.zbss));     LONG(SIZEOF(.CPU4.zbss));
  LONG(0 + ADDR(.CPU4.bss));    LONG(SIZEOF(.CPU4.bss));
  LONG(0 + ADDR(.CPU4.lmubss));    LONG(SIZEOF(.CPU4.lmubss));
  LONG(0 + ADDR(.CPU3.zbss));     LONG(SIZEOF(.CPU3.zbss));
  LONG(0 + ADDR(.CPU3.bss));    LONG(SIZEOF(.CPU3.bss));
  LONG(0 + ADDR(.CPU3.lmubss));    LONG(SIZEOF(.CPU3.lmubss));
  LONG(0 + ADDR(.CPU2.zbss));     LONG(SIZEOF(.CPU2.zbss));
  LONG(0 + ADDR(.CPU2.bss));    LONG(SIZEOF(.CPU2.bss));
  LONG(0 + ADDR(.CPU2.lmubss));    LONG(SIZEOF(.CPU2.lmubss));
  LONG(0 + ADDR(.CPU1.zbss));     LONG(SIZEOF(.CPU1.zbss));
  LONG(0 + ADDR(.CPU1.bss));    LONG(SIZEOF(.CPU1.bss));
  LONG(0 + ADDR(.CPU1.lmubss));    LONG(SIZEOF(.CPU1.lmubss));
  LONG(0 + ADDR(.CPU0.zbss));     LONG(SIZEOF(.CPU0.zbss));
  LONG(0 + ADDR(.CPU0.bss));    LONG(SIZEOF(.CPU0.bss));
  LONG(0 + ADDR(.CPU0.lmubss));    LONG(SIZEOF(.CPU0.lmubss));
  LONG(0 + ADDR(.zbss));     LONG(SIZEOF(.zbss));
  LONG(0 + ADDR(.sbss));     LONG(SIZEOF(.sbss));
  LONG(0 + ADDR(.bss));    LONG(SIZEOF(.bss));
  LONG(0 + ADDR(.lmubss));    LONG(SIZEOF(.lmubss));
  LONG(0 + ADDR(.sbss4));    LONG(SIZEOF(.sbss4));
  LONG(-1);                 LONG(-1);
  PROVIDE(__clear_table_powerOn = .) ;
  LONG(0 + ADDR(.zbss_powerOn));     LONG(SIZEOF(.zbss_powerOn));
  LONG(-1);                 LONG(-1);
  . = ALIGN(8) ; 
  PROVIDE(__copy_table = .) ;
  LONG(LOADADDR(.CPU5.zdata));    LONG(0 + ADDR(.CPU5.zdata));    LONG(SIZEOF(.CPU5.zdata));
  LONG(LOADADDR(.CPU5.data));    LONG(0 + ADDR(.CPU5.data));    LONG(SIZEOF(.CPU5.data));
  LONG(LOADADDR(.CPU5.lmudata));    LONG(0 + ADDR(.CPU5.lmudata));    LONG(SIZEOF(.CPU5.lmudata));
  LONG(LOADADDR(.CPU4.zdata));    LONG(0 + ADDR(.CPU4.zdata));    LONG(SIZEOF(.CPU4.zdata));
  LONG(LOADADDR(.CPU4.data));    LONG(0 + ADDR(.CPU4.data));    LONG(SIZEOF(.CPU4.data));
  LONG(LOADADDR(.CPU4.lmudata));    LONG(0 + ADDR(.CPU4.lmudata));    LONG(SIZEOF(.CPU4.lmudata));
  LONG(LOADADDR(.CPU3.zdata));    LONG(0 + ADDR(.CPU3.zdata));    LONG(SIZEOF(.CPU3.zdata));
  LONG(LOADADDR(.CPU3.data));    LONG(0 + ADDR(.CPU3.data));    LONG(SIZEOF(.CPU3.data));
  LONG(LOADADDR(.CPU3.lmudata));    LONG(0 + ADDR(.CPU3.lmudata));    LONG(SIZEOF(.CPU3.lmudata));
  LONG(LOADADDR(.CPU2.zdata));    LONG(0 + ADDR(.CPU2.zdata));    LONG(SIZEOF(.CPU2.zdata));
  LONG(LOADADDR(.CPU2.data));    LONG(0 + ADDR(.CPU2.data));    LONG(SIZEOF(.CPU2.data));
  LONG(LOADADDR(.CPU2.lmudata));    LONG(0 + ADDR(.CPU2.lmudata));    LONG(SIZEOF(.CPU2.lmudata));
  LONG(LOADADDR(.CPU1.zdata));    LONG(0 + ADDR(.CPU1.zdata));    LONG(SIZEOF(.CPU1.zdata));
  LONG(LOADADDR(.CPU1.data));    LONG(0 + ADDR(.CPU1.data));    LONG(SIZEOF(.CPU1.data));
  LONG(LOADADDR(.CPU1.lmudata));    LONG(0 + ADDR(.CPU1.lmudata));    LONG(SIZEOF(.CPU1.lmudata));
  LONG(LOADADDR(.CPU0.zdata));    LONG(0 + ADDR(.CPU0.zdata));    LONG(SIZEOF(.CPU0.zdata));
  LONG(LOADADDR(.CPU0.data));    LONG(0 + ADDR(.CPU0.data));    LONG(SIZEOF(.CPU0.data));
  LONG(LOADADDR(.CPU0.lmudata));    LONG(0 + ADDR(.CPU0.lmudata));    LONG(SIZEOF(.CPU0.lmudata));
  LONG(LOADADDR(.zdata));    LONG(0 + ADDR(.zdata));    LONG(SIZEOF(.zdata));
  LONG(LOADADDR(.sdata));    LONG(0 + ADDR(.sdata));    LONG(SIZEOF(.sdata));
  LONG(LOADADDR(.data));    LONG(0 + ADDR(.data));    LONG(SIZEOF(.data));
  LONG(LOADADDR(.lmudata));    LONG(0 + ADDR(.lmudata));    LONG(SIZEOF(.lmudata));
  LONG(LOADADDR(.sdata4));    LONG(0 + ADDR(.sdata4));    LONG(SIZEOF(.sdata4));
  LONG(LOADADDR(.CPU0.psram_text));    LONG(0 + ADDR(.CPU0.psram_text));    LONG(SIZEOF(.CPU0.psram_text));
  LONG(LOADADDR(.CPU1.psram_text));    LONG(0 + ADDR(.CPU1.psram_text));    LONG(SIZEOF(.CPU1.psram_text));
  LONG(LOADADDR(.CPU2.psram_text));    LONG(0 + ADDR(.CPU2.psram_text));    LONG(SIZEOF(.CPU2.psram_text));
  LONG(LOADADDR(.CPU3.psram_text));    LONG(0 + ADDR(.CPU3.psram_text));    LONG(SIZEOF(.CPU3.psram_text));
  LONG(LOADADDR(.CPU4.psram_text));    LONG(0 + ADDR(.CPU4.psram_text));    LONG(SIZEOF(.CPU4.psram_text));
  LONG(LOADADDR(.CPU5.psram_text));    LONG(0 + ADDR(.CPU5.psram_text));    LONG(SIZEOF(.CPU5.psram_text));
  LONG(-1);                 LONG(-1);                 LONG(-1);
  PROVIDE(__copy_table_powerOn = .) ;
  LONG(LOADADDR(.zdata_powerOn));    LONG(0 + ADDR(.zdata_powerOn));    LONG(SIZEOF(.zdata_powerOn));
  LONG(-1);                 LONG(-1);                 LONG(-1);
  . = ALIGN(8);
  } > default_rom
}

/*Code selections*/
/*Code Sections, selectable with patterns and user defined sections*/
CORE_ID = CPU0;
SECTIONS
{
  CORE_SEC(.text) : FLAGS(axl)
  {
    . = ALIGN(4);
    *Code.Cpu0 (.text)
	*Code.Fast.Cpu0(.text)
    *Ifx_Ssw_Tc0.* (.text)
    *Cpu0_Main.* (.text)
    *(.text_cpu0)
	*(Code.Cpu0*)
	*(.Code.Cpu0*)
	*(Code.CPU0*)	
  } > pfls0_b

  /*
   * Code executed before calling main extra section for C++ constructor init 
   *  -------------------------Start-----------------------------------------
   */
  .init :
  {
    PROVIDE(__init_start = .);
    KEEP(*(.init))
    KEEP(*(.init*))
    PROVIDE(__init_end = .);
    . = ALIGN(8);

  } > pfls0_b
  
  .fini :
  {
    PROVIDE(__fini_start = .);
    KEEP(*(.fini))
    KEEP(*(.fini*))
    PROVIDE(__fini_end = .);
    . = ALIGN(8);
  } > pfls0_b

  /*
   * Code executed before calling main extra section for C++ constructor init 
   *  -------------------------End-----------------------------------------
   */
  CORE_SEC(.psram_text)  : FLAGS(awx)
  {
    . = ALIGN(4);
    *(.psram_text_cpu0)
    *(.cpu0_psram)
  } > psram0 AT> pfls0_b
}

SECTIONS
{
  .CALIB_DATA : ALIGN(4)
  {
    PROVIDE(__RODATA_CALIB_START = .);
    *(.rodata.Calib_bool)
    *(.rodata.Calib_8)
    *(.rodata.Calib_16)
    *(.rodata.Calib_32)
    *(.rodata.Calib_unspec)
  } > calib_mem
}
SECTIONS
{
  .OVERLAY_RAM_DATA : 
  {
    PROVIDE(__OVERLAY_RAM_START = .);
  } > calib_ram
}
CORE_ID = CPU1;
SECTIONS
{
  CORE_SEC(.text) : FLAGS(axl)
  {
    . = ALIGN(4);
	*Code.Cpu1 (.text)
	*Code.Fast.Cpu1(.text)
    *Ifx_Ssw_Tc1.*(.text)
    *Cpu1_Main.*(.text)
    *(.text_cpu1)
  } > pfls1

  CORE_SEC(.psram_text)  : FLAGS(awx)
  {
    . = ALIGN(4);
    *(.psram_text_cpu1)
    *(.cpu1_psram)
  } > psram1 AT> pfls1
}

CORE_ID = CPU2;
SECTIONS
{
  CORE_SEC(.text) : FLAGS(axl)
  {
    . = ALIGN(4);
	*Code.Cpu2 (.text)
	*Code.Fast.Cpu2(.text)
    *Ifx_Ssw_Tc2.* (.text)
    *Cpu2_Main.* (.text)
    *(.text_cpu2)
  } > pfls2

  CORE_SEC(.psram_text)  : FLAGS(awx)
  {
    . = ALIGN(4);
    *(.psram_text_cpu2)
    *(.cpu2_psram)
  } > psram2 AT> pfls2
}

CORE_ID = CPU3;
SECTIONS
{
  CORE_SEC(.text) : FLAGS(axl)
  {
    . = ALIGN(4);
	*Code.Cpu3 (.text)
	*Code.Fast.Cpu3(.text)
    *Ifx_Ssw_Tc3.* (.text)
    *Cpu3_Main.* (.text)
    *(.text_cpu3)
  } > pfls3

  CORE_SEC(.psram_text)  : FLAGS(awx)
  {
    . = ALIGN(4);
    *(.psram_text_cpu3)
    *(.cpu3_psram)
  } > psram3 AT> pfls3
}

CORE_ID = CPU4;
SECTIONS
{
  CORE_SEC(.text) : FLAGS(axl)
  {
    . = ALIGN(4);
	*Code.Cpu4 (.text)
	*Code.Fast.Cpu4(.text)
    *Ifx_Ssw_Tc4.* (.text)
    *Cpu4_Main.* (.text)
    *(.text_cpu4)
  } > pfls4

  CORE_SEC(.psram_text)  : FLAGS(awx)
  {
    . = ALIGN(4);
    *(.psram_text_cpu4)
    *(.cpu4_psram)
  } > psram4 AT> pfls4
}

CORE_ID = CPU5;
SECTIONS
{
  CORE_SEC(.text) : FLAGS(axl)
  {
    . = ALIGN(4);
	*Code.Cpu5 (.text)
	*Code.Fast.Cpu5(.text)
    *Ifx_Ssw_Tc5.* (.text)
    *Cpu5_Main.* (.text)
    *(.text_cpu5)
  } > pfls5

  CORE_SEC(.psram_text)  : FLAGS(awx)
  {
    . = ALIGN(4);
    *(.psram_text_cpu5)
    *(.cpu5_psram)
  } > psram5 AT> pfls5
}

/*Code Sections, selectable by toolchain*/
CORE_ID = GLOBAL;
SECTIONS
{
  CORE_SEC(.text) : FLAGS(axl)
  {
	*(.text)
	*(.text.*)
    *(.text.fast.pfls.cpu0)
    *(.text.slow.pfls.cpu0)
    *(.text.5ms.pfls.cpu0)
    *(.text.10ms.pfls.cpu0)
    *(.text.callout.pfls.cpu0)
    *(.gnu.linkonce.t.*)
    *(.gnu.warning)        /* .gnu.warning sections are handled specially by elf32.em. */
    . = ALIGN(4);
  } > default_rom

  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the "strip" and "objcopy" utilities to remove
   * the .eh_frame section from the executable.
   */
  .eh_frame  :
  {
    *(.gcc_except_table)
    __EH_FRAME_BEGIN__ = . ;
    KEEP (*(.eh_frame))
    __EH_FRAME_END__ = . ;
    . = ALIGN(8);
  } > default_rom

  /*
   * Constructors and destructors.
   */
  .ctors : FLAGS(ar)
  {
    __CTOR_LIST__ = . ;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2);
    /*
     * Code executed before calling main extra section for C++ constructor init 
     *  -------------------------Start-----------------------------------------
     */  
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    /*
     * Code executed before calling main extra section for C++ constructor init 
     *  -------------------------End-----------------------------------------
     */
    LONG(0) ;
    __CTOR_END__ = . ;
    . = ALIGN(8);
  } > default_rom
  .dtors : FLAGS(ar)
  {
    __DTOR_LIST__ = . ;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2);
    /*
     * Code executed before calling main extra section for C++ distructor init 
     *  -------------------------Start-----------------------------------------
     */  
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*
     * Code executed before calling main extra section for C++ distructor init 
     *  -------------------------End-----------------------------------------
     */  
    LONG(0) ;
    __DTOR_END__ = . ;
    . = ALIGN(8);
  } > default_rom  
  /*
   * DWARF debug sections.
   * Symbols in the DWARF debugging sections are relative to the
   * beginning of the section, so we begin them at 0.
   */
  /*
   * DWARF 1
   */
  .comment         0 : { *(.comment) }
  .debug           0 : { *(.debug) }
  .line            0 : { *(.line) }
  /*
   * GNU DWARF 1 extensions
   */
  .debug_srcinfo   0 : { *(.debug_srcinfo) }
  .debug_sfnames   0 : { *(.debug_sfnames) }
  /*
   * DWARF 1.1 and DWARF 2
   */
  .debug_aranges   0 : { *(.debug_aranges) }
  .debug_pubnames  0 : { *(.debug_pubnames) }
  /*
   * DWARF 2
   */
  .debug_info      0 : { *(.debug_info) }
  .debug_abbrev    0 : { *(.debug_abbrev) }
  .debug_line      0 : { *(.debug_line) }
  .debug_frame     0 : { *(.debug_frame) }
  .debug_str       0 : { *(.debug_str) }
  .debug_loc       0 : { *(.debug_loc) }
  .debug_macinfo   0 : { *(.debug_macinfo) }
  .debug_ranges    0 : { *(.debug_ranges) }
  /*
   * SGI/MIPS DWARF 2 extensions
   */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /*
   * Optional sections that may only appear when relocating.
   */
  /*
   * Optional sections that may appear regardless of relocating.
   */
  .version_info    0 : { *(.version_info) }
  .boffs           0 : { KEEP (*(.boffs)) }
}

