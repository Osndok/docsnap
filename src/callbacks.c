#include <gtk/gtk.h>
#include <config.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include <gnome.h>
#include <libgnomeui/gnome-about.h>
#include <libgnomeui/gnome-propertybox.h>
#include <libgnomeui/gnome-window-icon.h>
#include <pthread.h>
#include <libv4l1.h>

extern GtkWidget *main_window, *prefswindow;
//extern state func_state;
//extern gint effect_mask;
extern int frames;
extern int frames2;
extern int seconds;
extern GtkWidget *dentry, *entry2, *string_entry;
extern GtkWidget *host_entry,
    *directory_entry, *filename_entry, *login_entry, *pw_entry;
int frame;

/*
 * pref callbacks
 */

void ts_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->timestamp = gtk_toggle_button_get_active ((GtkToggleButton *) rb);
    gconf_client_set_bool (cam->gc, KEY4, cam->timestamp, NULL);
}

void customstring_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->usestring = gtk_toggle_button_get_active ((GtkToggleButton *) rb);
    gconf_client_set_bool (cam->gc, KEY18, cam->usestring, NULL);
    gtk_widget_set_sensitive (glade_xml_get_widget
                              (cam->xml, "string_entry"), cam->usestring);
}

void drawdate_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->usedate = gtk_toggle_button_get_active ((GtkToggleButton *) rb);
    gconf_client_set_bool (cam->gc, KEY19, cam->usedate, NULL);
}

//XXX: void append_func (GtkWidget * rb, cam * cam)
//XXX: void rappend_func (GtkWidget * rb, cam * cam);

void jpg_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->savetype = JPEG;
    gconf_client_set_int (cam->gc, KEY3, cam->savetype, NULL);

}

void png_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->savetype = PNG;
    gconf_client_set_int (cam->gc, KEY3, cam->savetype, NULL);
}

void ppm_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->savetype = PPM;
    gconf_client_set_int (cam->gc, KEY3, cam->savetype, NULL);
}

void set_sensitive (cam * cam)
{
    //no-op?
    //TODO: remove this function? we no longer have any gui elements to toggle on/off
}

void cap_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    //cam->cap = gtk_toggle_button_get_active ((GtkToggleButton *) rb);
    //gconf_client_set_bool (cam->gc, KEY12, cam->cap, NULL);
    //XXX: update_tooltip (cam);
    set_sensitive (cam);

}

//XXX: void rcap_func (GtkWidget * rb, cam * cam);
//XXX: void acap_func (GtkWidget * rb, cam * cam);
//XXX: void interval_change (GtkWidget * sb, cam * cam);
//XXX: void rjpg_func (GtkWidget * rb, cam * cam);
//XXX: void rpng_func (GtkWidget * rb, cam * cam);
//XXX: void rppm_func (GtkWidget * rb, cam * cam);
//XXX: void rts_func (GtkWidget * rb, cam * cam);

void
gconf_notify_func (GConfClient * client, guint cnxn_id, GConfEntry * entry,
                   char *str)
{
    GConfValue *value;

    value = gconf_entry_get_value (entry);
    str = g_strdup (gconf_value_get_string (value));

}

void
gconf_notify_func_bool (GConfClient * client, guint cnxn_id,
                        GConfEntry * entry, gboolean val)
{
    GConfValue *value;
    value = gconf_entry_get_value (entry);
    val = gconf_value_get_bool (value);

}

void
gconf_notify_func_int (GConfClient * client, guint cnxn_id,
                       GConfEntry * entry, int val)
{
    GConfValue *value;
    value = gconf_entry_get_value (entry);
    val = gconf_value_get_int (value);

}

//TODO: RENAME THIS... maybe: quit_callback_shunt ?
int delete_event (GtkWidget * widget, gpointer data)
{
    gtk_main_quit ();
    return FALSE;
}

//XXX: void prefs_func (GtkWidget * okbutton, cam * cam)

void on_quit_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    gtk_main_quit ();
}

void on_key_press(GtkWidget *mainWindow, GdkEventKey *event, cam* cam)
{
    if (event->keyval == GDK_BackSpace)
    {
        fprintf(stderr, "TODO: remove last image: %s\n", "unknown.png");
        return;
    }

    gchar* name=gdk_keyval_name(event->keyval);

    if (strlen(name) == 1)
    {
		//fprintf(stderr, "trigger: %s (%d)\n", name, event->keyval);
		capture_func(NULL, cam, name);
    }
    else
    {
		fprintf(stderr, "key-press: %s (%d)\n", name, event->keyval);
    }
}

//XXX: void on_preferences1_activate (GtkMenuItem * menuitem, gpointer user_data);

void on_change_size_activate (GtkWidget * widget, cam * cam)
{
    gchar const *name;
    gchar       *title;

    name = gtk_widget_get_name (widget);
    printf("name = %s\n",name);
    if (strcmp (name, "small") == 0) {
        cam->x = cam->vid_cap.minwidth;
        cam->y = cam->vid_cap.minheight;
        if (cam->debug) {
            printf ("\nsmall\n");
        }
    } else if (strcmp (name, "medium") == 0) {
        cam->x = cam->vid_cap.maxwidth / 2;
        cam->y = cam->vid_cap.maxheight / 2;
        if (cam->debug) {
            printf ("\nmed\n");
        }
    } else {
        cam->x = cam->vid_cap.maxwidth;
        cam->y = cam->vid_cap.maxheight;
        if (cam->debug) {
            printf ("\nlarge\n");
        }
    }


    /*
     * if(cam->read == FALSE) {
     *  cam->pic = v4l1_mmap(0, cam->vid_buf.size, PROT_READ | PROT_WRITE, MAP_SHARED, cam->dev, 0);
     *  
     *  if((unsigned char *) -1 == (unsigned char *) cam->pic) {
     *   if(cam->debug == TRUE) {
     *   fprintf(stderr, "Unable to capture image (mmap).\n");
     *   }
     *   error_dialog(_("Unable to capture image."));
     *   exit(-1);
     *  }
     *  }else{
     *   cam->pic_buf = malloc(cam->x * cam->y * cam->depth);
     *   v4l1_read(cam->dev,cam->pic,(cam->x * cam->y * 3));
     *  } 
     */

    cam->vid_win.x = 0;
    cam->vid_win.y = 0;
    cam->vid_win.width = cam->x;
    cam->vid_win.height = cam->y;
    cam->vid_win.chromakey = 0;
    cam->vid_win.flags = 0;

    set_win_info (cam);
    // get_win_info(cam);
    cam->vid_map.height = cam->y;
    cam->vid_map.width = cam->x;
    /*
     * cam->vid_win.height = cam->y;
     * * cam->vid_win.width = cam->x;
     * * get_win_info(cam); 
     */
    cam->vid_map.format = cam->vid_pic.palette;
    // get_win_info(cam);
    /*
     * if(cam->read == FALSE) {
     * * for(frame = 0; frame < cam->vid_buf.frames; frame++) {
     * * cam->vid_map.frame = frame;
     * * if(v4l1_ioctl(cam->dev, VIDIOCMCAPTURE, &cam->vid_map) < 0) {
     * * if(cam->debug == TRUE) {
     * * fprintf(stderr, "Unable to capture image (VIDIOCMCAPTURE) during resize.\n");
     * * }
     * * //error_dialog(_("Unable to capture image."));
     * * //exit(-1);
     * * }
     * * }
     * * } 
     */
    get_win_info (cam);

    cam->pixmap = gdk_pixmap_new (NULL, cam->x, cam->y, cam->desk_depth);

	gtk_widget_set_size_request (glade_xml_get_widget (cam->xml, "viewfinder"), cam->x, cam->y);

    frame = 0;
	
	/*
    gtk_window_resize (GTK_WINDOW
                       (glade_xml_get_widget (cam->xml, "main_window")), 320,
                       240);
	*/

    title = g_strdup_printf ("Camorama - %s - %dx%d", cam->vid_cap.name,
                             cam->x, cam->y);
    gtk_window_set_title (GTK_WINDOW
                          (glade_xml_get_widget (cam->xml, "main_window")),
                          title);
    g_free (title);
}

//XXX: void on_show_adjustments_activate (GtkMenuItem * menuitem, cam * cam);
//XXX: void on_show_effects_activate(GtkMenuItem* menuitem, cam* cam);

void on_about_activate (GtkMenuItem * menuitem, cam * cam)
{
    static GtkWidget *about = NULL;
    const gchar *authors[] = {
	"Greg Jones  <greg@fixedgear.org>",
        "Jens Knutson  <tempest@magusbooks.com>",
	NULL
    };
    const gchar *documenters[] = { NULL };
    GdkPixbuf *logo =
        (GdkPixbuf *) create_pixbuf (PACKAGE_DATA_DIR "/pixmaps/camorama.png");
    char *translators = _("translator_credits");

    if (!strcmp (translators, "translator_credits"))
        translators = NULL;
    if (about != NULL) {
        gtk_window_present (GTK_WINDOW (about));
        return;
    }

    about = gnome_about_new (_("Camorama"), PACKAGE_VERSION,
                             "Copyright \xc2\xa9 2002 Greg Jones",
                             _("View, alter and save images from a webcam"),
                             (const char **) authors,
                             (const char **) documenters, translators, logo);
    gtk_window_set_transient_for (GTK_WINDOW (about),
                                  GTK_WINDOW (glade_xml_get_widget
                                              (cam->xml, "main_window")));

    g_object_add_weak_pointer (G_OBJECT (about), (void **) &(about));

    gtk_widget_show (about);
}

void
camorama_filter_color_filter(void* filter, guchar *image, int x, int y, int depth);

//XXX: apply_filters(cam* cam);

/*
 * Not called on my system... perhaps some cameras (or kernels?) require the direct read() logic, 
 * and not the buffering?
 */
gint
read_timeout_func(cam* cam) {
    int i, count = 0;
    GdkGC *gc;

    v4l1_read (cam->dev, cam->pic, (cam->x * cam->y * 3));
    frames2++;
    /*
     * update_rec.x = 0;
     * update_rec.y = 0;
     * update_rec.width = cam->x;
     * update_rec.height = cam->y; 
     */
    count++;
    /*
     * refer the frame 
     */
    cam->pic_buf = cam->pic;    // + cam->vid_buf.offsets[frame];

	/* WAS PART OF THE KERNED-OUT FILTERS FILE
    if (cam->vid_pic.palette == VIDEO_PALETTE_YUV420P) {
        yuv420p_to_rgb (cam->pic_buf, cam->tmp, cam->x, cam->y, cam->depth);
        cam->pic_buf = cam->tmp;
    }
    */

	//XXX: apply_filters(cam);

    gc = gdk_gc_new (cam->pixmap);
    gdk_draw_rgb_image (cam->pixmap,
                        gc, 0, 0,
                        cam->vid_win.width, cam->vid_win.height,
                        GDK_RGB_DITHER_NORMAL, cam->pic_buf,
                        cam->vid_win.width * cam->depth);

    gtk_widget_queue_draw_area (glade_xml_get_widget (cam->xml, "viewfinder"), 0, 0, cam->x, cam->y);
    return 1;

}

/*
 * This is our idle function.
 * It copies image data (frames) from the camera to our viewfinder.
 */
gint timeout_func (cam * cam)
{
    int i, count = 0;
    GdkGC *gc;

    i = -1;
    while (i < 0) {
        i = v4l1_ioctl (cam->dev, VIDIOCSYNC, &frame);

        if (i < 0 && errno == EINTR) {
            if (cam->debug == TRUE) {
                printf ("i = %d\n", i);
            }
            continue;
        }
        if (i < 0) {
            if (cam->debug == TRUE) {
                fprintf (stderr, "Unable to capture image (VIDIOCSYNC)\n");
            }
            error_dialog (_("Unable to capture image."));
            exit (-1);
        }
        break;
    }
    count++;

    /*
     * make pic_buf point to the current frame...
     */
    cam->pic_buf = cam->pic + cam->vid_buf.offsets[frame];

	/* WAS PART OF THE KERNED-OUT FILTERS FILE
    if (cam->vid_pic.palette == VIDEO_PALETTE_YUV420P)
	{
        yuv420p_to_rgb (cam->pic_buf, cam->tmp, cam->x, cam->y, cam->depth);
        cam->pic_buf = cam->tmp;
    }
    */

	//XXX: apply_filters(cam);

	//int bytesPerFrame=(cam->x * cam->y * cam->depth);

	if (offThreadCaptureTrigger)
	{
		capture_func(NULL, cam, "SIGNAL");
		offThreadCaptureTrigger=0;
	}

    gc = gdk_gc_new (cam->pixmap);

    gdk_draw_rgb_image (cam->pixmap,
                        gc, 0, 0,
                        cam->vid_win.width, cam->vid_win.height,
                        GDK_RGB_DITHER_NORMAL, cam->pic_buf,
                        cam->vid_win.width * cam->depth);

    gtk_widget_queue_draw_area (glade_xml_get_widget (cam->xml, "viewfinder"), 0, 0, cam->x, cam->y);

    cam->vid_map.frame = frame;

	//VIDIOCMCAPTURE tells the hardware to fill the memory buffer
	//after this point, we are not gaurnteed that the memory buffer will be consistent.
    if (v4l1_ioctl (cam->dev, VIDIOCMCAPTURE, &cam->vid_map) < 0)
	{
        if (cam->debug == TRUE)
		{
            fprintf (stderr, "Unable to capture image (VIDIOCMCAPTURE)\n");
        }
        error_dialog (_("Unable to capture image."));
        exit (-1);
    }

    /*
     * next frame 
     */
    frame++;

    /*
     * reset to the 1st frame 
     */
    if (frame >= cam->vid_buf.frames) {
        frame = 0;
    }

    frames2++;
    g_object_unref ((gpointer) gc);
    return 1;
}

gint fps (GtkWidget * sb)
{
    gchar *stat;

    seconds++;
    //NB: the 2 magic number comes from the fact that this callback is called every 2 seconds.
    stat = g_strdup_printf ("%.2f fps - current     %.2f fps - average",
                            (float) frames / (float) (2),
                            (float) frames2 / (float) (seconds * 2));
    frames = 0;
    //TODO: FIXME: gnome_appbar_push (GNOME_APPBAR (sb), stat);
    g_free (stat);
    return 1;
}

void on_status_show (GtkWidget * sb, cam * cam)
{
    cam->status = sb;
}

void init_cam (GtkWidget * capture, cam * cam)
{
    cam->pic =
        v4l1_mmap (0, cam->vid_buf.size, PROT_READ | PROT_WRITE,
              MAP_SHARED, cam->dev, 0);

    if ((unsigned char *) -1 == (unsigned char *) cam->pic) {
        if (cam->debug == TRUE) {
            fprintf (stderr, "Unable to capture image (mmap).\n");
        }
        error_dialog (_("Unable to capture image."));
        exit (-1);
    }
    cam->vid_map.height = cam->y;
    cam->vid_map.width = cam->x;
    cam->vid_map.format = cam->vid_pic.palette;
    for (frame = 0; frame < cam->vid_buf.frames; frame++) {
        cam->vid_map.frame = frame;
        if (v4l1_ioctl (cam->dev, VIDIOCMCAPTURE, &cam->vid_map) < 0) {
            if (cam->debug == TRUE) {
                fprintf (stderr,
                         "Unable to capture image (VIDIOCMCAPTURE).\n");
            }
            error_dialog (_("Unable to capture image."));
            exit (-1);
        }
    }
    frame = 0;
}


//NB: Assumes UNSIGNED char...
char basicThreeByteMedia(char a2, char b2, char c2)
{
	//Since equality can be ignored, there are six possible permutations....
	if (a2<b2)
	{
		if (c2<a2)
		{
			//C A B
			return a2;
		}
		else
		if (b2<c2)
		{
			//A B C
			return b2;
		}
		else
		{
			//A C B
			return c2;
		}
	}
	else
	{
		//b2 < a2 (effectively)

		if (c2<b2)
		{
			//C B A
			return b2;
		}
		else
		if (a2<c2)
		{
			//B A C
			return a2;
		}
		else
		{
			//B C A
			return c2;
		}
	}
}

void basicThreeByteMedianOfThreePixelMerge(char *a, char *b, char *c, char *out)
{
	*(out)=basicThreeByteMedia(*a, *b, *c);
	*(out+1)=basicThreeByteMedia(*(a+1), *(b+1), *(c+1));
	*(out+2)=basicThreeByteMedia(*(a+2), *(b+2), *(c+2));
}

void capture_func (GtkWidget* widget, cam* cam, gchar* keyqualifier)
{
    if (cam->debug == TRUE)
	{
        fprintf(
			stderr,
			"capture_func\nx = %d, y = %d, depth = %d, realloc size = %d\n",
			cam->x,
			cam->y,
			cam->depth,
			(cam->x * cam->y * cam->depth)
		);
    }

    /*
	 * To improve image quality, we take the TEMPORAL MEDIAN of all the frames that are currently
	 * in the video buffer (in my tests, this is 4, but the docs say it can be as high as 32).
	 *
	 * TODO: use all the available frames, not just three recent frames.
    int frame;

    for(frame = 0; frame < cam->vid_buf.frames; frame++)
	{
		//cam->pic_buf = cam->pic + cam->vid_buf.offsets[frame];
    	
	}
	*/

	//fprintf(stderr, "cam->depth=%d\n", cam->depth);

	if (cam->depth == 3 && cam->vid_buf.frames >= 3)
	{
		int numPixels=cam->x * cam->y;
		int pixel;

		char* alpha=cam->pic + cam->vid_buf.offsets[0];
		char* beta=cam->pic + cam->vid_buf.offsets[1];
		char* delta=cam->pic + cam->vid_buf.offsets[2];

		for (pixel=0; pixel<numPixels; pixel++)
		{
			int o=pixel*3;
			basicThreeByteMedianOfThreePixelMerge(alpha+o, beta+o, delta+o, cam->tmp+o);
		}
	}
	else
	{
		fprintf(stderr, "unable to use 3b3 median (%d, %d)\n", cam->depth, cam->vid_buf.frames);
		memcpy (cam->tmp, cam->pic_buf, cam->x * cam->y * cam->depth);
	}

    local_save(cam, keyqualifier);
}

//XXX: void contrast_change (GtkHScale * sc1, cam * cam)
//XXX: void brightness_change (GtkHScale * sc1, cam * cam)
//XXX: void colour_change (GtkHScale * sc1, cam * cam)
//XXX: void hue_change (GtkHScale * sc1, cam * cam)
//XXX: void wb_change (GtkHScale * sc1, cam * cam)

void help_cb (GtkWidget * widget, gpointer data)
{
    GError *error = NULL;

    if (error != NULL) {
        /*
         * FIXME: This is bad :) 
         */
        g_warning ("%s\n", error->message);
        g_error_free (error);
    }
}

//XXX: void update_tooltip (cam * cam);

