#include"fileio.h"
#include<time.h>
#include<errno.h>
#include "support.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include<stdio.h>
# define CHAR_HEIGHT  11
# define CHAR_WIDTH   6
# define CHAR_START   4
# include "font_6x11.h"
#include <gnome.h>

int offThreadCaptureTrigger;

static int print_error (GnomeVFSResult result, const char *uri_string);

/* add timestamp/text to image - "borrowed" from gspy */
//XXX: int add_rgb_text (guchar *image, int width, int height, char *cstring, char *format, gboolean str, gboolean date);

//XXX: void remote_save (cam * cam);
//XXX: void save_thread (cam * cam); [seemed to only be doing remote saving via gvfs]

static int print_error (GnomeVFSResult result, const char *uri_string)
{
    const char *error_string;
    /* get the string corresponding to this GnomeVFSResult value */
    error_string = gnome_vfs_result_to_string (result);
    printf ("Error %s occurred opening location %s\n", error_string,
            uri_string);
    return 1;
}

int local_save (cam * cam)
{
	if (cam->debug == TRUE)
	{
		fprintf(stderr,"local_save()\n");
	}

    int fc;
    gchar *filename, *ext;
    time_t t;
    struct tm *tm;
    char timenow[64], *error_message;
    int len, mkd;
    gboolean pbs;
    GdkPixbuf *pb;

    /* todo:
     *   run gdk-pixbuf-query-loaders to get available image types*/

    switch (cam->savetype) {
    case JPEG:
        ext = g_strdup ((gchar *) "jpeg");
        break;
    case PNG:
        ext = g_strdup ((gchar *) "png");
        break;
    default:
        ext = g_strdup ((gchar *) "jpeg");
    }
    //cam->tmp = NULL;
    //memcpy (cam->tmp, cam->pic_buf, cam->x * cam->y * cam->depth);

    time (&t);
    tm = localtime (&t);
    len = strftime (timenow, sizeof (timenow) - 1, "%s", tm);

    if (cam->debug == TRUE) {
        fprintf (stderr, "time = %s\n", timenow);
    }

    if (cam->timefn == TRUE) {
        filename =
            g_strdup_printf ("%s-%s.%s", cam->capturefile, timenow, ext);
    } else {
        filename = g_strdup_printf ("%s.%s", cam->capturefile, ext);
    }

    if (cam->debug == TRUE) {
        fprintf (stderr, "filename = %s\n", filename);
    }
    mkd = mkdir (cam->pixdir, 0777);

    if (cam->debug == TRUE) {
        perror ("create dir: ");
    }

    if (mkd != 0 && errno != EEXIST) {
        error_message =
            g_strdup_printf (_
                             ("Could not create directory '%s'."),
                             cam->pixdir);
        error_dialog (error_message);
        g_free (filename);
        g_free (error_message);
        return -1;
    }

    if (chdir (cam->pixdir) != 0) {
        error_message =
            g_strdup_printf (_
                             ("Could not change to directory '%s'."),
                             cam->pixdir);
        error_dialog (error_message);
        g_free (filename);
        g_free (error_message);
        return -1;
    }

    if (cam->debug == TRUE)
	{
		fprintf(stderr, "gdk_pixbuf_new_from_data\n");
	}

    pb = gdk_pixbuf_new_from_data (cam->tmp, GDK_COLORSPACE_RGB, FALSE, 8,
                                   cam->x, cam->y,
                                   (cam->x * cam->vid_pic.depth / 8), NULL,
                                   NULL);
    if (cam->debug == TRUE)
	{
		fprintf(stderr, "gdk_pixbuf_save\n");
	}

	//NB: This method is SLOW...
	pbs = gdk_pixbuf_save (pb, filename, ext, NULL, NULL);

     if (pbs == FALSE) {
        error_message =
            g_strdup_printf (_("Could not save image '%s/%s'."),
                             cam->pixdir, filename);
        error_dialog (error_message);
        g_free (filename);
        g_free (error_message);
        return -1;
    }

    fprintf(stdout, "%s/%s\n", cam->pixdir, filename);
	fflush(stdout);

    g_free (filename);
    return 0;
}
