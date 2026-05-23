idf_component_register(
        SRCS "main.c" "mpu6050.c"
        INCLUDE_DIRS "include"
        REQUIRES esp_driver_gpio freertos spi_flash esp_driver_ledc driver esp_driver_i2c esp_ringbuf esp_pm
        
)