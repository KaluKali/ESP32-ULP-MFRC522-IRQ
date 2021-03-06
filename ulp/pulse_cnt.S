#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/soc_ulp.h"
// macros
#include "stack.S"

	.set bit_mask, 	0x8000
	.set bit_len, 	0x10
	/* Define variables, which go into .bss section (zero-initialized data) */
	.bss
#if 1
	.global io_number
io_number:
	.long 0
	.global next_edge
next_edge:
	.long 0
#endif

	/* Code goes into .text section */
	.text
	.global entry
entry:
/*
r1 free
r2 free, change in function
r3 Return from subroutine
r0 change RTC_READ_REG
*/
	clear_MOSI
	clear_SCLK
	set_CS

	rc522_write_data 0x0100 0x0f00 // restart mfrc
	wait 8000*50
	// init mfrc
	rc522_write_data 0x2A00 0x8D00
	rc522_write_data 0x2B00 0x3E00
	rc522_write_data 0x2D00 0x1E00
	rc522_write_data 0x2C00 0x0000
	rc522_write_data 0x1500 0x4000
	rc522_write_data 0x1100 0x3D00
	rc522_write_data 0x1400 0x8300
	rc522_write_data 0x2600 0x6000
	rc522_write_data 0x0200 0xA000
	// end init mfrc
	jump irq_wait

  .global irq_wait
irq_wait:
	rc522_write_data 0x0900 0x5200
	rc522_write_data 0x0100 0x0c00
	rc522_write_data 0x0d00 0x8700
	move r2,100
	wait 8000*90 // 90ms
	//
	move r2, io_number
	ld r2, r2, 0

	move r0, r2
	jumpr read_io_high, 16, ge

	READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S, 16)
	rsh r0, r0, r2
	jump read_done

	read_io_high:
	READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S + 16, 2)
	sub r2, r2, 16
	rsh r0, r0, r2

	read_done:
	and r0, r0, 1

	move r2, next_edge
	ld r2, r2, 0
	add r2, r0, r2
	and r2, r2, 1
	jump wake_up, eq
	jump irq_wait

  .global SPI_Write
SPI_Write:
	move r0,0
	stage_rst
write_loop1:
	clear_SCLK
	and r0, r2, bit_mask
	lsh r2, r2, 1
	jumpr loop1_bit0, 1, lt
	set_MOSI
	jump loop1_bit1
loop1_bit0:
	clear_MOSI
loop1_bit1:
	spi_delay
	set_SCLK
	spi_delay
	stage_inc 1
	jumps write_loop1, bit_len, lt
	clear_SCLK
	jump spi_write_byte_end
spi_write_byte_end:
	clear_MOSI
	ret

	.global wake_up
wake_up:                   // Read RTC_CNTL_RDY_FOR_WAKEUP bit
  READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP)
  AND r0, r0, 1
  JUMP wake_up, eq    // Retry until the bit is set
  WAKE                          // Trigger wake up
  REG_WR 0x006, 24, 24, 0       // Stop ULP timer (clear RTC_CNTL_ULP_CP_SLP_TIMER_EN)
  HALT                          // Stop the ULP program
