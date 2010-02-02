#include <httpd.h>
#include <ap_mpm.h>
#include <http_config.h>
#include <mod_status.h>

/* Forward declarations (You got c++ in my c!) */
#include "mod_plate_io.h"

module AP_MODULE_DECLARE_DATA plate_module;

static void plate_register_hooks(apr_pool_t *p)
{
  int threaded;

  if (ap_mpm_query(AP_MPMQ_IS_THREADED, &threaded) != APR_SUCCESS) {
    fprintf(stderr, "mod_plate could not query mpm: we have a problem. Refusing to register!");
    return;
  }

  if (threaded) {
    fprintf(stderr, "Refusing to start mod_plate inside a threaded MPM. We don't know how our threads will interact with apache's.");
    return;
  }

  ap_hook_handler(mod_plate_handler, NULL, NULL, APR_HOOK_MIDDLE);
  ap_hook_child_init(mod_plate_child_init, NULL, NULL, APR_HOOK_MIDDLE);
  APR_OPTIONAL_HOOK(ap, status_hook, mod_plate_status, NULL, NULL, APR_HOOK_MIDDLE);
}

static const char* handle_ip(cmd_parms* cmd, void* null, const char* arg) {
  plate_config *cfg = get_plate_config_mutable(cmd->server);
  cfg->rabbit_ip = arg;
  ap_set_module_config(cmd->server->module_config, &plate_module, (void*)cfg);
  return NULL;
}

static const char* handle_rule(cmd_parms* cmd, void* cfg, const char* raw_level, const char* name) {
  // if an exception is thrown from inside this function (anywhere) it will crash at throw-time.

  int level;

  if (strcmp(raw_level, "ErrorMessage") == 0)
    level = 0;
  else if (strcmp(raw_level, "WarningMessage") == 0)
    level = 10;
  else if (strcmp(raw_level, "InfoMessage") == 0)
    level = 20;
  else if (strcmp(raw_level, "DebugMessage") == 0)
    level = 30;
  else if (strcmp(raw_level, "VerboseDebugMessage") == 0)
    level = 40;
  else if (strcmp(raw_level, "EveryMessage") == 0)
    level = 100;
  else if (strcmp(raw_level, "*") == 0)
    level = 100;
  else
    return "Illegal log level";

  plate_config *conf = get_plate_config_mutable(cmd->server);
  rule_entry *rule   = (rule_entry*)apr_array_push(conf->rules);

  if (!name)
    name = "plate.apache";

  rule->name  = name;
  rule->level = level;

  return NULL;
}

static const command_rec my_cmds[] = {
  AP_INIT_TAKE1("PlateRabbitMQIP", handle_ip,   NULL, RSRC_CONF, "The IP of the rabbitmq server"),
  AP_INIT_TAKE12("PlateLogRule",   handle_rule, NULL, RSRC_CONF, "A log rule to add to the vw::RuleSet"),
  { NULL }
};

static void* create_plate_config(apr_pool_t* p, server_rec* s) {
  plate_config* conf = (plate_config*)apr_pcalloc(p, sizeof(plate_config));
  conf->rabbit_ip = "127.0.0.1";
  conf->rules = apr_array_make(p, 8, sizeof(rule_entry));
  return conf;
}

plate_config* get_plate_config_mutable(server_rec* s) {
  return (plate_config*)ap_get_module_config(s->module_config, &plate_module);
}

const plate_config* get_plate_config(const server_rec* s) {
  return (const plate_config*)ap_get_module_config(s->module_config, &plate_module);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA plate_module = {
  STANDARD20_MODULE_STUFF, 
  NULL,                  /* create per-dir    config structures */
  NULL,                  /* merge  per-dir    config structures */
  create_plate_config,   /* create per-server config structures */
  NULL,                  /* merge  per-server config structures */
  my_cmds,               /* table of config file commands       */
  plate_register_hooks  /* register hooks                      */
};
