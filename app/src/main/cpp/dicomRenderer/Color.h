#ifndef COLOR_H
#define COLOR_H
inline void getRGBAColors(const int* hexValues, float* color, int num){
	for(int i=0; i<num; i++){
		int idx = 4*i;
		const int hexValue = hexValues[i];
		color[idx] = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
		color[idx+1] = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
		color[idx+2] = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte
		color[idx+3] = 1.0f;
	}
}

const int default_transcolor_num = 25;
const int default_trans_color[25] = {
		//blue
		0x011f4b,
		0x03396c,
		0x005b96,
		0x6497b1,
		0xb3cde0,
		///green1
		0x009688,
		0x35a79c,
		0x54b2a9,
		0x65c3ba,
		0x83d0c9,

		////green2
//		0x617B30,
//		0x74923A,
//		0x81A140,
//		0xA3BF45,
//		0xB5CF49,


		//GREEN-YELLOS
		0xa98600,
		0xdab600,
		0xf8ed62,
		0xfed601,
		0xfee402,

		///orange
		0xf0750f,
		0xf48020,
		0xf09537,
		0xf0a150,
		0Xf6412D,

//red-orange
		0xff9a00,
		0xff4d00,
		0xff7400,
		0xcc0000,
		0xff0000,

};
#endif