ifeq ($(call is-vendor-board-platform,QCOM),true)
#
# mm-qcamera-daemon
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_LDFLAGS := $(mmcamera_debug_lflags)

LOCAL_CFLAGS := -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags) \
  -DMSM_CAMERA_BIONIC

LOCAL_CFLAGS  += -Werror
LOCAL_CFLAGS  += -D_ANDROID_
LOCAL_CFLAGS += -DMEMLEAK_FLAG
LOCAL_CFLAGS  += -DFDLEAK_FLAG

LOCAL_SRC_FILES:= ii.c \


LOCAL_MODULE:= ii
LOCAL_MODULE_TAGS := optional

ifeq ($(32_BIT_FLAG), true)
LOCAL_32_BIT_ONLY := true
endif #32_BIT_FLAG

include $(BUILD_EXECUTABLE)

endif #is-vendor-board-platform
