#include "data.h"
#include "gui.h"

void null_char_buffer(char* buf, u_int32_t size) {
    for(u_int32_t i = 0; i < size; i++) {
        buf[i] = '\0';
    }
}

void exit_w_one_on_failed_xalloc(void* ptr)
{ 
    if(ptr == NULL) {
        printf("Allocing memory failed!\n");
        exit(1);
    }
}

void exit_w_one_on_failed_fopen(FILE* fptr)
{
    if(!fptr) {
        printf("Opening file failed!\n");
        exit(1);
    }
}

void free_valid_ptr(void* ptr_to_free) {
    if(ptr_to_free) {
        free(ptr_to_free);
    }
}

char* strcat_heap_strs(char* str1, char* str2)
{
    u_int32_t len_str1 = strlen(str1);
    u_int32_t len_str2 = strlen(str2);

    char* dst = malloc((len_str1 + len_str2 + 2) * sizeof(char));
    exit_w_one_on_failed_xalloc(dst); 

    char* startpos = dst;

    null_char_buffer(dst, (len_str1 + len_str2 + 1));

    for(u_int32_t i = 0; i < len_str1; i++) {
        *dst = str1[i];
        dst++;
    }

    for(u_int32_t j = 0; j < len_str2 + 1; j++) { 
        *dst = str2[j];
        dst++;
    }

    dst = startpos;
    return dst;
}

char* delimit_unnullified_str(char* pseudo_str, u_int32_t len) 
{
    /* len is the length of the string to delimit. DO NOT ACCOUNT FOR THE NULL-DELIMITER, YET!*/
    char* proper_str = malloc((len + 1) * sizeof(char));
    exit_w_one_on_failed_xalloc(proper_str);

    for(u_int32_t i = 0; i < len; i++) {
        proper_str[i] = pseudo_str[i];
    }
    
    proper_str[len] = '\0';
    
    return proper_str;  
}

char* make_str_from_char(char c) 
{
    char* str = malloc(2 * sizeof(char));
    exit_w_one_on_failed_xalloc(str);
    
    str[0] = c;
    str[1] = '\0';
    
    return str;
}

bool csv_check_empty_line(char* cur_line)
{   
    if((cur_line[0] == '\n') && (cur_line[1] == '\0')) {
        printf("Skipping empty line!\n");
        return true;
    }
    return false;
}

bool csv_check_header(char* cur_line)
{
    if(cur_line[0] == 'i' && cur_line[1] == 'd' && cur_line[2] == ';') {
        printf("Ignoring header line!\n");
        return true;
    }  
    return false;
}

void print_deck(flashcard* flashcards, u_int32_t cards_to_load)
{
    for(u_int8_t i = 0; i < cards_to_load; i++) {
        printf("Flashcard #%u: \n", i + 1);
        printf("%u\n",   flashcards[i].id);
        printf("%s\n",   flashcards[i].front_content);
        printf("%s\n",   flashcards[i].back_content);
        printf("%s\n\n", flashcards[i].front_sound_file_path);
    }
}

controller* malloc_min_mem_for_controller_and_members_first_load(widget_pool* wp)
{
    /**
    *  Alloc the minimum of memory needed for user_data/controller 
    *  To give program a valid entry point. 
    **/
    controller* user_data                    = malloc(1 * sizeof(controller));
    exit_w_one_on_failed_xalloc(user_data);
    user_data->deck                          = malloc(1 * sizeof(flashcard));
    exit_w_one_on_failed_xalloc(user_data->deck);
    user_data->size                          = 1;
    user_data->deck[0].front_content         = malloc(3 * sizeof(char));
    exit_w_one_on_failed_xalloc(user_data->deck[0].front_content);
    null_char_buffer(user_data->deck[0].front_content, 3);
    user_data->deck[0].back_content          = malloc(3 * sizeof(char));
    exit_w_one_on_failed_xalloc(user_data->deck[0].back_content);
    null_char_buffer(user_data->deck[0].back_content, 3);
    user_data->deck[0].front_sound_file_path = malloc(3 * sizeof(char));
    exit_w_one_on_failed_xalloc(user_data->deck[0].front_sound_file_path);
    null_char_buffer(user_data->deck[0].front_sound_file_path, 3);

    user_data->wp                            = wp;
    
    return user_data;
}

void realloc_mem_for_deck_controller_and_members_after_csv_file_opened(u_int32_t cards_to_load, controller* user_data, u_int16_t** bytes_per_card_per_col)
{
    user_data->deck = realloc(user_data->deck, (cards_to_load * sizeof(flashcard)));

    for(u_int32_t i = 0; i < cards_to_load; i++) {
        u_int16_t* sizes_of_columns_of_this_line = bytes_per_card_per_col[i];

        if(i == 0) {
            /* Starting at 0 (0 would be id column we do not care about): Front side is column 1, back side is 2 and sound file name is 3 */
            //printf("malloc_mem_for_deck_controller_and_members_after_csv_file_opened(): Will malloc %u bytes for line/card %u for front_content column!\n", sizes_of_columns_of_this_line[1], i + 1);
            user_data->deck[i].front_content = realloc(user_data->deck[i].front_content, (sizes_of_columns_of_this_line[1] * sizeof(char)));
            exit_w_one_on_failed_xalloc(user_data->deck[i].front_content);
            null_char_buffer(user_data->deck[i].front_content, sizes_of_columns_of_this_line[1]);

            user_data->deck[i].back_content = realloc(user_data->deck[i].back_content, (sizes_of_columns_of_this_line[2] * sizeof(char)));
            exit_w_one_on_failed_xalloc(user_data->deck[i].back_content);
            null_char_buffer(user_data->deck[i].back_content, sizes_of_columns_of_this_line[2]);
            user_data->deck[i].front_sound_file_path = realloc(user_data->deck[i].front_sound_file_path, (sizes_of_columns_of_this_line[3] * sizeof(char)));

            exit_w_one_on_failed_xalloc(user_data->deck[i].front_sound_file_path);
            null_char_buffer(user_data->deck[i].front_sound_file_path, sizes_of_columns_of_this_line[3]);
        } else {
            user_data->deck[i].front_content = malloc(sizes_of_columns_of_this_line[1] * sizeof(char));;
            exit_w_one_on_failed_xalloc(user_data->deck[i].front_content);
            null_char_buffer(user_data->deck[i].front_content, sizes_of_columns_of_this_line[1]);

            user_data->deck[i].back_content = malloc(sizes_of_columns_of_this_line[2] * sizeof(char));
            exit_w_one_on_failed_xalloc(user_data->deck[i].back_content);
            null_char_buffer(user_data->deck[i].back_content, sizes_of_columns_of_this_line[2]);

            user_data->deck[i].front_sound_file_path = malloc(sizes_of_columns_of_this_line[3] * sizeof(char));
            exit_w_one_on_failed_xalloc(user_data->deck[i].front_sound_file_path);
            null_char_buffer(user_data->deck[i].front_sound_file_path, sizes_of_columns_of_this_line[3]);
        }
    }
}

void free_controller(controller* ctrler)
{
    for(u_int8_t i = 0; i < ctrler->size; i++) {
        free_valid_ptr(ctrler->deck[i].front_content);
        free_valid_ptr(ctrler->deck[i].back_content);
        free_valid_ptr(ctrler->deck[i].front_sound_file_path);
    }
    free_valid_ptr(ctrler->wp);
    free_valid_ptr(ctrler->deck);
    free_valid_ptr(ctrler);
}

void free_csv_file_info(csv_fileinfo* cfi)
{
    /* Not needed anymore after all the data is inside struct user_data */
    /* So all texts can be rendered by GUI and all sound filenames are known */
    u_int32_t detected_num_cards = cfi->lines_minus_one_aka_cards;
    free_valid_ptr(cfi->bytes_per_line);

    for(u_int32_t i = 0; i < detected_num_cards; i++) {
        u_int32_t reverse_index = ((i - detected_num_cards) * -1) - 1;
        free_valid_ptr((cfi->bytes_per_line_per_col)[reverse_index]);
    }

    for(u_int32_t j = 0; j < detected_num_cards; j++) {
        u_int32_t reverse_index = ((j - detected_num_cards) * -1) - 1;
        free_valid_ptr((cfi->content)[reverse_index]);
    }
}

void free_mem_after_gtk_exits(controller* ctrler)
{
    printf("Freeing csv metadata!\n");
    free_csv_file_info(ctrler->csv_fileinfo);
    printf("Done! Now freeing application data!\n");
    free_controller(ctrler);
    printf("Done all user data freed!\n");
}

void get_csv_file_info(char* path_to_csv, bool is_deck_reload, csv_fileinfo* cfi)
{
    printf("Analyzing CSV file properties...\n");
    printf("Counting all lines but header. Each line = 1 card!\n");

    /* First we count the cards */
    char* line_buffer = malloc(MAX_CSV_FILE_SIZE * sizeof(char));
    exit_w_one_on_failed_xalloc(line_buffer);

    FILE* fp = fopen(path_to_csv, "r");
    exit_w_one_on_failed_fopen(fp);

    u_int32_t card_counter = 0;
    char*     cur_line     = NULL;

    while(cur_line = fgets(line_buffer, MAX_CSV_FILE_SIZE, fp)) {
        /* Ignore the header line and empty line */
        if(csv_check_header(cur_line) || csv_check_empty_line(cur_line)) continue;
        card_counter++;
    }
    fclose(fp);
        
    cfi->lines_minus_one_aka_cards = card_counter;
    
    /** 
    * The result for number of cards was very important to get a loop variable 
    * This will healp us to get all the rest of the info we need much more easily
    * Now for getting the filesize, the size of each line and to copy the content 
    * into our struct we need to read the file one last time 
    **/
    fp = fopen(path_to_csv, "r");
    exit_w_one_on_failed_fopen(fp);
    
    cur_line = NULL;

    u_int32_t detected_num_cards = cfi->lines_minus_one_aka_cards;
    u_int32_t byte_counter = 0;

    /* Lets malloc the string array for the content, each element being one line/card */
    char** content = NULL;
    if(!is_deck_reload) {
        content = malloc(detected_num_cards * sizeof(char*));
        exit_w_one_on_failed_xalloc(content);
    } else {
        content = cfi->content;
    }
        
    /* One line has at least 10 bytes: 4 semicolons, 1 newline, 1 null-delimiter, and 1 char in each column */
    /* Realloc later when known how many bytes are definitely needed*/
    for (u_int32_t i = 0; i < detected_num_cards; i++) {
        if(!is_deck_reload) {
            content[i] = malloc(10 * sizeof(char));
            exit_w_one_on_failed_xalloc(content[i]);
            null_char_buffer(content[i], 10);
        }
    }

    /* Malloc memory for the u_int16_t array that will store an int for each line's bytesize */
    u_int16_t* map_bytesize_per_line = NULL;

    if(!is_deck_reload) {
        map_bytesize_per_line = malloc(detected_num_cards * sizeof(u_int16_t));
        exit_w_one_on_failed_xalloc(map_bytesize_per_line);
    } else {
        map_bytesize_per_line = cfi->bytes_per_line;
    }
       
    /* Properly initialize all values to 0 */
    for(u_int16_t j = 0; j < detected_num_cards; j++) {
        map_bytesize_per_line[j] = 0;
    }

    /* card_counter + 1 because we need to account for the header line, which we'll skip :D */
    for(u_int32_t j = 0; j < detected_num_cards + 1; j++) {
        cur_line = fgets(line_buffer, MAX_CSV_FILE_SIZE, fp);

        if(csv_check_header(cur_line) || csv_check_empty_line(cur_line)) continue;
        u_int16_t cur_line_len = strlen(cur_line);
        map_bytesize_per_line[j - 1] = cur_line_len + 1;

        for(u_int16_t k = 0; k < cur_line_len + 1; k++) {
            if(k <= 10) {                
                content[j - 1][k] = cur_line[k];
            } else {
                content[j - 1] = realloc(content[j - 1], k + 1);
                printf("After realloc for bytesize_per_line_array at line/card %u: %u\n", j, j - 1);
                exit_w_one_on_failed_xalloc(content[j - 1]);
                content[j - 1][k] = cur_line[k];
            }

            if(k == cur_line_len) content[j - 1][k + 1] = '\0';
            byte_counter++;
        }
    }
    fclose(fp);
    free_valid_ptr(line_buffer);

    /* Card counter equals the missing bytes for the new lines we need to add */
    /* TODO: Find out why I am always 4 bytes off (too much) */
    cfi->real_size_in_bytes = byte_counter + detected_num_cards + STD_STR_SIZE_CSV_HEADER - 4;

    if(!is_deck_reload) {
        cfi->bytes_per_line     = map_bytesize_per_line;
        cfi->content            = content;
    }

    printf("Bytes counted without header:     %u\n", byte_counter);
    printf("Accounted for linebreaks (added): %u\n", detected_num_cards);
    printf("Added fixed size of header (51):  %u\n", STD_STR_SIZE_CSV_HEADER);
    printf("Total real file size in bytes:    %u\n", cfi->real_size_in_bytes);

    for(u_int16_t l = 0; l < detected_num_cards; l++) {
        printf("Line %u is %u bytes in size!\n", l + 1, cfi->bytes_per_line[l]);
    }
    printf("\n");
    /**
    * Round number 3: We do not need to open the csv file anymore:
    * Because we stored it contents in our struct.
    * Let's get the size of each column in each line now
    * For that need a an array of u_int16_t arrays. It will be detected_num_cards x columns in csv!
    **/
    u_int16_t** map_bytesize_per_line_per_col = NULL;

    if(!is_deck_reload) {
        map_bytesize_per_line_per_col = malloc(detected_num_cards * sizeof(u_int16_t*));
        exit_w_one_on_failed_xalloc(map_bytesize_per_line_per_col);
    } else {
        map_bytesize_per_line_per_col = cfi->bytes_per_line_per_col;
    }

    /* Properly initialize all values to 0 */
    for(u_int32_t m = 0; m < detected_num_cards; m++) {
        if(!is_deck_reload)  {
            map_bytesize_per_line_per_col[m] = malloc(COLUMNS_IN_CSV * sizeof(u_int16_t));
            exit_w_one_on_failed_xalloc(map_bytesize_per_line_per_col[m]);
        }
        
        for(u_int32_t n = 0; n < COLUMNS_IN_CSV; n++) {
            map_bytesize_per_line_per_col[m][n] = 0;
        }        
    }

    /* Let's go through each line/card and for each column count the bytes */
    /* We detect column by looking for the semicolon seperator. Last column then needs an extra loop */
    for(u_int32_t o = 0; o < detected_num_cards; o++) {
        u_int8_t column_counter_this_line = 0;
        u_int8_t byte_counter_this_column = 0;
        for(u_int16_t p = 0; p < cfi->bytes_per_line[o]; p++) {
            if(content[o][p] == ';') {
                map_bytesize_per_line_per_col[o][column_counter_this_line] = byte_counter_this_column;
                column_counter_this_line++;
                byte_counter_this_column = 0;
                if(column_counter_this_line == COLUMNS_IN_CSV - 1) {
                    char* ptr_cur_char = &(content[o][p]);
                    while(*ptr_cur_char != '\0') {
                        ptr_cur_char++;
                        byte_counter_this_column++;
                    }
                    map_bytesize_per_line_per_col[o][column_counter_this_line] = byte_counter_this_column;
                }
            }
            byte_counter_this_column++;
        }
    }

    if(!is_deck_reload)
        cfi->bytes_per_line_per_col = map_bytesize_per_line_per_col;
}

void load_flashcard_deck(controller* ctrler)
{
    printf("\nLoading cards from CSV file into program...\n");
    u_int32_t   detected_num_cards    = ctrler->csv_fileinfo->lines_minus_one_aka_cards;
    //u_int16_t*  size_per_line         = ctrler->csv_fileinfo->bytes_per_line;
    u_int16_t** size_per_line_per_col = ctrler->csv_fileinfo->bytes_per_line_per_col;
    char**      content               = ctrler->csv_fileinfo->content;
    flashcard*  flashcards            = ctrler->deck;

    for(u_int32_t i = 0; i < detected_num_cards; i++) {
        char* str_this_card_this_val_delimited = NULL;
        for(u_int8_t j = 0; j < COLUMNS_IN_CSV; j++) {
            if(j == 0) {
                char* id_str = "";
                u_int16_t num_of_digits_in_id = size_per_line_per_col[i][0];
                if(num_of_digits_in_id == 1) {
                    printf("Card %u has a single digit id which is: %c!\n", i, content[i][0]);
                    id_str = make_str_from_char(content[i][0]);
                } else if(num_of_digits_in_id > 1) {
                    id_str = delimit_unnullified_str(&(content[i][0]), num_of_digits_in_id);
                    printf("Card %u has a %u digit id which is: %s!\n", i, num_of_digits_in_id, id_str);
                }
                flashcards[i].id = (u_int16_t)atoi(id_str);
            } else if(j == 1) {
                str_this_card_this_val_delimited = delimit_unnullified_str(&(content[i][size_per_line_per_col[i][0] + 1]), size_per_line_per_col[i][1] - 1);
                flashcards[i].front_content = str_this_card_this_val_delimited;
            } else if(j == 2) {
                str_this_card_this_val_delimited = delimit_unnullified_str(&(content[i][size_per_line_per_col[i][1] + size_per_line_per_col[i][0] + 1]), size_per_line_per_col[i][2] - 1);
                flashcards[i].back_content = str_this_card_this_val_delimited;
            } else if(j == 3) {
                str_this_card_this_val_delimited = delimit_unnullified_str(&(content[i][size_per_line_per_col[i][2] + size_per_line_per_col[i][1] + 2]), size_per_line_per_col[i][3]);
                flashcards[i].front_sound_file_path = str_this_card_this_val_delimited;
            }
        }
    }
    printf("All cards loaded successfully!\n\n");
}
