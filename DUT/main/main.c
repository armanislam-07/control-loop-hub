#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "driver/gpio.h"
#include "semphr.h"
#include "driver/ledc.h"
#include "driver/i2c.h"


static SemaphoreHandle_t i2cSemaphore;

#define MOTOR_IN1 18
#define MOTOR_IN2 19]

#define PWM_FREQ 20000
#define PWM_RES  LEDC_TIMER_10_BIT
#define PWM_MAX  1023


//Handles changing the PWM
void dutyCycle(int speed)
{
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}


// Function for ISR | pdTRUE and then sets the flag 
void IRAM_ATTR i2c_isr_handler(void * arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(i2cSemaphore, &xHigherPriorityTaskWoken);

    // we can add flag for isr completion using portYIELD but i dont think we need to 
}

//Function for the Task that listens waiting for ISR flag then calls i2c_task 
void process_i2c(void * arg)
{
  uint8_t data[6];

    while(1)
    {
      if(xSemaphoreTake(i2cSemaphore, portMAX_DELAY) == pdTRUE)
      {
        i2c_master_write_read_device(I2C_MASTER_NUM, SENSOR_ADDR, &reg_addr, 1, data, 6, 1000 / portTICK_PERIOD_MS);
        dutyCycle(data[6]);
      }
    }

}

void motors_init(void)
{
    // Configure PWM timer
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RES,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ledc_timer_config(&timer);

    // Channel for IN1
    ledc_channel_config_t ch1 = {
        .gpio_num = MOTOR_IN1,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,


        .duty = 0,

        .hpoint = 0
    };

    ledc_channel_config(&ch1);

    // Channel for IN2
    ledc_channel_config_t ch2 = {
        .gpio_num = MOTOR_IN2,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,

        .duty = 0,

        .hpoint = 0
    };

    ledc_channel_config(&ch2);
}

void app_main(void)
{   
    i2cSemaphore = xSemaphoreCreateBinary();
    motors_init();

    gpio_install_isr_service(0);
    gpio_isr_handler_add(I2C_INT_PIN, i2c_isr_handler, NULL);

    xTaskCreate(process_i2c, "i2c_task",  3072, NULL, 10, NULL);
}   