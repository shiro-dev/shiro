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
    // Static so the returned pointer stays valid after return.
    // The previous implementation used a stack-local VLA and returned
    // its address — undefined behavior. With GCC 13 -O2 it manifests
    // as the caller reading scancode bytes from the abandoned stack
    // frame instead of the concatenated string.
    static char buffer[256];

    size_t gi = 0;
    for (size_t i = 0; first[i] != '\0' && gi < sizeof(buffer) - 1; i++)
        buffer[gi++] = first[i];
    for (size_t i = 0; second[i] != '\0' && gi < sizeof(buffer) - 1; i++)
        buffer[gi++] = second[i];
    buffer[gi] = '\0';

    return buffer;
}

#endif