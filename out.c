#include <ZeeBasic/Runtime/ZeeRuntime.h>

void program(void)
{
    zrt_String* v_s_s = zrt_str_empty();
    zrt_String* t_1 = zrt_str_new("Hi ");
    zrt_Int t_2 = 10;
    zrt_String* t_3 = zrt_str_new_from_int(t_2);
    zrt_String* t_4 = zrt_str_concat(t_1, t_3);
    zrt_str_del(t_3);
    zrt_str_del(t_1);
    zrt_str_copy(v_s_s, t_4);
    zrt_str_del(t_4);
    zrt_println_str(v_s_s);
    zrt_str_del(v_s_s);
}

int main(int argc, char* argv[])
{
    zrt_init(argc, argv);
    program();
    return 0;
}
