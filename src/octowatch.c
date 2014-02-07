#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

static TextLayer *file_label;
static TextLayer *file_layer;
static TextLayer *temp_label;
static TextLayer *temp_layer;
static TextLayer *progress_label;
static TextLayer *progress_layer;

static AppSync sync;
static uint8_t sync_buffer[64];

enum PrintJobKey {
  PRINT_FILE_KEY = 0x0,
  PRINT_TEMP_KEY = 0x1,
  PRINT_PROGRESS_KEY = 0x2,
};

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "got tuple changed callback");
  
  switch (key) {
    case PRINT_FILE_KEY:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "setting file name to: %s", new_tuple->value->cstring);
      text_layer_set_text(file_layer, new_tuple->value->cstring);
      break;

    case PRINT_TEMP_KEY:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "setting temp to: %s", new_tuple->value->cstring);
      text_layer_set_text(temp_layer, new_tuple->value->cstring);
      break;

    case PRINT_PROGRESS_KEY:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "setting progress to: %s", new_tuple->value->cstring);
      text_layer_set_text(progress_layer, new_tuple->value->cstring);
      break;
  }
}

static void send_cmd(void) {
  Tuplet value = TupletInteger(1, 1);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &value);
  dict_write_end(iter);

  app_message_outbox_send();
}

// timer events
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Time flies!");
        
        send_cmd();
}
    
// button events
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // file label
  file_label = text_layer_create((GRect) {.origin={0,0}, .size = {bounds.size.w, 30}});
  text_layer_set_text(file_label, "Printing:");
  text_layer_set_text_color(file_label, GColorWhite);
  text_layer_set_background_color(file_label, GColorBlack);
  text_layer_set_font(file_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(file_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(file_label));
  
  // filename
  file_layer = text_layer_create((GRect) {.origin={0,30}, .size = {bounds.size.w, 20}});
  text_layer_set_text(file_layer, "");
  text_layer_set_text_alignment(file_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(file_layer));
  
  // temp label
  temp_label = text_layer_create((GRect) {.origin={0,50}, .size = {bounds.size.w, 30}});
  text_layer_set_text(temp_label, "Temp:");
  text_layer_set_text_color(temp_label, GColorWhite);
  text_layer_set_background_color(temp_label, GColorBlack);
  text_layer_set_font(temp_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(temp_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(temp_label));
  
  // temp
  temp_layer = text_layer_create((GRect) {.origin={0,80}, .size = {bounds.size.w, 20}});
  text_layer_set_text(temp_layer, "");
  text_layer_set_text_alignment(temp_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(temp_layer));
  
  // progress label
  progress_label = text_layer_create((GRect) {.origin={0,100}, .size = {bounds.size.w, 30}});
  text_layer_set_text(progress_label, "Progress:");
  text_layer_set_text_color(progress_label, GColorWhite);
  text_layer_set_background_color(progress_label, GColorBlack);
  text_layer_set_font(progress_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(progress_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(progress_label));
  
  // progress  
  progress_layer = text_layer_create((GRect) {.origin={0,130}, .size = {bounds.size.w, 20}});
  text_layer_set_text(progress_layer, "");
  text_layer_set_text_alignment(progress_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(progress_layer));
  
  // input status
  /*
  text_layer = text_layer_create((GRect) { .origin = { 0, 100 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  */
  
  Tuplet initial_values[] = {
    TupletCString(PRINT_FILE_KEY, "loading file"),
    TupletCString(PRINT_TEMP_KEY, "loading temp"),
    TupletCString(PRINT_PROGRESS_KEY, "loading prog"),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);

  //send_cmd();
  
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void window_unload(Window *window) {
  app_sync_deinit(&sync);
  
  text_layer_destroy(file_label);
  text_layer_destroy(file_layer);
  text_layer_destroy(temp_label);
  text_layer_destroy(temp_layer);
  text_layer_destroy(progress_label);
  text_layer_destroy(progress_layer);
  
  //text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  const int inbound_size = 64;
  const int outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
