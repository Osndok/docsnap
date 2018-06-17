#include <gtk/gtk.h>
#include <config.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "filter.h"
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

void append_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->timefn = gtk_toggle_button_get_active ((GtkToggleButton *) rb);
    gconf_client_set_bool (cam->gc, KEY14, cam->timefn, NULL);

}

void rappend_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->rtimefn = gtk_toggle_button_get_active ((GtkToggleButton *) rb);
    gconf_client_set_bool (cam->gc, KEY15, cam->rtimefn, NULL);

}

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
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "table4"),
                              cam->cap);

    gtk_widget_set_sensitive (glade_xml_get_widget
                              (cam->xml, "appendbutton"), cam->cap);
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "tsbutton"),
                              cam->cap);
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "jpgb"),
                              cam->cap);
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "pngb"),
                              cam->cap);
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "table5"),
                              cam->rcap);
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "timecb"),
                              cam->rcap);
    gtk_widget_set_sensitive (glade_xml_get_widget
                              (cam->xml, "tsbutton2"), cam->rcap);
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "fjpgb"),
                              cam->rcap);
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "fpngb"),
                              cam->rcap);
    gtk_widget_set_sensitive (glade_xml_get_widget (cam->xml, "hbox20"),
                              cam->acap);

}

void cap_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->cap = gtk_toggle_button_get_active ((GtkToggleButton *) rb);

    gconf_client_set_bool (cam->gc, KEY12, cam->cap, NULL);
    update_tooltip (cam);
    set_sensitive (cam);

}

void rcap_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->rcap = gtk_toggle_button_get_active ((GtkToggleButton *) rb);
    gconf_client_set_bool (cam->gc, KEY13, cam->rcap, NULL);
    update_tooltip (cam);
    set_sensitive (cam);

}

void acap_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->acap = gtk_toggle_button_get_active ((GtkToggleButton *) rb);

    gconf_client_set_bool (cam->gc, KEY20, cam->acap, NULL);

    if (cam->acap == TRUE) {
        cam->timeout_id =
            gtk_timeout_add (cam->timeout_interval,
                             (GSourceFunc) timeout_capture_func, cam);
        if (cam->debug == TRUE) {
            printf ("add autocap - %d -  timeout_interval = \n",
                    cam->timeout_id, cam->timeout_interval);
        }
    } else {
        if (cam->debug == TRUE) {
            printf ("remove autocap - %d -  timeout_interval = \n",
                    cam->timeout_id, cam->timeout_interval);
        }
        gtk_timeout_remove (cam->timeout_id);
    }
    update_tooltip (cam);
    set_sensitive (cam);
}

void interval_change (GtkWidget * sb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->timeout_interval =
        gtk_spin_button_get_value ((GtkSpinButton *) sb) * 60000;
    gconf_client_set_int (cam->gc, KEY21, cam->timeout_interval, NULL);
    if (cam->acap == TRUE) {
        if (cam->debug == TRUE) {
            printf
                ("interval_change; old timeout_id = %d old interval = %d\n",
                 cam->timeout_id, cam->timeout_interval);
        }
        gtk_timeout_remove (cam->timeout_id);
        cam->timeout_id =
            gtk_timeout_add (cam->timeout_interval,
                             (GSourceFunc) timeout_capture_func, cam);
        if (cam->debug == TRUE) {
            printf ("new timeout_id = %d, new interval = %d\n",
                    cam->timeout_id, cam->timeout_interval);
        }
    }
    update_tooltip (cam);
}

void rjpg_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->rsavetype = JPEG;
    gconf_client_set_int (cam->gc, KEY10, cam->rsavetype, NULL);

}

void rpng_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->rsavetype = PNG;
    gconf_client_set_int (cam->gc, KEY10, cam->rsavetype, NULL);
}

void rppm_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->rsavetype = PPM;
    gconf_client_set_int (cam->gc, KEY10, cam->rsavetype, NULL);
}

void rts_func (GtkWidget * rb, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();
    cam->rtimestamp = gtk_toggle_button_get_active ((GtkToggleButton *) rb);
    gconf_client_set_bool (cam->gc, KEY11, cam->rtimestamp, NULL);

}

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

int delete_event (GtkWidget * widget, gpointer data)
{
    gtk_main_quit ();
    return FALSE;
}

/*
 * apply preferences 
 */
void prefs_func (GtkWidget * okbutton, cam * cam)
{
    GConfClient *client;

    client = gconf_client_get_default ();

    if (gnome_file_entry_get_full_path ((GnomeFileEntry *) dentry, TRUE)
        != NULL) {
        cam->pixdir = g_strdup ((gchar *)
                                gnome_file_entry_get_full_path ((GnomeFileEntry *) dentry, FALSE));
        gconf_client_set_string (cam->gc, KEY1, cam->pixdir, NULL);

    } else {
        if (cam->debug == TRUE) {
            fprintf (stderr, "null directory\ndirectory unchanged.");
        }
    }

    /*
     * this is stupid, even if the string is empty, it will not return NULL 
     */
    if (strlen (gtk_entry_get_text ((GtkEntry *) entry2)) > 0) {
        cam->capturefile =
            g_strdup (gtk_entry_get_text ((GtkEntry *) entry2));
        gconf_client_set_string (cam->gc, KEY2, cam->capturefile, NULL);
    }

    if (strlen (gtk_entry_get_text ((GtkEntry *) host_entry)) > 0) {
        cam->rhost = g_strdup (gtk_entry_get_text ((GtkEntry *) host_entry));
        gconf_client_set_string (cam->gc, KEY5, cam->rhost, NULL);
    }
    if (strlen (gtk_entry_get_text ((GtkEntry *) login_entry)) > 0) {
        cam->rlogin =
            g_strdup (gtk_entry_get_text ((GtkEntry *) login_entry));
        gconf_client_set_string (cam->gc, KEY6, cam->rlogin, NULL);
    }
    if (strlen (gtk_entry_get_text ((GtkEntry *) pw_entry)) > 0) {
        cam->rpw = g_strdup (gtk_entry_get_text ((GtkEntry *) pw_entry));
        gconf_client_set_string (cam->gc, KEY7, cam->rpw, NULL);
    }
    if (strlen (gtk_entry_get_text ((GtkEntry *) directory_entry)) > 0) {
        cam->rpixdir =
            g_strdup (gtk_entry_get_text ((GtkEntry *) directory_entry));
        gconf_client_set_string (cam->gc, KEY8, cam->rpixdir, NULL);
    }
    if (strlen (gtk_entry_get_text ((GtkEntry *) filename_entry)) > 0) {
        cam->rcapturefile =
            g_strdup (gtk_entry_get_text ((GtkEntry *) filename_entry));
        gconf_client_set_string (cam->gc, KEY9, cam->rcapturefile, NULL);
    }
    if (strlen (gtk_entry_get_text ((GtkEntry *) string_entry)) > 0) {
        cam->ts_string =
            g_strdup (gtk_entry_get_text ((GtkEntry *) string_entry));
        gconf_client_set_string (cam->gc, KEY16, cam->ts_string, NULL);
    }
    if (cam->debug == TRUE) {
        fprintf (stderr, "dir now = %s\nfile now = %s\n", cam->pixdir,
                 cam->capturefile);
    }
    gtk_widget_hide (prefswindow);

}

void on_quit_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    gtk_main_quit ();
}

void on_preferences1_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    gtk_widget_show (prefswindow);
}

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

	//gtk_widget_set_size_request (glade_xml_get_widget (cam->xml, "da"), cam->x, cam->y);

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

void on_show_adjustments_activate (GtkMenuItem * menuitem, cam * cam)
{

    if (GTK_WIDGET_VISIBLE (glade_xml_get_widget (cam->xml, "adjustments_table"))) {
        gtk_widget_hide (glade_xml_get_widget (cam->xml, "adjustments_table"));
        gtk_window_resize (GTK_WINDOW
                           (glade_xml_get_widget
                            (cam->xml, "main_window")), 320, 240);
        cam->show_adjustments = FALSE;

    } else {
        gtk_widget_show (glade_xml_get_widget (cam->xml, "adjustments_table"));
        cam->show_adjustments = TRUE;
    }
    gconf_client_set_bool (cam->gc, KEY22, cam->show_adjustments, NULL);
}

void
on_show_effects_activate(GtkMenuItem* menuitem, cam* cam) {
	GtkWidget* effects = glade_xml_get_widget(cam->xml, "scrolledwindow_effects");
	cam->show_effects = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
	
	if(!cam->show_effects) {
		gtk_widget_hide(effects);
		gtk_window_resize(GTK_WINDOW(glade_xml_get_widget(cam->xml, "main_window")), 320, 240);
	} else {
		gtk_widget_show(effects);
	}

	gconf_client_set_bool(cam->gc, KEY23, cam->show_effects, NULL);
}

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

static void
apply_filters(cam* cam) {
	/* v4l has reverse rgb order from what camora expect so call the color
	   filter to fix things up before running the user selected filters */
	camorama_filter_color_filter(NULL, cam->pic_buf, cam->x, cam->y, cam->depth);
	camorama_filter_chain_apply(cam->filter_chain, cam->pic_buf, cam->x, cam->y, cam->depth);
#warning "FIXME: enable the threshold channel filter"
//	if((effect_mask & CAMORAMA_FILTER_THRESHOLD_CHANNEL)  != 0) 
//		threshold_channel (cam->pic_buf, cam->x, cam->y, cam->dither);
#warning "FIXME: enable the threshold filter"
//	if((effect_mask & CAMORAMA_FILTER_THRESHOLD)  != 0) 
//		threshold (cam->pic_buf, cam->x, cam->y, cam->dither);
}

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

    if (cam->vid_pic.palette == VIDEO_PALETTE_YUV420P) {
        yuv420p_to_rgb (cam->pic_buf, cam->tmp, cam->x, cam->y, cam->depth);
        cam->pic_buf = cam->tmp;
    }

	apply_filters(cam);

    gc = gdk_gc_new (cam->pixmap);
    gdk_draw_rgb_image (cam->pixmap,
                        gc, 0, 0,
                        cam->vid_win.width, cam->vid_win.height,
                        GDK_RGB_DITHER_NORMAL, cam->pic_buf,
                        cam->vid_win.width * cam->depth);

    gtk_widget_queue_draw_area (glade_xml_get_widget (cam->xml, "da"), 0,
                                0, cam->x, cam->y);
    return 1;

}

/*
 * get image from cam - does all the work :-) 
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

    if (cam->vid_pic.palette == VIDEO_PALETTE_YUV420P)
	{
        yuv420p_to_rgb (cam->pic_buf, cam->tmp, cam->x, cam->y, cam->depth);
        cam->pic_buf = cam->tmp;
    }

	apply_filters(cam);

	//int bytesPerFrame=(cam->x * cam->y * cam->depth);

	if (offThreadCaptureTrigger)
	{
		capture_func(NULL, cam);
		offThreadCaptureTrigger=0;
	}

    gc = gdk_gc_new (cam->pixmap);

    gdk_draw_rgb_image (cam->pixmap,
                        gc, 0, 0,
                        cam->vid_win.width, cam->vid_win.height,
                        GDK_RGB_DITHER_NORMAL, cam->pic_buf,
                        cam->vid_win.width * cam->depth);

    gtk_widget_queue_draw_area (glade_xml_get_widget (cam->xml, "da"), 0,
                                0, cam->x, cam->y);

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
    stat = g_strdup_printf ("%.2f fps - current     %.2f fps - average",
                            (float) frames / (float) (2),
                            (float) frames2 / (float) (seconds * 2));
    frames = 0;
    gnome_appbar_push (GNOME_APPBAR (sb), stat);
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

void capture_func (GtkWidget * widget, cam * cam)
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

    if (cam->rcap == TRUE) {
        remote_save (cam);
    }
    if (cam->cap == TRUE) {
        local_save (cam);
    }

}

gint timeout_capture_func (cam * cam)
{
    /* GdkRectangle rect;
     * rect->x = 0; rect->y = 0;
     * rect->width = cam->x; rect->height = cam->y; */

    /* need to return true, or the timeout will be destroyed - don't forget! :) */
    if (cam->hidden == TRUE) {
        /* call timeout_func to get a new picture.   stupid, but it works.
         * also need to add this to capture_func 
         * maybe add a "window_state_event" handler to do the same when window is iconified */

        pt2Function (cam);
        pt2Function (cam);
        pt2Function (cam);
        pt2Function (cam);

    }
    memcpy (cam->tmp, cam->pic_buf, cam->x * cam->y * cam->depth);

    if (cam->cap == TRUE) {
        local_save (cam);
    }
    if (cam->rcap == TRUE) {
        remote_save (cam);
    }
    return 1;
}

void contrast_change (GtkHScale * sc1, cam * cam)
{

    cam->vid_pic.contrast =
        256 * (int) gtk_range_get_value ((GtkRange *) sc1);
    set_pic_info (cam);
}

void brightness_change (GtkHScale * sc1, cam * cam)
{

    cam->vid_pic.brightness =
        256 * (int) gtk_range_get_value ((GtkRange *) sc1);
    set_pic_info (cam);
}

void colour_change (GtkHScale * sc1, cam * cam)
{

    cam->vid_pic.colour = 256 * (int) gtk_range_get_value ((GtkRange *) sc1);
    set_pic_info (cam);
}

void hue_change (GtkHScale * sc1, cam * cam)
{

    cam->vid_pic.hue = 256 * (int) gtk_range_get_value ((GtkRange *) sc1);
    set_pic_info (cam);
}

void wb_change (GtkHScale * sc1, cam * cam)
{

    cam->vid_pic.whiteness =
        256 * (int) gtk_range_get_value ((GtkRange *) sc1);
    set_pic_info (cam);
}

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

void update_tooltip (cam * cam)
{
    gchar *tooltip_text;

    if (cam->debug == TRUE)
	{
        fprintf(stderr, "update_tooltip called\n");
    }

    if (cam->acap == TRUE)
	{
        tooltip_text =
            g_strdup_printf
            (_("Local Capture: %d\nRemote Capture: %d\nCapture Interval: %d"),
             cam->cap, cam->rcap, cam->timeout_interval / 60000);

		if (cam->debug == TRUE)
		{
            fprintf(stderr, "tip - acap on\n");
        }
    }
    else
	{
        if (cam->debug == TRUE)
		{
            fprintf(stderr, "tip - acap off\n");
        }

        tooltip_text = g_strdup_printf (_("Automatic Capture Disabled"));
    }
    gtk_status_icon_set_tooltip(cam->tray_icon, tooltip_text);
    g_free (tooltip_text);
}
