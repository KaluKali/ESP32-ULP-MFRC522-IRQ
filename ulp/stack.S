/*
 * ULP stack and subroutine macros
 */
.macro push rx
	st \rx,r3,0
	sub r3,r3,1
.endm

.macro pop rx
	add r3,r3,1
	ld \rx,r3,0
.endm

// Prepare subroutine jump, uses scratch register sr
.macro psr sr=r1 pos=.
	.set _next2,(\pos+16)
	move \sr,_next2
	push \sr
.endm

// Return from subroutine
.macro ret sr=r1
	pop \sr
	jump \sr
.endm
/*
 * END
 */
.macro write_mode reg
	move r2,\reg
	lsh r2,r2,1
	and r2,r2,0x7e00
.endm
.macro read_mode reg
	move r2,\reg
	lsh r2,r2,1
	and r2,r2,0x7e00
	or r2,r2,0x8000
.endm
.macro rc522_write_data reg data
	clear_CS
	write_mode \reg
	move r1,\data
	rsh r1,r1,8
	or r2,r2,r1
	psr
	jump SPI_Write
	set_CS
.endm

.macro spi_delay
	wait 10
.endm

.macro read_IRQ
	READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S + 9, 1)
.endm
.macro read_MISO
	READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S + 15, 1)
.endm
.macro clear_SCLK
	WRITE_RTC_REG(RTC_GPIO_OUT_W1TC_REG, RTC_GPIO_OUT_DATA_W1TC_S + 16, 1, 1)
.endm
.macro set_SCLK
	WRITE_RTC_REG(RTC_GPIO_OUT_W1TS_REG, RTC_GPIO_OUT_DATA_W1TS_S + 16, 1, 1)
.endm

.macro clear_MOSI
	WRITE_RTC_REG(RTC_GPIO_OUT_W1TC_REG, RTC_GPIO_OUT_DATA_W1TC_S + 14, 1, 1)
.endm
.macro set_MOSI
	WRITE_RTC_REG(RTC_GPIO_OUT_W1TS_REG, RTC_GPIO_OUT_DATA_W1TS_S + 14, 1, 1)
.endm

.macro clear_CS
	WRITE_RTC_REG(RTC_GPIO_OUT_W1TC_REG, RTC_GPIO_OUT_DATA_W1TC_S + 13, 1, 1)
.endm
.macro set_CS
	WRITE_RTC_REG(RTC_GPIO_OUT_W1TS_REG, RTC_GPIO_OUT_DATA_W1TS_S + 13, 1, 1)
.endm
