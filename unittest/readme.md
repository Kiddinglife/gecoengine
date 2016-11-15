for i in xrange(params_size)
    totalnum_func_handler += power(types_size, i)

3^0+3^1+3^2+3^3+3^4+3^5+3^6+3^7+3^8 = 
3 types power 0 param
0p_func(void);

typedef void (is_parser_t*)(is);
is_parser_t parsers[8][]=
{
    {0p_func,},
    {1p_int_func,1p_float_func,1p_str_func},
    {2p_int_int_func(is), 2p_int_float_func(is)...},
    .....
}
3 power 1 param
1p_int_func(is);
1p_float_func(is);
1p_str_func(is);

3 power 2 params
2p_int_int_func(is);
2p_int_float_func(is);
...

3 power 3 params
3p_int_int_int_func(is);
3p_int_int_floatfunc(is);
...

3 power 4 params
...

