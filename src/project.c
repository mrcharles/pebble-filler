#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x99, 0x5B, 0x2A, 0xBE, 0x92, 0x9A, 0x4F, 0x9D, 0x95, 0x7D, 0x2F, 0x7B, 0xB5, 0x1B, 0xB0, 0xC0 }
PBL_APP_INFO(MY_UUID,
             "Filler", "Charles Randall",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
Layer layer;

#define Q_WIDTH (144 / 4)
#define Q_HEIGHT (168 / 3)

#define MIN_HEIGHT 10

#define BOTTOM_STRIP 1

#define FORECOLOR GColorBlack
#define BACKCOLOR GColorWhite

void fill_quarter(GContext* ctx, int row, int column)
{
  graphics_context_set_stroke_color(ctx, FORECOLOR);
  graphics_context_set_fill_color(ctx, FORECOLOR);

  graphics_fill_rect(ctx, GRect( Q_WIDTH * column, Q_HEIGHT * row, Q_WIDTH,Q_HEIGHT), 0, GCornerTopLeft);
}

void draw_quarter(GContext* ctx, int row, int column, int minutes)
{
  fill_quarter(ctx, row, column);

  //now fill based on minutes, so I'm thinking 7 chunks
  // 36 wide per block, so 5px and one extra pixel we can leave maybe for info
  // 
  if( minutes < 8 ) //then we're just clearing chunks away
  {
    graphics_context_set_stroke_color(ctx, BACKCOLOR);
    graphics_context_set_fill_color(ctx, BACKCOLOR);

    for( int i = 0; i < minutes; i++)
    {

      graphics_fill_rect(ctx, GRect( Q_WIDTH * column + i * 5, Q_HEIGHT * (row+1) -BOTTOM_STRIP - MIN_HEIGHT, 5, MIN_HEIGHT), 0, GCornerTopLeft);
    }
  }
  else // drawing new chunks
  {
    // clear out the strip
    graphics_context_set_stroke_color(ctx, BACKCOLOR);
    graphics_context_set_fill_color(ctx, BACKCOLOR);

    graphics_fill_rect(ctx, GRect( Q_WIDTH * column, Q_HEIGHT * (row+1)-BOTTOM_STRIP - MIN_HEIGHT, 35, MIN_HEIGHT), 0, GCornerTopLeft);

    graphics_context_set_stroke_color(ctx, FORECOLOR);
    graphics_context_set_fill_color(ctx, FORECOLOR);
    minutes = minutes - 7;
    for( int i = 0; i < minutes; i++)
      graphics_fill_rect(ctx, GRect( Q_WIDTH * column + i * 5, Q_HEIGHT * (row+1) -BOTTOM_STRIP - MIN_HEIGHT, 5, MIN_HEIGHT), 0, GCornerTopLeft);
  }
}

#define CHUNK_WIDTH 8
#define CHUNK_HEIGHT 8
#define CHUNK_SPACE 3

#define CHUNK_X 2
#define CHUNK_Y 2

void draw_chunks(GContext *ctx, int chunks)
{
    graphics_context_set_stroke_color(ctx, BACKCOLOR);
    graphics_context_set_fill_color(ctx, BACKCOLOR);

    for (int i = 0; i < chunks; ++i)
    {
      graphics_fill_rect(ctx, GRect( CHUNK_X + i * (CHUNK_WIDTH+CHUNK_SPACE), CHUNK_Y, CHUNK_WIDTH, CHUNK_HEIGHT), 0, GCornerTopLeft);
    }
}
  
void layer_update_callback(Layer *me, GContext* ctx) 
{
  (void)me;

  PblTm t;

  get_time(&t);

  unsigned short hour = t.tm_hour % 12;
  unsigned short chunk = hour / 3;

  unsigned short hours = hour % 3;
  unsigned short blocks = t.tm_min / 15;
  unsigned short minutes = t.tm_min % 15;

  // draw our filled in hours
  for( int i = 0; i < hours; i++)
  {
    for( int j = 0; j < 4; j++)
      fill_quarter(ctx, i, j);
  }

  // draw our filled in blocks
  for( int i = 0; i < blocks; i++)
  {
    fill_quarter(ctx, hours, i);
  }

  // draw our active time block
  draw_quarter(ctx, hours, blocks, minutes);
  draw_chunks(ctx, chunk);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;

  layer_mark_dirty(&layer);
}

 
void handle_init(AppContextRef ctx) 
{
  (void)ctx;

  window_init(&window, "Filler");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, BACKCOLOR);

  layer_init(&layer, window.layer.frame );
  layer_add_child(&window.layer, &layer);
  layer.update_proc = layer_update_callback;


}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
