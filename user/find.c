#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
char buf[1024];
// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9.

int matchhere(char *, char *);
int matchstar(int, char *, char *);

int match(char *re, char *text)
{
  if (re[0] == '^')
    return matchhere(re + 1, text);
  do
  { // must look at empty string
    if (matchhere(re, text))
      return 1;
  } while (*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if (re[0] == '\0')
    return 1;
  if (re[1] == '*')
    return matchstar(re[0], re + 2, text);
  if (re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if (*text != '\0' && (re[0] == '.' || re[0] == *text))
    return matchhere(re + 1, text + 1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do
  { // a * matches zero or more instances
    if (matchhere(re, text))
      return 1;
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}

void find(char *path, char *pattern)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
  {
    fprintf(2,"find: path too long\n");
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == 0)
      continue;
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
    {
      continue;
    }
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if (match(pattern, de.name))
    {
      fprintf(1,"%s\n", buf);
    }
    int sub_fd;
    struct stat new_st;
    if ((sub_fd = open(buf, 0)) < 0)
    {
      fprintf(2, "find: cannot open %s\n", buf);
      continue;
    }
    if (fstat(sub_fd, &new_st) < 0)
    {
      fprintf(2, "find: cannot stat %s\n", buf);
      close(sub_fd);
      continue;
    }
    close(sub_fd);
    if (new_st.type == T_DIR)
      find(buf, pattern);
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("find: failed\n");
    exit();
  }

  find(argv[1], argv[2]);
  exit();
}