#ifndef DATA_H
#define DATA_H

#include <gtk/gtk.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAIN_CSS_FILE                "./flashcard.css"
#define MAX_CSV_FILE_SIZE            1048576
#define STD_STR_SIZE_CSV_HEADER      51
#define COLUMNS_IN_CSV               4

typedef struct widget_pool
{
    GtkWidget* window;
    GtkWidget* grid;
    GtkWidget* label;
    GtkWidget* frame;
    GtkWidget* fwd_btn;
    GtkWidget* rev_btn;
    GtkWidget* flip_btn;
    GtkWidget* snd_btn;
    GtkWidget* filechooser_btn;
    GtkWidget* filechooser;
}widget_pool;

typedef struct csv_fileinfo
{
    /* Header line is skipped and each line represents a card */
    u_int32_t   lines_minus_one_aka_cards;
    /* Hard maximum size is 1MB */
    u_int32_t   real_size_in_bytes;
    u_int16_t*  bytes_per_line;
    u_int16_t** bytes_per_line_per_col;
    char**      content;

}csv_fileinfo;


typedef struct flashcard 
{
    u_int16_t id;
    char*     front_content;
    char*     back_content;
    char*     front_sound_file_path;
}flashcard;

typedef struct controller
{
    csv_fileinfo* csv_fileinfo;
    widget_pool*  wp;
    u_int32_t     size;
    u_int32_t     cur_card;
    flashcard*    deck;
}controller;

void null_char_buffer(char* buf, u_int32_t size);
void exit_w_one_on_failed_xalloc(void* ptr);
void exit_w_one_on_failed_fopen(FILE* fptr);
void free_valid_ptr(void* ptr_to_free);
char* strcat_heap_strs(char* str1, char* str2);
char* delimit_unnullified_str(char* str, u_int32_t len);
char* make_str_from_char(char c);

bool          csv_check_empty_line(char* cur_line);
bool          csv_check_header(char* cur_line);
void          print_deck(flashcard* flashcards, u_int32_t cards_to_count);
controller*   malloc_min_mem_for_controller_and_members_first_load();
void          realloc_mem_for_deck_controller_and_members_after_csv_file_opened(u_int32_t cards_to_load, controller* user_data, u_int16_t** bytes_per_card_per_col);
void          free_controller(controller* deck);
void          free_csv_file_info(csv_fileinfo* cfi);
void          free_mem_after_gtk_exits(controller* ctrler);
void          get_csv_file_info(char* path_to_csv, bool is_first_call, csv_fileinfo* cfi);
void          load_flashcard_deck(controller* ctrler);
#endif /* DATA_H */

