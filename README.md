# PSoC&trade; 6 MCU: basic device firmware upgrade (DFU)

This example demonstrates basic device firmware update (DFU), also known as "bootloading", with PSoC&trade; 6 MCU. This includes downloading an application from a host and installing it in device flash, and then transferring control to that application. This example bundles two applications:

**Bootloader app:** Implements a [DFU middleware library](https://github.com/cypresssemiconductorco/dfu)-based basic bootloader application run by the CM4+ CPU. The bootloader handles image download, verification, and upgrades. When the image is valid, the bootloader lets the CM4 CPU boot the application.

**Blinky app:** This is a tiny application run by the CM4 CPU that blinks an LED at a 5-Hz rate continuously. This application transfers control to the bootloader when the user button is pressed.

**Note:** This example supports only *I2C*, *UART*, and *USB_CDC* for DFU transport currently.

[View this README on GitHub.](https://github.com/cypresssemiconductorco/mtb-example-psoc6-dfu-basic)

[Provide feedback on this code example.](https://cypress.co1.qualtrics.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyMzI1MDQiLCJTcGVjIE51bWJlciI6IjAwMi0zMjUwNCIsIkRvYyBUaXRsZSI6IlBTb0MmdHJhZGU7IDYgTUNVOiBiYXNpYyBkZXZpY2UgZmlybXdhcmUgdXBncmFkZSAoREZVKSIsInJpZCI6InJ4aHYiLCJEb2MgdmVyc2lvbiI6IjEuMy4wIiwiRG9jIExhbmd1YWdlIjoiRW5nbGlzaCIsIkRvYyBEaXZpc2lvbiI6Ik1DRCIsIkRvYyBCVSI6IklDVyIsIkRvYyBGYW1pbHkiOiJQU09DIn0=)

## Requirements

- [ModusToolbox&trade; software](https://www.cypress.com/products/modustoolbox-software-environment) v2.2.1 or later

    **Note:** This code example version requires ModusToolbox&trade; software version 2.2.1 or later and is not backward compatible with v2.2 or older versions.

- Board support package (BSP) minimum required version: 2.0.0

- Programming language: C

- Associated parts: All [PSoC&trade; 6 MCU](http://www.cypress.com/PSoC6) parts

## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm® embedded compiler v9.3.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`
- Arm&reg; compiler v6.13 (`ARM`)
- IAR C/C++ compiler v8.42.2 (`IAR`)

## Supported kits (make variable 'TARGET')

- [PSoC&trade; 6 Bluetooth&reg; LE pioneer kit](https://www.cypress.com/CY8CKIT-062-BLE) (`CY8CKIT-062-BLE`) - Default value of `TARGET`
- [PSoC&trade; 6 Wi-Fi Bluetooth&reg; prototyping kit](https://www.cypress.com/CY8CPROTO-062-4343W) (`CY8CPROTO-062-4343W`)
- [PSoC&trade; 6 Wi-Fi Bluetooth&reg; pioneer kit](https://www.cypress.com/CY8CKIT-062-WiFi-BT) (`CY8CKIT-062-WIFI-BT`)
- [PSoC&trade; 6 Bluetooth&reg; LE prototyping kit](https://www.cypress.com/CY8CPROTO-063-BLE) (`CY8CPROTO-063-BLE`)
- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; pioneer kit](https://www.cypress.com/CY8CKIT-062S2-43012) (`CY8CKIT-062S2-43012`)
- [PSoC&trade; 62S1 Wi-Fi Bluetooth&reg; pioneer kit](https://www.cypress.com/CYW9P62S1-43438EVB-01) (`CYW9P62S1-43438EVB-01`)
- [PSoC&trade; 62S1 Wi-Fi Bluetooth&reg; pioneer kit](https://www.cypress.com/CYW9P62S1-43012EVB-01) (`CYW9P62S1-43012EVB-01`)
- [PSoC&trade; 62S3 Wi-Fi Bluetooth&reg; prototyping kit](https://www.cypress.com/CY8CPROTO-062S3-4343W) (`CY8CPROTO-062S3-4343W`)
- [PSoC&trade; 62S4 pioneer kit](https://www.cypress.com/CY8CKIT-062S4) (`CY8CKIT-062S4`)

## Supported transports (make variable 'TRANSPORT_OPT')

The default transport is I2C. See the [Operation](#operation) section for instructions to change  `TRANSPORT_OPT`.

| TARGET | UART | I2C | USB_CDC |
| :----- | :--- | :--- | :----- |
| CY8CKIT-062-BLE | Yes | Yes | No |
| CY8CPROTO-062-4343W | Yes | Yes | Yes |
| CY8CKIT-062-WIFI-BT | Yes | Yes | Yes |
| CY8CPROTO-063-BLE | Yes | Yes | No |
| CY8CKIT-062S2-43012 | Yes | Yes | Yes |
| CYW9P62S1-43438EVB-01 | Yes | Yes | Yes |
| CYW9P62S1-43012EVB-01 | Yes | Yes | Yes |
| CY8CPROTO-062S3-4343W | Yes | Yes | Yes |
| CY8CKIT-062S4 | Yes | Yes | Yes |

## Hardware setup

This example uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly.

**Note:** The PSoC&trade; 6 Bluetooth&reg; LE pioneer kit (CY8CKIT-062-BLE) and the PSoC&trade; 6 Wi-Fi Bluetooth&reg; pioneer kit (CY8CKIT-062-WIFI-BT) ship with KitProg2 installed. The ModusToolbox&trade; software requires KitProg3. Before using this code example, make sure that the board is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/cypresssemiconductorco/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".

## Software setup

Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en).

This example requires no additional software or tools.

## Using the code example

Create the project and open it using one of the following:

<details><summary><b>In Eclipse IDE for ModusToolbox&trade; software</b></summary>

1. Click the **New Application** link in the **Quick Panel** (or, use **File** > **New** > **ModusToolbox Application**). This launches the [Project Creator](https://www.cypress.com/ModusToolboxProjectCreator) tool.

2. Pick a kit supported by the code example from the list shown in the **Project Creator - Choose Board Support Package (BSP)** dialog.

   When you select a supported kit, the example is reconfigured automatically to work with the kit. To work with a different supported kit later, use the [Library Manager](https://www.cypress.com/ModusToolboxLibraryManager) to choose the BSP for the supported kit. You can use the Library Manager to select or update the BSP and firmware libraries used in this application. To access the Library Manager, click the link from the **Quick Panel**.

   You can also just start the application creation process again and select a different kit.

   If you want to use the application for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. In the **Project Creator - Select Application** dialog, choose the example by enabling the checkbox.

4. Optionally, change the suggested **New Application Name**.

5. The **Application(s) Root Path** defaults to the Eclipse workspace which is usually the desired location for the application. If you want to store the application in a different location, you can change the *Application(s) Root Path* value. Applications that share libraries should be in the same root path.

6. Click **Create** to complete the application creation process.

For more details, see the [Eclipse IDE for ModusToolbox&trade; software user guide](https://www.cypress.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/ide_{version}/docs/mt_ide_user_guide.pdf*).

</details>

<details><summary><b>In command-line interface (CLI)</b></summary>

ModusToolbox&trade; software provides the Project Creator as both a GUI tool and a command line tool, "project-creator-cli". The CLI tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; software install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the "project-creator-cli" tool. On Windows, use the command line "modus-shell" program provided in the ModusToolbox&trade; software installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; software tools. You can access it by typing `modus-shell` in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The following table lists the arguments for this tool:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the `<id>` field of the [BSP](https://github.com/cypresssemiconductorco?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the `<id>` field of the [CE](https://github.com/cypresssemiconductorco?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional


The following example will clone the "[Hello World](https://github.com/cypresssemiconductorco/mtb-example-psoc6-hello-world)" application with the desired name "MyHelloWorld" configured for the *CY8CKIT-062-WIFI-BT* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id CY8CKIT-062-WIFI-BT --app-id mtb-example-psoc6-hello-world --user-app-name MyHelloWorld --target-dir "C:/mtb_projects"
   ```

**Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For more details, see the "Project creator tools" section of the [ModusToolbox&trade; software user guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>

<details><summary><b>In third-party IDEs</b></summary>

Use one of the following options:

- **Use the standalone [Project Creator](https://www.cypress.com/ModusToolboxProjectCreator) tool:**

   1. Launch Project Creator from the Windows Start menu or from *{ModusToolbox&trade; software install directory}/tools_{version}/project-creator/project-creator.exe*.

   2. In the initial **Choose Board Support Package** screen, select the BSP and click **Next**.

   3. In the **Select Application** screen, select the appropriate IDE from the **Target IDE** drop-down menu.

   4. Click **Create** and follow the instructions printed in the bottom pane to import or open the exported project in the respective IDE.

- **Use command-line interface (CLI):**

   1. Follow the instructions from the **In command-line interface (CLI)** section to create the application, and import the libraries using the `make getlibs` command.

   2. Export the application to a supported IDE using the `make <ide>` command.

   3. Follow the instructions displayed in the terminal to create or import the application as an IDE project.

For a list of supported IDEs and more details, see the "Exporting to IDEs" section of the [ModusToolbox&trade; software user guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>




   **Instructions for using Keil IDE**

   1. To export to Keil IDE, MDK compiler must be installed. See [ModusToolBox&trade user guide](https://www.cypress.com/file/512716/download) chapter **Export to Keil µVision 5** for instructions.

      **Note:** Before running the `make uvision5` command, make sure that `CY_COMPILER_PATH` in Makefile is set to the compiler's *\bin* directory. See [KBA229177](https://community.cypress.com/t5/Knowledge-Base-Articles/Managing-the-Makefile-for-ModusToolbox-v2-x-KBA229177/ta-p/250624) to learn how to manage the Makefile.

   2. Generate the *.cyacd2* file in Keil uVision as follows:

      1. On the *Output* tab, append the *.elf* extension to change the name of the executable to generate *\<Application Name>.elf* on build.

      2. On the *User* tab, add post-build instructions to generate the *.cyacd2* file. You can write post-build commands as follows:

          *Post-build command 1*:
       \<Install path of ModusToolbox>/tools_2.2/cymcuelftool-1.0/bin/cymcuelftool.exe --sign {Local Path to \<Application Name>.elf} CRC --output {Local Path to generate \<Application Name>_crc.elf} ;

          *Post-build command 2*:
       \<Install path of ModusToolbox>/tools_2.2/cymcuelftool-1.0/bin/cymcuelftool.exe -P {Local Path to \<Application Name>_crc.elf} --output {Local Path to generate \<Application Name>_crc.cyacd2} ;

          Example:

          *Post-build command 1* (Run#1):

          ```
          C:/Users/<USERNAME>/ModusToolbox/tools_2.2/cymcuelftool-1.0/bin/cymcuelftool.exe --sign C:/ModusWorkspace/mtb-example-psoc6-dfu-basic/blinky_cm4/blinky_cm4_build/blinky_cm4.elf CRC --output C:/ModusWorkspace/mtb-example-psoc6-dfu-basic/blinky_cm4/blinky_cm4_build/blinky_cm4_crc.elf ;
          ```
          <br>

          *Post-build command 2* (Run#2):

          ```
          C:/Users/<USERNAME>/ModusToolbox/tools_2.2/cymcuelftool-1.0/bin/cymcuelftool.exe -P C:/ModusWorkspace/mtb-example-psoc6-dfu-basic/blinky_cm4/blinky_cm4_build/blinky_cm4_crc.elf --output C:/ModusWorkspace/mtb-example-psoc6-dfu-basic/blinky_cm4/blinky_cm4_build/blinky_cm4_crc.cyacd2 ;
          ```

         **Figure 1. Keil post-build steps**

         ![](images/keil-post-build.png)


   **Instructions for using IAR**

   1. To export to IAR, IAR compiler must be installed. See [ModusToolBox&trade; software user guide](https://www.cypress.com/file/512716/download) chapter **Export IAR EWARM** for instructions.

      **Note:** Before running the `make ewarm8` command, make sure that `CY_COMPILER_PATH` in Makefile is set to the compiler's *\bin* directory. See [KBA229177](https://community.cypress.com/t5/Knowledge-Base-Articles/Managing-the-Makefile-for-ModusToolbox-v2-x-KBA229177/ta-p/250624) to learn how to manage the Makefile.

   2. Generate the *.cyacd2* file in IAR EW for Arm as follows:

      1. Use **Project** > **Options** > **Linker** > *Output* tab to change the output file extension from *.out* to *.elf*.

      2. Create a *.bat* file in the project directory using any text editor for post-build operation to generate a *.cyacd2* file. You can write the post-build command in a *.bat* file as follows:

         *Post-build command* (Syntax):

         ```
         \<Install path of ModusToolbox>/tools_2.2/cymcuelftool-1.0/bin/cymcuelftool.exe --sign {Local Path to \<Application Name>.elf} CRC --output {Local Path to generate \<Application Name>_crc.elf} && \<Install path of ModusToolbox>/tools_2.2/cymcuelftool-1.0/bin/cymcuelftool.exe -P {Local Path to \<Application Name>_crc.elf} --output {Local Path to generate \<Application Name>_crc.cyacd2}
         ```

         Example:

         *Post-build command* (RUN):

         ```
         C:/Users/<USERNAME>/ModusToolbox/tools_2.2/cymcuelftool-1.0/bin/cymcuelftool.exe --sign C:/ModusWorkspace/mtb-example-psoc6-dfu-basic/blinky_cm4/Debug/Exe/blinky_cm4.elf CRC --output C:/ModusWorkspace/mtb-example-psoc6-dfu-basic/blinky_cm4/Debug/Exe/blinky_cm4_crc.elf && C:/Users/<USERNAME>/ModusToolbox/tools_2.2/cymcuelftool-1.0/bin/cymcuelftool.exe -P C:/ModusWorkspace/mtb-example-psoc6-dfu-basic/blinky_cm4/Debug/Exe/blinky_cm4_crc.elf --output C:/ModusWorkspace/mtb-example-psoc6-dfu-basic/blinky_cm4/Debug/Exe/blinky_cm4_crc.cyacd2
         ```

      3. On **Project** > **Options** > **Build Actions** tab, select the path to the *.bat* file created for post-build operation, and click **OK**.

         **Figure 2. IAR post-build steps**

         ![](images/iar-post-build.png)

</details>


## Operation

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Build and program the bootloader to the board using one of the following:

   **Note:** *I2C* is configured as the default DFU transport. To change the DFU transport, edit the Makefile to set `TRANSPORT_OPT?=<transport>`.

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade; software</b></summary>

      1. Select the bootloader project in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**.

   </details>


   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target with default DFU transport. You can specify a target, toolchain and transport manually:
      ```
      make program TARGET=<BSP> TOOLCHAIN=<toolchain> TRANSPORT_OPT=<transport>
      ```

      Example:
      ```
      make program TARGET=CY8CKIT-062-BLE TOOLCHAIN=GCC_ARM TRANSPORT_OPT=UART
      ```
   </details>

3. After programming, press the *RESET* button on the board. The bootloader starts automatically and starts blinking the LED. Confirm that the kit LED blinks at approximately 1 Hz.

4. Generate an upgradable application image using one of the following. On a successful build, an *\<APPNAME>.cyacd2* file will be generated.

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade; software</b></summary>

      1. Select the blinky project in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Build (KitProg3_MiniProg4)**.
   </details>

   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make build` command to build the application using the default toolchain to the default target. You can specify a target and toolchain manually:
      ```
      make build TARGET=<BSP> TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make build TARGET=CY8CKIT-062-BLE TOOLCHAIN=GCC_ARM
      ```
   **Note:** The application is DFU-transport-agnostic.

   </details>

5. Perform device firmware upgrade using the DFU host tool:

   1. Open the [DFU host tool](https://www.cypress.com/file/504426/download). Connect to the device using the transport configured.

   2. Select *blinky_crc.cyacd2*. By default, it will be generated in the *mtb-example-psoc6-dfu-basic/<blinky_cm4>/build/* directory on a successful build.

   3. Select an appropriate port based on the transport (`TRANSPORT_OPT`) configured in the bootloader. *I2C* is the default transport configuration. Select *400kHz* speed, set address to *12*, and then click **Program**.

   4. Observe the image download progress status on the progress bar, and wait for the download to complete.

      **Note:** See [DFU host tool](https://www.cypress.com/file/504426/download) documentation for further details on selecting a port and configuring it for communication based on the transport enabled in the bootloader.

**Figure 3. Downloading the application using the DFU host tool**

![](images/dfu-operation.png)

After a successful download, the device will boot to *blinky_cm4*. Observe the user LED blinking at 5 Hz.


### Switching between *bootloader* and *application*

Dynamic switching between the bootloader and the application is enabled with user button events. Press the user button to switch to the bootloader while the application is running. Similarly, press the user button to switch to the application while the bootloader is running. Note that a switching request will be honored by the bootloader only if there is a valid application in the memory.

**Note:** Build the App0 and App1 projects with the same toolchain. Application transfer may fail otherwise. Check the **Build Settings** for each project.


## Debugging

You can debug the example to step through the code. In the IDE, use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For more details, see the "Program and Debug" section in the [Eclipse IDE for ModusToolbox&trade; software user guide](https://www.cypress.com/MTBEclipseIDEUserGuide).

**Note:** **(Only while debugging)** On the CM4 CPU, some code in `main()` may execute before the debugger halts at the beginning of `main()`. This means that some code executes twice - once before the debugger stops execution, and again after the debugger resets the program counter to the beginning of `main()`. See [KBA231071](https://community.cypress.com/docs/DOC-21143) to learn about this and for the workaround.


## Design and implementation

### Overview

This example demonstrates basic device firmware upgrade (DFU) operations based on the [DFU middleware library](https://github.com/cypresssemiconductorco/dfu).


### Bootloader implementation

This bootloader sample implements an immutable bootloader with support for upgrading the application.

The [DFU host tool](https://www.cypress.com/file/504426/download) (typically running on the host PC) sends the application (*\<APPNAME>.cyacd2*) to the device. The bootloader receives the application image in chunks and overwrites the existing application image. See Figure 4 below. If a DFU session is interrupted, the application firmware will not be in a usable state. However, the device can still run the bootloader and perform necessary actions to download and install the application in subsequent DFU sessions.

**Figure 4. DFU design overview**

![](images/dfu-overwrite.png)


### DFU interfaces

The bootloader supports *I2C*, *UART*, and *USB_CDC* interfaces for communicating with the DFU host. The *COMPONENT_CUSTOM_DESIGN_MODUS* directory includes all the necessary configurations to select the supported interfaces. See Table below for the default configuration details. These default configurations can be changed according to the use case. However, you must ensure that the configuration of the DFU host tool matches with that of the bootloader.

**DFU transport configurations**

   |DFU transport: I2C| Default |Description     |
   | :--------------- | :------ | :-----|
   | Mode  | Slave | Device acts as a slave|
   | Address | 12  | 7-bit slave device address |
   | Data rate |400 kbps| DFU supports standard data rates from 50 kbps to 1 Mbps|

   <br>

   |DFU transport: UART| Default |Description  |
   | :--------------- | :------ | :-----|
   | Mode | Standard | Standard, SmartCard, and IrDA are supported UART modes in SCB |
   | Baud rate(bps)|115200 | Supports standard baud rates from 19200 to 115200 |
   | Data width| 8 bits| Standard frame |
   | Parity| None | Standard frame |
   | Stop bits| 1 bit | Standard frame |
   | Bit order| LSB first| Standard frame |

   <br>

   |DFU transport: USB_CDC| Default |Description  |
   | :--------------- | :------ | :-----|
   | Endpoints mask | 255 | |
   | Endpoint 1 transfer type | Interrupt | To initiate the transport |
   | Endpoint 2 transfer type | Bulk | To download and verify the binary image |
   | Endpoints buffer management| Manual CPU | USB supports communication using Auto/Manual DMA|
   | Endpoints access type | 8-bit| |

   <br>

### Memory layout

The first 8-KB region of the flash is reserved for the CM0+ CPU. 64 KB is reserved for the bootloader followed by a 128-KB reserved region for future enhancements. Another 64 KB is allocated to the application firmware followed by the reserved region. The size of this Empty/Reserved region depends on the size of the flash available on the selected target device. See respective device datasheets for details. The last 1-KB region of the flash is reserved for bootloader metadata.

To change the memory layout or usage, update the respective target linker script files. The linker scripts can also be modified to define dedicated regions of the memory for each application.

The RAM is shared by the bootloader and the blinky applications, with a common area used by both projects. The RAM regions must be aligned to the 1-KB boundary because they contain the Interrupt Vector Table Remapped at the start.

**Figure 5. DFU memory map**

![](images/dfu-memory-map.png)


### Software reset

When transferring control from one application to another, the recommended method is through a device software reset. This enables each application to initialize device hardware blocks and signal routing from a known state. It is possible to freeze the state of I/O pins so that they are maintained through a software reset. Defined portions of the SRAM are also maintained through a software reset. For more information, see the device-specific Technical Reference Manual.


### Running the code example on a single-CPU device

This code example is designed to run both application and bootloader on the CM4 CPU. The CM0+ CPU is reserved, which runs a tiny pre-built binary that transfers control to the CM4 CPU on every power cycle. For single-CPU devices, you must modify the linker script to exclude the CM0+ binary.


### Resources and settings

**Table 1. Bootloader resources**

| Resource  |  Alias/object     |    Purpose     |
| :------- | :------------    | :------------ |
| SCB (I2C) (PDL) | DFU_I2C          | I2C slave driver to communicate with the DFU host |
| SCB (UART)(PDL) | DFU_UART          | UART driver to communicate with the DFU host |
| USB (PDL) | DFU_USB_CDC  | USB_CDC driver to communicate with the DFU host |
| GPIO (HAL)    | CYBSP_USER_LED         | User LED                  |
| GPIO (HAL)    | CYBSP_USER_BTN         | User button                  |

<br>

**Table 2. Application resources**

| Resource  |  Alias/object     |    Purpose     |
| :------- | :------------    | :------------ |
| GPIO (HAL)    | CYBSP_USER_LED         | User LED                  |
| GPIO (HAL)    | CYBSP_USER_BTN         | User button                  |

<br>

## Related resources

| Application notes                                            |                                                              |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| [AN228571](https://www.cypress.com/AN228571) – Getting started with PSoC&trade; 6 MCU on ModusToolbox&trade; software | Describes PSoC&trade; 6 MCU devices and how to build your first application with ModusToolbox&trade; software |
| [AN221774](https://www.cypress.com/AN221774) – Getting started with PSoC&trade; 6 MCU on PSoC&trade; Creator | Describes PSoC&trade; 6 MCU devices and how to build your first application with PSoC&trade; Creator |
| [AN210781](https://www.cypress.com/AN210781) – Getting started with PSoC&trade; 6 MCU with Bluetooth&reg; Low Energy connectivity on PSoC&trade; Creator | Describes PSoC&trade; 6 MCU with Bluetooth&reg; LE connectivity devices and how to build your first application with PSoC&trade; Creator |
| [AN215656](https://www.cypress.com/AN215656) – PSoC&trade; 6 MCU: Dual-CPU system design | Describes the dual-CPU architecture in PSoC&trade; 6 MCU, and shows how to build a simple dual-CPU design |
| **Code examples**                                            |                                                              |
| [Using ModusToolbox&trade; software](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software) |  |
| **Device Documentation**                                     |                                                              |
| [PSoC&trade; 6 MCU datasheets](https://www.cypress.com/search/all?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A575&f[2]=field_related_products%3A114026) | [PSoC&trade; 6 technical reference manuals](https://www.cypress.com/search/all/PSoC%206%20Technical%20Reference%20Manual?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A583) |
| **Development kits**                                         |Visit www.cypress.com/microcontrollers-mcus-kits and use the options in the **Select your kit** section to filter kits by *Product family* or *Features*.                                       |
| **Libraries**                                                |                                                              |
| PSoC&trade; 6 peripheral driver library (PDL) and docs  | [mtb-pdl-cat1](https://github.com/cypresssemiconductorco/mtb-pdl-cat1) on GitHub |
| Hardware abstraction layer (HAL) Library and docs    | [mtb-hal-cat1](https://github.com/cypresssemiconductorco/mtb-hal-cat1) on GitHub |
| **Middleware**                                               |                                                              |
| Links to all PSoC&trade; 6 MCU middleware                           | [psoc6-middleware](https://github.com/cypresssemiconductorco/psoc6-middleware) on GitHub |
| **Tools**                                                    |                                                              |
| [Eclipse IDE for ModusToolbox&trade; software](https://www.cypress.com/modustoolbox) | ModusToolbox&trade; software is a collection of easy-to-use software and tools enabling rapid development with Infineon MCUs, covering applications from embedded sense and control to wireless and cloud-connected systems using AIROC&trade; Wi-Fi and Bluetooth® connectivity devices. |

## Other resources

Cypress provides a wealth of data at www.cypress.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC&trade; 6 MCU devices, see [How to design with PSoC&trade; 6 MCU - KBA223067](https://community.cypress.com/docs/DOC-14644) in the Cypress community.

## Document history

Document Title: *CE232504* - *PSoC&trade; 6 MCU: basic device firmware upgrade (DFU)*

| Version | Description of Change |
| ------- | --------------------- |
| 1.0.0   | New code example      |
| 1.1.0   | Updated code example to support USB_CDC |
| 1.2.0   | Updated DFU_PATH in Makefile|
| 1.3.0   | Added support for 256KB CY8CKIT-062S4 kit |
------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

![banner](images/ifx-cy-banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2021. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product.  CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications.  To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document.  Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br>
Cypress, the Cypress logo, and combinations thereof, WICED, ModusToolBox, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries.  For a more complete list of Cypress trademarks, visit cypress.com.  Other names and brands may be claimed as property of their respective owners.
