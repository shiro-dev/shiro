#ifndef COMMON_H
#define COMMON_H

/**
 *  When using this function, make sure to attach it to a const char* variable.
 *  Example:
 *      const char* string = concat("A", "B");
 *      size_t size = strlen(string);
 */
const char *concat(const char *first, const char *second)
{
    // Vars
    size_t size_first = strlen(first);
    size_t size_second = strlen(second);
    size_t size_total = size_first + size_second;
    size_t i = 0;
    size_t gi = 0;

    // Set buffer
    char buffer[size_total];
    
    // Run first string and push to buffer
    for (i = 0; i < size_first; i++)
    {
        buffer[gi] = first[i];
        gi++;
    }

    // Run second string and push to buffer
    for (i = 0; i < size_second; i++)
    {
        buffer[gi] = second[i];
        gi++;
    }

    // Mark the end of the string and push to buffer
    buffer[gi] = {'\0'};

    // Turn it into const char*
    const char* string = buffer;

    // Return
    return string;
}

#endif