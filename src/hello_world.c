#include <pebble.h>

Window *window;
TextLayer *text_layer;

static void up_button_pressed(ClickRecognizerRef recognizer, void *context)
{
}

static void down_button_pressed(ClickRecognizerRef recognizer, void *context)
{
}
static void select_button_pressed(ClickRecognizerRef recognizer, void *context)
{
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_UP, up_button_pressed);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_button_pressed);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_button_pressed);
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

void init(void) {
	// Create a window and text layer
	window = window_create();
	text_layer = text_layer_create(GRect(0, 0, 144, 154));
	
	// Set the text, font, and text alignment
	text_layer_set_text(text_layer, "fgwHi, I'm a Pebble!");
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));


	// Set up button handlers
	window_set_click_config_provider(window, click_config_provider);
	
	// Push the window
	window_stack_push(window, true);
	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");

	// Set window handlers
	window_set_window_handlers(window, 
			(WindowHandlers) 
			{
				.load = window_load,
				.appear = window_appear,
				.disappear = window_disappear,
			});
}



void deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void)
{
	init();
	app_event_loop();
	deinit();
}
