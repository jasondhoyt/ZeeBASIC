#include <ZeeBasic/Runtime/ZeeRuntime.h>

void program(void)
{
    zrt_Int v_a = 0;
    zrt_Bool t_1 = 1;
    zrt_Int t_2 = t_1 == 0 ? 0 : 1;
    v_a = t_2;
    zrt_println_int(v_a);
}

int main(int argc, char* argv[])
{
    zrt_init(argc, argv);
    program();
    return 0;
}
