/* This file is part of camorama
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@gnome-de.org>
 *
 * Copyright (C) 2007  Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "camorama-display.h"

#include "camorama-globals.h" // frames

struct _CamoDisplayPrivate {
	cam* camera;
};

enum {
	PROP_0,
	PROP_CAMERA
};

G_DEFINE_TYPE (CamoDisplay, camo_display, GTK_TYPE_DRAWING_AREA);

GtkWidget*
camo_display_new (cam* camera)
{
	return g_object_new (CAMO_TYPE_DISPLAY,
			     "camera", camera,
			     NULL);
}

static void
camo_display_init (CamoDisplay* self)
{
	self->_private = G_TYPE_INSTANCE_GET_PRIVATE (self, CAMO_TYPE_DISPLAY, CamoDisplayPrivate);
}

static void
display_get_property (GObject   * object,
		      guint       prop_id,
		      GValue    * value,
		      GParamSpec* pspec)
{
	CamoDisplay* self = CAMO_DISPLAY (object);

	switch (prop_id) {
	case PROP_CAMERA:
		g_value_set_pointer (value, self->_private->camera);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
display_set_property (GObject     * object,
		      guint         prop_id,
		      GValue const* value,
		      GParamSpec  * pspec)
{
	CamoDisplay* self = CAMO_DISPLAY (object);

	switch (prop_id) {
	case PROP_CAMERA:
		self->_private->camera = g_value_get_pointer (value);
		g_object_notify (object, "camera");
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static gboolean
display_expose_event (GtkWidget     * widget,
		      GdkEventExpose* event)
{
	CamoDisplay* self = CAMO_DISPLAY (widget);

	cairo_t* downsampled=gdk_cairo_create(widget->window);
	{
		gdk_cairo_region(downsampled, event->region);
	}

	/*
	GdkPixbuf *downsampled=gdk_pixbuf_scale_simple(
		self->_private->camera->pixmap,
		320,
		240,
		GDK_INTERP_NEAREST
	);
	*/

	if (0)
	gdk_draw_drawable (widget->window,
			   widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			   self->_private->camera->pixmap,
			   event->area.x, event->area.y, event->area.x,
			   event->area.y, -1, -1);//event->area.width, event->area.height);

	//cairo_set_source_rgb(downsampled, 0.25, 0.6, 0.8);
	gdk_cairo_set_source_pixmap(
		downsampled,
		self->_private->camera->pixmap,
		0,
		0
	);

	double scale_factor=0.1;//100/3268;
	/*
	cairo_fill(downsampled);
	cairo_scale(downsampled, 0.25, 0.25);
	cairo_rectangle(downsampled, 0,0, 0.5, 0.6);
	cairo_stroke(downsampled);
	*/
	cairo_t* cr=downsampled;
	//cairo_set_source_rgb(cr, 0.2, 0.3, 0.8);
  //cairo_rectangle(cr, 10, 10, 90, 90);    
  cairo_fill(cr);

  cairo_scale(cr, scale_factor, scale_factor);
  gdk_cairo_set_source_pixmap(downsampled, self->_private->camera->pixmap, 0, 0);
  cairo_rectangle(cr, 0, 0, 3264, 2448);    
  cairo_fill(cr);      

	frames++;

	//g_object_unref(downsampled);
	cairo_destroy(downsampled);

	return FALSE;
}

static void
camo_display_class_init (CamoDisplayClass* self_class)
{
	GObjectClass*   object_class = G_OBJECT_CLASS (self_class);
	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

	object_class->get_property = display_get_property;
	object_class->set_property = display_set_property;

	g_object_class_install_property (object_class,
					 PROP_CAMERA,
#warning "FIXME: use a camera object here"
					 g_param_spec_pointer ("camera",
							       "camera",
							       "camera",
							       G_PARAM_READWRITE));

	widget_class->expose_event = display_expose_event;

	g_type_class_add_private (self_class, sizeof (CamoDisplayPrivate));
}

