LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sensors.$(TARGET_BOARD_PLATFORM)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\" 
LOCAL_SRC_FILES :=  sensor_cfg.c \
					sensorbase.c \
					sensors.c  \
					mxc622x.c \
					lis3de.c

LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libutils
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
