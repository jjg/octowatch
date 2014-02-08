#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

static TextLayer *file_label;
static TextLayer *file_layer;
static TextLayer *temp_label;
static TextLayer *temp_layer;
static TextLayer *progress_label;
static TextLayer *progress_layer;

//static AppSync sync;
//static uint8_t sync_buffer[64];
/*
enum PrintJobKey {
  PRINT_FILE_KEY = 0x0,
  PRINT_TEMP_KEY = 0x1,
  PRINT_PROGRESS_KEY = 0x2,
};
*/

enum {
  BUTTON_CLICK = 0x0,
  SELECT_CLICK = 0x1,
  DOWN_CLICK = 0x2,
};

enum {
	AKEY_NUMBER,
	AKEY_TEXT,
};

// AppMessage plumbing
void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
}


void in_received_handler(DictionaryIterator *received, void *context) {
	// incoming message received
	
	// Check for fields you expect to receive
	Tuple *filename_tuple = dict_find(received, 0);
	Tuple *temp_tuple = dict_find(received, 1);
	Tuple *progress_tuple = dict_find(received, 2);

	// Act on the found fields received
	if (filename_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "setting file name to: %s", filename_tuple->value->cstring);
		text_layer_set_text(file_layer, filename_tuple->value->cstring);
	}
	
	if (filename_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "setting temp to: %s", temp_tuple->value->cstring);
		text_layer_set_text(temp_layer, temp_tuple->value->cstring);
	}
	
	if (progress_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "setting progress to: %s", progress_tuple->value->cstring);
		text_layer_set_text(progress_layer, progress_tuple->value->cstring);
	}
}


void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
}

// timer events
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "update timer fired");
        
        //send_cmd();
}
    
// button events
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
	Tuplet button_tuple = TupletCString(BUTTON_CLICK, "select");
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
	return;
	}
	
	dict_write_tuplet(iter, &button_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {

	Tuplet button_tuple = TupletCString(BUTTON_CLICK, "up");
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
	return;
	}
	
	dict_write_tuplet(iter, &button_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {

	Tuplet button_tuple = TupletCString(BUTTON_CLICK, "down");
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
	return;
	}
	
	dict_write_tuplet(iter, &button_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
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
  
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void window_unload(Window *window) {
  //app_sync_deinit(&sync);
  
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
  
	// init AppMessage & wire-up handlers
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	
	const uint32_t inbound_size = 64;
	const uint32_t outbound_size = 64;
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
