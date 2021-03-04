/******************************************************************************
* File Name:   main.c
*
* Description:
* This is the source code for the PSoC6 MCU: Basic Device Firmware Upgrade (DFU)
* Example for ModusToolbox.
* This file implements the DFUSDK based Bootloader.
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
#include "cycfg_pins.h"
#include "cy_dfu.h"
#include "dfu_user.h"
#include "cyhal_gpio.h"

/*******************************************************************************
* Macros
********************************************************************************/

/* Interval for LED toggle: 1  blink per second */
#define    LED_TOGGLE_INTERVAL_MS                  (500u)

/* User button interrupt priority. */
#define GPIO_INTERRUPT_PRIORITY                    (7u)

/* Timeout for Cy_DFU_Continue(), in milliseconds */
#define DFU_SESSION_TIMEOUT_MS                     (20u)

/* Application ID */
#define USER_APP_ID                                (1u)

/* DFU idle wait timeout: 300 seconds*/
#define DFU_IDLE_TIMEOUT_MS                        (300000u)

/* DFU session timeout: 5 seconds */
#define DFU_COMMAND_TIMEOUT_MS                     (5000u)

/*******************************************************************************
* Function Prototypes
********************************************************************************/
static void user_button_event_handler(void *handler_arg, cyhal_gpio_irq_event_t event);
static cy_en_dfu_status_t handle_metadata(cy_stc_dfu_params_t *params);
static cy_en_dfu_status_t copy_row(uint32_t dest, uint32_t src, uint32_t rowSize, cy_stc_dfu_params_t * params);
static cy_en_dfu_status_t restart_dfu(uint32_t *state, cy_stc_dfu_params_t *dfu_params );
static cy_en_dfu_status_t manage_bootable_app(cy_stc_dfu_params_t *dfu_params);
static uint32_t counter_timeout_seconds(uint32_t seconds, uint32_t timeout);
void Cy_OnResetUser(void);

/*******************************************************************************
* Global Variables
********************************************************************************/
volatile static bool is_user_event_detected = false;

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the bootloder main function for CM4 CPU.
*  1. If application started from Non-Software reset it validates app #1
*        --> If app#1 is valid it switches to app#1, else goto #2.
*  2. Start DFU communication.
*  3. If updated application has been received it validates this app.
*  4. If app#1 is valid it switches to it, else wait for new application.
*  5. If DFU_IDLE_TIMEOUT_MS seconds has passed and no new application has been received
*     then validate app#1, if it is valid then switch to it, else continue as is.
*     #5 will be repeated for every DFU_IDLE_TIMEOUT_MS seconds, if the new firmware is not received.
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
    cy_rslt_t result = CY_RSLT_SUCCESS;

    result = cybsp_init() ;
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Used to count seconds. */
    uint32_t count = 0;

    /* Status codes for DFU API. */
    cy_en_dfu_status_t status;

    /* Timeout variable. */
    uint32_t timeout_seconds = 0;

    /*
    * DFU state, one of the:
    * - CY_DFU_STATE_NONE
    * - CY_DFU_STATE_UPDATING
    * - CY_DFU_STATE_FINISHED
    * - CY_DFU_STATE_FAILED
    */
    uint32_t state = CY_DFU_STATE_NONE;

    /* Buffer to store DFU commands. */
    CY_ALIGN(4) static uint8_t buffer[CY_DFU_SIZEOF_DATA_BUFFER];

    /* Buffer for DFU data packets for transport API. */
    CY_ALIGN(4) static uint8_t packet[CY_DFU_SIZEOF_CMD_BUFFER];

    /* DFU params, used to configure DFU. */
    cy_stc_dfu_params_t dfu_params;

    /* Initialize dfu_params structure. */
    dfu_params.timeout          = DFU_SESSION_TIMEOUT_MS;
    dfu_params.dataBuffer       = &buffer[0];
    dfu_params.packetBuffer     = &packet[0];

    status = Cy_DFU_Init(&state, &dfu_params);
    CY_ASSERT(CY_DFU_SUCCESS == status);

    /* Ensure DFU Metadata is valid. */
    status = handle_metadata(&dfu_params);
    CY_ASSERT(CY_DFU_SUCCESS == status);

    /* In the case of non-software reset check if there is a valid app image.
     * If these is - switch to it.
     */
    if( (Cy_SysLib_GetResetReason() != CY_SYSLIB_RESET_SOFT) )
    {
        /* Boot to application image, if valid. */
        status = manage_bootable_app(&dfu_params);

        if (status == CY_DFU_SUCCESS)
        {
           /* Never returns from here. */
           Cy_DFU_ExecuteApp(USER_APP_ID);
        }

        /* Continue DFU execution (below), if user application not valid !. */
    }

    /* Initialize the user button. */
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                             CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Configure interrupt. */
    cyhal_gpio_register_callback(CYBSP_USER_BTN,
                                 user_button_event_handler, NULL);

    /* Enable interrupt. */
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                            GPIO_INTERRUPT_PRIORITY, true);

    /* Initialize the User LED. */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Enable interrupts. */
    __enable_irq();

    /* Initialize DFU communication. */
    Cy_DFU_TransportStart();

    /* Keep the compiler happy. */
    (void)result;

    for (;;)
    {
        status = Cy_DFU_Continue(&state, &dfu_params);

        ++count;
        if (state == CY_DFU_STATE_FINISHED)
        {
           /* Finished loading the application image.
            * Validate DFU application, if it is valid then switch to it.
            */

            status = manage_bootable_app(&dfu_params);
           if (status == CY_DFU_SUCCESS)
           {
               Cy_DFU_TransportStop();
               Cy_DFU_ExecuteApp(USER_APP_ID);
           }
           else if (status == CY_DFU_ERROR_VERIFY)
           {
               /* Restarts loading, an alternatives are to Halt MCU here
                * or switch to the other app if it is valid.
                * Error code may be handled here, i.e. print to debug UART.
                */
               status = restart_dfu(&state, &dfu_params );
           }
        }
        else if (state == CY_DFU_STATE_FAILED)
        {
           /* An error has happened during the loading process.
            * In this Code Example just restart loading process.
            * Restart DFU.
            */
            status = restart_dfu(&state, &dfu_params );
        }
        else if (state == CY_DFU_STATE_UPDATING)
        {
           timeout_seconds = (count >= counter_timeout_seconds(DFU_COMMAND_TIMEOUT_MS, DFU_SESSION_TIMEOUT_MS)) ? USER_APP_ID : 0u;

           /* if no command has been received during 5 seconds when the loading
            * has started then restart loading.
            */
           if (status == CY_DFU_SUCCESS)
           {
               count = 0u;
           }
           else if (status == CY_DFU_ERROR_TIMEOUT)
           {
               if (timeout_seconds != 0u)
               {
                   count = 0u;

                   /* Restart DFU. */
                   status = restart_dfu(&state, &dfu_params );
               }
           }
           else
           {
               count = 0u;

               /* Delay because Transport still may be sending error response to a host. */
               Cy_SysLib_Delay(DFU_SESSION_TIMEOUT_MS);

               /* Restart DFU. */
               status = restart_dfu(&state, &dfu_params );
           }
        }

        /* Blink once per seconds */
        if ( ( count % counter_timeout_seconds(LED_TOGGLE_INTERVAL_MS, DFU_SESSION_TIMEOUT_MS)) == 0u)
        {
            /* Invert the USER LED state */
            cyhal_gpio_toggle(CYBSP_USER_LED);
        }

        if((count >= counter_timeout_seconds(DFU_IDLE_TIMEOUT_MS, DFU_SESSION_TIMEOUT_MS) ) && (state == CY_DFU_STATE_NONE))
        {
            status = manage_bootable_app(&dfu_params);

            if (status == CY_DFU_SUCCESS)
            {
                Cy_DFU_TransportStop();
                Cy_DFU_ExecuteApp(USER_APP_ID);
            }

            /* In case, no valid user application, lets start fresh all over.
             * This is just for demonstration.
             * Final application can change it to either assert, reboot, enter low power mode etc,
             * based on usecase requirements.
             */
            count = 0 ;
        }

        /* Is user_button event detected ?. */
        if(is_user_event_detected == true)
        {
            is_user_event_detected = false;

            /* Boot to application image, if valid. */
            status = manage_bootable_app(&dfu_params);
            if (status == CY_DFU_SUCCESS)
            {
                /* Freeup the resources acquired. */
                cyhal_gpio_free(CYBSP_USER_BTN);

               /* Never returns from here. */
               Cy_DFU_ExecuteApp(USER_APP_ID);
            }
        }
    }
}

/*******************************************************************************
* Function Name: counter_timeout_seconds
********************************************************************************
* Returns number of counts that correspond to number of seconds passed as
* a parameter.
* E.g. comparing counter with 300 seconds is like this.
* ---
* uint32_t counter = 0u;
* for (;;)
* {
*     Cy_SysLib_Delay(UART_TIMEOUT);
*     ++count;
*     if (count >= counter_timeout_seconds(seconds: 300u, timeout: UART_TIMEOUT))
*     {
*         count = 0u;
*         DoSomething();
*     }
* }
* ---
*
* Both parameters are required to be compile time constants,
* so this function gets optimized out to single constant value.
*
* Parameters:
*  seconds    Number of seconds to pass. Must be less that 4_294_967 seconds.
*  timeout    Timeout for Cy_DFU_Continue() function, in milliseconds.
*             Must be greater than zero.
*             It is recommended to be a value that produces no reminder
*             for this function to be precise.
* Return:
*  See description.
*******************************************************************************/
static uint32_t counter_timeout_seconds(uint32_t seconds, uint32_t timeout)
{
    uint32_t count = 1 ;

    if(timeout != 0 )
    {
        count = ( (seconds) / timeout);
    }

    return count;
}

/*******************************************************************************
* Function Name: ManageBootableApp
********************************************************************************
* Performs validation of application, clears the existing rest reason if
* a valid image is found.
*
* Parameters:
*  dfuParams     input DFU parameters.
*
* Returns:
*  CY_DFU_SUCCESS if operation is successful and an error code in case of failure.
*******************************************************************************/
static cy_en_dfu_status_t manage_bootable_app(cy_stc_dfu_params_t *dfu_params)
{
    cy_en_dfu_status_t status = CY_DFU_SUCCESS;

    if(!dfu_params)
    {
        status = CY_DFU_ERROR_UNKNOWN;
    }

    /* Satisfy the compiler. */
    (void)dfu_params;

    if (status == CY_DFU_SUCCESS)
    {
        /* Validate the APP in SECONDARY slot. */
        status = Cy_DFU_ValidateApp(USER_APP_ID, NULL);
    }

    if (status == CY_DFU_SUCCESS)
    {
        /*
        * Clear reset reason because Cy_DFU_ExecuteApp() performs a
        * software reset. Without clearing, two reset reasons would be present.
        */
        do
        {
            Cy_SysLib_ClearResetReason();
        }while(Cy_SysLib_GetResetReason() != 0 );
    }
    return status;
}

/*******************************************************************************
* Function Name: restart_dfu
********************************************************************************
* This function re-initializes the DFU and resets the DFU transport.
*
* Parameters:
*  dfu_params     input DFU parameters.
*  state          input current state of the DFU
*
* Return:
*  Status of operation.
*******************************************************************************/
static cy_en_dfu_status_t restart_dfu(uint32_t *state, cy_stc_dfu_params_t *dfu_params )
{
    cy_en_dfu_status_t status = CY_DFU_SUCCESS;

    if(!state || !dfu_params)
    {
        status = CY_DFU_ERROR_UNKNOWN;
    }

    /* Restart DFU process. */
    if( status == CY_DFU_SUCCESS)
    {
        status = Cy_DFU_Init(state, dfu_params);
    }

    if( status == CY_DFU_SUCCESS)
    {
        Cy_DFU_TransportReset();
    }

    return status;
}

/*******************************************************************************
* Function Name: copy_row
********************************************************************************
* Copies data from a source address to a flash row with the address destination.
* If "src" data is the same as "dest" data then no copy is performed.
*
* Parameters:
*  dest     Destination address. Has to be an address of the start of flash row.
*  src      Source address.
*  rowSize  Size of the flash row.
*
* Returns:
*  CY_DFU_SUCCESS if operation is successful. Error code in a case of failure.
*******************************************************************************/
static cy_en_dfu_status_t copy_row(uint32_t dest, uint32_t src, uint32_t rowSize, cy_stc_dfu_params_t * params)
{
    cy_en_dfu_status_t status;

    /* Save params->dataBuffer value. */
    uint8_t *buffer = params->dataBuffer;

    /* Compare "dest" and "src" content. */
    params->dataBuffer = (uint8_t *)src;
    status = Cy_DFU_ReadData(dest, rowSize, CY_DFU_IOCTL_COMPARE, params);

    /* Restore params->dataBuffer. */
    params->dataBuffer = buffer;

    /* If "dest" differs from "src" then copy "src" to "dest". */
    if (status != CY_DFU_SUCCESS)
    {
        (void) memcpy((void *) params->dataBuffer, (const void*)src, rowSize);
        status = Cy_DFU_WriteData(dest, rowSize, CY_DFU_IOCTL_WRITE, params);
    }
    /* Restore params->dataBuffer. */
    params->dataBuffer = buffer;

    return (status);
}

/*******************************************************************************
* Function Name: handle_metadata
********************************************************************************
* The goal of this function is to make DFU SDK metadata (MD) valid.
* The following algorithm is used (in C-like pseudo code):
* ---
* if (isValid(MD) == true)
* {   if (MDC != MD)
*         MDC = MD;
* } else
* {   if(isValid(MDC) )
*         MD = MDC;
*     else
*         MD = INITIAL_VALUE;
* }
* ---
* Here MD is metadata flash row, MDC is flash row with metadata copy,
* INITIAL_VALUE is known initial value.
*
* In this code example MDC is placed in the next flash row after the MD, and
* INITIAL_VALUE is MD with only CRC, App0 start and size initialized,
* all the other fields are not touched.
*
* Parameters:
*  params   A pointer to a DFU SDK parameters structure.
*
* Returns:
* - CY_DFU_SUCCESS when finished normally.
* - Any other status code on error.
*******************************************************************************/
static cy_en_dfu_status_t handle_metadata(cy_stc_dfu_params_t *params)
{
    const uint32_t MD     = (uint32_t)(&__cy_boot_metadata_addr   ); /* MD address.  */
    const uint32_t mdSize = (uint32_t)(&__cy_boot_metadata_length ); /* MD size, assumed to be one flash row .*/
    const uint32_t MDC    = MD + mdSize;                             /* MDC address. */

    cy_en_dfu_status_t status = CY_DFU_SUCCESS;

    status = Cy_DFU_ValidateMetadata(MD, params);
    if (status == CY_DFU_SUCCESS)
    {
        /* Checks if MDC equals to DC, if no then copies MD to MDC. */
        status = copy_row(MDC, MD, mdSize, params);
    }
    else
    {
        status = Cy_DFU_ValidateMetadata(MDC, params);
        if (status == CY_DFU_SUCCESS)
        {
            /* Copy MDC to MD. */
            status = copy_row(MD, MDC, mdSize, params);
        }

        /* Possibly a clean boot or first boot. */
        if (status != CY_DFU_SUCCESS)
        {
            const uint32_t elfStartAddress = 0x10000000;
            const uint32_t elfAppSize      = 0x8000;

            /* Set MD to INITIAL_VALUE. */
            status = Cy_DFU_SetAppMetadata(0u, elfStartAddress, elfAppSize, params);
        }
    }
    return (status);
}

/*******************************************************************************
* Function Name: Cy_OnResetUser
********************************************************************************
*
*  This function is called at the start of Reset_Handler().
*  DFU requires it to call Cy_DFU_OnResetApp0() in app#0.
*
*******************************************************************************/
void Cy_OnResetUser(void)
{
    Cy_DFU_OnResetApp0();
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
