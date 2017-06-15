#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "../core/index.h"
#include "../error/error.h"

#include "parameters.h"

static void set_default_to_all_fields
(
   struct JH_parameters param [const restrict static 1]
)
{
   param->socket_name = (const char *) NULL;
   param->dest_socket_name = (const char *) NULL;
}

static int is_valid
(
   struct JH_parameters param [const restrict static 1]
)
{
   int valid;

   valid = 1;

   if (param->socket_name == (const char *) NULL)
   {
      JH_S_FATAL(stderr, "Missing parameter: This entity's socket name.");

      valid = 0;
   }

   if (param->dest_socket_name == (const char *) NULL)
   {
      JH_S_FATAL(stderr, "Missing parameter: The destination's socket name.");

      valid = 0;
   }

   return valid;
}

static void set_parameters
(
   struct JH_parameters param [const restrict static 1],
   int const argc,
   const char * argv [const static argc]
)
{
   if (argc < 2)
   {
      return;
   }

   param->socket_name = argv[1];

   if (argc < 3)
   {
      return;
   }

   param->dest_socket_name = argv[2];

}

int JH_parameters_initialize
(
   struct JH_parameters param [const restrict static 1],
   int const argc,
   const char * argv [const static argc]
)
{
   set_default_to_all_fields(param);

   set_parameters(param, argc, argv);

   if (!is_valid(param))
   {
      return -1;
   }

   return 0;
}
