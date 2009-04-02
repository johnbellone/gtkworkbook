#include "application.h"

int 
main(int argc, char *argv[])
{
  ApplicationState *app = application_init (&argc, &argv);

  app->run (app);
  app->close (app);
  
  return 0; 
}


