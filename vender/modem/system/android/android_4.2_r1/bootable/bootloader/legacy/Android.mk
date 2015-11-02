#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


# Most of these don't make sense to build unless the board has defined
# a link script.
common_dirs := libboot usb include libc arch_balongv7r2 nand usbloader nandwrite
ifeq ($(strip $(MBB_CHARGE)),FEATURE_ON)
common_dirs += charge
endif

ifeq ($(strip $(MBB_LED)),FEATURE_ON)
common_dirs += led_boot
endif

ifeq ($(MBB_REB),FEATURE_ON)
common_dirs += reb_boot
endif
ifdef TARGET_BOOTLOADER_LINK_SCRIPT
$(warning LINK_SCRIPT=$(TARGET_BOOTLOADER_LINK_SCRIPT))
  include $(call all-named-subdir-makefiles,$(common_dirs))
endif