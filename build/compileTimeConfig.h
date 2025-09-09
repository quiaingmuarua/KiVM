#pragma once

#ifdef HAVE_CONFIG_H

#define HAVE_ZIP_FLAGS_T 1
#define HAVE_ZIP_SOURCE_T 1
#define HAVE_ZIP_FL_ENC_GUESS 1
#define HAVE_ZIP_H 1
#define KIVM_THREADED 0
#define KIVM_ARCH_x86 0
#define KIVM_ARCH_x86_64 1
#define KIVM_ARCH_arm 0
#define KIVM_ARCH_aarch64 0

#define KIVM_ARCH_NAME L"x86_64"

#if KIVM_ARCH_x86_64 || KIVM_ARCH_aarch64
#define KIVM_ARCH_DATA_MODEL L"64"
#else
#define KIVM_ARCH_DATA_MODEL L"32"
#endif

#if HAVE_ZIP_H
#define KIVM_JAR_CLASS_LOADING
#if HAVE_ZIP_SOURCE_T
#define KIVM_ZIP_OPEN_SOURCE
#endif
#endif

#if KIVM_ARCH_x86_64 || KIVM_ARCH_x86
#define KIVM_SUPPORT_CS8
#endif

#endif
