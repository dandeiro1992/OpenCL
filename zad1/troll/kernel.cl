__kernel void dist( __global float *dst, __global float *c_x, __global float *c_y ) 
{ 
 dst[get_global_id(0)] = c_x[get_global_id(0)]*c_x[get_global_id(0)]+c_y[get_global_id(0)]*c_y[get_global_id(0)]; 
} 


