#include "pebble.h"
#include "stdio.h"

#define NUM_MENU_MAIN_SECTIONS 2
#define NUM_MENU_ICONS 3
#define NUM_FIRST_MENU_ITEMS 2
#define NUM_MENU_MAIN_SECOND_SECTION_ITEMS 2

char debug_str[10];

static Window *main_window;
static Window *window_about;
static Window *window_schedule;

static MenuLayer *main_menu_layer;
static MenuLayer *schedule_menu_layer;
static TextLayer *text_layer_about;

// Menu items can optionally have an icon drawn with them
//static GBitmap *menu_icons[NUM_MENU_ICONS];

static char* menu_main_headers[2] = {"Select direction", "Information"};

#define NUM_MENU_SCHEDULE_SECTIONS 2
#define WINDOW_SCHEDULE_DIRECTION_FROM_BC 0
#define WINDOW_SCHEDULE_DIRECTION_TO_BC 1
static int window_schedule_active_direction;

struct {
    char title[2][30];
    char subtitle[2][30];
}direction_menu[2] = {
    {
        {"From Office", "To Office"},
        {"BC Protasov -> Subway", "Subway -> BC Protasov"}
    },
    {
        {"Settings", "About"},
        {"",""}
    }
};

struct {
    char* header[30];
    char* time[2][20];
    int size[2];
}schedule_array[2] = {
    {
        {"To 'Palats Ukraina'", "To 'Druzhby Narodiv'"},
        {
            {"11:45", "12:45", "13:45", "14:45", "15:45", "17:05", "17:35", "18:00", "18:10", "18:15", "18:30", "18:40", "19:00", "19:20", "19:40", "20:35", "21:00"},
            {"18:50", "20:05", "21:30"}
        },
        {17, 3}
    },
    {
        {"From 'Palats Ukraina'", "From 'Druzhby Narodiv'"},
        {
            {"7:45", "8:20", "8:40", "8:50", "9:00", "9:10", "9:30", "9:40", "9:50", "10:00", "10:10", "10:20", "10:30", "11:00", "11:30", "12:00", "13:00", "14:00", "15:00", "16:00"},
            {"8:45", "9:35"}
        },
        {20, 2}
    }
};

char time_str[] = "00:00";
int last_used_direction = 0, last_used_section = 0;

static uint16_t menu_main_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    return NUM_MENU_MAIN_SECTIONS;
}

static uint16_t menu_schedule_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    return NUM_MENU_SCHEDULE_SECTIONS;
}

static uint16_t menu_main_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    switch (section_index) {
        case 0:
            return NUM_FIRST_MENU_ITEMS;
        case 1:
            return NUM_MENU_MAIN_SECOND_SECTION_ITEMS;
        default:
            return 0;
    }
}

static uint16_t menu_schedule_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    //return sizeof schedule_array[window_schedule_active_direction].time[section_index];
    return schedule_array[window_schedule_active_direction].size[section_index];
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_main_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    menu_cell_basic_header_draw(ctx, cell_layer, menu_main_headers[section_index]);
}

static void menu_schedule_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    menu_cell_basic_header_draw(ctx, cell_layer, schedule_array[window_schedule_active_direction].header[section_index]);
}

static void menu_main_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    char* title = direction_menu[cell_index->section].title[cell_index->row];
    char* subtitle = direction_menu[cell_index->section].subtitle[cell_index->row];
    if (strcmp(subtitle, "") == 0) {
        subtitle = NULL;
    }
    menu_cell_basic_draw(ctx, cell_layer, title, subtitle, NULL);
}

static void menu_schedule_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    menu_cell_basic_draw(ctx, cell_layer, schedule_array[window_schedule_active_direction].time[cell_index->section][cell_index->row], NULL, NULL);
}

int get_time_int(char *time) {
    char* t = malloc(5);
    int pos, time_int, length = strlen(time);
    strcpy(t, time);
    pos = strcspn(t, ":");
    memmove(t + pos, t + pos + 1, length - pos + 1);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, t);
    time_int = atol(t);
    return time_int;
}

void select_nearest() {
    clock_copy_time_string(time_str, sizeof("00:00"));
    //strcpy(time_str, "20:05");
    int time_int, schedule_int, i, row_index = 0, time_prev = 0, size = schedule_array[window_schedule_active_direction].size[last_used_section];
    time_int = get_time_int(time_str);
    /*APP_LOG(APP_LOG_LEVEL_DEBUG, "TIME_STR");
    APP_LOG(APP_LOG_LEVEL_DEBUG, time_str);
    snprintf(debug_str, 10, "%d", time_int);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "TIME_INT");
    APP_LOG(APP_LOG_LEVEL_DEBUG, debug_str);*/
    
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "TIMES");
    
    if (last_used_direction != window_schedule_active_direction) {
        last_used_direction = window_schedule_active_direction;
        last_used_section = 0;
    }
    
    for (i = 0; i < size; i++) {
        schedule_int = get_time_int(schedule_array[window_schedule_active_direction].time[last_used_section][i]);
        
        /*snprintf(debug_str, 10, "%d", time_int);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "TIME_INT");
        APP_LOG(APP_LOG_LEVEL_DEBUG, debug_str);
        snprintf(debug_str, 10, "%d", schedule_int);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "SCHEDULE_INT");
        APP_LOG(APP_LOG_LEVEL_DEBUG, debug_str);*/
        
        if (i != 0 && time_int < time_prev) {
            
            /*snprintf(debug_str, 10, "%d", time_prev);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "TIME_PREV");
            APP_LOG(APP_LOG_LEVEL_DEBUG, debug_str);*/
            
            row_index = i - 1;
            break;
        } else {
            if (i != size -1) {
                time_prev = schedule_int;
            } else {
                if (time_int < schedule_int) {
                    row_index = i;
                } else {
                    row_index = 0;
                }
            }
        }
    }
    
    menu_layer_set_selected_index(schedule_menu_layer, MenuIndex(last_used_section, row_index), MenuRowAlignCenter, true);
    
    if (last_used_section == 0) {
        last_used_section = 1;
    } else {
        last_used_section = 0;
    }
}

void menu_schedule_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    select_nearest();
}

/**
 * Schedule window
 * @param window
 */
void window_schedule_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    schedule_menu_layer = menu_layer_create(bounds);
    menu_layer_set_callbacks(schedule_menu_layer, NULL, (MenuLayerCallbacks) {
        .get_num_sections = menu_schedule_get_num_sections_callback,
        .get_num_rows = menu_schedule_get_num_rows_callback,
        .get_header_height = menu_get_header_height_callback,
        .draw_header = menu_schedule_draw_header_callback,
        .draw_row = menu_schedule_draw_row_callback,
        .select_click = menu_schedule_select_callback,
    });
    menu_layer_set_click_config_onto_window(schedule_menu_layer, window);
    layer_add_child(window_layer, menu_layer_get_layer(schedule_menu_layer));
}

void window_schedule_unload(Window *window) {
    tick_timer_service_unsubscribe();
    menu_layer_destroy(schedule_menu_layer);
    window_destroy(window);
}

static void open_window_schedule(char window_schedule_direction) {
    window_schedule_active_direction = window_schedule_direction;
    window_schedule = window_create();
    window_set_window_handlers(window_schedule, (WindowHandlers) {
        .load = window_schedule_load,
        .unload = window_schedule_unload,
    });
    window_stack_push(window_schedule, true);
}


/**
 * About window
 * @param window
 */
void window_about_load(Window *window) {
    Layer *window_about_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_about_layer);
    text_layer_about = text_layer_create(bounds);
    text_layer_set_text(text_layer_about, "2014 Alexei Koliada");
    text_layer_set_text_alignment(text_layer_about, GTextAlignmentCenter);
    layer_add_child(window_about_layer, text_layer_get_layer(text_layer_about));
}

void window_about_unload(Window *window) {
    text_layer_destroy(text_layer_about);
    window_destroy(window);
}

static void open_window_about() {
    window_about = window_create();
    window_set_window_handlers(window_about, (WindowHandlers) {
        .load = window_about_load,
        .unload = window_about_unload,
    });
    window_stack_push(window_about, true);
}


void menu_main_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    switch (cell_index->section) {
        case 0:
            open_window_schedule(cell_index->row);
            break;
        case 1:
            switch (cell_index->row) {
                case 0:
                    break;
                case 1:
                    open_window_about();
                    break;
            }
    }
}

/**
 * Main window
 * @param window
 */
void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    main_menu_layer = menu_layer_create(bounds);
    menu_layer_set_callbacks(main_menu_layer, NULL, (MenuLayerCallbacks) {
        .get_num_sections = menu_main_get_num_sections_callback,
        .get_num_rows = menu_main_get_num_rows_callback,
        .get_header_height = menu_get_header_height_callback,
        .draw_header = menu_main_draw_header_callback,
        .draw_row = menu_main_draw_row_callback,
        .select_click = menu_main_select_callback,
    });
    menu_layer_set_click_config_onto_window(main_menu_layer, window);
    layer_add_child(window_layer, menu_layer_get_layer(main_menu_layer));
}

void window_unload(Window *window) {
    menu_layer_destroy(main_menu_layer);
}

int main(void) {
    main_window = window_create();
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    window_stack_push(main_window, true);

    app_event_loop();

    window_destroy(main_window);
}