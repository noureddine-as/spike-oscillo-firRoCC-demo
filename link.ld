/*======================================================================*/
/* TIMA LABORATORY                                                      */
/* Spike with signal and image visual presentation                      */ 
/* noureddine-as                                                        */
/* noureddine.aitsaid0@gmail.com                                        */
/*======================================================================*/

/*----------------------------------------------------------------------*/
/* Setup                                                                */
/*----------------------------------------------------------------------*/

OUTPUT_ARCH( "riscv" )

MONITOR_BASE = 0x81000000;
OSCILLO_BASE = 0x82000000;

ENTRY(_start)

/*----------------------------------------------------------------------*/
/* Sections                                                             */
/*----------------------------------------------------------------------*/

SECTIONS
{

  /* text: test code section */
  . = 0x80000000;
  .text.init : { *(.text.init) }

  . = ALIGN(0x1000);
  .tohost : { *(.tohost) }

  .text : { *(.text) }

  /* data segment */
  .data : { *(.data) }

  .sdata : {
    _gp = . + 0x800;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  }

  /* bss segment */
  .sbss : {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  }
  .bss : {
    *(.bss)
   }

  /* thread-local data segment */
  .tdata :
  {
    _tls_data = .;
    _tdata_begin = . ;
    *(.tdata)
    _tdata_end = .;
  }
  .tbss :
  {
    *(.tbss)
    _tbss_end = .;
  }

  .heap : {
    . = ALIGN(4);
    _heap = .;
    _heap_start = .;
    . += 0x1000;
    _heap_end = .;
  }

  .stack : {
    _stack_end = .;
    . += 0x1000;

    . = ALIGN(4);       

    _stack = .;
    __stack = _stack;
  }

  /* End of uninitalized data segement */
  _end = .;

/*
  . = MONITOR_BASE;
  .monitor : {
    *(.monitor)
    __monitor_end = .;
  }
*/
  . = OSCILLO_BASE;
  .oscillo : {
    *(.oscillo)
    __oscillo_end = .;
  }



}

