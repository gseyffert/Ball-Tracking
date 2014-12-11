__kernel void gaussBlur(__global uchar* in,
        __global uchar* out,
        int rows,
        int cols) {

	size_t gid = get_global_id(0);
	int i = gid/cols;
	int j = gid % cols;
	int radius = 6;
	if (gid < cols*rows) {
		float val = 0, wsum = 0;
		for(int iy = i-radius; iy<i+radius+1; iy++) {
	    	for(int ix = j-radius; ix<j+radius+1; ix++) {			
	        	int x = min(cols-1, max(0, ix));
	        	int y = min(rows-1, max(0, iy));
	        	float dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
	        	float wght = exp( -dsq / (2*radius*radius) ) / (3*2*radius*radius);
	        	val += in[y*cols+x] * wght;
	        	wsum += wght;
	        }
		}
		out[gid] = (unsigned char) round(val/wsum);
	}
}