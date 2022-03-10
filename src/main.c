#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define PATHNAME "/sys/class/leds/charging/brightness"
#define BUF_LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

static void handle_event() {
  FILE *file = fopen(PATHNAME, "r");
  int num = -1;
  if (!file) {
    fprintf(stderr, "cannot open file for read");
    return;
  }
  fscanf(file, "%d", &num);
  fclose(file);
  if (num != 0) {
    file = fopen(PATHNAME, "w");
    if (!file) {
      fprintf(stderr, "cannot open file for write");
      return;
    }
    fputs("0", file);
    fclose(file);
  }
}

int main() {
  int fd = inotify_init(), wd;
  char buf[BUF_LEN];
  ssize_t numRead;
  char *p;
  struct inotify_event *event;

  if (fd == -1) {
    fprintf(stderr, "inotify_init returned -1");
    exit(1);
  }
  wd = inotify_add_watch(fd, PATHNAME, IN_CLOSE_WRITE);
  if (wd == -1) {
    fprintf(stderr, "inotify_add_watch returned -1");
    exit(1);
  }

  for (;;) {
    numRead = read(fd, buf, BUF_LEN);
    if (numRead == 0 || numRead == -1) {
      fprintf(stderr, "failed to read fd");
      exit(1);
    }
    for (p = buf; p < buf + numRead;) {
      event = (struct inotify_event *)p;
      handle_event();
      p += sizeof(struct inotify_event) + event->len;
    }
  }
}