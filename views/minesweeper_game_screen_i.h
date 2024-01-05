#ifndef MINESWEEPERGAMESCREEN_I_H
#define MINESWEEPERGAMESCREEN_I_H

#include <stdlib.h>
#include <stdbool.h>
#include "m-rbtree.h"
#include "m-deque.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t x,y;
} Point;

typedef struct {
    bool allocated;
	Point p;
} Point_t[1];

static inline void pointobj_init(Point_t dest) {
    dest->p = (Point) {.x = 0, .y = 0};
    dest->allocated = true;
}

static inline void pointobj_clear(Point_t dest) {
    assert(dest->allocated);
    dest->allocated = false;
}

static inline void pointobj_init_set(Point_t dest, const Point_t source) {
    dest->p = source->p;
    dest->allocated = true;
}

static inline void pointobj_set(Point_t dest, const Point_t source) {
    if (dest != source) {
        pointobj_clear(dest);
        pointobj_init_set(dest, source); 
    }
}

static inline void pointobj_set_point(Point_t dest, Point value) {
    dest->p = value;
}

static inline void pointobj_init_set_point(Point_t dest, Point val) {
    pointobj_init(dest);
    pointobj_set_point(dest, val);
}

static inline bool pointobj_equal_p(const Point_t a, const Point_t b) {

    return (a->p.x == b->p.x && a->p.y == b->p.y) ? true : false;
}

static inline int pointobj_cmp(const Point_t a, const Point_t b) {
    if (a->p.x != b->p.x) return (a->p.x < b->p.x) ? -1 : 1;
    if (a->p.y != b->p.y) return (a->p.y < b->p.y) ? -1 : 1; 
    return 0;
}

// This is the oplist needed to define the rb tree and dequeue and uses the/
// above functions.
#define POINT_OPLIST (                      \
            TYPE(Point_t),                  \
            INIT(pointobj_init),            \
            INIT_SET(pointobj_init_set),    \
            SET(pointobj_set),              \
            CLEAR(pointobj_clear),          \
            EQUAL(pointobj_equal_p),        \
            CMP(pointobj_cmp)               \
        )

// Example Macro defining the RBTREE for Point that will be used as an ordered set
//      RBTREE_DEF(point_set, Point_t, POINT_OPLIST)

//
// Example Macro defining the DEQ for Point that will be used as a double ended queue
//      DEQUE_DEF (point_deq, Point_t, POINT_OPLIST)


//// Printing deq to debug
//static inline void print_deq_to_debug(point_deq_t* deq) {
//
//    point_deq_it_t it;
//
//    for(point_deq_it(it, *deq); !point_deq_end_p(it); point_deq_next(it)) {
//        Point ref = pointobj_get_point(*point_deq_ref(it));
//		
//		
//        // ADD FURI LOG TO DEBUG INSTEAD OF PRINTF
//		printf("(%hd,%hd), ", ref.x, ref.y);
//    }
//}
//
//// Printing ordered set to debug
//static inline void print_ordered_set_to_debug(point_set_t* tree) {
//
//    point_set_it_t it;
//
//    for(point_set_it(it, *tree); !point_set_end_p(it); point_set_next(it)) {
//        Point ref = pointobj_get_point(*point_set_ref(it));
//		
//		
//        // ADD FURI LOG TO DEBUG INSTEAD OF PRINTF
//		printf("(%hd,%hd), ", ref.x, ref.y);
//    }
//}
//
//// Helper to convert the Point_t type to Point
//static inline Point pointobj_get_point(const Point_t z) {
//    return z->p;
//}

#ifdef __cplusplus
}
#endif

#endif
