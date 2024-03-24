#include <gui/canvas.h>
// A simple swap function for integers
void swap(int32_t* a, int32_t* b) {
    int32_t temp = *a;
    *a = *b;
    *b = temp;
}
void canvas_draw_filled_triangle(
    Canvas* const canvas,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    int32_t x3,
    int32_t y3) {
    // First, sort the vertices by y-coordinate ascending (y1 <= y2 <= y3)
    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }
    if(y1 > y3) {
        swap(&x1, &x3);
        swap(&y1, &y3);
    }
    if(y2 > y3) {
        swap(&x2, &x3);
        swap(&y2, &y3);
    }

    // Calculate the slopes of the triangle's sides
    float slope13 = (float)(x3 - x1) / (y3 - y1);
    float slope12 = (y2 - y1) ? (float)(x2 - x1) / (y2 - y1) : 0;
    float slope23 = (y3 - y2) ? (float)(x3 - x2) / (y3 - y2) : 0;

    // Draw the first part of the triangle (from y1 to y2)
    for(int y = y1; y <= y2; y++) {
        int xa = x1 + (int)((y - y1) * slope13);
        int xb = x1 + (int)((y - y1) * slope12);
        if(xa > xb) {
            //swap
            int temp = xa;
            xa = xb;
            xb = temp;
        } // Ensure xa is left of xb
        canvas_draw_line(canvas, xa, y, xb, y);
    }

    // Draw the second part of the triangle (from y2 to y3)
    for(int y = y2; y <= y3; y++) {
        int xa = x1 + (int)((y - y1) * slope13);
        int xb = x2 + (int)((y - y2) * slope23);
        if(xa > xb) {
            //swap
            int temp = xa;
            xa = xb;
            xb = temp;
        } // Ensure xa is left of xb
        canvas_draw_line(canvas, xa, y, xb, y);
    }
}

void draw_empty_or_filled_arrow(
    Canvas* const canvas,
    int8_t pos_x,
    int8_t pos_y,
    char direction,
    bool fill) {
    // Arrow dimensions remain unchanged
    const int arrow_body_width = 5;
    const int arrow_body_height = 5;
    const int arrow_head_width = 10;
    const int arrow_head_height = 5;
    // adjust position to center the arrow
    switch(direction) {
    case 'D':
        pos_x -= arrow_body_width / 2;
        pos_y -= (arrow_body_height / 2 + arrow_head_height);
        break;
    case 'U':
        pos_x -= arrow_body_width / 2;
        pos_y += (arrow_body_height + arrow_head_height) / 2 - arrow_body_height / 2;
        break;
    case 'R':
        pos_x -= arrow_head_height + arrow_body_width / 2;
        pos_y -= arrow_body_width / 2;
        break;
    case 'L':
        pos_x += arrow_head_height / 2;
        pos_y -= arrow_body_width / 2;
        break;
    default:
        break;
    }

    int body_start_x = 0, body_end_x = 0, body_start_y = 0, body_end_y = 0;
    int head_tip_x = 0, head_tip_y = 0, head_base_start_x = 0, head_base_end_x = 0,
        head_base_y = 0;
    int head_base_x = 0, head_base_start_y = 0, head_base_end_y = 0;

    switch(direction) {
    case 'U': // Upward arrow
        body_start_x = pos_x - arrow_body_width / 2;
        body_end_x = pos_x + arrow_body_width / 2;
        body_start_y = pos_y - arrow_body_height;
        body_end_y = pos_y;
        head_tip_x = pos_x;
        head_tip_y = body_start_y - arrow_head_height;
        head_base_start_x = pos_x - arrow_head_width / 2;
        head_base_end_x = pos_x + arrow_head_width / 2;
        head_base_y = body_start_y;
        break;
    case 'D': // Downward arrow
        body_start_x = pos_x - arrow_body_width / 2;
        body_end_x = pos_x + arrow_body_width / 2;
        body_start_y = pos_y;
        body_end_y = pos_y + arrow_body_height;
        head_tip_x = pos_x;
        head_tip_y = body_end_y + arrow_head_height;
        head_base_start_x = pos_x - arrow_head_width / 2;
        head_base_end_x = pos_x + arrow_head_width / 2;
        head_base_y = body_end_y;
        break;
    case 'L': // Leftward arrow
        body_start_y = pos_y - arrow_body_width / 2;
        body_end_y = pos_y + arrow_body_width / 2;
        body_start_x = pos_x - arrow_body_height;
        body_end_x = pos_x;
        head_tip_y = pos_y;
        head_tip_x = body_start_x - arrow_head_height;
        head_base_start_y = pos_y - arrow_head_width / 2;
        head_base_end_y = pos_y + arrow_head_width / 2;
        head_base_x = body_start_x;
        break;
    case 'R': // Rightward arrow
        body_start_y = pos_y - arrow_body_width / 2;
        body_end_y = pos_y + arrow_body_width / 2;
        body_start_x = pos_x;
        body_end_x = pos_x + arrow_body_height;
        head_tip_y = pos_y;
        head_tip_x = body_end_x + arrow_head_height;
        head_base_start_y = pos_y - arrow_head_width / 2;
        head_base_end_y = pos_y + arrow_head_width / 2;
        head_base_x = body_end_x;
        break;
    }

    // Draw arrow body
    if(direction == 'L' || direction == 'R') {
        canvas_draw_line(canvas, body_start_x, body_start_y, body_end_x, body_start_y); // Top side
        canvas_draw_line(canvas, body_start_x, body_end_y, body_end_x, body_end_y); // Bottom side
        if(direction == 'L') {
            canvas_draw_line(
                canvas, body_end_x, body_start_y, body_end_x, body_end_y); // Right side
        } else {
            canvas_draw_line(
                canvas, body_start_x, body_start_y, body_start_x, body_end_y); // Left side
        }
        if(fill) {
            canvas_draw_box(
                canvas, body_start_x, body_start_y, arrow_body_width, arrow_body_height);
        }

    } else {
        canvas_draw_line(
            canvas, body_start_x, body_start_y, body_start_x, body_end_y); // Left side
        canvas_draw_line(canvas, body_end_x, body_start_y, body_end_x, body_end_y); // Right side
        // draw arrow side base line
        if(direction == 'U') {
            canvas_draw_line(canvas, body_start_x, body_end_y, body_end_x, body_end_y);
        } else {
            canvas_draw_line(canvas, body_start_x, body_start_y, body_end_x, body_start_y);
        }
        if(fill) {
            canvas_draw_box(
                canvas, body_start_x, body_start_y, arrow_body_height, arrow_body_width);
        }
    }
    // Draw arrow head
    if(direction == 'L' || direction == 'R') {
        canvas_draw_line(canvas, head_tip_x, head_tip_y, head_base_x, head_base_start_y); // Side 1
        canvas_draw_line(canvas, head_tip_x, head_tip_y, head_base_x, head_base_end_y); // Side 2

        //  These lines now start from the body sides and end where they meet the arrow head slants
        if(direction == 'L') {
            canvas_draw_line(canvas, body_start_x, body_start_y, head_base_x, head_base_start_y);
            canvas_draw_line(canvas, body_start_x, body_end_y, head_base_x, head_base_end_y);
        } else {
            canvas_draw_line(canvas, body_end_x, body_start_y, head_base_x, head_base_start_y);
            canvas_draw_line(canvas, body_end_x, body_end_y, head_base_x, head_base_end_y);
        }

        if(fill) {
            canvas_draw_filled_triangle(
                canvas,
                head_tip_x,
                head_tip_y,
                head_base_x,
                head_base_start_y,
                head_base_x,
                head_base_end_y);
        }
    } else {
        canvas_draw_line(canvas, head_tip_x, head_tip_y, head_base_start_x, head_base_y); // Side 1
        canvas_draw_line(canvas, head_tip_x, head_tip_y, head_base_end_x, head_base_y); // Side 2
        //  These lines now start from the body sides and end where they meet the arrow head slants
        if(direction == 'U') {
            canvas_draw_line(canvas, body_start_x, body_start_y, head_base_start_x, head_base_y);
            canvas_draw_line(canvas, body_end_x, body_start_y, head_base_end_x, head_base_y);
        } else {
            canvas_draw_line(canvas, body_start_x, body_end_y, head_base_start_x, head_base_y);
            canvas_draw_line(canvas, body_end_x, body_end_y, head_base_end_x, head_base_y);
        }
        if(fill) {
            canvas_draw_filled_triangle(
                canvas,
                head_tip_x,
                head_tip_y,
                head_base_start_x,
                head_base_y,
                head_base_end_x,
                head_base_y);
        }
    }
}
