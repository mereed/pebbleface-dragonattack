/*
Copyright (C) 2017 Mark Reed

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <pebble.h>
#include "main.h"

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *battery_text_layer;
static TextLayer *layer_ampm_text;
static GBitmap *s_bitmap = NULL;
static GBitmap *start_bitmap = NULL;
static GBitmap *building_bitmap = NULL;
static GBitmap *grass_bitmap = NULL;
static BitmapLayer *s_bitmap_layer;
static BitmapLayer *start_bitmap_layer;
static BitmapLayer *building_layer;
static BitmapLayer *grass_layer;
static GBitmapSequence *s_sequence = NULL;
int charge_percent = 0;
int stringToInt(char *str);

// A struct for our specific settings (see main.h)
ClaySettings settings;



static void timer_handler(void *context) {
  uint32_t next_delay;

  // Advance to the next APNG frame
  if(gbitmap_sequence_update_bitmap_next_frame(s_sequence, s_bitmap, &next_delay)) {
    bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
	bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);	
    gbitmap_sequence_set_play_count(s_sequence, settings.flap);  // number of animation loops
    layer_mark_dirty(bitmap_layer_get_layer(s_bitmap_layer));
    app_timer_register(next_delay, timer_handler, NULL); // Timer for that delay
	  
  } else {
    // Start again
    gbitmap_sequence_restart(s_sequence);
  }
}


static void load_sequence() {
	
  layer_set_hidden(bitmap_layer_get_layer(start_bitmap_layer), true); 

  if(s_sequence) {
    gbitmap_sequence_destroy(s_sequence);
    s_sequence = NULL;
  }
  if(s_bitmap) {

    gbitmap_destroy(s_bitmap);
    s_bitmap = NULL;
  }

  s_sequence = gbitmap_sequence_create_with_resource(RESOURCE_ID_ANIMATION);
  s_bitmap = gbitmap_create_blank(gbitmap_sequence_get_bitmap_size(s_sequence), GBitmapFormat8Bit);
  app_timer_register(1, timer_handler, NULL);

}


// Initialize the default settings
static void prv_default_settings() {	
  settings.flap = 1;
  settings.stat = 1;
  settings.anim = true;
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
	
}


int stringToInt(char *str){
    int i=0,sum=0;
    while(str[i]!='\0'){
         if(str[i]< 48 || str[i] > 57){
            // if (DEBUG) APP_LOG(APP_LOG_LEVEL_ERROR, "Unable to convert it into integer.");
          //   return 0;
         }
         else{
             sum = sum*10 + (str[i] - 48);
             i++;
         }
    }
    return sum;
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
		
 // flap
  Tuple *flap_t = dict_find(iter, MESSAGE_KEY_flap);
  if (flap_t) {
     settings.flap = stringToInt((char*) flap_t->value->data);
  }
 // anim
  Tuple *anim_t = dict_find(iter, MESSAGE_KEY_anim);
  if (anim_t) {
    settings.anim = anim_t->value->int32 == 1;
  }
 // stat
  Tuple *stat_t = dict_find(iter, MESSAGE_KEY_stat);
  if (stat_t) {
     settings.stat = stringToInt((char*) stat_t->value->data);
  }	
  // Save the new settings to persistent storage
  prv_save_settings();
}


static void update_datetime() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char time_buffer[] = "00:00";
  static char date_buffer[] = "Sun, Oct 25";
	static char ampm_text[] = "am";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(time_buffer, sizeof("00:00"), "%l:%M", tick_time);
	  		strftime(ampm_text, sizeof(ampm_text), "%P", tick_time);
        text_layer_set_text(layer_ampm_text, ampm_text);
  }
  strftime(date_buffer, sizeof("Sun, Oct 25"), "%a, %b %e", tick_time);

  // Display on the TextLayer
  text_layer_set_text(s_time_layer, time_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
	
	
  if (tick_time->tm_hour < 5) {
	  
	    window_set_background_color(s_main_window, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text_color(layer_ampm_text, GColorWhite);
	  
  } else if (tick_time->tm_hour < 7) {
		   
	    window_set_background_color(s_main_window, GColorOxfordBlue);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_text_color(layer_ampm_text, GColorBlack);
	  
  } else if (tick_time->tm_hour < 12) {
		   
	    window_set_background_color(s_main_window, GColorWhite);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_text_color(layer_ampm_text, GColorBlack);
	  
  } else if (tick_time->tm_hour < 16) {
		   
	    window_set_background_color(s_main_window, GColorPastelYellow );
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_text_color(layer_ampm_text, GColorBlack);	
	  
  } else if (tick_time->tm_hour < 19) {	   
	
	    window_set_background_color(s_main_window, GColorMelon );
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_text_color(layer_ampm_text, GColorBlack);
	  
  } else if (tick_time->tm_hour < 21) {	   
	
	    window_set_background_color(s_main_window, GColorBulgarianRose );
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text_color(layer_ampm_text, GColorWhite);
	  
	  
  } else if (tick_time->tm_hour < 24) {
		   
	    window_set_background_color(s_main_window, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text_color(layer_ampm_text, GColorWhite);

  }   
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_datetime();

  
  if (settings.anim && (units_changed & MINUTE_UNIT)) {
load_sequence();
  }	
}

void update_battery_state(BatteryChargeState charge_state) {
    static char battery_text[] = "x100%";

    if (charge_state.is_charging) {

        snprintf(battery_text, sizeof(battery_text), "+%d%%", charge_state.charge_percent);
    } else {
        snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
        
    } 
    charge_percent = charge_state.charge_percent;
    text_layer_set_text(battery_text_layer, battery_text);
} 

static void toggle_bluetooth_icon(bool connected) {

  layer_set_hidden(bitmap_layer_get_layer(building_layer), !connected);
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth_icon(connected);
}


static void main_window_load(Window *window) {
	
  Layer *window_layer = window_get_root_layer(window);

 // BitmapLayer - start frame
  start_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ANIMATIONSTART);
#ifdef PBL_PLATFORM_CHALK
  start_bitmap_layer = bitmap_layer_create(GRect(14, 50, 144, 121));
#else
  start_bitmap_layer = bitmap_layer_create(GRect(0, 50, 144, 121));
#endif
  bitmap_layer_set_bitmap(start_bitmap_layer, start_bitmap);
  bitmap_layer_set_compositing_mode(start_bitmap_layer, GCompOpSet);	
  layer_add_child(window_layer, bitmap_layer_get_layer(start_bitmap_layer));
	
	// grass
  grass_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GRASS);
#ifdef PBL_PLATFORM_CHALK
  grass_layer = bitmap_layer_create(GRect(24, 151, 114, 29));
#else
  grass_layer = bitmap_layer_create(GRect(0, 158, 144, 20));
#endif
  bitmap_layer_set_bitmap(grass_layer, grass_bitmap);
  bitmap_layer_set_compositing_mode(grass_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(grass_layer));	
	
	// BitmapLayer - PNG animation layer
#ifdef PBL_PLATFORM_CHALK
  s_bitmap_layer = bitmap_layer_create(GRect(14, 48, 154, 121));
#else
  s_bitmap_layer = bitmap_layer_create(GRect(0, 50, 144, 121));
#endif
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
	
	// barbarian
  building_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BUILDING);
#ifdef PBL_PLATFORM_CHALK
  building_layer = bitmap_layer_create(GRect(26, 130, 30, 30));
#else
  building_layer = bitmap_layer_create(GRect(0, 134, 30, 30));
#endif
  bitmap_layer_set_bitmap(building_layer, building_bitmap);
  bitmap_layer_set_compositing_mode(building_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(building_layer));	

	
	// Time - begin
#ifdef PBL_PLATFORM_CHALK
  s_time_layer = text_layer_create(GRect(0, 18, 180, 44));
#else
  s_time_layer = text_layer_create(GRect(0, -4, 144, 44));
#endif
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS ));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Date - begin
#ifdef PBL_PLATFORM_CHALK
  s_date_layer = text_layer_create(GRect(0, 6, 180, 30));
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
#else
  s_date_layer = text_layer_create(GRect(0, 26, 144, 30));
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
#endif
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text(s_date_layer, "00:00");
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	
#ifdef PBL_PLATFORM_CHALK
    layer_ampm_text = text_layer_create(GRect(129,32, 32, 22));
#else
    layer_ampm_text = text_layer_create(GRect(118, 10, 32, 22));
#endif 	
	text_layer_set_background_color(layer_ampm_text, GColorClear);
    text_layer_set_font(layer_ampm_text, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(layer_ampm_text, GTextAlignmentLeft);
    layer_add_child(window_layer, text_layer_get_layer(layer_ampm_text));

#ifdef PBL_PLATFORM_CHALK
    battery_text_layer = text_layer_create(GRect(62, 156, 40, 20));
#else
    battery_text_layer = text_layer_create(GRect(102, 0, 40, 20));
#endif 	
	text_layer_set_font(battery_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));

    text_layer_set_text_color(battery_text_layer, GColorWhite);
	text_layer_set_background_color(battery_text_layer, GColorClear);
    text_layer_set_text_alignment(battery_text_layer, GTextAlignmentRight);
    layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));
	
  update_datetime();
	
 //load_sequence();

}

static void main_window_unload(Window *window) {
  text_layer_destroy(layer_ampm_text);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(battery_text_layer);

  bitmap_layer_destroy(start_bitmap_layer);
  bitmap_layer_destroy(building_layer);
  bitmap_layer_destroy(grass_layer);
  bitmap_layer_destroy(s_bitmap_layer);
	
  gbitmap_sequence_destroy(s_sequence);
  gbitmap_destroy(s_bitmap);	  
  gbitmap_destroy(start_bitmap);	  
  gbitmap_destroy(building_bitmap);	  
  gbitmap_destroy(grass_bitmap);	  
}

static void init() {
	
prv_load_settings();
	
  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);

  // international support
  setlocale(LC_ALL, "");
	
  s_main_window = window_create();

  window_set_background_color(s_main_window, GColorBlack );
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });

  prv_update_display();

	// handlers
    battery_state_service_subscribe(&update_battery_state);
    bluetooth_connection_service_subscribe(&toggle_bluetooth_icon);
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
    window_stack_push(s_main_window, true);
	
	// update the battery on launch
    update_battery_state(battery_state_service_peek());
	toggle_bluetooth_icon(bluetooth_connection_service_peek());
}

static void deinit() {
   tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
	
	window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}