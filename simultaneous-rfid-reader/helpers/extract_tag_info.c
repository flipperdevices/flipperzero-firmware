#include "extract_tag_info.h"

/**
 * @brief      Function to extract the EPC value from the saved epcs file
 * @details    This function extracts the EPC from the saved epcs file.
 * @param      input  The char* input for the saved UHF tag in the file
 * @return     The EPC value from the given input
*/
char* extractEPC(const char* input) {

    // Find the position of the colon
    const char* colonPos = strchr(input, ':');

    if(colonPos != NULL) {
        
        //Size of epc
        size_t epcLen = 24;

        // Calculate the length of the string after the colon
        size_t strLen = strlen(colonPos + 1);

        if(strLen >= epcLen) {
            // Allocate memory for the EPC
            char* result = malloc(epcLen + 1);

            if(result != NULL) {
                // Copy the EPC into the result
                strncpy(result, colonPos + 1, epcLen);

                // Null-terminate the string
                result[epcLen] = '\0'; 
                return result;
            }
        }
    }

    return NULL;
}

/**
 * @brief      Function to extract the Reserved Memory value from the saved epcs file
 * @details    This function extracts the Reserved Memory from the saved epcs file.
 * @param      input  The char* input for the saved UHF tag in the file
 * @return     The Reserved Memory value from the given input
*/
char* extractRES(const char* input) {
    
    //There is probably a better way to do this...
    // Find the position of the first colon
    const char* firstColonPos = strchr(input, ':');
    
    
    if(firstColonPos != NULL) {
        
        // Move to the character after the first colon
        const char* secondColonPos = strchr(firstColonPos + 1, ':');
        
        if(secondColonPos != NULL) {
            
            // Move to the character after the second colon
            const char* thirdColonPos = strchr(secondColonPos + 1, ':');
            
            if(thirdColonPos != NULL) {
                
                // Move to the character after the third colon
                const char* startOfFourthValue = thirdColonPos + 1;

                // Find the next colon after the start of the fourth value
                const char* fourthColonPos = strchr(startOfFourthValue, ':');
                
                if(fourthColonPos != NULL) {
                    
                    // Calculate the length of the fourth value by subtracting the positions
                    size_t fourthValueLen = fourthColonPos - startOfFourthValue;

                    // Allocate memory for the fourth value
                    char* result = malloc(fourthValueLen + 1);
                    
                    if(result != NULL) {
                        
                        // Copy the fourth value into the result
                        strncpy(result, startOfFourthValue, fourthValueLen);
                        
                        // Null-terminate the string
                        result[fourthValueLen] = '\0'; 

                        return result;
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
 * @param      input  The char* input for the saved UHF tag in the file
 * @return     The TID value from the given input
*/
char* extractTID(const char* input) {
    
    // Find the position of the first colon
    const char* firstColonPos = strchr(input, ':');
    
    if(firstColonPos != NULL) {
        
        // Move to the character after the first colon
        const char* secondColonPos = strchr(firstColonPos + 1, ':');
        
        if(secondColonPos != NULL) {
            
            // Move to the character after the second colon
            const char* startOfThirdValue = secondColonPos + 1;

            // Find the next colon after the start of the third value
            const char* thirdColonPos = strchr(startOfThirdValue, ':');
            
            if(thirdColonPos != NULL) {
                
                // Calculate the length of the third value by subtracting the positions
                size_t thirdValueLen = thirdColonPos - startOfThirdValue;

                // Allocate memory for the third value
                char* result = malloc(thirdValueLen + 1);
                
                if(result != NULL) {
                    
                    // Copy the third value into the result
                    strncpy(result, startOfThirdValue, thirdValueLen);

                    // Null-terminate the string
                    result[thirdValueLen] = '\0'; 
                    return result;
                }
            }
        }
    }

    return NULL;
}

/**
 * @brief      Function to extract the User Memory value from the saved epcs file
 * @details    This function extracts the User Memory from the saved epcs file.
 * @param      input  The char* input for the saved UHF tag in the file
 * @return     The User Memory value from the given input
*/
char* extractMEM(const char* input) {

    // Find the position of the last colon
    const char* lastColonPos = strrchr(input, ':'); 
    
    if(lastColonPos != NULL) {
        
        // Move to the character after the last colon
        const char* startOfLastValue = lastColonPos + 1;

        // Calculate the length of the last value
        size_t lastValueLen = strlen(startOfLastValue);

        // Allocate memory for the last value
        char* result = malloc(lastValueLen + 1);
        
        if(result != NULL) {
            
            // Copy the last value into the result
            strncpy(result, startOfLastValue, lastValueLen);

            // Null-terminate the string
            result[lastValueLen] = '\0'; 

            return result;
        }
    }

    return NULL;
}

/**
 * @brief      Function to extract the Name from the saved epcs file
 * @details    This function extracts the Name from the saved epcs file.
 * @param      input  The char* input for the saved UHF tag in the file
 * @return     The Name from the given input
*/
char* extractName(const char* input) {
    
    // Find the position of the colon
    const char* colonPos = strchr(input, ':');

    if(colonPos != NULL) {
        
        // Calculate the length of the name
        size_t len = colonPos - input;

        // Allocate memory for the name
        char* result = malloc(len + 1);

        if(result != NULL) {
           
            // Copy the name into the result
            strncpy(result, input, len);

            // Null-terminate the string
            result[len] = '\0'; 
            return result;
        }
    }
    return NULL;
}