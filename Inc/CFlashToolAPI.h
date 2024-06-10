#ifndef CFLASHTOOLAPI_H
#define CFLASHTOOLAPI_H

#include <QString>
#include "Flash.Mode.Struct.h"
#include "images_checker.h"

class CFlashToolAPI
{
public:
    static void cflashtool_env_set_log(logging_level_e level,
                                      const QString &log_path);

    static ZHRESULT cflashtool_startup(const QString &host_manifest_version);

    static void cflashtool_cleanup();

    static ZHRESULT cflashtool_scan_device(const QString &symbolic_clue,
                                          /*OUT*/ char *symbolic_link, int length,
                                          int timeout_ms,
                                          const struct callback_stop_t *cb);

    static HSESSION cflashtool_create_session();

    static ZHRESULT cflashtool_destroy_session(HSESSION hs);

    static ZHRESULT cflashtool_connect_device(HSESSION hs,
                                             const QString &symbolic_link,
                                             const QString &options,
                                             const QString &auth,
                                             const QString &cert,
                                             const struct callbacks_struct_t *callbacks);

    static ZHRESULT cflashtool_enter_mode(HSESSION hs,
                                         const QString &mode,
                                         const QString &flash_xml_file_path,
                                         const QString &ui_config_xml_string,
                                         const struct callback_transfer_t *callbacks);

    static ZHRESULT cflashtool_execute_command(HSESSION hs,
                                              const QString &xml,
                                              const struct callback_transfer_t *callbacks);

    static QString cflashtool_get_last_error_msg(HSESSION hs);

    //image checksum API
    static int cflashtool_images_checker_verify(const QString &scatter_path, const struct callback_transfer_t *callbacks);
};

#endif // CFLASHTOOLAPI_H
