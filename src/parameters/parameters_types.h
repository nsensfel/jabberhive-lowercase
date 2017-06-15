#ifndef _JH_CLI_PARAMETERS_TYPES_H_
#define _JH_CLI_PARAMETERS_TYPES_H_

#include <stdlib.h>

#define JH_PARAMETERS_COUNT 2

#define JH_PARAMETERS_DEFAULT_MAIN_STORAGE_FILENAME "storage.txt"
#define JH_PARAMETERS_DEFAULT_TEMP_STORAGE_PREFIX    "/tmp/jabberhive-storage/storage_thread_"

struct JH_parameters
{
   /* JH **********************************************************************/
   const char * restrict socket_name;
   const char * restrict dest_socket_name;
};

#endif
