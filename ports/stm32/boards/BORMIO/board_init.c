#include <string.h>
#include "py/mphal.h"
#include "storage.h"
#include "ulpi.h"

#if OPENAMP_PY
#include "modopenamp.h"
#endif

void BORMIO_board_early_init(void) {
    HAL_InitTick(0);

    // Enable oscillator pin
    // This is enabled in the bootloader anyway.
    BORMIO_board_osc_enable(true);

    // Make sure UPLI is Not in low-power mode.
    ulpi_leave_low_power();

    #if OPENAMP_PY
    OpenAMP_MPU_Config();
    SystemCoreClockUpdate();
    #endif
}

void BORMIO_reboot_to_bootloader(void) {
    RTC_HandleTypeDef RTCHandle;
    RTCHandle.Instance = RTC;
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0xDF59);
    NVIC_SystemReset();
}

void BORMIO_board_osc_enable(int enable) {
    __HAL_RCC_GPIOH_CLK_ENABLE();
    GPIO_InitTypeDef  gpio_osc_init_structure;
    gpio_osc_init_structure.Pin = GPIO_PIN_1;
    gpio_osc_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_osc_init_structure.Pull = GPIO_PULLUP;
    gpio_osc_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOH, &gpio_osc_init_structure);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, enable);
}

void BORMIO_board_low_power(int mode)
{
    switch (mode) {
        case 0:     // Leave stop mode.
            ulpi_leave_low_power();
            break;
        case 1:     // Enter stop mode.
            ulpi_enter_low_power();
            break;
        case 2:     // Enter standby mode.
            ulpi_enter_low_power();
            break;
    }
    // Enable QSPI deepsleep for modes 1 and 2
    mp_spiflash_deepsleep(&spi_bdev.spiflash, (mode != 0));
}
