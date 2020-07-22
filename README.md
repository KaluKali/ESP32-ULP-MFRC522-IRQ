##esp32-ulp-MFRC522-IRQ

Using interrupt on mfrc522 with ulp.
You are changed pins for spi if you dont use SPI on main cores esp32.


For change pins you must write RTC port instead of + port in the stack.S
For example:

`
READ_RTC_REG (RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S + {YOUR RTC-PORT}, 1)`
##PINS

| PIN  | USAGE |
| ------------- | ------------- |
| D15  | CS  |
| D13  | MOSI  |
| D12  | MISO  |
| D14  | SCK  |
| D32  | IRQ  |