#include <stdlib.h>

#include "parameters.h"

const char * JH_parameters_get_socket_name
(
   const struct JH_parameters param [const restrict static 1]
)
{
   return param->socket_name;
}

const char * JH_parameters_get_dest_socket_name
(
   const struct JH_parameters param [const restrict static 1]
)
{
   return param->dest_socket_name;
}
