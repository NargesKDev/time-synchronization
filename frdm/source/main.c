#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "MK64F12.h"
#include "fsl_uart.h"
#include "peripherals.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_rtc.h"

#define BUFFER_SIZE 32
char rxBuffer[BUFFER_SIZE] = {0};

void receive_datetime_from_uart(UART_Type *base)
{
    uint8_t ch;
    int idx = 0;

    while (1)
    {
        if (UART_ReadBlocking(base, &ch, 1) == kStatus_Success)
        {
            if (ch == '\n' || idx >= BUFFER_SIZE - 1)
            {
                rxBuffer[idx] = '\0'; // Null-terminate string
                break;
            }
            else
            {
                rxBuffer[idx++] = ch;
            }
        }
    }
}

void set_rtc(const char *datetime_str)
{
    int year, month, day, hour, minute, second;
    sscanf(datetime_str, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second);
    rtc_datetime_t rtcDate;
    rtc_config_t rtc_config;

    rtcDate.year   = year;
    rtcDate.month  = month;
    rtcDate.day    = day;
    rtcDate.hour   = hour;
    rtcDate.minute = minute;
    rtcDate.second = second;

    RTC_Init(RTC, &rtc_config);
    RTC_SetDatetime(RTC, &rtcDate);
}

int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    BOARD_InitBootPeripherals();

    uart_config_t config;
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

    UART_Init(UART3, &config, CLOCK_GetFreq(UART3_CLK_SRC));

    PRINTF("Waiting for date and time from ESP32...\r\n");

    while (1)
    {
        receive_datetime_from_uart(UART3);
        PRINTF("Received datetime: %s\r\n", rxBuffer);
        set_rtc(rxBuffer);

        rtc_datetime_t currentRtcTime;
        RTC_GetDatetime(RTC, &currentRtcTime);

        PRINTF("Current RTC Time: %04d-%02d-%02d %02d:%02d:%02d\r\n",
               currentRtcTime.year,
               currentRtcTime.month,
               currentRtcTime.day,
               currentRtcTime.hour,
               currentRtcTime.minute,
               currentRtcTime.second);
    }

    return 0;
}
