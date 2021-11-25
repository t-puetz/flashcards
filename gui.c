#include <malloc.h>
#include "gui.h"

widget_pool* initialize_widgets_to_null()
{
    widget_pool* wp     = malloc(1 * sizeof(widget_pool));
    exit_w_one_on_failed_xalloc(wp);
    wp->filechooser     = malloc(1 * sizeof(GtkWidget));
    wp->filechooser_btn = malloc(1 * sizeof(GtkWidget));
    wp->flip_btn        = malloc(1 * sizeof(GtkWidget));
    wp->frame           = malloc(1 * sizeof(GtkWidget));
    wp->fwd_btn         = malloc(1 * sizeof(GtkWidget));
    wp->grid            = malloc(1 * sizeof(GtkWidget));
    wp->label           = malloc(1 * sizeof(GtkWidget));
    wp->rev_btn         = malloc(1 * sizeof(GtkWidget));
    wp->snd_btn         = malloc(1 * sizeof(GtkWidget));
    wp->window          = malloc(1 * sizeof(GtkWidget));

    return wp;
}

void instantiate_widgets(widget_pool* wp, GtkApplication* app)
{
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    /* Now instantiate all the widgets. */
    wp->window                      = instantiate_top_lvl_window(wp, app);
    wp->grid                        = gtk_grid_new();
    wp->frame                       = gtk_aspect_frame_new(0.5, 0.5, 1, 1);
    wp->flip_btn                    = gtk_button_new_with_label("FLIP");
    wp->rev_btn                     = gtk_button_new_with_label("<<");
    wp->fwd_btn                     = gtk_button_new_with_label(">>");
    wp->snd_btn                     = gtk_button_new_with_label("🔊");
    wp->label                       = gtk_label_new("No data");
    wp->filechooser_btn             = (GtkWidget*)gtk_file_chooser_button_new("Open File", action);
}

GtkWidget* instantiate_top_lvl_window(widget_pool* wp, GtkApplication* app)
{
    /* Setup most rudimentary parameters for our GUI: get a window, set title and resolution*/
    wp->window = gtk_application_window_new(app);

    char* win_title_pre = strcat_heap_strs("Flashcards ", VERSION_W_PREFIX);
    char* win_title = strcat_heap_strs(win_title_pre, " - Memorize anything");

    gtk_window_set_title(GTK_WINDOW(wp->window),        win_title);
    gtk_window_set_default_size(GTK_WINDOW(wp->window), WIDTH, HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(wp->window),    FALSE);

    return wp->window;
}

static void next_card(controller* user_data)
{
    if(user_data->cur_card == 0 || user_data->cur_card < user_data->size)
        gtk_widget_set_sensitive(user_data->wp->rev_btn, TRUE);

    if(user_data->cur_card >= user_data->size - 1) {
        gtk_widget_set_sensitive(user_data->wp->fwd_btn, FALSE);
    } else {
        user_data->cur_card++;
    }
    char* front_content_cur_card = user_data->deck[user_data->cur_card].front_content;
    render_backslash_n_from_csv_as_newline(front_content_cur_card, user_data->wp->label);
}

static void prev_card(controller* user_data)
{
    if(user_data->cur_card == 0 || user_data->cur_card < user_data->size)
        gtk_widget_set_sensitive(user_data->wp->fwd_btn, TRUE);

    if(user_data->cur_card == 0) {
        gtk_widget_set_sensitive(user_data->wp->rev_btn, FALSE);
    } else {
        user_data->cur_card--;
    }
    char* front_content_cur_card = user_data->deck[user_data->cur_card].front_content;
    render_backslash_n_from_csv_as_newline(front_content_cur_card, user_data->wp->label);
}

static void flip_card(controller* user_data)
{
    char*     front_content_cur_card = user_data->deck[user_data->cur_card].front_content;
    char*     back_content_cur_card  = user_data->deck[user_data->cur_card].back_content;
    GtkLabel* flashcard_label        = GTK_LABEL(user_data->wp->label);
    bool      front_is_being_shown   = ((strcmp((char*)gtk_label_get_text(flashcard_label), (char*)g_strcompress(front_content_cur_card))) == 0);

    if(front_is_being_shown)
        render_backslash_n_from_csv_as_newline(back_content_cur_card, user_data->wp->label);
    else
        render_backslash_n_from_csv_as_newline(front_content_cur_card, user_data->wp->label);
}

static void play_sound_front(controller* user_data)
{
    char* snd_file = user_data->deck[user_data->cur_card].front_sound_file_path;

    if(fopen(snd_file, "r")) {
        char* ffplay_cmd = strcat_heap_strs("ffplay -nodisp -vn -autoexit ", user_data->deck[user_data->cur_card].front_sound_file_path);
        system(ffplay_cmd);
    } else {
        printf("No sound file for this card found!\n");
    }
}

static void on_response(GtkFileChooserButton* filechooser_btn, controller* user_data)
{
    static u_int16_t user_loaded_decks_times = 0;

    GtkFileChooser* chooser    = GTK_FILE_CHOOSER(filechooser_btn);

    if(!user_loaded_decks_times)
        user_data->wp->filechooser = GTK_WIDGET(chooser);

    GFile* file                = gtk_file_chooser_get_file(chooser);
    char*  filename_fullpath   = g_file_get_path(file);
    //char* filename_only      = g_file_get_basename(file);

    /* File was opened and we got the filename, let's load the real user_data */
    /* But first let's analyze the CSV for rudimentary infos like filesize and amoutn of cards */

    if(!user_loaded_decks_times) {
        user_data->csv_fileinfo = malloc(1 * sizeof(csv_fileinfo));
        exit_w_one_on_failed_xalloc(user_data->csv_fileinfo);
        printf("Initial deck load. Malloced fresh csv_fileinfo. Calling get_csv_file_info() now!\n");
        get_csv_file_info(filename_fullpath, user_loaded_decks_times, user_data->csv_fileinfo);
    }
    else {
        printf("This is a new lod. Getting csv_fileinfo anew. This happened %u times!\n", user_loaded_decks_times);
        get_csv_file_info(filename_fullpath, user_loaded_decks_times, user_data->csv_fileinfo);
    }

    u_int32_t   cards_to_load           = user_data->csv_fileinfo->lines_minus_one_aka_cards;
    //u_int32_t   deck_csv_file_real_size = user_data->csv_fileinfo->real_size_in_bytes;
    //u_int16_t*  bytes_per_card          = user_data->csv_fileinfo->bytes_per_line;
    u_int16_t** bytes_per_card_per_val  = user_data->csv_fileinfo->bytes_per_line_per_col;


    if(!user_data->csv_fileinfo)
        render_backslash_n_from_csv_as_newline("Invalid CSV/deck", user_data->wp->label);

    if(user_data->csv_fileinfo->lines_minus_one_aka_cards == 0 || user_data->csv_fileinfo->real_size_in_bytes == 0)
        render_backslash_n_from_csv_as_newline("Invalid CSV/deck", user_data->wp->label);

    /**
    * With this preliminary, basic info we can realloc the application's major
    * struct that holds all info to be self-maintainable (it's basically the app controller)
    * with all its members to their final sizes. Most importantly the array for the flashcards.
    * We also tell it the size of that array and set thew current_card to 0 (so 1)
    * To create an entry point for the navigation buttons.
    **/

    realloc_mem_for_deck_controller_and_members_after_csv_file_opened(cards_to_load, user_data, bytes_per_card_per_val);

    user_data->size = cards_to_load;
    /* Set the first card as the starting point for the GUI rendering */
    user_data->cur_card = 0;

    /* Finally we load our real data: Most importantly create our flashcards and their front and backsides :)*/
    load_flashcard_deck(user_data);

    if(!user_data->deck[0].front_content)
        render_backslash_n_from_csv_as_newline("Invalid CSV/deck", user_data->wp->label);

    /* CSV/deck seems valid. Ready for take off :)*/
    render_backslash_n_from_csv_as_newline(user_data->deck[user_data->cur_card].front_content, user_data->wp->label);

    print_deck(user_data->deck, cards_to_load);

    if(!user_loaded_decks_times) {
        g_signal_connect_swapped(user_data->wp->fwd_btn,  "clicked", G_CALLBACK(next_card),        user_data);
        g_signal_connect_swapped(user_data->wp->flip_btn, "clicked", G_CALLBACK(flip_card),        user_data);
        g_signal_connect_swapped(user_data->wp->rev_btn,  "clicked", G_CALLBACK(prev_card),        user_data);
        g_signal_connect_swapped(user_data->wp->snd_btn,  "clicked", G_CALLBACK(play_sound_front), user_data);
    }
    user_loaded_decks_times++;
}

static void free_mem_on_close_gtk(GtkWindow* window, controller* user_data)
{
    free_mem_after_gtk_exits(user_data);
}

void connect_early_signals_needed_before_window_shown(widget_pool* wp, controller* ctrler)
{
    /**
    *  Let's pass this empty user_data to a function that actually fills it with real data
    *  when the user opens a csv file from the FileChooser
    **/
    g_signal_connect(wp->filechooser_btn, "file-set", G_CALLBACK(on_response),           ctrler);
    g_signal_connect(wp->window,          "destroy",  G_CALLBACK(free_mem_on_close_gtk), ctrler);
}

void load_css_for_widgets(widget_pool* wp)
{
    /**
    * Use CSS mostly to work together with later alignment (halign, valign, hexpand, vexpand)
    * against default grid behavior That condenses all widgets as much as possible. 
    * We want our GUI to breath and to look "natural" 
    **/
    load_css_onto_widget(MAIN_CSS_FILE, wp->grid,     "flashcard-grid-content",     1);
    load_css_onto_widget(MAIN_CSS_FILE, wp->frame,    "flashcard-frame-content",    1);
    load_css_onto_widget(MAIN_CSS_FILE, wp->label,    "flashcard-label-content",    1); 
    load_css_onto_widget(MAIN_CSS_FILE, wp->flip_btn, "flashcard-flip_btn-content", 1); 
    load_css_onto_widget(MAIN_CSS_FILE, wp->fwd_btn,  "flashcard-fwd_btn-content",  1); 
    load_css_onto_widget(MAIN_CSS_FILE, wp->rev_btn,  "flashcard-rev_btn-content",  1); 
    
}

void h_v_align_and_expand_for_widgets_representing_the_flashcard(widget_pool* wp)
{
    /* Arrange and center our widgets */
    /* If we don't do this the grid packing will condense them as much as possible*/
    gtk_widget_set_hexpand(wp->label, TRUE);
    gtk_widget_set_vexpand(wp->label, TRUE);
    gtk_widget_set_hexpand(wp->frame, TRUE);
    gtk_widget_set_vexpand(wp->frame, TRUE);
    
    gtk_widget_set_halign(wp->label, GTK_ALIGN_CENTER);    
    gtk_widget_set_valign(wp->label, GTK_ALIGN_CENTER);    
    gtk_widget_set_halign(wp->frame, GTK_ALIGN_CENTER);    
    gtk_widget_set_valign(wp->frame, GTK_ALIGN_CENTER);
}

void fill_and_align_grid(widget_pool* wp)
{
    /** 
    * grid last 4 parameters: col, row, width, height
    * I basically drew a 6x6 grid on a piece of paper to
    * visualize this better. 6x6 was big enough to nicely 
    * place all the widgets and to put the flashcard in the center
    * and make it the biggest widget of course. However GtkGrid
    * Will condense the widgets as much as possible and leave no
    * room between them in any direction. That is why we work with
    * CSS plus alignment functions
    **/  
    gtk_grid_attach(GTK_GRID(wp->grid), wp->rev_btn,         2, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(wp->grid), wp->flip_btn,        3, 5, 2, 1);
    gtk_grid_attach(GTK_GRID(wp->grid), wp->fwd_btn,         5, 5, 1, 1);    
    gtk_grid_attach(GTK_GRID(wp->grid), wp->frame,           2, 3, 4, 2);
    gtk_grid_attach(GTK_GRID(wp->grid), wp->label,           2, 3, 4, 2);
    gtk_grid_attach(GTK_GRID(wp->grid), wp->filechooser_btn, 6, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(wp->grid), wp->snd_btn,         6, 4, 1, 1);
}

void load_css_onto_widget(const char* path_to_css_file, GtkWidget* widget_to_affect, char* named_style_class, u_int32_t render_priority) 
{
    GtkCssProvider* css_provider = gtk_css_provider_new();
    GtkStyleContext* style_context = gtk_widget_get_style_context(widget_to_affect);
    gtk_style_context_add_class(style_context, named_style_class);
    gtk_css_provider_load_from_path(css_provider, path_to_css_file);
    gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(css_provider), render_priority);    
}

void render_backslash_n_from_csv_as_newline(char* str_to_strcompress, GtkWidget* label_that_renders_the_text)
{
    gchar* new_txt = g_strcompress(str_to_strcompress);
    gtk_label_set_text(GTK_LABEL(label_that_renders_the_text), new_txt);
    g_free(new_txt);
}
