#include "v4l.h"
#include <libgnomevfs/gnome-vfs.h>

//XXX: int add_rgb_text (guchar *, int, int, char *, char *, gboolean, gboolean);
//XXX: void remote_save (cam *);
//XXX: void save_thread (cam *);
int local_save (cam *, gchar* keyqualifier);

/*
 * To trigger an image capture on the main thread from any thread (such as the signal handler
 * or an interrupt) and thus avoid torn frames, simply set this to 1 and it will be caught on
 * the next time through the loop. If this is non-zero, then it means that there is a capture
 * in progress.
 */
extern
int offThreadCaptureTrigger;
