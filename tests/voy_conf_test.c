#include "voy_unittest.h"
#include "voy_conf.h"

char *conf_file = "testdata/voy.conf";

char *test_conf_load()
{
    voy_conf_t *conf = voy_conf_load(conf_file);

    voy_assert(conf != NULL, "The conf file cannot be NULL");
    voy_assert(strcmp(conf->user->string, "www-data") == 0, "The main user in the conf file must be www-data");
    voy_assert(strcmp(conf->group->string, "www-data") == 0, "The main group in the conf file must be www-data");
    voy_assert(conf->default_server != NULL, "The default_server cannot be NULL");
    voy_assert(conf->vhosts != NULL, "The vhosts cannot be NULL");
    voy_conf_free(conf);

    return NULL;
}

char *test_default_server()
{
    voy_conf_t *conf = voy_conf_load(conf_file);
    voy_server_conf_t *def_server = conf->default_server;

    voy_assert(strcmp(def_server->root->string, "/var/www/localhost") == 0, "The root strings are not the same");
    voy_assert(strcmp(def_server->error_log->string, "/etc/log/voy/errors.log") == 0, "The strings of the error log are not the same");
    voy_assert(strcmp(def_server->access_log->string, "/etc/log/voy/access.log") == 0, "The strings of the access log are not the same");
    voy_assert(def_server->names != NULL, "The server names cannot be NULL");

    voy_array_t *names = def_server->names;
    voy_assert(names->len == 1, "There should be just 1 name");

    voy_str_t *f_name = voy_array_get(names, 0);
    voy_assert(strcmp(f_name->string, "localhost") == 0, "The server name should be localhost");

    voy_array_t *ports = def_server->ports;
    voy_assert(ports != NULL, "The server ports cannot be NULL");
    voy_assert(ports->len == 2, "There should be 2 port numbers");

    VOY_ARRAY_FOREACH(ports) {
        int *p = voy_array_get(ports, i);
        voy_assert(p != NULL, "The current port cannot be NULL");
        voy_assert((*p == 80 || *p == 443), "The port number must be 80 or 443");
    }

    voy_assert(def_server->error_pages != NULL, "The error pages cannot be NULL");
    voy_assert(def_server->access_log != NULL, "The access log cannot be NULL");
    voy_conf_free(conf);

    return NULL;
}

char *test_virtual_hosts()
{
    voy_conf_t *conf = voy_conf_load(conf_file);
    voy_array_t *vhosts = conf->vhosts;

    voy_assert(vhosts != NULL, "The vhosts cannot be NULL");
    voy_assert(vhosts->len == 2, "There should be 2 virtual hosts");
    voy_server_conf_t *mydom_server = voy_array_get(vhosts, 0);
    voy_server_conf_t *another_server = voy_array_get(vhosts, 1);

    voy_assert(strcmp(mydom_server->root->string, "/var/www/mydomain.com/public") == 0, "The root strings are not the same");
    voy_assert(strcmp(mydom_server->error_log->string, "/etc/log/voy/mydomain.com.log") == 0, "The strings of the error log are not the same");
    voy_assert(strcmp(mydom_server->access_log->string, "/etc/log/voy/mydomain.com.log") == 0, "The strings of the access log are not the same");
    voy_assert(mydom_server->names != NULL, "The server names cannot be NULL");

    voy_array_t *names = mydom_server->names;
    voy_assert(names->len == 2, "There should be 2 names");

    voy_str_t *f_name = voy_array_get(names, 0);
    voy_str_t *s_name = voy_array_get(names, 1);

    voy_assert(f_name != NULL, "The first name should not be NULL");
    voy_assert(s_name != NULL, "The second name should not be NULL");
    voy_assert(strcmp(f_name->string, "mydomain.com") == 0, "The server name should be mydomain.com");
    voy_assert(strcmp(s_name->string, "www.mydomain.com") == 0, "The server name should be www.mydomain.com");

    voy_array_t *ports = mydom_server->ports;
    voy_assert(ports != NULL, "The server ports cannot be NULL");
    voy_assert(ports->len == 1, "There should be 1 port number");

    VOY_ARRAY_FOREACH(ports) {
        int *p = voy_array_get(ports, i);
        voy_assert(p != NULL, "The current port cannot be NULL");
        voy_assert(*p == 8081, "The port number must be 8081");
    }

    voy_assert(mydom_server->error_pages != NULL, "The error pages cannot be NULL");
    voy_assert(mydom_server->access_log != NULL, "The access log cannot be NULL");
 
    voy_assert(strcmp(another_server->root->string, "/var/www/another.com/public") == 0, "The root strings are not the same");
    voy_assert(strcmp(another_server->error_log->string, "/etc/log/voy/another.com.log") == 0, "The strings of the error log are not the same");
    voy_assert(strcmp(another_server->access_log->string, "/etc/log/voy/another.com.log") == 0, "The strings of the access log are not the same");
    voy_assert(another_server->names != NULL, "The server names cannot be NULL");

    names = another_server->names;
    voy_assert(names->len == 1, "There should be 1 name");

    f_name = voy_array_get(names, 0);

    voy_assert(f_name != NULL, "The first name should not be NULL");
    voy_assert(strcmp(f_name->string, "another.com") == 0, "The server name should be another.com");

    ports = another_server->ports;
    voy_assert(ports != NULL, "The server ports cannot be NULL");
    voy_assert(ports->len == 1, "There should be 1 port number");

    VOY_ARRAY_FOREACH(ports) {
        int *p = voy_array_get(ports, i);
        voy_assert(p != NULL, "The current port cannot be NULL");
        voy_assert(*p == 80, "The port number must be 80");
    }

    voy_assert(another_server->error_pages != NULL, "The error pages cannot be NULL");
    voy_assert(another_server->access_log != NULL, "The access log cannot be NULL");
   
    voy_conf_free(conf);

    return NULL;
}

int main()
{
    voy_start_tests("voy conf tests");
    voy_run_test(test_conf_load);
    voy_run_test(test_default_server);
    voy_run_test(test_virtual_hosts);
    voy_end_tests();
    return 0;
}
