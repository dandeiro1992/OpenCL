__kernel void Add(__global double* a, __global double* b)
{
    int n = get_global_id(0);
    b[n]=a[2*n] + a[2*n + 1];
}

__kernel void init(__global double* a)
{
    int n = get_global_id(0);
    a[n]=12/((2*n + 1.0)*(2*n + 1.0))-12/((2*n + 2.0)*(2*n + 2.0));
}
