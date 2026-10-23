#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

uint64
du(char *path)
{
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512], *p;
  uint64 total_size = 0;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "du: cannot open %s\n", path);
    return 0;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "du: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  switch(st.type){
  case T_FILE:
  case T_DEVICE:
    total_size = st.size;
    printf("%d\t%s\n", (int)st.size, path);
    break;

  case T_DIR:
    total_size = st.size;

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

      char name[DIRSIZ + 1];
      int i;
      for(i = 0; i < DIRSIZ && de.name[i] != 0; i++){
        name[i] = de.name[i];
      }
      name[i] = 0;

      if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        continue;

      memmove(p, name, i);
      p[i] = 0;

      struct stat st_child;
      if(stat(buf, &st_child) < 0){
        continue;
      }

      if(st_child.type == T_DIR){
        total_size += du(buf);
      } else {
        total_size += st_child.size;
      }
    }
    printf("%d\t%s\n", (int)total_size, path);
    break;
  }

  close(fd);
  return total_size;
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    du(".");
  } else {
    for(int i = 1; i < argc; i++){
      du(argv[i]);
    }
  }
  exit(0);
}