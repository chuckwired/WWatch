#include <pebble.h>
#define MFALSE 0
#define MTRUE 1
//Persistence Keys
#define TOTAL_LAPSED_KEY 25
#define HISTORY_KEY 26

//UI
static Window *window;
static TextLayer *minutes_display, *history_display;
static GBitmap *star_bitmap, *refresh_bitmap;
static BitmapLayer *star_layer, *refresh_layer;
//Timing Variables
static AppTimer *stopwatch_timer;
static int total_lapsed, history_showing = MFALSE;
static int stopwatch_begun = MFALSE;
static int session_history[5];

/*====================
History functionality */
static void push_record(){
	if(total_lapsed == 0){ return;}
	for(int i = 0; i < 5; i++){ if(!session_history[i]){session_history[i] = 0;} }
	for(int i = 4; i > 0; i--){session_history[i] = session_history[i-1];}
	session_history[0] = total_lapsed;
}

static void display_records(){
	if(history_showing == MTRUE){text_layer_destroy(history_display); history_showing = MFALSE; return;}
	Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

	history_display = text_layer_create((GRect) { .origin = { 5, 0 }, .size = { bounds.size.w, 180 } });
	text_layer_set_overflow_mode(history_display, GTextOverflowModeWordWrap);
	static char hdisplay[45] = "";
	snprintf(hdisplay, 45, "1) %02i:%02i\n2) %02i:%02i\n3) %02i:%02i\n4) %02i:%02i\n5) %02i:%02i", 
		session_history[0] / 60, session_history[0] % 60,
		session_history[1] / 60, session_history[1] % 60,
		session_history[2] / 60, session_history[2] % 60,
		session_history[3] / 60, session_history[3] % 60,
		session_history[4] / 60, session_history[4] % 60);
	text_layer_set_text(history_display, hdisplay);
    text_layer_set_text_alignment(minutes_display, GTextAlignmentCenter);
    text_layer_set_font(history_display, fonts_get_system_font("RESOURCE_ID_DROID_SERIF_28_BOLD"));
    layer_add_child(window_layer, text_layer_get_layer(history_display));
	history_showing = MTRUE;
}

/*=================
Core functionality */
static void reset_timer() {
	//Step 1: Stop and record history
	app_timer_cancel(stopwatch_timer);
	push_record();
	//Step 2: Reset timing mechanisms
	total_lapsed = 0;
	stopwatch_begun = MFALSE;
	text_layer_set_text(minutes_display, "00:00");
}

static void check_overtime(){
	if (total_lapsed >= 3600){push_record(); reset_timer();}
}

//Display time on watch
static void display_time_elapsed(){
	//Calculate times
	int minutes_lapsed = total_lapsed/60;
	int seconds_lapsed = total_lapsed % 60;
	//Display time
	static char tdisplay[] = "00:00";
	snprintf(tdisplay, 6, "%02i:%02i", minutes_lapsed, seconds_lapsed);
	text_layer_set_text(minutes_display, tdisplay);
}

static void timer_callback(){
	//Increment seconds
	total_lapsed += 1;
	check_overtime();
	
	//Display time
	display_time_elapsed();
	
	//If still timing, call self again
	if(stopwatch_begun == MTRUE){
		stopwatch_timer = app_timer_register(1000, (AppTimerCallback) timer_callback, NULL);
	}
}

static void start_stop_timer() {
	if(stopwatch_begun == MFALSE){
		//Start timing and kickoff timer
		stopwatch_begun = MTRUE;
		stopwatch_timer = app_timer_register(600, (AppTimerCallback) timer_callback, NULL);
	}
	else{
		//Stop recording and cancel timer
		stopwatch_begun = MFALSE;
		app_timer_cancel(stopwatch_timer);
	}
}

/*==========================
Controls implementation */
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	display_records();
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	start_stop_timer();
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	reset_timer();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  minutes_display = text_layer_create((GRect) { .origin = { 0, 40 }, .size = { bounds.size.w, 50 } });
  text_layer_set_text(minutes_display, "00:00");
  text_layer_set_text_alignment(minutes_display, GTextAlignmentCenter);
  text_layer_set_font(minutes_display, fonts_get_system_font("RESOURCE_ID_ROBOTO_BOLD_SUBSET_49"));
  layer_add_child(window_layer, text_layer_get_layer(minutes_display));
  
  star_bitmap = gbitmap_create_with_resource(RESOURCE_ID_STAR_ICON);
  star_layer = bitmap_layer_create(GRect(100, 0, 48, 48));
  bitmap_layer_set_bitmap(star_layer, star_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(star_layer));
  
  refresh_bitmap = gbitmap_create_with_resource(RESOURCE_ID_REFRESH_ICON);
  refresh_layer = bitmap_layer_create(GRect(100, 100, 48, 48));
  bitmap_layer_set_bitmap(refresh_layer, refresh_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(refresh_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(minutes_display);
  bitmap_layer_destroy(star_layer); bitmap_layer_destroy(refresh_layer);
  gbitmap_destroy(star_bitmap); gbitmap_destroy(refresh_bitmap);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  
  //Restore state
  total_lapsed = persist_exists(TOTAL_LAPSED_KEY) ? persist_read_int(TOTAL_LAPSED_KEY) : 0;
  
  window_stack_push(window, animated);
  display_time_elapsed();
}

static void deinit(void) {
	//Persist state
	if(persist_exists(TOTAL_LAPSED_KEY)){persist_delete(TOTAL_LAPSED_KEY);}
	persist_write_int(TOTAL_LAPSED_KEY, total_lapsed);
	window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
