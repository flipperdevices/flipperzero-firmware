#include "extract_tag_info.h"

/**
 * @brief      Function to extract the EPC value from the saved epcs file
 * @details    This function extracts the EPC from the saved epcs file.
 * @param      Input  The char* input for the saved UHF tag in the file
 * @return     The EPC value from the given input
*/
char* extract_epc(const char* Input) {
    // Find the position of the colon
    const char* ColonPos = strchr(Input, ':');

    if(ColonPos != NULL) {
        // Size of epc
        size_t EpcLen = 24;

        // Calculate the length of the string after the colon
        size_t StrLen = strlen(ColonPos + 1);

        if(StrLen >= EpcLen) {
            // Allocate memory for the EPC
            char* Result = malloc(EpcLen + 1);

            if(Result != NULL) {
                // Copy the EPC into the result
                strncpy(Result, ColonPos + 1, EpcLen);

                // Null-terminate the string
                Result[EpcLen] = '\0';
                return Result;
            }
        }
    }

    return NULL;
}

/**
 * @brief      Function to extract the Reserved Memory value from the saved epcs file
 * @details    This function extracts the Reserved Memory from the saved epcs file.
 * @param      Input  The char* input for the saved UHF tag in the file
 * @return     The Reserved Memory value from the given input
*/
char* extract_res(const char* Input) {
    // There is probably a better way to do this...
    // Find the position of the first colon
    const char* FirstColonPos = strchr(Input, ':');

    if(FirstColonPos != NULL) {
        // Move to the character after the first colon
        const char* SecondColonPos = strchr(FirstColonPos + 1, ':');

        if(SecondColonPos != NULL) {
            // Move to the character after the second colon
            const char* ThirdColonPos = strchr(SecondColonPos + 1, ':');

            if(ThirdColonPos != NULL) {
                // Move to the character after the third colon
                const char* StartOfFourthValue = ThirdColonPos + 1;

                // Find the next colon after the start of the fourth value
                const char* FourthColonPos = strchr(StartOfFourthValue, ':');

                if(FourthColonPos != NULL) {
                    // Calculate the length of the fourth value by subtracting the positions
                    size_t FourthValueLen = FourthColonPos - StartOfFourthValue;

                    // Allocate memory for the fourth value
                    char* Result = malloc(FourthValueLen + 1);

                    if(Result != NULL) {
                        // Copy the fourth value into the result
                        strncpy(Result, StartOfFourthValue, FourthValueLen);

                        // Null-terminate the string
                        Result[FourthValueLen] = '\0';

                        return Result;
                    }
                }
            }
        }
    }

    return NULL;
}

/**
 * @brief      Function to extract the TID value from the saved epcs file
 * @details    This function extracts the TID from the saved epcs file.
 * @param      Input  The char* input for the saved UHF tag in the file
 * @return     The TID value from the given input
*/
char* extract_tid(const char* Input) {
    // Find the position of the first colon
    const char* FirstColonPos = strchr(Input, ':');

    if(FirstColonPos != NULL) {
        // Move to the character after the first colon
        const char* SecondColonPos = strchr(FirstColonPos + 1, ':');

        if(SecondColonPos != NULL) {
            // Move to the character after the second colon
            const char* StartOfThirdValue = SecondColonPos + 1;

            // Find the next colon after the start of the third value
            const char* ThirdColonPos = strchr(StartOfThirdValue, ':');

            if(ThirdColonPos != NULL) {
                // Calculate the length of the third value by subtracting the positions
                size_t ThirdValueLen = ThirdColonPos - StartOfThirdValue;

                // Allocate memory for the third value
                char* Result = malloc(ThirdValueLen + 1);

                if(Result != NULL) {
                    // Copy the third value into the result
                    strncpy(Result, StartOfThirdValue, ThirdValueLen);

                    // Null-terminate the string
                    Result[ThirdValueLen] = '\0';
                    return Result;
                }
            }
        }
    }

    return NULL;
}

/**
 * @brief      Function to extract the User Memory value from the saved epcs file
 * @details    This function extracts the User Memory from the saved epcs file.
 * @param      Input  The char* input for the saved UHF tag in the file
 * @return     The User Memory value from the given input
*/
char* extract_mem(const char* Input) {
    // Find the position of the last colon
    const char* LastColonPos = strrchr(Input, ':');

    if(LastColonPos != NULL) {
        // Move to the character after the last colon
        const char* StartOfLastValue = LastColonPos + 1;

        // Calculate the length of the last value
        size_t LastValueLen = strlen(StartOfLastValue);

        // Allocate memory for the last value
        char* Result = malloc(LastValueLen + 1);

        if(Result != NULL) {
            // Copy the last value into the result
            strncpy(Result, StartOfLastValue, LastValueLen);

            // Null-terminate the string
            Result[LastValueLen] = '\0';

            return Result;
        }
    }

    return NULL;
}

/**
 * @brief      Function to extract the Name from the saved epcs file
 * @details    This function extracts the Name from the saved epcs file.
 * @param      Input  The char* input for the saved UHF tag in the file
 * @return     The Name from the given input
*/
char* extract_name(const char* Input) {
    // Find the position of the colon
    const char* ColonPos = strchr(Input, ':');

    if(ColonPos != NULL) {
        // Calculate the length of the name
        size_t Len = ColonPos - Input;

        // Allocate memory for the name
        char* Result = malloc(Len + 1);

        if(Result != NULL) {
            // Copy the name into the result
            strncpy(Result, Input, Len);

            // Null-terminate the string
            Result[Len] = '\0';
            return Result;
        }
    }
    return NULL;
}
