#include "CFlashToolAPI.h"
#include "Flash.Mode.API.h"

void CFlashToolAPI::cflashtool_env_set_log(logging_level_e level,
                                           const QString &log_path)
{
    flashtool_env_set_log(level, log_path.isEmpty() ? nullptr : log_path.toUtf8().data());
}

ZHRESULT CFlashToolAPI::cflashtool_startup(const QString &host_manifest_version)
{
    return flashtool_startup(host_manifest_version.isEmpty() ? nullptr : host_manifest_version.toUtf8().data());
}

void CFlashToolAPI::cflashtool_cleanup()
{
    flashtool_cleanup();
}

ZHRESULT CFlashToolAPI::cflashtool_scan_device(const QString &symbolic_clue,
                                               char *symbolic_link, int length,
                                               int timeout_ms,
                                               const callback_stop_t *cb)
{
    return flashtool_scan_device(symbolic_clue.isEmpty() ? nullptr : symbolic_clue.toUtf8().data(),
                          symbolic_link, length, timeout_ms, cb);
}

HSESSION CFlashToolAPI::cflashtool_create_session()
{
    return flashtool_create_session();
}

ZHRESULT CFlashToolAPI::cflashtool_destroy_session(HSESSION hs)
{
    return flashtool_destroy_session(hs);
}

ZHRESULT CFlashToolAPI::cflashtool_connect_device(HSESSION hs,
                                                  const QString &symbolic_link,
                                                  const QString &options,
                                                  const QString &auth,
                                                  const QString &cert,
                                                  const callbacks_struct_t *callbacks)
{
    return flashtool_connect_device(hs,
                                    symbolic_link.isEmpty() ? nullptr : symbolic_link.toUtf8().data(),
                                    options.isEmpty() ? nullptr : options.toUtf8().data(),
                                    auth.isEmpty() ? nullptr : auth.toUtf8().data(),
                                    cert.isEmpty() ? nullptr : cert.toUtf8().data(),
                                    callbacks);
}

ZHRESULT CFlashToolAPI::cflashtool_enter_mode(HSESSION hs,
                                              const QString &mode,
                                              const QString &flash_xml_file_path,
                                              const QString &ui_config_xml_string,
                                              const callback_transfer_t *callbacks)
{
    return flashtool_enter_mode(hs,
                                mode.isEmpty() ? nullptr : mode.toUtf8().data(),
                                flash_xml_file_path.isEmpty() ? nullptr : flash_xml_file_path.toUtf8().data(),
                                ui_config_xml_string.isEmpty() ? nullptr : ui_config_xml_string.toUtf8().data(),
                                callbacks);
}

ZHRESULT CFlashToolAPI::cflashtool_execute_command(HSESSION hs,
                                                   const QString &xml,
                                                   const callback_transfer_t *callbacks)
{
    return flashtool_execute_command(hs,
                                     xml.isEmpty() ? nullptr : xml.toUtf8().data(),
                                     callbacks);
}

QString CFlashToolAPI::cflashtool_get_last_error_msg(HSESSION hs)
{
    return flashtool_get_last_error_msg(hs);
}

/********************************************** image checksum API **************************************************************/
int CFlashToolAPI::cflashtool_images_checker_verify(const QString &scatter_path, const callback_transfer_t *callbacks)
{
    return flashtool_images_checker_verify(scatter_path.isEmpty() ? nullptr : scatter_path.toUtf8().data(), callbacks);
}
