#pragma once

#if defined(_WIN64) || defined(_WIN32)
#if defined(FLASHMODEAPI_EXPORTS)
#define LIBEX_EXPORT __declspec(dllexport)
#else
#define LIBEX_EXPORT __declspec(dllimport)
#endif

#define LIB_API __cdecl
#elif defined(_LINUX64)
#if defined(FLASHMODEAPI_EXPORTS)
#define LIBEX_EXPORT __attribute__((visibility("default")))
#else
#define LIBEX_EXPORT
#endif
#define LIB_API
#else
#error "win64 or linux64 platform is not specified."
#endif

#include "Flash.Mode.Struct.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Effects: set the DLL global logging level.
//! there are 6 levels, default is level 3: kinfo level.
//!
//! Parameters: level: log level enum.
//!             log_path: set log folder path.
//! Returns:  none.
LIBEX_EXPORT void LIB_API flashtool_env_set_log(logging_level_e level,
                                                const char *log_path);

//! Effects: Setup DLL runtime environment.
//!
//! Parameters: host_manifest_version: reserved, currently be nullptr
//!
//! Returns:  E_OK if success, or other status indicate error.
LIBEX_EXPORT ZHRESULT LIB_API
flashtool_startup(const char *host_manifest_version);

//! Effects: Cleanup DLL environment. This must be called.
//!
//! Parameters: none
//!
//! Returns:  none.
LIBEX_EXPORT void LIB_API flashtool_cleanup();

//! Effects: wait for device and get the device port name.
//! When the correct device connecting PC with USB via virtual COM or other path
//! like PCIE, the function will return, or it will block until timeout.
//!
//! Parameters: symbolic_clue: the transport type and clue for device
//! searching. "COM://" or "COM://COM1" or "PCIE://"
//!            symbolic_link: the device port symbolic link name returned in
//!            here. 
//!            length: the symbolic link buffer length. preferred 512.
//!            timeout: the searching timeout in millisecond.
//!            cb: callbacks can stop the searching process.
//!
//! Returns:  E_OK if success, or other status indicate specified
//! error.
LIBEX_EXPORT ZHRESULT LIB_API
flashtool_scan_device(const char *symbolic_clue,
                      /*OUT*/ char *symbolic_link, int length, int timeout_ms,
                      const struct callback_stop_t *cb);

//! Effects: the session of unique device.
//! one connected device has single session, all operations must related with a
//! session.
//!
//! Parameters: none
//!
//! Returns:  E_OK if success, or other status indicate specified error.
//! 
LIBEX_EXPORT HSESSION LIB_API flashtool_create_session();

//! Effects: destroy the session.
//! If a device is disconnected, the session must be destroyed to release
//! resources.
//!
//! Parameters: hs: the session handle.
//!
//! Returns:  E_OK if success, or other status indicate specified
//! error.
LIBEX_EXPORT ZHRESULT LIB_API flashtool_destroy_session(HSESSION hs);

//! Effects: connect with BROM or loader.
//! handshake with loader, notify the loader to enter BROM download mode.
//!
//! Parameters: hs: the session handle.
//!   symbolic_link: the port name such as COM name with which to connect the
//!   device.  such as "COM://COM10" or returned string by flashtool_scan_device
//!   options : string format "baud-rate@others". like "115200"
//!   auth: the authentication file path,
//!      used to determine whether the tool having the right to connect loader.
//!         if the chip is set to ignore this secure policy, this parameter can
//!         be nullptr.
//!   cert: security switch file path. for special debug. this parameter can be nullptr
//!   callbacks: the callback functions used in brom operations.
//!
//! Returns:  E_OK if success, or other status indicate specified
//! error.
LIBEX_EXPORT ZHRESULT LIB_API flashtool_connect_device(
    HSESSION hs, const char *symbolic_link, const char *options,
    const char *auth, const char *cert,
    const struct callbacks_struct_t *callbacks);

//! Effects: sync with BROM.
//! notify the loader to enter specific flash mode.
//!
//!
//! Parameters: hs: the session handle.
//!   mode: set device into flash
//!         mode(FLASH-MODE-DA, FLASH-MODE-DA-SRAM). 
//!   config_xml: include necessary info. refer to specification document.
//!   callbacks: the callback functions used in brom
//!   operations.
//!
//! Returns:  E_OK if success, or other status indicate specified
//! error.
LIBEX_EXPORT ZHRESULT LIB_API flashtool_enter_mode(
    HSESSION hs, const char *mode, const char *flash_xml_file_path,
    const char *ui_config_xml_string,
    const struct callback_transfer_t *callbacks);

//! Effects: Execute command specified by xml.
//!
//! Parameters: hs: the session handle.
//!   xml: xml string contain formatted DA command. UTF-8 string.
//!
//!   length: max xml string length.
//!   callbacks: the callback functions used in device operations.
//!
//! Returns:  E_OK if success, or other status indicate specified
//! error.

LIBEX_EXPORT ZHRESULT LIB_API flashtool_execute_command(
    HSESSION hs, const char *xml, const struct callback_transfer_t *callbacks);

//! Effects: return the detailed error message.
//!
//! Parameters: hs: the session handle.
//!
//! Returns:  the error message string.
//! error.
LIBEX_EXPORT const char *LIB_API flashtool_get_last_error_msg(HSESSION hs);

#ifdef __cplusplus
}
#endif
