// PnG2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "math.h"
#include <map>
#include <highgui.h>
#include <cv.h>
#include <png.hpp>
using namespace png;
using namespace std;

#define SIZE 32


//Строит негативчик
image<rgba_pixel> GetNegative(image<rgba_pixel> positive)
{
	image< rgba_pixel > negative(positive.get_width(), positive.get_height());
	for (size_t y = 0; y < negative.get_height(); ++y)
	{
		for (size_t x = 0; x < negative.get_width(); ++x)
		{
			negative[y][x] = rgba_pixel(255 - positive[y][x].red, 255 - positive[y][x].green, 255 - positive[y][x].blue, positive[y][x].alpha);
		}
	}
	return negative;
}

//строит гистограмку по цветам
map<int, int> GetHistogram(image<rgba_pixel> image)
{
	map<int, int> hist;
	for (size_t y = 0; y < image.get_height(); ++y)
	{
		for (size_t x = 0; x < image.get_width(); ++x)
		{
			rgba_pixel px = image[y][x];
			int r = px.red / SIZE;
			int g = px.green / SIZE;
			int b = px.blue / SIZE;
			int a = px.alpha / SIZE;

			int num = r*SIZE*SIZE*SIZE + g*SIZE*SIZE +
				b*SIZE + a;
			if (hist.count(num) == 0)
			{
				hist.insert(pair<int, int> {num, 1});
			}
			else
			{
				hist[num] = hist[num] + 1;
			}
		}
	}
	return hist;
}


//возвращает процент совпадения гистограмм
double CompareTwo(map<int, int> hist1, map<int, int> hist2)
{
	map<int, int> compared;
	double match = 0, mismatch = 0;

	for each (pair<int, int> p1 in hist1)
	{
		int n1 = p1.second;
		int n2 = (hist2.find(p1.first) == hist2.end()) ? 0 : hist2.find(p1.first)->second;
		compared.insert(pair<int, int> {p1.first, 0});
		match += n1 < n2 ? n1 : n2;
		mismatch += abs(n1 - n2);
	}


	for each (pair<int, int> p2 in hist2)
	{
		if (compared.find(p2.first) == compared.end())
			mismatch += p2.second;
	}

	return match / (mismatch + match);
}


//возвращает черно-белый вариант картинки
image<rgba_pixel> GetBlackWhite(image<rgba_pixel> positive)
{
	image< rgba_pixel > bw(positive.get_width(), positive.get_height());
	for (size_t y = 0; y < bw.get_height(); ++y)
	{
		for (size_t x = 0; x < bw.get_width(); ++x)
		{
			int num = (int)(0.21*positive[y][x].red + 0.72*positive[y][x].green + 0.07*positive[y][x].blue);
			bw[y][x] = rgba_pixel(num, num, num, 255);
		}
	}
	return bw;
}

//уменьшает размер картинки
image<rgba_pixel> Scale(image<rgba_pixel> source, int scale)
{
	image< rgba_pixel > scaled(source.get_width() / scale + 1, source.get_height() / scale + 1);
	for (size_t y = 0; y < scaled.get_height()-1; ++y)
	{
		for (size_t x = 0; x < scaled.get_width()-1; ++x)
		{
			int red = source[y*scale][x*scale].red;// 0;
			int green = source[y*scale][x*scale].green;// 0;
			int blue = source[y*scale][x*scale].blue;// 0;
			/*for (int i = y*scale; i < (y + 1)*scale;i++)
				for (int j = x*scale; j < (x + 1)*scale; j++)
				{
					red += positive[i][j].red;
					green += positive[i][j].green;
					blue += positive[i][j].blue;
				}*/
				scaled[y][x] = rgba_pixel(red/scale, green/scale, blue/scale, 255);
		}
	}
	return scaled;
}

//возвращает изображение сост. из черный и белых пикселей в зависимости от положения относительно среднего цвета
image<rgba_pixel> Bit(image<rgba_pixel> positive)
{
	int aver = 0;
	for (size_t y = 0; y < positive.get_height(); ++y)
	{
		for (size_t x = 0; x < positive.get_width(); ++x)
		{
			aver += positive[y][x].red;
		}
	}
	aver /= positive.get_height()* positive.get_width();

	image< rgba_pixel > bw(positive.get_width(), positive.get_height());
	for (size_t y = 0; y < bw.get_height(); ++y)
	{
		for (size_t x = 0; x < bw.get_width(); ++x)
		{
			int num = positive[y][x].red > aver ? 255 : 0;
			bw[y][x] = rgba_pixel(num, num, num, 255);
		}
	}
	return bw;
}


//возвращает процент совпадения картинок
double Percent(image<rgba_pixel> i1, image<rgba_pixel> i2)
{
	if (i1.get_height() != i2.get_height() || i1.get_width() != i2.get_width())
		return 0;

	int a = 0;
	for (size_t y = 0; y < i1.get_height(); ++y)
	{
		for (size_t x = 0; x < i1.get_width(); ++x)
		{
			rgba_pixel p1 = i1[y][x];
			rgba_pixel p2 = i2[y][x];
			if (i1[y][x].red == i2[y][x].red)
				a++;
		}
	}
	return a*100 / (i1.get_height()*i1.get_width());
}

//подряд уменьшает, обесцвечивает и строит битовую картинку
image<rgba_pixel> AllBit(image<rgba_pixel> positive,int scale)
{
	return (Bit(GetBlackWhite(Scale(positive, scale))));
}





int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	image<rgba_pixel> positive("input.png");
	image<rgba_pixel> negative = GetNegative(positive);
	negative.write("output.png");
	
	
	image< rgba_pixel > image1("input1.png");
	image< rgba_pixel > image2("input2.png");
	image1 = 
	AllBit(image1, 16);
	image2 = 
	AllBit(image2, 16);
	
	image1.write("output1.png");
	image2.write("output2.png");
	double percent = Percent(image1, image2);

	/*if (percent > 92)
		cout << "Картинки идентичны." << endl;
	else if (percent > 80)
		cout << "Картинки похожи." << endl;
	else
		cout << "Картинки различаются." << endl;
	system("pause");
	*/


	if (percent > 92)
		cout << "Картинки идентичны. Совпадение: " << Percent(image1, image2) << "%" << endl;
	else if (percent > 80)
		cout << "Картинки похожи. Совпадение: " << Percent(image1, image2) << "%" << endl;
	else 
		cout << "Картинки различаются. Совпадение: " << Percent(image1, image2) << "%" << endl;
	system("pause");

	/*cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	IplImage* imageS1 = cvLoadImage("output1.png", 1);
	cvShowImage("original", imageS1);
	
	cvWaitKey(0);

	imageS1 = cvLoadImage("output2.png", 1);
	cvShowImage("original", imageS1);

	cvWaitKey(0);
	
	*/
	return 0;
}


