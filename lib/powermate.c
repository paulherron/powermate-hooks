#include <linux/input.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "powermate.h"

#define NUM_VALID_PREFIXES 2

static const char *valid_prefix[NUM_VALID_PREFIXES] = {
  "Griffin PowerMate",
  "Griffin SoundKnob"
};

#define NUM_EVENT_DEVICES 16

#ifndef MSC_PULSELED
/* this may not have made its way into the kernel headers yet ... */
#define MSC_PULSELED 0x01
#endif

int open_powermate(const char *dev, int mode)
{
  int fd = open(dev, mode);
  int i;
  char name[255];

  if (fd < 0) {
    fprintf(stderr, "Unable to open \"%s\": %s\n", dev, strerror(errno));
    return -1;
  }

  if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) {
    fprintf(stderr, "\"%s\": EVIOCGNAME failed: %s\n", dev, strerror(errno));
    close(fd);
    return -1;
  }

  // it's the correct device if the prefix matches what we expect it to be:
  for (i=0; i<NUM_VALID_PREFIXES; i++)
    if (!strncasecmp(name, valid_prefix[i], strlen(valid_prefix[i])))
      return fd;

  close(fd);
  return -1;
}

int find_powermate(int mode)
{
  char devname[256];
  int i, r;

  for (i=0; i<NUM_EVENT_DEVICES; i++) {
    sprintf(devname, "/dev/input/event%d", i);
    r = open_powermate(devname, mode);
    if (r >= 0)
      return r;
  }

  return -1;
}

void set_led(int fd, int static_brightness, int pulse_speed, int pulse_table, int pulse_asleep, int pulse_awake)
{
  printf("fd %d, static brightness is %d, pulse speed is %d, pulse table is %d, pulse asleep is %d, pulse awake is %d\n", fd, static_brightness, pulse_speed, pulse_table, pulse_asleep, pulse_awake);

  struct input_event ev;
  memset(&ev, 0, sizeof(struct input_event));
  
  static_brightness &= 0xFF;

  if (pulse_speed < 0)
    pulse_speed = 0;
  if (pulse_speed > 510)
    pulse_speed = 510;
  if (pulse_table < 0)
    pulse_table = 0;
  if (pulse_table > 2)
    pulse_table = 2;
  pulse_asleep = !!pulse_asleep;
  pulse_awake = !!pulse_awake;

  ev.type = EV_MSC;
  ev.code = MSC_PULSELED;
  ev.value = static_brightness | (pulse_speed << 8) | (pulse_table << 17) | (pulse_asleep << 19) | (pulse_awake << 20);

  printf("%d, %d, %d", ev.type, ev.code, ev.value);

  if (write(fd, &ev, sizeof(struct input_event)) != sizeof(struct input_event))
    fprintf(stderr, "write(): %s\n", strerror(errno));  
}
