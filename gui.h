#ifndef HLPRFNCTS_H
#define HLPRFNCTS_H

#include <gtk/gtk.h>
                                                                                                                                                                                             
#include <sys/types.h>                                                                                                                                                                                             
#include <fcntl.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include <stdbool.h>
#include "data.h"

#define WIDGETS           10
#define VERSION           "0.0.7"
#define VERSION_W_PREFIX  strcat_heap_strs("v", VERSION)
#define WIDTH             1368
#define HEIGHT            720

void load_css_for_widgets(widget_pool* wp);
void h_v_align_and_expand_for_widgets_representing_the_flashcard(widget_pool* wp);
void fill_and_align_grid(widget_pool* wp);

void load_css_onto_widget(const char* path_to_css_file, GtkWidget* widget_to_affect, char* named_style_class, u_int32_t render_priority);
void render_backslash_n_from_csv_as_newline(char* str_to_strcompress, GtkWidget* label_that_renders_the_text);

widget_pool*          initialize_widgets_to_null();
void                  instantiate_widgets(widget_pool* wp,GtkApplication* app);
GtkWidget*            instantiate_top_lvl_window(widget_pool* wp, GtkApplication* app);
void                  connect_early_signals_needed_before_window_shown(widget_pool* wp, controller* ctrler);



#endif /* HLPRFNCTS_H */

