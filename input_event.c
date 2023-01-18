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

            break;
        }

        case INST_EDITOR_VIEW:
        {
            tracker->focus++;

            if (tracker->focus > EDIT_PROGRAM)
            {
                tracker->focus = EDIT_INSTRUMENT;
            }

            break;
        }

        default:
            break;
    }
}

void cycle_view(FlizzerTrackerApp *tracker)
{
    if (tracker->mode == PATTERN_VIEW)
    {
        tracker->mode = INST_EDITOR_VIEW;
        tracker->focus = EDIT_INSTRUMENT;

        tracker->selected_param = 0;
        tracker->current_digit = 0;

        return;
    }

    if (tracker->mode == INST_EDITOR_VIEW)
    {
        tracker->mode = PATTERN_VIEW;
        tracker->focus = EDIT_PATTERN;

        tracker->selected_param = 0;
        tracker->current_digit = 0;

        return;
    }
}

void process_input_event(FlizzerTrackerApp *tracker, FlizzerTrackerEvent *event)
{
    if (event->input.key == InputKeyBack && event->input.type == InputTypeShort && event->period > 0 && event->period < 300 && !(tracker->editing))
    {
        cycle_view(tracker);
        return;
    }

    else if (event->input.key == InputKeyBack && event->input.type == InputTypeShort && !(tracker->editing))
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

        case EDIT_INSTRUMENT:
        {
            instrument_edit_event(tracker, event);
            break;
        }

        case EDIT_PROGRAM:
        {
            instrument_program_edit_event(tracker, event);
            break;
        }

        default:
            break;
    }
}