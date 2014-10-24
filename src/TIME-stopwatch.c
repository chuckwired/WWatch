#include <pebble.h>

static Window *window;
static TextLayer *minutes_display, *seconds_display, *ms_display;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  minutes_display = text_layer_create((GRect) { .origin = { 0, 10 }, .size = { bounds.size.w, 50 } });
  text_layer_set_text(minutes_display, "XX");
  text_layer_set_text_alignment(minutes_display, GTextAlignmentCenter);
  text_layer_set_font(minutes_display, fonts_get_system_font("RESOURCE_ID_BITHAM_42_BOLD"));
  layer_add_child(window_layer, text_layer_get_layer(minutes_display));
  
  seconds_display = text_layer_create((GRect) { .origin = { 0, 80 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(seconds_display, "XX.");
  text_layer_set_text_alignment(seconds_display, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(seconds_display));
  
  ms_display = text_layer_create((GRect) { .origin = { 82, 80 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(ms_display, "x");
  //text_layer_set_text_alignment(ms_display, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ms_display));
}

static void window_unload(Window *window) {
  text_layer_destroy(minutes_display);
  text_layer_destroy(seconds_display);
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
