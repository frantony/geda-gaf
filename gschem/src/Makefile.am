bin_PROGRAMS = gschem

BUILT_SOURCES = \
	g_attrib.x \
	g_hook.x \
	g_keys.x \
	g_util.x \
	g_window.x \
	g_select.x

gschem_SOURCES = \
	a_pan.c \
	a_zoom.c \
	g_action.c \
	g_attrib.c \
	g_builtins.c \
	g_funcs.c \
	g_hook.c \
	g_keys.c \
	g_rc.c \
	g_register.c \
	g_select.c \
	g_util.c \
	g_window.c \
	globals.c \
	gschem.c \
	gschemhotkeystore.c \
	gschem_accel_label.c \
	gschem_action.c \
	gschem_dialog.c \
	gschem_integer_combo_box.c \
	gschem_main_window.c \
	gschem_page_geometry.c \
	gschem_page_view.c \
	gschem_selection_adapter.c \
	gschem_swatch_column_renderer.c \
	gschem_toplevel.c \
	i_basic.c \
	i_callbacks.c \
	i_vars.c \
	m_basic.c \
	o_arc.c \
	o_attrib.c \
	o_basic.c \
	o_box.c \
	o_buffer.c \
	o_bus.c \
	o_circle.c \
	o_complex.c \
	o_copy.c \
	o_delete.c \
	o_find.c \
	o_grips.c \
	o_line.c \
	o_misc.c \
	o_move.c \
	o_net.c \
	o_path.c \
	o_picture.c \
	o_pin.c \
	o_place.c \
	o_select.c \
	o_slot.c \
	o_text.c \
	o_undo.c \
	parsecmd.c \
	s_stretch.c \
	x_aligncb.c \
	x_attribedit.c \
	x_autonumber.c \
	x_basic.c \
	x_clipboard.c \
	x_color.c \
	x_colorcb.c \
	x_compselect.c \
	x_dialog.c \
	x_editcolor.c \
	x_editfprop.c \
	x_editlprop.c \
	x_edittext.c \
	x_event.c \
	x_fileselect.c \
	x_fstylecb.c \
	x_grid.c \
	x_image.c \
	x_integerls.c \
	x_linecapcb.c \
	x_linetypecb.c \
	x_log.c \
	x_menus.c \
	x_misc.c \
	x_multiattrib.c \
	x_newtext.c \
	x_pagesel.c \
	x_preview.c \
	x_print.c \
	x_rc.c \
	x_rotatecb.c \
	x_script.c \
	x_stroke.c \
	x_window.c

gschem_CPPFLAGS = -I$(top_srcdir)/libgeda/include  -I$(srcdir)/../include \
	-I$(top_srcdir) -I$(includedir) -I$(top_srcdir)/intl
gschem_CFLAGS = $(GCC_CFLAGS) $(LIBSTROKE_CFLAGS) \
	$(MINGW_CFLAGS) \
	$(GLIB_CFLAGS) $(GTK_CFLAGS) $(GTHREAD_CFLAGS) $(GUILE_CFLAGS)
gschem_LDFLAGS = $(LIBSTROKE_LDFLAGS) $(GLIB_LIBS) $(GTK_LIBS) \
	$(GTHREAD_LIBS) $(GUILE_LIBS) $(MINGW_GUI_LDFLAGS)
gschem_LDADD = \
	$(top_builddir)/libgedacairo/libgedacairo.la \
	$(top_builddir)/libgeda/src/libgeda.la \
	@LIBINTL@


# This is used to generate boilerplate for defining Scheme functions
# in C.
SUFFIXES = .x
snarf_cpp_opts = $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(gschem_CPPFLAGS) $(AM_CFLAGS) $(gschem_CFLAGS)
.c.x:
	CPP="$(CPP)" $(GUILE_SNARF) -o $@ $< $(snarf_cpp_opts)

localedir = @datadir@/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

MOSTLYCLEANFILES = *.log core FILE *~
CLEANFILES = *.log core FILE *~ $(BUILT_SOURCES)
DISTCLEANFILES = *.log core FILE *~
MAINTAINERCLEANFILES = *.log core FILE *~ Makefile.in
