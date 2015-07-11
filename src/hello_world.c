#include <pebble.h>

Window *window;
TextLayer *text_time_layer;
static char time_text[] = "00:00";

static void up_button_pressed(ClickRecognizerRef recognizer, void *context)
{
	if(click_recognizer_is_repeating(recognizer))
	{
	}

	Window *window = (Window *)context;
}

static void down_button_pressed(ClickRecognizerRef recognizer, void *context)
{
}
static void select_button_pressed(ClickRecognizerRef recognizer, void *context)
{
}

// Called every time window becomes visible
static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_UP, up_button_pressed);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_button_pressed);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_button_pressed);

//	window_single_repeating_click_subscribe(), window_multi_click_subscribe(), window_long_click_subscribe()
/*
	// single click / repeat-on-hold config:
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);

	// multi click config:
	window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);

	// long click config:
	window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
*/
}

void update_anim(void *data)
{
	// Draw:

	// Set timer for next draw
	app_timer_register(1000 / 30, update_anim, NULL);
}

static void window_load(Window *window) 
{
	// Run loop
	update_anim(NULL);
}

static void window_appear(Window *window)
{
}	

static void window_disappear(Window *window) 
{
}

static void update_time(void)		// Called every tick by tick handler
{
	// Get a tm structure
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);

	// Write the current hours and minutes into the buffer
	strftime(time_text, sizeof("00:00"), "%H:%M", tick_time);

	/*
	// Write the current hours and minutes into the buffer
	if(clock_is_24h_style() == true) {
	    // Use 24 hour format
	    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	      } else {
			  // Use 12 hour format
			  strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
			    }
	*/

	// Change the text in the time text layer
	text_layer_set_text(time_text_layer, time_text);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{
	update_time();
}

void init(void) {
	// Create a window and text layer
	window = window_create();
	text_time_layer = text_layer_create(GRect(0, 0, 144, 154));
	text_layer_set_background_color(text_time_layer, GColorWhite);
	text_layer_set_text_color(text_time_layer, GColorBlack);
	
	// Set the text, font, and text alignment
	text_layer_set_text(text_time_layer, "00:00");
	text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_time_layer));


	// Set up button handlers
	window_set_click_config_provider(window, click_config_provider);
	
	// Set window handlers
	window_set_window_handlers(window, 
			(WindowHandlers) 
			{
				.load = window_load,
				.appear = window_appear,
				.disappear = window_disappear,
			});
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	// Push the window
	window_stack_push(window, true);
	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}

void deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_time_layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void)
{
	init();
	app_event_loop();
	deinit();
}
