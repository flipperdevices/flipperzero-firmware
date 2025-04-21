#include "../test.h" // IWYU pragma: keep
#include <float_tools.h>
#include <notification/notification_messages_notes.h>

bool frequency_is_equal(const char* note_name, const NotificationMessage* message) {
    float a = notification_messages_notes_frequency_from_name(note_name);
    float b = message->data.sound.frequency;
    const float epsilon = message->data.sound.frequency > 5000 ? 0.02f : 0.01f;
    return fabsf(a - b) < epsilon;
}

MU_TEST(notification_messages_notes_frequency_from_name_test) {
    // Upper case
    mu_check(float_is_equal(
        notification_messages_notes_frequency_from_name("C0"),
        notification_messages_notes_frequency_from_name("c0")));

    // Mixed case
    mu_check(float_is_equal(
        notification_messages_notes_frequency_from_name("Cs0"),
        notification_messages_notes_frequency_from_name("cs0")));

    // Check errors
    mu_check(
        float_is_equal(notification_messages_notes_frequency_from_name("0"), 0.0)); // Without note
    mu_check(float_is_equal(
        notification_messages_notes_frequency_from_name("C"), 0.0)); // Without octave
    mu_check(float_is_equal(
        notification_messages_notes_frequency_from_name("C9"), 0.0)); // Unsupported octave
    mu_check(float_is_equal(
        notification_messages_notes_frequency_from_name("C10"), 0.0)); // Unsupported octave
    mu_check(float_is_equal(
        notification_messages_notes_frequency_from_name("X0"), 0.0)); // Unknown note
    mu_check(float_is_equal(
        notification_messages_notes_frequency_from_name("CCC0"), 0.0)); // Note name overflow

    // Notes and structures
    mu_check(frequency_is_equal("c0", &message_note_c0));
    mu_check(frequency_is_equal("cs0", &message_note_cs0));
    mu_check(frequency_is_equal("d0", &message_note_d0));
    mu_check(frequency_is_equal("ds0", &message_note_ds0));
    mu_check(frequency_is_equal("e0", &message_note_e0));
    mu_check(frequency_is_equal("f0", &message_note_f0));
    mu_check(frequency_is_equal("fs0", &message_note_fs0));
    mu_check(frequency_is_equal("g0", &message_note_g0));
    mu_check(frequency_is_equal("gs0", &message_note_gs0));
    mu_check(frequency_is_equal("a0", &message_note_a0));
    mu_check(frequency_is_equal("as0", &message_note_as0));
    mu_check(frequency_is_equal("b0", &message_note_b0));

    mu_check(frequency_is_equal("c1", &message_note_c1));
    mu_check(frequency_is_equal("cs1", &message_note_cs1));
    mu_check(frequency_is_equal("d1", &message_note_d1));
    mu_check(frequency_is_equal("ds1", &message_note_ds1));
    mu_check(frequency_is_equal("e1", &message_note_e1));
    mu_check(frequency_is_equal("f1", &message_note_f1));
    mu_check(frequency_is_equal("fs1", &message_note_fs1));
    mu_check(frequency_is_equal("g1", &message_note_g1));
    mu_check(frequency_is_equal("gs1", &message_note_gs1));
    mu_check(frequency_is_equal("a1", &message_note_a1));
    mu_check(frequency_is_equal("as1", &message_note_as1));
    mu_check(frequency_is_equal("b1", &message_note_b1));

    mu_check(frequency_is_equal("c2", &message_note_c2));
    mu_check(frequency_is_equal("cs2", &message_note_cs2));
    mu_check(frequency_is_equal("d2", &message_note_d2));
    mu_check(frequency_is_equal("ds2", &message_note_ds2));
    mu_check(frequency_is_equal("e2", &message_note_e2));
    mu_check(frequency_is_equal("f2", &message_note_f2));
    mu_check(frequency_is_equal("fs2", &message_note_fs2));
    mu_check(frequency_is_equal("g2", &message_note_g2));
    mu_check(frequency_is_equal("gs2", &message_note_gs2));
    mu_check(frequency_is_equal("a2", &message_note_a2));
    mu_check(frequency_is_equal("as2", &message_note_as2));
    mu_check(frequency_is_equal("b2", &message_note_b2));

    mu_check(frequency_is_equal("c3", &message_note_c3));
    mu_check(frequency_is_equal("cs3", &message_note_cs3));
    mu_check(frequency_is_equal("d3", &message_note_d3));
    mu_check(frequency_is_equal("ds3", &message_note_ds3));
    mu_check(frequency_is_equal("e3", &message_note_e3));
    mu_check(frequency_is_equal("f3", &message_note_f3));
    mu_check(frequency_is_equal("fs3", &message_note_fs3));
    mu_check(frequency_is_equal("g3", &message_note_g3));
    mu_check(frequency_is_equal("gs3", &message_note_gs3));
    mu_check(frequency_is_equal("a3", &message_note_a3));
    mu_check(frequency_is_equal("as3", &message_note_as3));
    mu_check(frequency_is_equal("b3", &message_note_b3));

    mu_check(frequency_is_equal("c4", &message_note_c4));
    mu_check(frequency_is_equal("cs4", &message_note_cs4));
    mu_check(frequency_is_equal("d4", &message_note_d4));
    mu_check(frequency_is_equal("ds4", &message_note_ds4));
    mu_check(frequency_is_equal("e4", &message_note_e4));
    mu_check(frequency_is_equal("f4", &message_note_f4));
    mu_check(frequency_is_equal("fs4", &message_note_fs4));
    mu_check(frequency_is_equal("g4", &message_note_g4));
    mu_check(frequency_is_equal("gs4", &message_note_gs4));
    mu_check(frequency_is_equal("a4", &message_note_a4));
    mu_check(frequency_is_equal("as4", &message_note_as4));
    mu_check(frequency_is_equal("b4", &message_note_b4));

    mu_check(frequency_is_equal("c5", &message_note_c5));
    mu_check(frequency_is_equal("cs5", &message_note_cs5));
    mu_check(frequency_is_equal("d5", &message_note_d5));
    mu_check(frequency_is_equal("ds5", &message_note_ds5));
    mu_check(frequency_is_equal("e5", &message_note_e5));
    mu_check(frequency_is_equal("f5", &message_note_f5));
    mu_check(frequency_is_equal("fs5", &message_note_fs5));
    mu_check(frequency_is_equal("g5", &message_note_g5));
    mu_check(frequency_is_equal("gs5", &message_note_gs5));
    mu_check(frequency_is_equal("a5", &message_note_a5));
    mu_check(frequency_is_equal("as5", &message_note_as5));
    mu_check(frequency_is_equal("b5", &message_note_b5));

    mu_check(frequency_is_equal("c6", &message_note_c6));
    mu_check(frequency_is_equal("cs6", &message_note_cs6));
    mu_check(frequency_is_equal("d6", &message_note_d6));
    mu_check(frequency_is_equal("ds6", &message_note_ds6));
    mu_check(frequency_is_equal("e6", &message_note_e6));
    mu_check(frequency_is_equal("f6", &message_note_f6));
    mu_check(frequency_is_equal("fs6", &message_note_fs6));
    mu_check(frequency_is_equal("g6", &message_note_g6));
    mu_check(frequency_is_equal("gs6", &message_note_gs6));
    mu_check(frequency_is_equal("a6", &message_note_a6));
    mu_check(frequency_is_equal("as6", &message_note_as6));
    mu_check(frequency_is_equal("b6", &message_note_b6));

    mu_check(frequency_is_equal("c7", &message_note_c7));
    mu_check(frequency_is_equal("cs7", &message_note_cs7));
    mu_check(frequency_is_equal("d7", &message_note_d7));
    mu_check(frequency_is_equal("ds7", &message_note_ds7));
    mu_check(frequency_is_equal("e7", &message_note_e7));
    mu_check(frequency_is_equal("f7", &message_note_f7));
    mu_check(frequency_is_equal("fs7", &message_note_fs7));
    mu_check(frequency_is_equal("g7", &message_note_g7));
    mu_check(frequency_is_equal("gs7", &message_note_gs7));
    mu_check(frequency_is_equal("a7", &message_note_a7));
    mu_check(frequency_is_equal("as7", &message_note_as7));
    mu_check(frequency_is_equal("b7", &message_note_b7));

    mu_check(frequency_is_equal("c8", &message_note_c8));
    mu_check(frequency_is_equal("cs8", &message_note_cs8));
    mu_check(frequency_is_equal("d8", &message_note_d8));
    mu_check(frequency_is_equal("ds8", &message_note_ds8));
    mu_check(frequency_is_equal("e8", &message_note_e8));
    mu_check(frequency_is_equal("f8", &message_note_f8));
    mu_check(frequency_is_equal("fs8", &message_note_fs8));
    mu_check(frequency_is_equal("g8", &message_note_g8));
    mu_check(frequency_is_equal("gs8", &message_note_gs8));
    mu_check(frequency_is_equal("a8", &message_note_a8));
    mu_check(frequency_is_equal("as8", &message_note_as8));
    mu_check(frequency_is_equal("b8", &message_note_b8));
}

MU_TEST_SUITE(notes_suite) {
    MU_RUN_TEST(notification_messages_notes_frequency_from_name_test);
}

int run_minunit_test_notes(void) {
    MU_RUN_SUITE(notes_suite);
    return MU_EXIT_CODE;
}

TEST_API_DEFINE(run_minunit_test_notes)
