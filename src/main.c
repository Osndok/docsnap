#include <gnome.h>
#include "interface.h"

#include "callbacks.h"
#include "support.h"
#include <config.h>

#include <gdk/gdkx.h>
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlibrgb.h>
#include <locale.h>
#include <libv4l1.h>

#include "camorama-window.h"
#include "camorama-display.h"
#include "camorama-stock-items.h"

GtkWidget *prefswindow;
int frames, frames2, seconds;
GtkWidget *dentry, *entry2, *string_entry, *format_selection;
GtkWidget *host_entry, *protocol, *rdir_entry, *filename_entry;

static gboolean ver = FALSE, max = FALSE, min = FALSE, half =
    FALSE, use_read = FALSE;

/*static gint tray_icon_destroyed (GtkWidget *tray, gpointer data) 
{
  GConfClient *client = gconf_client_get_default ();

    //Somehow the delete_event never got called, so we use "destroy" 
  if (tray != MyApp->GetMainWindow ()->docklet)
    return true;
  GtkWidget *new_tray = gnomemeeting_init_tray ();

  if (gconf_client_get_bool 
      (client, GENERAL_KEY "do_not_disturb", 0)) 
    gnomemeeting_tray_set_content (new_tray, 2);

  
  MyApp->GetMainWindow ()->docklet = GTK_WIDGET (new_tray);
  gtk_widget_show (gm);

  return true;
}*/

static GtkWidget*
camorama_glade_handler (GladeXML* xml,
			gchar   * func_name,
			gchar   * name,
			gchar   * string1,
			gchar   * string2,
			gint      int1,
			gint      int2,
			gpointer  data)
{
	gboolean reached = TRUE;
#warning "FIXME: move the glade crap into the window class"
	if (string1 && !strcmp (string1, "CamoDisplay")) {
		GtkWidget* widget = camo_display_new ((cam*)data);
		gtk_widget_show (widget);
		return widget;
	}

	g_return_val_if_fail (!reached, NULL);
	return NULL;
}

void on_usr1_signal(int signo)
{
	fprintf(stderr, "received usr1 signal\n");
	
	if (offThreadCaptureTrigger)
	{
		fprintf(stderr, "ERROR: a capture series appears to already be in progress\n", offThreadCaptureTrigger);
	}
	else
	{
		//captureTriggerFrameNumber=NUM_FRAMES_TO_MERGE;
		offThreadCaptureTrigger=1;
	}
}

int
main(int argc, char *argv[])
{
    cam cam_object, *cam;
    Display *display;
    Screen *screen_num;
    gchar *specifiedDevice = NULL;
    //XXX: gchar *pixfilename = "camorama/camorama.png";
    gchar *directory=NULL;
    gchar *filename;            //= "/usr/opt/garnome/share/camorama/camorama.glade";
    int x = -1, y = -1;
    gboolean debugEnabled = FALSE;
    GtkWidget *button;
    GConfClient *gc;

    const struct poptOption popt_options[] = {
        {"version", 'V', POPT_ARG_NONE, &ver, 0, N_("show version and exit"), NULL},
        {"device" , 'd', POPT_ARG_STRING, &specifiedDevice, 0, N_("v4l device to use"), NULL},
        //{"debug"  , 'D', POPT_ARG_NONE, &debugEnabled, 0, N_("enable debugging code"), NULL},
        {"directory"  , 'D', POPT_ARG_STRING, &directory, 0, N_("the directory in which to place scanned items"), NULL},
        {"width"  , 'x', POPT_ARG_INT, &x, 0, N_("capture width"), NULL},
        {"height" , 'y', POPT_ARG_INT, &y, 0, N_("capture height"), NULL},
        {"max"    , 'M', POPT_ARG_NONE, &max, 0, N_("maximum capture size"), NULL},
        {"min"    , 'm', POPT_ARG_NONE, &min, 0, N_("minimum capture size"), NULL},
        {"half"   , 'H', POPT_ARG_NONE, &half, 0, N_("middle capture size"), NULL},
        {"read"   , 'R', POPT_ARG_NONE, &use_read, 0, N_("use read() rather than mmap()"), NULL},
        POPT_TABLEEND
    };

	signal(SIGUSR1, on_usr1_signal);

    cam = &cam_object;
    /* set some default values */
    cam->frame_number = 0;
    cam->pic = NULL;
    cam->pixmap = NULL;
    cam->size = PICHALF;
    cam->video_dev = NULL;
    cam->read = FALSE;
	cam->counter = 0;
	cam->lastSavedFile = NULL;

    bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
    setlocale (LC_ALL, "");

    /* gnome_program_init  - initialize everything (gconf, threads, etc) */
    gnome_program_init (PACKAGE_NAME, PACKAGE_VERSION, LIBGNOMEUI_MODULE, argc, argv,
                        GNOME_PARAM_APP_DATADIR, PACKAGE_DATA_DIR,
                        GNOME_PARAM_POPT_TABLE, popt_options,
                        GNOME_PARAM_HUMAN_READABLE_NAME, _("camorama"), NULL);

    /* gtk is initialized now */
    camorama_stock_init();
    //XXX: camorama_filters_init();

    cam->debug = debugEnabled;

	cam->x = x;
	cam->y = y;
	glade_gnome_init ();
	glade_set_custom_handler (camorama_glade_handler,
				  cam);


    if (ver) {
        fprintf (stderr, _("\n\nCamorama version %s\n\n"), PACKAGE_VERSION);
        exit (0);
    }
    if (max || 1) {
        cam->size = PICMAX;
    }
    if (min) {
        cam->size = PICMIN;
    }
    if (half) {
        cam->size = PICHALF;
    }
    if (use_read) {
        fprintf(stderr, "warning: must use_read for the image input? (gah!)\n");
        cam->read = TRUE;
    }
    gc = gconf_client_get_default ();
    cam->gc = gc;

    gconf_client_add_dir (cam->gc, CONFIG_PREFIX, GCONF_CLIENT_PRELOAD_NONE, NULL);
    //XXX: gconf_client_notify_add (cam->gc, KEY1, (void *) gconf_notify_func, cam->pixdir, NULL, NULL);
    //XXX: gconf_client_notify_add (cam->gc, KEY5, (void *) gconf_notify_func, cam->rhost, NULL, NULL);
    //XXX: gconf_client_notify_add (cam->gc, KEY2, (void *) gconf_notify_func, cam->capturefile, NULL, NULL);
    gconf_client_notify_add (cam->gc, KEY3,
                             (void *) gconf_notify_func_int,
                             GINT_TO_POINTER (cam->savetype), NULL, NULL);
    gconf_client_notify_add (cam->gc, KEY4,
                             (void *) gconf_notify_func_bool,
                             &cam->timestamp, NULL, NULL);

    if (specifiedDevice)
    {
		cam->video_dev = g_strdup (specifiedDevice);
    }
    else
    {
		cam->video_dev = g_strdup ("/dev/video0");
    }

	//TODO: also accept directory via environment variable
	if (directory)
	{
	    if (directory[0] == '/')
	    {
		    cam->pixdir = directory;
	    }
	    else
	    {
	        char *pixdir=(char*)g_malloc(2048);
	        getcwd(pixdir, 2048);
	        strcat(pixdir, "/");
	        strcat(pixdir, directory);
	        cam->pixdir=pixdir;
	    }
	}
	else
	{
	    const char *homeDir = getenv("HOME");

	    //TODO: make this a ./configure parameter
	    const char *defaultDir = "Documents/Scanned";

	    const int n=strlen(defaultDir)+1+strlen(homeDir)+1;
	    cam->pixdir = (gchar*)g_malloc(n);
	    g_snprintf(cam->pixdir, n, "%s/%s", homeDir, defaultDir);
	}
    //XXX: cam->capturefile = g_strdup (gconf_client_get_string (cam->gc, KEY2, NULL));
    /*
    cam->rhost = g_strdup (gconf_client_get_string (cam->gc, KEY5, NULL));
    cam->rlogin = g_strdup (gconf_client_get_string (cam->gc, KEY6, NULL));
    cam->rpw = g_strdup (gconf_client_get_string (cam->gc, KEY7, NULL));
    cam->rpixdir = g_strdup (gconf_client_get_string (cam->gc, KEY8, NULL));
    cam->rcapturefile =
        g_strdup (gconf_client_get_string (cam->gc, KEY9, NULL));
    */
    cam->savetype = gconf_client_get_int (cam->gc, KEY3, NULL);
    //XXX: cam->rsavetype = gconf_client_get_int (cam->gc, KEY10, NULL);
    cam->ts_string =
        g_strdup (gconf_client_get_string (cam->gc, KEY16, NULL));
    cam->date_format = "%Y-%m-%d %H:%M:%S";
    cam->timestamp = gconf_client_get_bool (cam->gc, KEY4, NULL);
    //XXX: cam->rtimestamp = gconf_client_get_bool (cam->gc, KEY11, NULL);

    //XXX: cam->cap = gconf_client_get_bool (cam->gc, KEY12, NULL);
    //XXX: cam->rcap = gconf_client_get_bool (cam->gc, KEY13, NULL);
    //XXX: cam->timefn = gconf_client_get_bool (cam->gc, KEY14, NULL);
    //XXX: cam->rtimefn = gconf_client_get_bool (cam->gc, KEY15, NULL);
    cam->usestring = gconf_client_get_bool (cam->gc, KEY18, NULL);
    cam->usedate = gconf_client_get_bool (cam->gc, KEY19, NULL);
    //XXX: cam->acap = gconf_client_get_bool (cam->gc, KEY20, NULL);
    //XXX: cam->timeout_interval = gconf_client_get_int (cam->gc, KEY21, NULL);
    //XXX: cam->show_adjustments = gconf_client_get_bool (cam->gc, KEY22, NULL);
	//XXX: cam->show_effects = gconf_client_get_bool (cam->gc, KEY23, NULL);


    /* get desktop depth */
    display = (Display *) gdk_x11_get_default_xdisplay ();
    screen_num = xlib_rgb_get_screen ();
    gdk_pixbuf_xlib_init (display, 0);
    cam->desk_depth = xlib_rgb_get_depth ();

    cam->dev = v4l1_open (cam->video_dev, O_RDWR);

    camera_cap (cam);
    get_win_info (cam);

    /* query/set window attributes */
    cam->vid_win.x = 0;
    cam->vid_win.y = 0;
    cam->vid_win.width = cam->x;
    cam->vid_win.height = cam->y;
    cam->vid_win.chromakey = 0;
    cam->vid_win.flags = 0;

    set_win_info (cam);
    get_win_info (cam);

    /* get picture attributes */
    get_pic_info (cam);
    set_pic_info (cam);
    /*
    cam->contrast = cam->vid_pic.contrast;
    cam->brightness = cam->vid_pic.brightness;
    cam->colour = cam->vid_pic.colour;
    cam->hue = cam->vid_pic.hue;
    cam->wb = cam->vid_pic.whiteness;
    */
    cam->depth = cam->vid_pic.depth / 8;
    cam->pic_buf = malloc (cam->x * cam->y * cam->depth);
    cam->tmp =
        malloc (cam->vid_cap.maxwidth * cam->vid_cap.maxheight * cam->depth);
    //cam->tmp = NULL;
    /* set the buffer size */
    if (cam->read == FALSE) {
        set_buffer (cam);
    }
    //cam->read = FALSE;
    /* initialize cam and create the window */

    if (cam->read == FALSE) {
        pt2Function = timeout_func;
        init_cam (NULL, cam);
    } else {
        printf ("using read()\n");
        cam->pic =
            realloc (cam->pic,
                     (cam->vid_cap.maxwidth * cam->vid_cap.maxheight * 3));
        pt2Function = read_timeout_func;
    }
    cam->pixmap = gdk_pixmap_new (NULL, cam->x, cam->y, cam->desk_depth);

	/*
	 * Use the local development glade file, if it is present, otherwise use the
	 * installed system-wide version.
	 */
    filename = "data/camorama.glade";

	if (access(filename, R_OK))
	{
		filename = gnome_program_locate_file(
			NULL,
			GNOME_FILE_DOMAIN_APP_DATADIR,
			"docsnap/docsnap.glade",
			TRUE,
			NULL
		);
	}

    if (filename == NULL)
	{
        error_dialog (_("Couldn't find the main interface file (docsnap.glade)."));
        exit (1);
    }

    //pixfilename = gnome_program_locate_file(NULL, GNOME_FILE_DOMAIN_APP_DATADIR, "pixmaps/camorama.png", TRUE, NULL);
    //printf("pixfile = %s\n",pixfilename);
    //pixfilename);
    //printf("pixfile = %s\n",pixfilename);
    cam->xml = glade_xml_new (filename, NULL, NULL);
    /*eggtray */

    if (!cam->xml)
    {
        fprintf(stderr, "unable to load glade interface file: %s\n", filename);
        exit(1);
    }
    else
    {
        fprintf(stderr, "using: %s\n", filename);
    }

    /*tray_icon = egg_tray_icon_new ("Our other cool tray icon");
     * button = gtk_button_new_with_label ("This is a another\ncool tray icon");
     * g_signal_connect (button, "clicked",
     * G_CALLBACK (second_button_pressed), tray_icon);
     * 
     * gtk_container_add (GTK_CONTAINER (tray_icon), button);
     * gtk_widget_show_all (GTK_WIDGET (tray_icon)); */
    load_interface (cam);

    cam->idle_id = gtk_idle_add ((GSourceFunc) pt2Function, (gpointer) cam);

    gtk_timeout_add (2000, (GSourceFunc) fps, cam->status);

    print_cam(cam);

    gtk_main ();
    v4l1_munmap(cam->pic, cam->vid_buf.size);
    v4l1_close(cam->dev);

    return 0;
}
