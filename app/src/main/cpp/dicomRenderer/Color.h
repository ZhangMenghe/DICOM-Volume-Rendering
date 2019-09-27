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
/*! \brief Convert RGB to HSV color space
 * input rgb: 0~1
 * output hsv: 0~360, 0~1, 0~1
*/
inline void RGBtoHSV(float& fR, float& fG, float fB, float& fH, float& fS, float& fV) {
	float fCMax = fmax(fmax(fR, fG), fB);
	float fCMin = fmin(fmin(fR, fG), fB);
	float fDelta = fCMax - fCMin;

	if(fDelta > 0) {
		if(fCMax == fR) {
			fH = 60 * (fmod(((fG - fB) / fDelta), 6));
		} else if(fCMax == fG) {
			fH = 60 * (((fB - fR) / fDelta) + 2);
		} else if(fCMax == fB) {
			fH = 60 * (((fR - fG) / fDelta) + 4);
		}

		if(fCMax > 0) {
			fS = fDelta / fCMax;
		} else {
			fS = 0;
		}

		fV = fCMax;
	} else {
		fH = 0;
		fS = 0;
		fV = fCMax;
	}

	if(fH < 0) {
		fH = 360 + fH;
	}
}


/*! \brief Convert HSV to RGB color space
 * output rgb: 0~1
 * input hsv: 0~360, 0~1, 0~1
*/
inline void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) {
	float fC = fV * fS; // Chroma
	float fHPrime = fmod(fH / 60.0, 6);
	float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
	float fM = fV - fC;

	if(0 <= fHPrime && fHPrime < 1) {
		fR = fC;
		fG = fX;
		fB = 0;
	} else if(1 <= fHPrime && fHPrime < 2) {
		fR = fX;
		fG = fC;
		fB = 0;
	} else if(2 <= fHPrime && fHPrime < 3) {
		fR = 0;
		fG = fC;
		fB = fX;
	} else if(3 <= fHPrime && fHPrime < 4) {
		fR = 0;
		fG = fX;
		fB = fC;
	} else if(4 <= fHPrime && fHPrime < 5) {
		fR = fX;
		fG = 0;
		fB = fC;
	} else if(5 <= fHPrime && fHPrime < 6) {
		fR = fC;
		fG = 0;
		fB = fX;
	} else {
		fR = 0;
		fG = 0;
		fB = 0;
	}

	fR += fM;
	fG += fM;
	fB += fM;
}
const int default_transcolor_num = 20;
const int default_trans_color[20] = {
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
//		0xa98600,
//		0xdab600,
//		0xf8ed62,
		0xfed601,
		0xfee402,

		///orange
		0xf0750f,
		0xf48020,
//		0xf09537,
//		0xf0a150,
//		0Xf6412D,
//
////red-orange
//		0xff9a00,
//		0xff4d00,
		0xff7400,

        0xff0000,
		0xcc0000,
        0xc20232,
		0xa3032b,
        0x530119,
};
#endif