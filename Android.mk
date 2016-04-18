LOCAL_PATH := $(call my-dir)

# import files
INIT_FILES := $(wildcard $(LOCAL_PATH)/init/*.c)
USB_FILES := $(wildcard $(LOCAL_PATH)/usb/*.c)
USER_FILES := $(wildcard $(LOCAL_PATH)/user/*.c)
CRYPTO_FILES := $(wildcard $(LOCAL_PATH)/crypto/*.c)

include $(CLEAR_VARS)
LOCAL_MODULE := libtransencrypt
LOCAL_CFLAGS := -std=gnu99 -DADB_LOG=1
LOCAL_SDK_VERSION := 9
LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES := \
	$(INIT_FILES:$(LOCAL_PATH)/%=%) 	\
	$(USB_FILES:$(LOCAL_PATH)/%=%)		\
	$(USER_FILES:$(LOCAL_PATH)/%=%) 	\
	$(CRYPTO_FILES:$(LOCAL_PATH)/%=%) 	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include		\
	$(LOCAL_PATH)/init			\
	system/core/include 		\
	external/openssl/include
LOCAL_SHARED_LIBRARIES := \
	libdl 		\
	liblog 		\
	libcutils 	\
	libcrypto
include $(BUILD_SHARED_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := cmccd
# LOCAL_SRC_FILES := $(common_DAEMON_FILES:$(LOCAL_PATH)/%=%)
# LOCAL_C_INCLUDES := \
# 	system/core/include 		\
# 	external/openssl/include
# LOCAL_CFLAGS := -Wall -Wno-unused-parameter -Werror -DADB_LOG=1
# LOCAL_SHARED_LIBRARIES := libc libcutils
# include $(BUILD_EXECUTABLE)

# import files
# TEST_FILES := $(wildcard $(LOCAL_PATH)/test/*.c)
#
# include $(CLEAR_VARS)
# LOCAL_MODULE := cmccd_test
# LOCAL_SRC_FILES := $(TEST_FILES:$(LOCAL_PATH)/%=%)
# LOCAL_SHARED_LIBRARIES := libtransencrypt
# include $(BUILD_EXECUTABLE)
