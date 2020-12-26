#include <ZeeBasic/Runtime/ZeeRuntime.h>

void program(void)
{
    zrt_Int t_1 = 2;
    zrt_Int t_2 = 2;
    zrt_Int t_3 = t_1 + t_2;
    zrt_Int t_4 = 5;
    zrt_Int t_5 = t_3 * t_4;
    zrt_println_int(t_5);
}

int main(int argc, char* argv[])
{
    zrt_init(argc, argv);
    program();
    return 0;
}
