#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>

#include "Timer.h"

#define USAGE printf("Usage: cp src dst [flag]\n")

int main(int argc, char** argv) {
  Timer t1;
  Timer t2;
  Timer t3;

  int meth = 3;

  // proper # of args
  if (argc < 3 || argc > 4) {
    USAGE;
    exit(1);
  }
  bool test = (argc == 4) ? strcmp(argv[3], "--test") == 0 : false;
  for(int i = 3; i < argc; ++i) {
    if (strlen(argv[i]) >= 2 && argv[i][0] == '-') {
      argv[i][2] = 0;
      meth = atoi(argv[i] + 1);
    } else {
      USAGE;
      exit(1);
    }
  }

  struct stat a1;
  struct stat a2;

  if (stat(argv[1], &a1) == -1) {
    perror(argv[1]);
    exit(1);
  }

  if (stat(argv[2], &a2) != -1) {
    USAGE;
    printf("Usage: dst cannot exist or be a directory\n");
    exit(1);
  }

  errno = 0;

  if (S_ISDIR(a1.st_mode)) {
    USAGE;
    printf("Usage: src cannot be a directory\n");
    exit(1);
  }


  if (test) {
    goto tst;
ret:
    exit(0);
  }


    
  switch (meth) {
    case 1:    // istream/ostream
    {
tst:
      if (test) {
        t1.start();
      }
      std::ifstream fin(argv[1], std::ios::binary);
      if (!fin.good()) {
        printf("Error opening %s\n", argv[1]);
        exit(1);
      }

      std::ofstream fout(argv[2], std::ios::out);
      if (!fout.is_open()) {
        printf("Fatal error: can't open %s\n", argv[2]);
        exit(1);
      }

      char c = fin.get();
      while (fin.good()) {
        fout.put(c);
        c = fin.get();
      }
    }

    if (test) {
      double etime;
      printf("C++ istream:\n");
      t1.elapsedWallclockTime(etime);
      printf("Wall clock: %f\n", etime);
      t1.elapsedUserTime(etime);
      printf("User clock: %f\n", etime);
      t1.elapsedSystemTime(etime);
      printf("System clock: %f\n", etime);
    } else {
      break;
    }

    case 2:    // read/write 1 char at a time
    {
      if (test) {
        t2.start();
      }
      int fd1 = open(argv[1], O_RDONLY);
      if(fd1 == -1) {
        perror(argv[1]);
        exit(1);
      }
      int fd2 = creat(argv[2], a1.st_mode);
      if(fd2 == -1) {
        perror(argv[2]);
        exit(1);
      }

      char c;
      int rret = read(fd1, &c, 1);

      while (rret) {
        if (write(fd2, &c, rret) == -1) {
          perror("write");
          exit(1);
        }
        rret = read(fd1, &c, 1);
      }
      if (close(fd1) == -1) {
        perror("close fd1");
        exit(1);
      }
      if (close(fd2) == -1) {
        perror("close fd2");
        exit(1);
      }
    }

    if (test) {
      double etime;
      printf("\nread/write 1 char at a time:\n");
      t2.elapsedWallclockTime(etime);
      printf("Wall clock: %f\n", etime);
      t2.elapsedUserTime(etime);
      printf("User clock: %f\n", etime);
      t2.elapsedSystemTime(etime);
      printf("System clock: %f\n", etime);
    } else {
      break;
    }

    case 3:    // read/write BUFSIZ at a time
    {
      if (test) {
        t3.start();
      }
      int fd1 = open(argv[1], O_RDONLY);
      if(fd1 == -1) {
        perror(argv[1]);
        exit(1);
      }
      int fd2 = creat(argv[2], a1.st_mode);
      if(fd2 == -1) {
        perror(argv[2]);
        exit(1);
      }

      char c[BUFSIZ];
      int rret = read(fd1, &c, BUFSIZ);

      while (rret) {
        if (write(fd2, &c, rret) == -1) {
          perror("write");
          exit(1);
        }
        rret = read(fd1, &c, BUFSIZ);
      }
      if (close(fd1) == -1) {
        perror("close fd1");
        exit(1);
      }
      if (close(fd2) == -1) {
        perror("close fd2");
        exit(1);
      }
    }
    if (test) {
      double etime;
      printf("\nread/write BUFSIZ chars at a time:\n");
      t3.elapsedWallclockTime(etime);
      printf("Wall clock: %f\n", etime);
      t3.elapsedUserTime(etime);
      printf("User clock: %f\n", etime);
      t3.elapsedSystemTime(etime);
      printf("System clock: %f\n", etime);
      goto ret;
    } else {
      break;
    }

    default:
      printf("No.\n");
      exit(1);
  }

  return 0;
  perror("fixed! :D");
  perror("fixed! :D");
  perror("fixed! :D");
  perror("fixed! :D");
}

