#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define PATHNAME "/sys/class/leds/charging/brightness"
#define BUF_LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

static void handle_event(void) {
  FILE *file = fopen(PATHNAME, "r");
  int num;
  if (!file) {
    fputs("failed to open file for read\n", stderr);
    return;
  }
  if (fscanf(file, "%d", &num) != 1)
    num = -1;
  fclose(file);
  if (num != 0) {
    file = fopen(PATHNAME, "w");
    if (!file) {
      fputs("failed to open file for write\n", stderr);
      return;
    }
    fputs("0", file);
    fclose(file);
  }
}

int main(void) {
  int fd, wd;
  char buf[BUF_LEN];
  ssize_t num_read;
  char *p;
  struct inotify_event *event;

  fd = inotify_init();
  if (fd == -1) {
    fputs("inotify_init returned -1\n", stderr);
    exit(1);
  }
  wd = inotify_add_watch(fd, PATHNAME, IN_CLOSE_WRITE);
  if (wd == -1) {
    fputs("inotify_add_watch returned -1\n", stderr);
    exit(1);
  }

  for (;;) {
    num_read = read(fd, buf, BUF_LEN);
    if (num_read == 0 || num_read == -1) {
      fputs("failed to read from fd\n", stderr);
      exit(1);
    }
    for (p = buf; p < buf + num_read;) {
      event = (struct inotify_event *)p;
      handle_event();
      p += sizeof(struct inotify_event) + event->len;
    }
  }
}
