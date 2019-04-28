void Log(const char* value)
{
    unsigned short port = 0x3f8;
    for (size_t i = 0; i < strlen(value); i++)
    {
        asm volatile ("outb %0, %1" :: "a" (value[i]), "dN" (port));
    }
}