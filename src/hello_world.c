#include <pebble.h>

/*
   watch face;
   	white on black, with face in center: sleeping, awake, drowsy
	
timeline shoot obstacles

include weather
	*/

Window *window;
TextLayer *text_time_layer;
static char time_text[] = "00:00";
static GPath *player = NULL;
static const GPathInfo PLAYER_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{0, 0}, {14, 26}, {28, 26}, {7, 60}, {14, 34}, {0, 34}}
};
int playerOffsetX = 0, playerOffsetY = 0;

Layer *mainLayer;

static void up_button_pressed(ClickRecognizerRef recognizer, void *context)
{
	if(click_recognizer_is_repeating(recognizer))
	{
	}

	Window *window = (Window *)context;
	light_enable(true);
	// Take damage:
	vibes_long_pulse();
	// Fire gun:
	vibes_short_pulse();
	// Rotate 15 degrees:
	gpath_rotate_to(player, TRIG_MAX_ANGLE / 360 * 15);
	// Translate by (5, 5):
	gpath_move_to(player, GPoint(playerOffsetX, playerOffsetY));
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

static void update_layer_callback(Layer *layer, GContext *ctx)	// Called by makeDirty
{
	GRect bounds = layer_get_frame(layer);
	// Draw:
	// Draw obstacles
	// Draw shots
	// Draw player
	graphics_context_set_fill_color(ctx, GColorWhite);
	gpath_draw_filled(ctx, player);
	// Stroke the path:
	graphics_context_set_stroke_color(ctx, GColorBlack);
	gpath_draw_outline(ctx, player);
		
	/*
	   make layer
	   put polygon in layer
	   mark layer as dirty in loop func, so that sys calls update_my_layer routine
	   */
/*	
graphics_context_set_text_color(ctx, GColorBlack);
graphics_draw_text(ctx, "Text here.", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(5, 5, bounds.size.w - 10, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
graphics_draw_text(ctx, "And text here as well.", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(90, 100, bounds.size.w - 95, 60), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
*/
}

void anim_loop(void *data)
{
	// request redraw of layer
	layer_mark_dirty(mainLayer);
	// Set timer for next draw
	app_timer_register(1000 / 30, anim_loop, NULL);
}

static void window_load(Window *window) 
{
	// Run loop
	anim_loop(NULL);
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
	text_layer_set_text(text_time_layer, time_text);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{
	update_time();
}

void init(void) {
	// Create a window 
	window = window_create();
	window_set_background_color(window, GColorBlack);
//	window_set_fullscreen(window, true);				// Not available on Basalt
	// Create text layer
	text_time_layer = text_layer_create(GRect(0, 0, 144, 15));
	text_layer_set_background_color(text_time_layer, GColorWhite);
	text_layer_set_text_color(text_time_layer, GColorBlack);
	
	// Set the text, font, and text alignment
	text_layer_set_text(text_time_layer, "00:00");
	text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);

	// Create main drawing layer
	GRect windowBounds = layer_get_frame(window_get_root_layer(window));
	mainLayer = layer_create(windowBounds);
	layer_set_update_proc(mainLayer, update_layer_callback);
	// Add background layer to window
	layer_add_child(window_get_root_layer(window), mainLayer);

	player = gpath_create(&PLAYER_INFO);

	
	// Add obstacle layer to window
	// Add shots layer to window
	// Add character layer to window
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

	layer_destroy(mainLayer);
	  
	
	// Destroy the window
	window_destroy(window);
}

int main(void)
{
	init();
	app_event_loop();
	deinit();
}
