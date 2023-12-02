#include <stdint.h>
#include <stddef.h>

#include "pokemon_char_encode.h"

/* XXX Current text_input module only offers alnum and space input */
char pokemon_char_to_encoded(int byte) {
    switch(byte) {
    case 'A':
        return A_;
    case 'B':
        return B_;
    case 'C':
        return C_;
    case 'D':
        return D_;
    case 'E':
        return E_;
    case 'F':
        return F_;
    case 'G':
        return G_;
    case 'H':
        return H_;
    case 'I':
        return I_;
    case 'J':
        return J_;
    case 'K':
        return K_;
    case 'L':
        return L_;
    case 'M':
        return M_;
    case 'N':
        return N_;
    case 'O':
        return O_;
    case 'P':
        return P_;
    case 'Q':
        return Q_;
    case 'R':
        return R_;
    case 'S':
        return S_;
    case 'T':
        return T_;
    case 'U':
        return U_;
    case 'V':
        return V_;
    case 'W':
        return W_;
    case 'X':
        return X_;
    case 'Y':
        return Y_;
    case 'Z':
        return Z_;
    case 'a':
        return a_;
    case 'b':
        return b_;
    case 'c':
        return c_;
    case 'd':
        return d_;
    case 'e':
        return e_;
    case 'f':
        return f_;
    case 'g':
        return g_;
    case 'h':
        return h_;
    case 'i':
        return i_;
    case 'j':
        return j_;
    case 'k':
        return k_;
    case 'l':
        return l_;
    case 'm':
        return m_;
    case 'n':
        return n_;
    case 'o':
        return o_;
    case 'p':
        return p_;
    case 'q':
        return q_;
    case 'r':
        return r_;
    case 's':
        return s_;
    case 't':
        return t_;
    case 'u':
        return u_;
    case 'v':
        return v_;
    case 'w':
        return w_;
    case 'x':
        return x_;
    case 'y':
        return y_;
    case 'z':
        return z_;
    case '0':
        return _0_;
    case '1':
        return _1_;
    case '2':
        return _2_;
    case '3':
        return _3_;
    case '4':
        return _4_;
    case '5':
        return _5_;
    case '6':
        return _6_;
    case '7':
        return _7_;
    case '8':
        return _8_;
    case '9':
        return _9_;

        /* This was previously implemented with unicode escape codes, however, that
     * seemed to cause compilation issues. Which is strange because others reported
     * compilation issues with the actual unicode characters. I'm not sure a good
     * universal way to resolve this.
     *
     * Additionally, the ♂/♀ symbols don't render properly on the flipper. Would
     * need to create a custom image/icon somehow, otherwise its nonobvious that
     * the traded pokemon would have this symbol in their name.
     */

    case '\201':
        return MALE_;
    case '\200':
        return FEMALE_;
    default:
        return TERM_;
    }
}

int pokemon_encoded_to_char(char byte) {
    switch(byte) {
    case A_:
        return 'A';
    case B_:
        return 'B';
    case C_:
        return 'C';
    case D_:
        return 'D';
    case E_:
        return 'E';
    case F_:
        return 'F';
    case G_:
        return 'G';
    case H_:
        return 'H';
    case I_:
        return 'I';
    case J_:
        return 'J';
    case K_:
        return 'K';
    case L_:
        return 'L';
    case M_:
        return 'M';
    case N_:
        return 'N';
    case O_:
        return 'O';
    case P_:
        return 'P';
    case Q_:
        return 'Q';
    case R_:
        return 'R';
    case S_:
        return 'S';
    case T_:
        return 'T';
    case U_:
        return 'U';
    case V_:
        return 'V';
    case W_:
        return 'W';
    case X_:
        return 'X';
    case Y_:
        return 'Y';
    case Z_:
        return 'Z';
    case a_:
        return 'a';
    case b_:
        return 'b';
    case c_:
        return 'c';
    case d_:
        return 'd';
    case e_:
        return 'e';
    case f_:
        return 'f';
    case g_:
        return 'g';
    case h_:
        return 'h';
    case i_:
        return 'i';
    case j_:
        return 'j';
    case k_:
        return 'k';
    case l_:
        return 'l';
    case m_:
        return 'm';
    case n_:
        return 'n';
    case o_:
        return 'o';
    case p_:
        return 'p';
    case q_:
        return 'q';
    case r_:
        return 'r';
    case s_:
        return 's';
    case t_:
        return 't';
    case u_:
        return 'u';
    case v_:
        return 'v';
    case w_:
        return 'w';
    case x_:
        return 'x';
    case y_:
        return 'y';
    case z_:
        return 'z';
    case _0_:
        return '0';
    case _1_:
        return '1';
    case _2_:
        return '2';
    case _3_:
        return '3';
    case _4_:
        return '4';
    case _5_:
        return '5';
    case _6_:
        return '6';
    case _7_:
        return '7';
    case _8_:
        return '8';
    case _9_:
        return '9';

    /* This was previously implemented with unicode escape codes, however, that
     * seemed to cause compilation issues. Which is strange because others reported
     * compilation issues with the actual unicode characters. I'm not sure a good
     * universal way to resolve this.
     *
     * Additionally, the ♂/♀ symbols don't render properly on the flipper. Would
     * need to create a custom image/icon somehow, otherwise its nonobvious that
     * the traded pokemon would have this symbol in their name.
     */
    case MALE_:
        return '\201';
    case FEMALE_:
        return '\200';
    default:
        return '\0';
    }
}

/* encode n bytes, any currently noninputtable characters are set with TERM_ */
void pokemon_str_to_encoded_array(uint8_t* dest, char* src, size_t n) {
    for(; n > 0; n--) {
        *dest = pokemon_char_to_encoded(*src);
        dest++;
        src++;
    }
}

/* decode n bytes, any currently noninputtable characters are set with '\0' */
void pokemon_encoded_array_to_str(char* dest, uint8_t* src, size_t n) {
    for(; n > 0; n--) {
        *dest = pokemon_encoded_to_char(*src);
        dest++;
        src++;
    }
}
