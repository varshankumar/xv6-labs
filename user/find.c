#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path[])
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path[2], O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path[2]);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path[2]);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  
  case T_FILE:
   if(strcmp(fmtname(buf), path[3])==0){
    fprintf("%s\n", fmtname(path[3]));
   }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      if(strcmp(fmtname(buf), path)==0){
      printf("%s\n", fmtname(buf));
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

for(int i=1; i<argc; i++){
    fprintf(1, "%s\n", argv[i]);
}

  if(argc < 3){
    exit(0);
  }
    ls(argv);
  exit(0);
}
