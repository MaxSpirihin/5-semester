// Coords.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <math.h>

double m = -0.12e-6,
Rx = 0, Ry = 0, Rz = 0.82e-6,
dX = -1.1, dY = -0.3, dZ = -0.9;

struct Vector3
{
	double x;
	double y;
	double z;
};

Vector3 PZ90toWGS(Vector3 PZ90)
{

	double M = 1 + m;
	Vector3 WGS;
	WGS.x = (PZ90.x - Rz*PZ90.y + Ry*PZ90.z)*M + dX;
	WGS.y = (Rz*PZ90.x + PZ90.y - Rx*PZ90.z)*M + dY;
	WGS.z = (-Ry*PZ90.x + Rx*PZ90.y + PZ90.z)*M + dZ;

	return WGS;
}


Vector3 ToMetres(double lat, double lon)
{
	Vector3 res;
	lat *= 3.14159265359 / 180;
	lon *= 3.14159265359 / 180;
	res.x = 6371000 * cos(lat) * cos(lon);
	res.y = 6371000 * cos(lat) * sin(lon);
	res.z = 6371000 * sin(lat);
	return res;
}


void toDegrees(double*lat, double*lon,Vector3 m)
{
	*lat = asin(m.z / 6371000) * 180 / 3.14159265359;
	*lon = atan2(m.y, m.x) * 180 / 3.14159265359;
}

Vector3 WGStoPZ90(Vector3 WGS)
{
	double M = 1 - m;
	Vector3 PZ90;
	PZ90.x = (WGS.x + Rz*WGS.y - Ry*WGS.z)*M - dX;
	PZ90.y = (-Rz*WGS.x + WGS.y + Rx*WGS.z)*M - dY;
	PZ90.z = (+Ry*WGS.x - Rx*WGS.y + WGS.z)*M - dZ;

	return PZ90;
}



int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian");

	double oldLat = 55.7522200;
	double oldLon = 37.6155600;

	Vector3 c1 = ToMetres(oldLat, oldLon);
	



	Vector3 c2 = WGStoPZ90(c1);
	cout << c2.x << " " << c2.y << " " << c2.z << endl;
	//Vector3 c3 = PZ90toWGS(c2);
	//cout << "Пересчет в обратную сторону:" << endl;
	cout << c2.x - c1.x << " " << c2.y - c1.y << " " << c2.z - c1.z << endl;


	double lat; 
	double lon;
	toDegrees(&lat, &lon, c2);
	c2 = ToMetres(lat - oldLat, lon - oldLon);

	cout << "Секунды - " << (lat - oldLat) * 3600 << " " << (lon - oldLon) * 3600 << endl;

	cout<<"МЕтры - " << (lat - oldLat) * 6371000 << " " << (lon - oldLon) * 6371000 << endl;

	system("pause");
	return 0;
}




