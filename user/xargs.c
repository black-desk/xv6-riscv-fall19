#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
  void *buf = malloc(MAXPATH * MAXARG);
  while (read(0, buf, MAXPATH * MAXARG) > 0)
  {
    for (char *ptr = buf; *ptr;)
    {
      char **argv_pass = malloc(sizeof(char *) * MAXARG);
      memmove(argv_pass, argv + 1, (argc - 1) * sizeof(char *));
      int argc_pass = argc - 1;
      argv_pass[argc_pass++] = ptr;
      for (; *ptr != '\n'; ptr++)
      {
        if (*ptr == ' ')
        {
          argv_pass[argc_pass++] = ptr + 1;
          *ptr = '\0';
        }
      }
      *ptr++ = '\0';
      if (fork())
      {
        wait();
      }
      else
      {
        exec(argv[1], argv_pass);
      }
      free(argv_pass);
    }
    memset(buf, 0, MAXPATH * MAXARG);
  }
  exit();
}
