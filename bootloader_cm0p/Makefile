################################################################################
# \file Makefile
# \version 1.0
#
# \brief
# Makefile for configurating the Cortex-M0+ bootloader project
#
################################################################################
# \copyright
# Copyright 2022-2024, Cypress Semiconductor Corporation (an Infineon company)
# SPDX-License-Identifier: Apache-2.0
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

include ../common.mk

################################################################################
# Basic Configuration
################################################################################
MTB_TYPE=PROJECT
# Name of application (used to derive name of final linked file).
#
# If APPNAME is edited, ensure to update or regenerate launch
# configurations for your IDE.
APPNAME=bootloader_cm0p

# If set to "true" or "1", display full command-lines when building.
VERBOSE=

# Build location.
CY_BUILD_LOCATION=./build/$(TRANSPORT_OPT)
CY_IGNORE=./build

################################################################################
# Advanced Configuration
################################################################################

# Enable optional code that is ordinarily disabled by default.
#
# Available components depend on the specific targeted hardware and firmware
# in use. In general, if you have
#
#    COMPONENTS=foo bar
#
# ... then code in directories named COMPONENT_foo and COMPONENT_bar will be
# added to the build
#
COMPONENTS=USBD_BASE

# Like COMPONENTS, but disable optional code that was enabled by default.
DISABLE_COMPONENTS+=CM0P_SECURE CM0P_CRYPTO CM0P_BLESS

# Set this application to run at the CM0+
CORE=CM0P
CORE_NAME=CM0P_0

# By default the build system automatically looks in the Makefile's directory
# tree for source code and builds it. The SOURCES variable can be used to
# manually add source code to the build process from a location not searched
# by default, or otherwise not found by the build system.
SOURCES=

# Like SOURCES, but for include directories. Value should be paths to
# directories (without a leading -I).
INCLUDES=

# Add additional defines to the build process (without a leading -D).
# Define appID for bootloader application
DEFINES+=CY_APP_ID=0 USBD_DISABLE_STANDARD_OUTPUT=1

# Defining the address for ram common region
RAM_COMMON_START=0x08000000
RAM_COMMON_LENGTH=0x0400

# adding the define to use the symbols in the sources
DEFINES+=RAM_COMMON_START=$(RAM_COMMON_START) \
RAM_COMMON_LENGTH=$(RAM_COMMON_LENGTH)

# Select softfp or hardfp floating point. Default is softfp.
VFP_SELECT=

# Additional / custom C compiler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
CFLAGS=

# Additional / custom C++ compiler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
CXXFLAGS=

# Additional / custom assembler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
ASFLAGS=

# Additional / custom linker flags.
ifeq (GCC_ARM, $(TOOLCHAIN))
LDFLAGS+=-Wl,--defsym=RAM_COMMON_ORIGIN=$(RAM_COMMON_START),--defsym=RAM_COMMON_SIZE=$(RAM_COMMON_LENGTH)
endif

# Additional / custom libraries to link in to the application.
LDLIBS=

# Path to the linker script to use (if empty, use the default linker script).
LINKER_SCRIPT=$(MTB_TOOLS__TARGET_DIR)/COMPONENT_$(MTB_RECIPE__CORE)/TOOLCHAIN_$(TOOLCHAIN)/dfu_cm0p_app0.$(MTB_RECIPE__SUFFIX_LS)

# Custom pre-build commands to run.
PREBUILD=

# Custom post-build commands to run.
POSTBUILD=

################################################################################
# Include additional configurations, source and headers.
################################################################################
#DFU_PATH=$(SEARCH_dfu)/config/CAT1A

ifeq ($(TRANSPORT_OPT), I2C)
    COMPONENTS+= DFU_I2C_PDL
    $(info Selected I2C transport for DFU.)
else ifeq ($(TRANSPORT_OPT), UART)
    COMPONENTS+= DFU_UART_PDL
    $(info Selected UART transport for DFU.)
else ifeq ($(TRANSPORT_OPT), SPI)
    COMPONENTS+= DFU_SPI_PDL
    $(info Selected SPI transport for DFU.)
else ifeq ($(TRANSPORT_OPT), EMUSB_CDC)
    COMPONENTS+= DFU_EMUSB_CDC
    CY_IGNORE+=./imports/dfu
    $(info Selected EMUSB_CDC transport for DFU.)    
else
    $(error Unknown DFU transport selected !.)
endif

# Transport opt should be in lowercase to pick the correct files for the build. 
TRANSPORT_OPT_LOWER_CASE=$(shell echo $(TRANSPORT_OPT) | tr A-Z a-z)

# Add transport sources and headers.
SOURCES+=../transport/transport_$(TRANSPORT_OPT_LOWER_CASE).c
INCLUDES+=../transport

################################################################################
# Paths
################################################################################

# Relative path to the project directory (default is the Makefile's directory).
#
# This controls where automatic source code discovery looks for code.
CY_APP_PATH=

# Relative path to the shared repo location.
#
# All .mtb files have the format, <URI>#<COMMIT>#<LOCATION>. If the <LOCATION> field 
# begins with $$ASSET_REPO$$, then the repo is deposited in the path specified by 
# the CY_GETLIBS_SHARED_PATH variable. The default location is one directory level 
# above the current app directory.
# This is used with CY_GETLIBS_SHARED_NAME variable, which specifies the directory name.
CY_GETLIBS_SHARED_PATH=../..

# Directory name of the shared repo location.
#
CY_GETLIBS_SHARED_NAME=mtb_shared

include $(CY_TOOLS_DIR)/make/start.mk