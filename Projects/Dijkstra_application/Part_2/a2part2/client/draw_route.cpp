#include "draw_route.h"
#include "map_drawing.h"

#define TFT_BLUE  0x001F

extern shared_vars shared;

void draw_route() {
  // Needed for scaling and graphics of route
  for (int i = 0; i < shared.num_waypoints - 1; i++) {
    int8_t zoom_level = shared.map_number;

    lon_lat_32 point1 = shared.waypoints[i];
    lon_lat_32 point2 = shared.waypoints[i + 1];

    xy_pos map_p1 = xy_pos(longitude_to_x(zoom_level, point1.lon), latitude_to_y(zoom_level, point1.lat));
    map_p1.x -= shared.map_coords.x;
    map_p1.y -= shared.map_coords.y;

    xy_pos map_p2 = xy_pos(longitude_to_x(zoom_level, point2.lon), latitude_to_y(zoom_level, point2.lat));
    map_p2.x -= shared.map_coords.x;
    map_p2.y -= shared.map_coords.y;

    shared.tft->drawLine(map_p1.x, map_p1.y, map_p2.x, map_p2.y, TFT_BLUE);
  }

  delay(100);
}
