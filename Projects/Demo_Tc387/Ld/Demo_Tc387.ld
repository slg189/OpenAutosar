/* ============================================================
 * Demo_Tc387.ld - AURIX TC387 链接脚本 (简化示例)
 * ============================================================ */
ENTRY(_START)

MEMORY
{
    pflash0 (rx) : ORIGIN = 0x80000000, LENGTH = 3M    /* 程序闪存 */
    dsram0  (rw) : ORIGIN = 0x70000000, LENGTH = 240K  /* CPU0 数据 RAM */
}

SECTIONS
{
    .text   : { *(.text*)   } > pflash0
    .rodata : { *(.rodata*) } > pflash0
    .data   : { *(.data*)   } > dsram0 AT > pflash0
    .bss    : { *(.bss*) *(COMMON) } > dsram0
}
