# Install script for directory: /Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Users/hanyi02/.espressif/tools/xtensa-esp32s3-elf/esp-12.2.0_20230208/xtensa-esp32s3-elf/bin/xtensa-esp32s3-elf-objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aes.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aria.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1write.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/base64.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/bignum.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/build_info.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/camellia.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ccm.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chacha20.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chachapoly.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/check_config.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cipher.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cmac.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/compat-2.x.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_psa.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/constant_time.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ctr_drbg.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/debug.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/des.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/dhm.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdh.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdsa.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecjpake.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecp.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/entropy.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/error.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/gcm.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hkdf.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hmac_drbg.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/legacy_or_psa.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/lms.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md5.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/net_sockets.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/nist_kw.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/oid.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pem.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pk.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs12.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs5.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs7.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_time.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_util.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/poly1305.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/private_access.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/psa_util.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ripemd160.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/rsa.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha1.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha256.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha512.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cache.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cookie.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ticket.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/threading.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/timing.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/version.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crl.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crt.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_csr.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_composites.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_primitives.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_compat.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_config.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_common.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_composites.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_primitives.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_extra.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_platform.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_se_driver.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_sizes.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_struct.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_types.h"
    "/Users/hanyi02/Documents/My_Project/MCU/esp/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_values.h"
    )
endif()

