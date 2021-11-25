#include "data.h"
#include "gui.h"

static void activate(GtkApplication* app)
{
    widget_pool* wp = initialize_widgets_to_null();
    instantiate_widgets(wp, app);
    
    controller* user_data = malloc_min_mem_for_controller_and_members_first_load(wp);
    connect_early_signals_needed_before_window_shown(wp, user_data);
    
    load_css_for_widgets(wp);
    h_v_align_and_expand_for_widgets_representing_the_flashcard(wp);
    fill_and_align_grid(wp);
    
    /* Grid will be the only child of the window and label will be a child of frame*/
    gtk_aspect_frame_set_child(GTK_ASPECT_FRAME(wp->frame), wp->label);
    gtk_window_set_child(GTK_WINDOW(wp->window), wp->grid);
    
    /* Finally eender our GUI by showing th top level window */
    gtk_widget_show(wp->window);         
}

int main(int argc, char **argv)
{
    /* Initialize GTK GUI application*/    
    GtkApplication* app = NULL;    
    int status = 0;
    
    app = gtk_application_new("de.tpsoft.flashcards", G_APPLICATION_FLAGS_NONE);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    /* We exited the GUI: Free memory and return to exit the program */
    g_object_unref(app);

    return status;
}