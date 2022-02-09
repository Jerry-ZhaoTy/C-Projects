#include "types.h"
#include "iostat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int m, n;
  int rc;
  struct iostat iostat;

  if(argc != 3){
    printf(1, "usage: sysios m n\n");
    exit();
  }

  m = atoi(argv[1]);
  n = atoi(argv[2]);

  for (int i = 0; i < m; i++)
    read(-1, (char*) 0, 0);
  for (int j = 0; j < n; j++)
    write(-1, (char*) 0, 0);

  rc = getiocounts(&iostat);
  printf(1, "%d %d %d\n", rc, iostat.readcount, iostat.writecount);

  exit();
}
