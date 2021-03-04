/******************************************************************************
* File Name:   main.c
*
* Description:
* This is the source code for the PSoC6 MCU: Basic Device Firmware Upgrade (DFU)
* Example for ModusToolbox.
* This file implements the simple blinky application that can be bootloaded by the
* Bootloader. 
*
* Related Document: See README.md
*
*******************************************************************************
* Copyright 2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_dfu.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* App ID of bootloader. */
#define    BOOTLOADER_ID                    (0u)

/* Define LED toggle frequency. */
#define LED_TOGGLE_INTERVAL_MS              (100u)

/* User button interrupt priority. */
#define GPIO_INTERRUPT_PRIORITY             (7u)

/*******************************************************************************
* Function Prototypes
********************************************************************************/
static void user_button_event_handler(void *handler_arg, cyhal_gpio_irq_event_t event);

/*******************************************************************************
* Global Variables
********************************************************************************/
/* Application signature. */
CY_SECTION(".cy_app_signature") __USED static const uint32_t cy_dfu_appSignature;

volatile static bool is_user_event_detected = false;

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM4 CPU.
*   1. It blinks LED at "LED_TOGGLE_INTERVAL_MS" interval.
*   2. If user button press event is detected, it will transfer control to 
*      Bootloader. 
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the user button */
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                             CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    /* Configure interrupt */
    cyhal_gpio_register_callback(CYBSP_USER_BTN,
                                 user_button_event_handler, NULL);

    /* Enable interrupt. */
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                            GPIO_INTERRUPT_PRIORITY, true);

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize the User LED */
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    for (;;)
    {
        /* Invert the USER LED state */
       cyhal_gpio_toggle(CYBSP_USER_LED);

        /* Delay between LED toggles */
        cyhal_system_delay_ms(LED_TOGGLE_INTERVAL_MS);

        /* Switch to bootloader, if user button is pressed. */
        if(is_user_event_detected == true)
        {
            is_user_event_detected = false;
            Cy_DFU_ExecuteApp(BOOTLOADER_ID);
        }
    }
}

/*******************************************************************************
* Function Name: user_button_event_handler
********************************************************************************
* Summary:
*   User Button event handler.
*
* Parameters:
*  void *handler_arg (unused)
*  cyhal_gpio_irq_event_t (unused)
*
*******************************************************************************/
static void user_button_event_handler(void *handler_arg, cyhal_gpio_irq_event_t event)
{
    is_user_event_detected = true;
}

/* [] END OF FILE */
