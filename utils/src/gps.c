#include <math.h>

const double earth_radius_fai_km = 6371.0; /* km */
const double earth_radius_fai_sm = 3959.0; /* miles */
const double km_in_sm = 1.60934; /* convert miles to km */

#ifndef M_PI
#define M_PI 3.1415926535
#endif

/* latitude and longitude are specified in degrees */
double distance(double lat1, double lon1, double lat2, double lon2) {
	lat1 *= M_PI / 180.0; lon1 *= M_PI / 180.0;
	lat2 *= M_PI / 180.0; lon2 *= M_PI / 180.0;
	double a = sin((lat1 - lat2) / 2.0);
	double b = sin((lon1 - lon2) / 2.0); 
	return 2.0 * earth_radius_fai_sm * asin(sqrt(a * a + cos(lat1) * cos(lat2)* b * b));
}

