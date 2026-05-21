#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "../../../../../../../esp/v6.0.1/esp-idf/components/esp_driver_i2c/i2c_private.h"
#include "../../../../../../../esp/v6.0.1/esp-idf/components/esp_driver_i2c/include/driver/i2c_slave.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "driver/i2c_slave.h"

//I2C Define
#define I2C_SLAVE_SDA_IO   11
#define I2C_SLAVE_SCL_IO   12

static QueueHandle_t i2c_event_queue;

// Motors Define
#define MOTOR_IN1 35
#define MOTOR_IN2 36
#define PWM_FREQ 20000
#define PWM_RES  LEDC_TIMER_10_BIT
#define PWM_MAX  1023


//Handles changing the PWM
void dutyCycle(int speed)
{
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, speed);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
}


//Process I2C data and send to DUTYCYCLE FUNCTION
static void process_i2c( void *arg)
{
    i2c_slave_rx_done_event_data_t evt;
    while(1)
    {
       if (xQueueReceive(i2c_event_queue, &evt, portMAX_DELAY) == pdTRUE) {
           dutyCycle(evt.buffer[0]); // check if the buffer includes the 0 bit for reading or is it just the data
       }
    }
}

void motors_init(void)
{
    //PWM timer
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RES,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ledc_timer_config(&timer);

    //Channel for IN1
    ledc_channel_config_t ch1 = {
        .gpio_num = MOTOR_IN1,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,


        .duty = 0,

        .hpoint = 0
    };

    ledc_channel_config(&ch1);

    //Channel for IN2
    ledc_channel_config_t ch2 = {
        .gpio_num = MOTOR_IN2,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,

        .duty = 0,

        .hpoint = 0
    };

    ledc_channel_config(&ch2);
}

static bool IRAM_ATTR on_receive(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(i2c_event_queue, evt_data, &xHigherPriorityTaskWoken);
    return xHigherPriorityTaskWoken; //look up how to utilize this //return false | you return true if you want a higher task to be woken up
}

void i2c_slave_init(void) {
    i2c_slave_dev_handle_t slave_handle;

    i2c_slave_config_t slave_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_11,
        .scl_io_num = GPIO_NUM_12,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .send_buf_depth = 128,
        .receive_buf_depth = 128,
        .slave_addr = 0x42,
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .intr_priority = 0,
        .flags = {
        .allow_pd = 0,
        .enable_internal_pullup = 1}
    };
    i2c_new_slave_device(&slave_config, &slave_handle);

    i2c_event_queue = xQueueCreate(5, sizeof(i2c_slave_rx_done_event_data_t));

    i2c_slave_event_callbacks_t cbs= {
        .on_receive = on_receive,
    };

    i2c_slave_register_event_callbacks(slave_handle, &cbs, NULL);
}

void app_main(void)
{
    motors_init();
    i2c_slave_init();
    xTaskCreate(process_i2c, "i2c_task", 4096, i2c_event_queue, 10, NULL);
}