#include <linux/input.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lib/powermate.h"

int abs_offset = 0;
int percentage_offset = 60;
bool on = false;
char rotate_command[256];

void process_event(struct input_event *ev, int powermate)
{
#ifdef VERBOSE
  fprintf(stderr, "type=0x%04x, code=0x%04x, value=%d\n",
          ev->type, ev->code, (int)ev->value);
#endif

  switch (ev->type) {
    case EV_MSC:
      printf("The LED pulse settings were changed; code=0x%04x, value=0x%08x\n", ev->code, ev->value);
      break;
    case EV_REL:
      if (ev->code != REL_DIAL)
        fprintf(stderr, "Warning: unexpected rotation event; ev->code = 0x%04x\n", ev->code);
      else {
        abs_offset += (int)ev->value;
        percentage_offset += (int)ev->value;
        if (percentage_offset > 100)
          percentage_offset = 100;

        if (percentage_offset < 0)
          percentage_offset = 0;

        printf(
          "Button was rotated %d units; Offset from start is now %d units\nPercentage is now %d",
          (int)ev->value,
          abs_offset,
          percentage_offset
        );

        snprintf(rotate_command, sizeof rotate_command, "./hooks/rotate.sh %d", percentage_offset);
        system(rotate_command);
      }
      break;
    case EV_KEY:
      if (ev->code != BTN_0)
        fprintf(stderr, "Warning: unexpected key event; ev->code = 0x%04x\n", ev->code);
      else {
        printf("Button was %s\n", ev->value? "pressed":"released");

        if (!ev->value) {
          on = !on;

          if (on) {
            printf("Pressed on\n");
            system("./hooks/press-on.sh &");
            set_led(powermate, 255, 255, 0, 1, 0);
          } else {
            printf("Pressed off\n");
            system("./hooks/press-off.sh &");
            set_led(powermate, 0, 255, 0, 1, 0);
          }
        }
      }
      break;
    default:
      fprintf(stderr, "Warning: unexpected event type; ev->type = 0x%04x\n", ev->type);
  }

  fflush(stdout);
}

#define BUFFER_SIZE 32
void watch_powermate(int fd)
{
  struct input_event ibuffer[BUFFER_SIZE];
  int r, events, i;

  while (1) {
    r = read(fd, ibuffer, sizeof(struct input_event) * BUFFER_SIZE);
    if (r > 0) {
      events = r / sizeof(struct input_event);
      for (i=0; i<events; i++)
        process_event(&ibuffer[i], fd);
    } else {
      fprintf(stderr, "read() failed: %s\n", strerror(errno));
      return;
    }
  }
}

int main(int argc, char *argv[])
{
  int powermate = -1;

  if (argc == 1)
    powermate = find_powermate(O_RDWR);
  else
    powermate = open_powermate(argv[1], O_RDWR);

  if (powermate < 0) {
    fprintf(stderr, "Unable to locate powermate\n");
    fprintf(stderr, "Try: %s [device]\n", argv[0]);
    return 1;
  }

  set_led(powermate, 0, 255, 0, 1, 0);
  watch_powermate(powermate);

  close(powermate);

  return 0;
}
