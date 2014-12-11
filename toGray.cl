__kernel void toGray(__global uchar* in,
        __global uchar* out,
        int rows,
        int cols) {

    size_t tid = get_local_id(0);
    size_t gid = get_group_id(0);
    size_t idx = get_global_id(0);

    int write_addr = idx;
    int pull_addr = 3*write_addr;
    float tmp;    

    if (write_addr < cols*rows) {
        int b = in[pull_addr];
        int g = in[pull_addr+1];
        int r = in[pull_addr+2];
        tmp = b * 0.144;
        tmp += g * 0.587;
        tmp += r * 0.299;
        out[write_addr] = (unsigned char) tmp;
    }

}
