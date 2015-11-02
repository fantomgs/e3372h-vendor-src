#
#build dhcp executable bin file
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#########################################
#LOCAL_CFLAGS := $(DFLAGS)
#LOCAL_CFLAGS += $(EC508_CFLAGS)

#LOCAL_C_INCLUDES := $(LOCAL_PATH)/

#LOCAL_SRC_FILES:= custom_fw_module.c

#LOCAL_MODULE:= libcustom_fw_module

#include $(BUILD_STATIC_LIBRARY)

#########################################
include $(CLEAR_VARS)

LOCAL_CFLAGS := $(DFLAGS)
LOCAL_CFLAGS += $(EC508_CFLAGS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/osip2/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/osipparser2/

LOCAL_STATIC_LIBRARIES :=  libosipparser2

LOCAL_SRC_FILES := ict_fsm.c ist_fsm.c nict_fsm.c nist_fsm.c ict.c ist.c nict.c nist.c
LOCAL_SRC_FILES += fsm_misc.c osip.c osip_transaction.c osip_event.c port_sema.c port_fifo.c
LOCAL_SRC_FILES += port_thread.c osip_dialog.c osip_negotiation.c port_condv.c osip_time.c

LOCAL_SHARED_LIBRARIES := 

LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= libosip2

include $(BUILD_STATIC_LIBRARY)
