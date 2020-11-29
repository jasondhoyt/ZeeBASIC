#include <ZeeBasic/Runtime/ZeeRuntime.h>

void program(void)
{
    zrt_Int t_1 = 12;
    zrt_println_int(t_1);
}

int main(int argc, char* argv[])
{
    zrt_init(argc, argv);
    program();
    return 0;
}
