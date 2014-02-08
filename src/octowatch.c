#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

// action bar parts
ActionBarLayer *action_bar;
static GBitmap *image_up;

//HeapBitmap button_image_up;
//HeapBitmap button_image_down;
//HeapBitmap button_image_setup;

/*
static TextLayer *file_label;
static TextLayer *file_layer;
static TextLayer *temp_label;
static TextLayer *temp_layer;
static TextLayer *progress_label;
static TextLayer *progress_layer;
*/

static Layer *bg_layer;
static TextLayer *time_remaining_label;
static TextLayer *time_remaining_counter;
static TextLayer *filename_label;
//static TextLayer *filename_display;
static TextLayer *progress_label;
//static TextLayer *progress_display;


enum {
	OCTOPRINT_COMMAND = 0x0,
};

enum {
	AKEY_NUMBER,
	AKEY_TEXT,
};

// AppMessage plumbing
void send_octoprint_command(char *command){
	Tuplet button_tuple = TupletCString(OCTOPRINT_COMMAND, command);
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
	return;
	}
	
	dict_write_tuplet(iter, &button_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

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
	Tuple *remain_tuple = dict_find(received, 1);
	Tuple *progress_tuple = dict_find(received, 2);

	// Act on the found fields received
	if (filename_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "setting file name to: %s", filename_tuple->value->cstring);
		text_layer_set_text(filename_label, filename_tuple->value->cstring);
	}
	
	if (remain_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "time remaining: %s", remain_tuple->value->cstring);
		text_layer_set_text(time_remaining_counter, remain_tuple->value->cstring);
	}
	
	if (progress_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "setting progress to: %s", progress_tuple->value->cstring);
		text_layer_set_text(progress_label, progress_tuple->value->cstring);
	}
}

void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
}

// timer events
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "update timer fired");
	
	send_octoprint_command("update");
}
    
// button events
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
	APP_LOG(APP_LOG_LEVEL_DEBUG, "select click");
  
	send_octoprint_command("update");

}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "up click");
	
	send_octoprint_command("pause");
	
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "down click");
	
	send_octoprint_command("cancel");
	
}

void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler);
}

/*
static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}
*/

static void bg_layer_draw(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	
	// time remaining container
	graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, 75), 4, GCornersAll);
	
	// filename container
	graphics_draw_round_rect(ctx, GRect(0,77, bounds.size.w, 40), 4);
	
	// progress container
	graphics_fill_rect(ctx, GRect(0, 121, bounds.size.w, 25), 4, GCornersAll);
	
	// Draw a black filled rectangle with sharp corners
	//graphics_context_set_fill_color(ctx, GColorBlack);
	//graphics_fill_rect(ctx, bounds, 0, GCornerNone);
	
	// Draw a white filled circle a radius of half the layer height
	//graphics_context_set_fill_color(ctx, GColorWhite);
	//const int16_t half_h = bounds.size.h / 2;
	//graphics_fill_circle(ctx, GPoint(half_h, half_h), half_h);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	image_up = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP);
	
	// Initialize the action bar:
	action_bar = action_bar_layer_create();
	
	// Associate the action bar with the window:
	action_bar_layer_add_to_window(action_bar, window);
	
	action_bar_layer_set_click_config_provider(action_bar,click_config_provider);
	
	action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, image_up);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, image_up);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, image_up);
	
	// background layer
	bg_layer = layer_create(GRect(1, 3, (bounds.size.w - 22), bounds.size.h));
	layer_set_update_proc(bg_layer, bg_layer_draw);
	layer_add_child(window_layer, bg_layer);
	
	// time remaining
	GRect bg_bounds = layer_get_bounds(bg_layer);
	time_remaining_label = text_layer_create(GRect(0, 0, bg_bounds.size.w, 25));
	text_layer_set_text(time_remaining_label, "time remaining:");
	text_layer_set_text_color(time_remaining_label, GColorWhite);
	text_layer_set_background_color(time_remaining_label, GColorClear);
	text_layer_set_font(time_remaining_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(time_remaining_label, GTextAlignmentCenter);
	layer_add_child(bg_layer, text_layer_get_layer(time_remaining_label));
	
	time_remaining_counter = text_layer_create(GRect(0, 20, bg_bounds.size.w, 60));
	text_layer_set_text(time_remaining_counter, "??:??");
	text_layer_set_text_color(time_remaining_counter, GColorWhite);
	text_layer_set_background_color(time_remaining_counter, GColorClear);
	text_layer_set_font(time_remaining_counter, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
	text_layer_set_text_alignment(time_remaining_counter, GTextAlignmentCenter);
	layer_add_child(bg_layer, text_layer_get_layer(time_remaining_counter));
	
	// filename
	filename_label = text_layer_create(GRect(3, 78, (bg_bounds.size.w -4), 30));
	text_layer_set_overflow_mode(filename_label, GTextOverflowModeWordWrap);
	text_layer_set_text(filename_label, "loading...");
	text_layer_set_text_color(filename_label, GColorBlack);
	text_layer_set_background_color(filename_label, GColorClear);
	text_layer_set_font(filename_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(filename_label, GTextAlignmentLeft);
	layer_add_child(bg_layer, text_layer_get_layer(filename_label));
	
	// progress
	progress_label = text_layer_create(GRect(3, 122, (bg_bounds.size.w -4), 25));
	text_layer_set_text(progress_label, "loading...");
	text_layer_set_text_color(progress_label, GColorWhite);
	text_layer_set_background_color(progress_label, GColorClear);
	text_layer_set_font(progress_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(progress_label, GTextAlignmentLeft);
	layer_add_child(bg_layer, text_layer_get_layer(progress_label));
	
  	/*
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
	*/
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void window_unload(Window *window) {
 
 	/*
	text_layer_destroy(file_label);
	text_layer_destroy(file_layer);
	text_layer_destroy(temp_label);
	text_layer_destroy(temp_layer);
	text_layer_destroy(progress_label);
	text_layer_destroy(progress_layer);
	*/
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
