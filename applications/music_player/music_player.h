// TODO float note freq
typedef enum {
    // Delay
    N = 0,
    // Octave 3
    C3 = 131,
    C_3 = 139,
    D3 = 147,
    D_3 = 156,
    E3 = 165,
    F3 = 175,
    F_3 = 185,
    G3 = 196,
    G_3 = 208,
    A3 = 220,
    A_3 = 233,
    B3 = 247,
    // Octave 4
    C4 = 262,
    C_4 = 277,
    D4 = 294,
    D_4 = 311,
    E4 = 330,
    F4 = 349,
    F_4 = 370,
    G4 = 400,
    G_4 = 415,
    A4 = 440,
    A_4 = 466,
    B4 = 494,
    // Octave 5
    C5 = 523,
    C_5 = 554,
    D5 = 587,
    D_5 = 622,
    E5 = 659,
    F5 = 689,
    F_5 = 740,
    G5 = 784,
    G_5 = 831,
    A5 = 880,
    A_5 = 932,
    B5 = 988,
    // Octave 6
    C6 = 1046,
    C_6 = 1109,
    D6 = 1175,
    D_6 = 1245,
    E6 = 1319,
    F6 = 1397,
    F_6 = 1480,
    G6 = 1568,
    G_6 = 1661,
    A6 = 1760,
    A_6 = 1865,
    B6 = 1976,
    // Octave 7
    C7 = 2093,
    C_7 = 2217,
    D7 = 2349,
    D_7 = 2489,
    E7 = 2637,
    F7 = 2794,
    F_7 = 2960,
    G7 = 3136,
    G_7 = 3322,
    A7 = 3520,
    A_7 = 3729,
    B7 = 3951,
    // Ocatave 8
    C8 = 4186,
    C_8 = 4435,
    D8 = 4699,
    D_8 = 4978,
    E8 = 5274,
    F8 = 5588,
    F_8 = 5920,
    G8 = 6272,
    G_8 = 6645,
    A8 = 7040,
    A_8 = 7459,
    B8 = 7902,
} MelodyEventNote;

typedef enum {
    L1 = 1,
    L2 = 2,
    L4 = 4,
    L8 = 8,
    L12 = 12, //Needed for triplets
    L16 = 16,
    L32 = 32,
    L64 = 64,
    L128 = 128,
} MelodyEventLength;

typedef struct {
    MelodyEventNote note;
    MelodyEventLength length;
} MelodyEventRecord;

const char* get_note_name(const MelodyEventRecord* note_record) {
    if(note_record == NULL) return "";

    switch(note_record->note) {
    case N:
        return N;
        break;
    case C3:
        return "C3-";
        break;
    case C_3:
        return "C#3";
        break;
    case D3:
        return "D3-";
        break;
    case D_3:
        return "D#3";
        break;
    case E3:
        return "E3-";
        break;
    case F3:
        return "F3-";
        break;
    case F_3:
        return "F#3";
        break;
    case G3:
        return "G3-";
        break;
    case G_3:
        return "G#3";
        break;
    case A3:
        return "A3-";
        break;
    case A_3:
        return "A#3";
        break;
    case B3:
        return "B3-";
        break;
    case C4:
        return "C4-";
        break;
    case C_4:
        return "C#4";
        break;
    case D4:
        return "D4-";
        break;
    case D_4:
        return "D#4";
        break;
    case E4:
        return "E4-";
        break;
    case F4:
        return "F4-";
        break;
    case F_4:
        return "F#4";
        break;
    case G4:
        return "G4-";
        break;
    case G_4:
        return "G#4";
        break;
    case A4:
        return "A4-";
        break;
    case A_4:
        return "A#4";
        break;
    case B4:
        return "B4-";
        break;
    case C5:
        return "C5-";
        break;
    case C_5:
        return "C#5";
        break;
    case D5:
        return "D5-";
        break;
    case D_5:
        return "D#5";
        break;
    case E5:
        return "E5-";
        break;
    case F5:
        return "F5-";
        break;
    case F_5:
        return "F#5";
        break;
    case G5:
        return "G5-";
        break;
    case G_5:
        return "G#5";
        break;
    case A5:
        return "A5-";
        break;
    case A_5:
        return "A#5";
        break;
    case B5:
        return "B5-";
        break;
    case C6:
        return "C6-";
        break;
    case C_6:
        return "C#6";
        break;
    case D6:
        return "D6-";
        break;
    case D_6:
        return "D#6";
        break;
    case E6:
        return "E6-";
        break;
    case F6:
        return "F6-";
        break;
    case F_6:
        return "F#6";
        break;
    case G6:
        return "G6-";
        break;
    case G_6:
        return "G#6";
        break;
    case A6:
        return "A6-";
        break;
    case A_6:
        return "A#6";
        break;
    case B6:
        return "B6-";
        break;
    case C7:
        return "C7-";
        break;
    case C_7:
        return "C#7";
        break;
    case D7:
        return "D7-";
        break;
    case D_7:
        return "D#7";
        break;
    case E7:
        return "E7-";
        break;
    case F7:
        return "F7-";
        break;
    case F_7:
        return "F#7";
        break;
    case G7:
        return "G7-";
        break;
    case G_7:
        return "G#7";
        break;
    case A7:
        return "A7-";
        break;
    case A_7:
        return "A#7";
        break;
    case B7:
        return "B7-";
        break;
    case C8:
        return "C8-";
        break;
    case C_8:
        return "C#8";
        break;
    case D8:
        return "D8-";
        break;
    case D_8:
        return "D#8";
        break;
    case E8:
        return "E8-";
        break;
    case F8:
        return "F8-";
        break;
    case F_8:
        return "F#8";
        break;
    case G8:
        return "G8-";
        break;
    case G_8:
        return "G#8";
        break;
    case A8:
        return "A8-";
        break;
    case A_8:
        return "A#8";
        break;
    case B8:
        return "B8-";
        break;
    default:
        return "UNK";
        break;
    }
}

const char* get_note_len_name(const MelodyEventRecord* note_record) {
    if(note_record == NULL) return "";

    switch(note_record->length) {
    case L1:
        return "1-";
        break;
    case L2:
        return "2-";
        break;
    case L4:
        return "4-";
        break;
    case L8:
        return "8-";
        break;
    case L12:
        return "12";
        break;
    case L16:
        return "16";
        break;
    case L32:
        return "32";
        break;
    case L64:
        return "64";
        break;
    case L128:
        return "1+";
        break;
    default:
        return "--";
        break;
    }
}
