#pragma once
#include "error_code.h"

#if defined(_WIN64) || defined(_WIN32)
#define CDECL_API __cdecl
#define STDCALL_API __stdcall
#elif defined(_LINUX64)
#define CDECL_API
#define STDCALL_API
#endif

enum class logging_level_e : unsigned int {
  ktrace = 0,
  kdebug,
  kinfo,
  kwarning,
  kerror,
  kfatal,
  koff
};

using CB_OPERATION_PROGRESS = void(CDECL_API *)(void *_this,
                                                unsigned int progress,
                                                unsigned long long data_xferd,
                                                const char *info);
using CB_NOTIFY_STOP = bool(CDECL_API *)(void *_this);

using CB_SLA_CHALLENGE = int(STDCALL_API *)(void *usr_arg,
                                          const unsigned char *p_challenge_in,
                                          unsigned int challenge_in_len,
                                          unsigned char **pp_challenge_out,
                                          unsigned int *p_challenge_out_len);
using CB_SLA_CHALLENGE_END = int(STDCALL_API *)(void *usr_arg,
                                              unsigned char *p_challenge_out);

struct sla_callbacks_t {
  CB_SLA_CHALLENGE cb_start;
  CB_SLA_CHALLENGE_END cb_end;
  void *start_user_arg;
  void *end_user_arg;
};

struct callback_stop_t {
  void *_this;
  CB_NOTIFY_STOP cb_notify_stop;
};

struct callback_transfer_t {
  void *_this;
  CB_NOTIFY_STOP cb_notify_stop;
  CB_OPERATION_PROGRESS cb_op_progress;
};

struct callbacks_struct_t {
  callback_transfer_t trans;
  sla_callbacks_t cb_sla;
};

struct zbuffer_info_t {
  unsigned char *data;
  unsigned int length;
};

using HSESSION = int;
#define INVALID_HSESSION_VALUE 0
