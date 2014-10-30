#include <pebble.h>

static Window *window;
static TextLayer *minutes_display, *ms_display;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	//Start/stop button
}

/*==========================
Reset button implementation */
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	//Step 1: Reset displays
	text_layer_set_text(minutes_display, "00:00");
	text_layer_set_text(ms_display, "0");
	//Step 2: Reset timing mechanism
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  minutes_display = text_layer_create((GRect) { .origin = { 0, 25 }, .size = { bounds.size.w, 50 } });
  text_layer_set_text(minutes_display, "59:59");
  text_layer_set_text_alignment(minutes_display, GTextAlignmentCenter);
  text_layer_set_font(minutes_display, fonts_get_system_font("RESOURCE_ID_ROBOTO_BOLD_SUBSET_49"));
  layer_add_child(window_layer, text_layer_get_layer(minutes_display));
  
  ms_display = text_layer_create((GRect) { .origin = { 110, 75 }, .size = { bounds.size.w, 35 } });
  text_layer_set_text(ms_display, "9");
  text_layer_set_font(ms_display, fonts_get_system_font("RESOURCE_ID_BITHAM_30_BLACK"));
  //text_layer_set_text_alignment(ms_display, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ms_display));
}

static void window_unload(Window *window) {
  text_layer_destroy(minutes_display);
  text_layer_destroy(ms_display);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
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
