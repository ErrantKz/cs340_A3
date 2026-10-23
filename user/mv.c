#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "Usage: mv [CUR NAME] [NEW NAME]\n");
    exit(1);
  }

  if(link(argv[1], argv[2]) < 0){
    fprintf(2, "mv: link %s to %s failed\n", argv[1], argv[2]);
    exit(1);
  }

  if(unlink(argv[1]) < 0){
    fprintf(2, "mv: unlink %s failed\n", argv[1]);
    exit(1);
  }

  exit(0);
}