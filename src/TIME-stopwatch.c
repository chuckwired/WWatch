#include <pebble.h>
#define MFALSE 0
#define MTRUE 1
//Persistence Keys
#define TOTAL_LAPSED_KEY 25

static Window *window;
static TextLayer *minutes_display;
//Timing Variables
static AppTimer *stopwatch_timer;
static int total_lapsed;
static int stopwatch_begun = MFALSE;

//TODO: Function to display history

//TODO: Function to display time on watch
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

static void reset_timer() {
	//Step 1: Record history
	
	//Step 2: Reset timing mechanisms
	app_timer_cancel(stopwatch_timer);
	total_lapsed = 0;
	stopwatch_begun = MFALSE;
	//Step 3: Reset displays
	text_layer_set_text(minutes_display, "00:00");
}

/*==========================
Records button implementation */
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	;
}

/*==========================
Start/Stop button implementation */
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	start_stop_timer();
}

/*==========================
Reset button implementation */
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

  minutes_display = text_layer_create((GRect) { .origin = { 0, 30 }, .size = { bounds.size.w, 50 } });
  text_layer_set_text(minutes_display, "00:00");
  text_layer_set_text_alignment(minutes_display, GTextAlignmentCenter);
  text_layer_set_font(minutes_display, fonts_get_system_font("RESOURCE_ID_ROBOTO_BOLD_SUBSET_49"));
  layer_add_child(window_layer, text_layer_get_layer(minutes_display));
}

static void window_unload(Window *window) {
  text_layer_destroy(minutes_display);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  
  //Restore time lapsed
  total_lapsed = persist_exists(TOTAL_LAPSED_KEY) ? persist_read_int(TOTAL_LAPSED_KEY) : 0;
  
  window_stack_push(window, animated);
  display_time_elapsed();
}

static void deinit(void) {
	//Save lapsed time
	if(persist_exists(TOTAL_LAPSED_KEY)){persist_delete(TOTAL_LAPSED_KEY);}
	persist_write_int(TOTAL_LAPSED_KEY, total_lapsed);
	window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
