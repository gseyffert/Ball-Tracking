__kernel void toGray(__global uchar* in,
        __global uchar* out,
        int rows,
        int cols,
        int rowStep,
        int channels) {

    //size_t tid = get_local_id(0);
    //size_t gid = get_group_id(0);
    //size_t dim = get_local_size(0);
    size_t idx = get_global_id(0);

    int tmp;
    int size = cols*(rows+rowStep)*channels;
    for (int i = idx; i < size; i+=idx) {
        int index = channels * (i);
        uchar b = in[index];
        uchar g = in[index+1];
        uchar r = in[index+2];
        int tmp;    
        tmp = b << 1;
        tmp += (g << 2) + g;
        tmp += r;
        out[i] = (unsigned char) tmp;
    }
}


__kernel void gaussianBlur(__global uchar* in,
        __global uchar* out){


}
