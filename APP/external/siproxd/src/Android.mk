#
#build dhcp executable bin file
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#########################################
LOCAL_CFLAGS := $(DFLAGS)
LOCAL_CFLAGS += $(EC508_CFLAGS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/
LOCAL_STATIC_LIBRARIES :=  libosipparser2 libosip2 
LOCAL_SRC_FILES:= custom_fw_module.c

LOCAL_MODULE:= libcustom_fw_module

include $(BUILD_STATIC_LIBRARY)

#########################################
include $(CLEAR_VARS)

AM_CFLAGS := -Wall -D_GNU_SOURCE -DBUILDSTR="\"`cat .buildno`\""

LOCAL_CFLAGS := $(DFLAGS)
LOCAL_CFLAGS += $(EC508_CFLAGS)
LOCAL_CFLAGS += $(AM_CFLAGS)
LOCAL_CFLAGS += $(APP_DEFINE)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libosip2/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libosip2/include/osip2/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libosip2/include/osipparser2/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libosip2/include/osipparser2/headers/
ifneq ($(findstring MBB_FEATURE_EUAP, $(APP_DEFINE)),)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/netplatforminc 
else
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../netplatforminc 
endif
LOCAL_SRC_FILES := siproxd.c proxy.c register.c sock.c utils.c sip_utils.c log.c readconf.c rtpproxy.c 
LOCAL_SRC_FILES += rtpproxy_relay.c accessctl.c route_processing.c security.c auth.c fwapi.c 

LOCAL_STATIC_LIBRARIES :=  libosipparser2 libosip2 libcustom_fw_module

LOCAL_SHARED_LIBRARIES := 

LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= siproxd

include $(BUILD_EXECUTABLE)
