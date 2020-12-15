LOCAL_PATH := $(call my-dir)

OLD_BUILD := 0	# gtasa gtavc gtalcs gta3 dadNMe
BLEACH_BVN_BUILD :=0 # bleach.bvn

ifeq ($(OLD_BUILD),1)
include $(LOCAL_PATH)/old_Android.mk
endif

#--------------------------------------------------------
ifeq ($(BLEACH_BVN_BUILD),1)
include $(LOCAL_PATH)/bleachBvn_Android.mk
endif
