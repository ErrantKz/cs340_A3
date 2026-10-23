#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

static uint64
du(char *path, int show_file)
{
  struct dirent de;
  struct stat st;
  char buf[512], *p;
  char name[DIRSIZ + 1];
  int fd;
  uint64 total;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "du: cannot open %s\n", path);
    return 0;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "du: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  if(st.type != T_DIR){
    total = st.size;
    close(fd);
    if(show_file)
      printf("%d\t%s\n", (int)total, path);
    return total;
  }

  total = st.size;

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
    fprintf(2, "du: path too long\n");
    close(fd);
    return 0;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  if(p > buf && *(p - 1) != '/'){
    *p++ = '/';
  }

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;

    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = '\0';

    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;

    int len = strlen(name);
    memmove(p, name, len);
    p[len] = '\0';

    total += du(buf, 0);
  }

  close(fd);

  printf("%d\t%s\n", (int)total, path);
  return total;
}

int
main(int argc, char *argv[])
{
  if(argc != 2){
        fprintf(2, "Usage: du [PATH]\n");
        exit(1);
    }

    du(argv[1], 1);
    exit(0);
}