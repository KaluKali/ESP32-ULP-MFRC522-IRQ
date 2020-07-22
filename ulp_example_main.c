/* ULP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "esp_sleep.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "soc/soc.h"
#include "driver/rtc_io.h"
#include "esp32/ulp.h"
#include "sdkconfig.h"
#include "ulp_main.h"
#include "math.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "rc522.c"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

const gpio_num_t GPIO_CS = GPIO_NUM_15;
const gpio_num_t GPIO_MOSI = GPIO_NUM_13;
const gpio_num_t GPIO_SCLK = GPIO_NUM_14;
const gpio_num_t GPIO_MISO = GPIO_NUM_12;
const gpio_num_t GPIO_IRQ = GPIO_NUM_32;

static esp_timer_handle_t to_deep_sleep_timer;

static void init_ulp_program()
{
    esp_err_t err = ulp_load_binary(0, ulp_main_bin_start,(ulp_main_bin_end - ulp_main_bin_start) / sizeof(uint32_t));
    ESP_ERROR_CHECK(err);

    rtc_gpio_init(GPIO_CS);
    rtc_gpio_set_direction(GPIO_CS, RTC_GPIO_MODE_OUTPUT_ONLY);


    rtc_gpio_init(GPIO_MOSI);
    rtc_gpio_set_direction(GPIO_MOSI, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_set_level(GPIO_MOSI, 0);

    rtc_gpio_init(GPIO_SCLK);
    rtc_gpio_set_direction(GPIO_SCLK, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_set_level(GPIO_SCLK, 0);

    rtc_gpio_init(GPIO_MISO);
    rtc_gpio_set_direction(GPIO_MISO, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pullup_en(GPIO_MISO);

    assert(rtc_gpio_desc[GPIO_IRQ].reg && "GPIO used for pulse counting must be an RTC IO");
    ulp_io_number = rtc_gpio_desc[GPIO_IRQ].rtc_num; /* map from GPIO# to RTC_IO# */
    ulp_next_edge = 0;

    rtc_gpio_init(GPIO_IRQ);
    rtc_gpio_set_direction(GPIO_IRQ, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(GPIO_IRQ);
    rtc_gpio_pullup_dis(GPIO_IRQ);
    rtc_gpio_hold_en(GPIO_IRQ);

    gpio_set_direction(2, GPIO_MODE_OUTPUT);
}
static void to_deep_sleep(){
	init_ulp_program();
	printf("Entering deep sleep\n\n");
	esp_deep_sleep_disable_rom_logging();
	ESP_ERROR_CHECK( ulp_run((&ulp_entry - RTC_SLOW_MEM) / sizeof(uint32_t)));
	ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup() );
	esp_deep_sleep_start();
}

void tag_handler(uint8_t* serial_no) {
	esp_timer_stop(to_deep_sleep_timer);

    for(int i = 0; i < 5; i++) {
        printf("%#x ", serial_no[i]);
    }
    printf("\n");
    rc522_end();
    to_deep_sleep();
}

void app_main()
{
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause != ESP_SLEEP_WAKEUP_ULP) {
        printf("Not ULP wakeup, initializing ULP\n");
        to_deep_sleep();
    } else {
    	rtc_gpio_force_hold_dis_all();
    	gpio_reset_pin(GPIO_NUM_15);
    	gpio_reset_pin(GPIO_NUM_13);
    	gpio_reset_pin(GPIO_NUM_14);
    	gpio_reset_pin(GPIO_NUM_12);

		const rc522_start_args_t start_args = {
			.miso_io  = 12,
			.mosi_io  = 13,
			.sck_io   = 14,
			.sda_io   = 15,
			.callback = &tag_handler
		};
		rc522_start(start_args);

	    const esp_timer_create_args_t timer_args = {
	        .callback = &to_deep_sleep
	    };

	    esp_err_t ret = esp_timer_create(&timer_args, &to_deep_sleep_timer);
	    if(ret != ESP_OK) {
	    	printf("ERROR: to_deep_sleep_timer");
	    	to_deep_sleep();
	    }

	    esp_timer_start_once(to_deep_sleep_timer,5000000);
    }
}
