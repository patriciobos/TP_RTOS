/*
 * GENERATED FILE - DO NOT EDIT
 * (C) Code Red Technologies Ltd, 2008-2013
 * Linker script memory definitions
 * Created from LinkMemoryTemplate
<<<<<<< HEAD
 * By LPCXpresso v5.0.10 [Build 1022] [2012-11-07]  on Tue Feb 19 20:01:31 ART 2013)
=======
 * By LPCXpresso v4.3.0 [Build 1023] [2012-09-19]  on Tue Feb 19 21:13:11 ART 2013)
>>>>>>> 16dc334be972a00615e1436758310cc63aa7d0f4
*/

MEMORY
{
  /* Define each memory region */
  MFlash512 (rx) : ORIGIN = 0x0, LENGTH = 0x80000 /* 512k */
  RamLoc32 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000 /* 32k */
  RamAHB32 (rwx) : ORIGIN = 0x2007c000, LENGTH = 0x8000 /* 32k */

}
  /* Define a symbol for the top of each memory region */
  __top_MFlash512 = 0x0 + 0x80000;
  __top_RamLoc32 = 0x10000000 + 0x8000;
  __top_RamAHB32 = 0x2007c000 + 0x8000;

