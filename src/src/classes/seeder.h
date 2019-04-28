extern "C" uint32_t timestamp();

void seeder_run(){
    Log("Seeder: Initialized");
    console_writestring("Seeder: ");
    size_t x = console_column;
    size_t y = console_row;
    for(;;){
        console_column = x;
        console_row = y;
        console_writeint(rand());
        console_writestring("        ");
        delay(100);
    }
}