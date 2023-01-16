#include "input_event.h"

void cycle_focus(FlizzerTrackerApp *tracker)
{
    switch (tracker->mode)
    {
        case PATTERN_VIEW:
        {
            tracker->focus++;

            if (tracker->focus > EDIT_SONGINFO)
            {
                tracker->focus = EDIT_PATTERN;
            }
        }

        default:
            break;
    }
}

void process_input_event(FlizzerTrackerApp *tracker, FlizzerTrackerEvent *event)
{
    /*if(event->input.key == InputKeyBack && event->input.type == InputTypeShort && event->period > 0 && event->period < 200)
    {
            cycle_focus(tracker);
    }*/

    if (event->input.key == InputKeyBack && event->input.type == InputTypeShort && !(tracker->editing))
    {
        cycle_focus(tracker);
        return;
    }

    // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
    if (event->input.key == InputKeyBack && event->input.type == InputTypeLong)
    {
        tracker->quit = true;
        return;
    }

    switch (tracker->focus)
    {
        case EDIT_PATTERN:
        {
            pattern_edit_event(tracker, event);
            break;
        }

        case EDIT_SEQUENCE:
        {
            sequence_edit_event(tracker, event);
            break;
        }

        case EDIT_SONGINFO:
        {
            songinfo_edit_event(tracker, event);
            break;
        }

        default:
            break;
    }
}