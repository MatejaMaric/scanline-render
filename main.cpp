#include<fstream>
#include<cmath>

class Vertice
{
public:
	float x, y, z;
	unsigned char red, green, blue;
	Vertice(float x, float y, float z, int red, int green, int blue)
	{
		this->x = x;
		this->y = y;
		this->z = z;

		this->red = (unsigned char)red;
		this->green = (unsigned char)green;
		this->blue = (unsigned char)blue;
	}
	void fitToImage(int imageWidth, int imageHeight)
	{
		x = (x * (imageWidth / 2)) + (imageWidth / 2);
		y = (-y * (imageHeight / 2)) + (imageHeight / 2);
	}
};

class Image
{
public:
	int imageWidth, imageHeight;
	unsigned char* pixels;
	Image(int imageWidth, int imageHeight)
	{
		this->imageWidth = imageWidth;
		this->imageHeight = imageHeight;
		pixels = new unsigned char[imageWidth * imageHeight * 3];
	}
	~Image()
	{
		delete[] pixels;
	}
	void setPixel(int x, int y, int red, int green, int blue)
	{
		int help_var = ((y * imageWidth) + x) * 3;
		pixels[help_var + 0] = (unsigned char)red;
		pixels[help_var + 1] = (unsigned char)green;
		pixels[help_var + 2] = (unsigned char)blue;
	}
	void fillPixels(int red, int green, int blue)
	{
		int help_var = imageWidth * imageHeight * 3;
		for (int i = 0; i < help_var; i += 3) {
			pixels[i + 0] = (unsigned char)red;
			pixels[i + 1] = (unsigned char)green;
			pixels[i + 2] = (unsigned char)blue;
		}
	}
	//-------------------SCANLINE TRIANGLE RASTERISATION---------------------------
	void fillFlatTopTriangle(Vertice* v1, Vertice* v2, Vertice* v3)
	{
		float invslope1 = (float)(v3->x - v1->x) / (float)(v3->y - v1->y);
		float invslope2 = (float)(v3->x - v2->x) / (float)(v3->y - v2->y);

		float curx1 = v3->x;
		float curx2 = v3->x;
		int cury1 = (int)ceil(v1->y - 0.5f);
		int cury2 = (int)ceil(v3->y - 0.5f);

		for (int i = cury2; i >= cury1; i--)
		{
			if(curx1 < curx2)
				for (int j = (int)ceil(curx1 - 0.5f); j <= (int)ceil(curx2 - 0.5f); j++)
					setPixel(j, i, 255, 0, 0);
			else
				for (int j = (int)ceil(curx2 - 0.5f); j <= (int)ceil(curx1 - 0.5f); j++)
					setPixel(j, i, 255, 0, 0);

			curx1 -= invslope1;
			curx2 -= invslope2;
		}
	}
	void fillFlatBottomTriangle(Vertice* v1, Vertice* v2, Vertice* v3)
	{
		float invslope1 = (float)(v2->x - v1->x) / (float)(v2->y - v1->y);
		float invslope2 = (float)(v3->x - v1->x) / (float)(v3->y - v1->y);

		float curx1 = v1->x;
		float curx2 = v1->x;
		int cury1 = (int)ceil(v1->y - 0.5f);
		int cury2 = (int)ceil(v2->y - 0.5f);
		for (int i = cury1; i <= cury2; i++)
		{
			if(curx1 < curx2)
				for (int j = (int)ceil(curx1 - 0.5f); j <= (int)ceil(curx2 - 0.5f); j++)
					setPixel(j, i, 255, 0, 0);
			else
				for (int j = (int)ceil(curx2 - 0.5f); j <= (int)ceil(curx1 - 0.5f); j++)
					setPixel(j, i, 255, 0, 0);

			curx1 += invslope1;
			curx2 += invslope2;
		}
	}
	void fillTriangle(Vertice* v1, Vertice* v2, Vertice* v3)
	{
		if (v1->y > v2->y)
			std::swap(v1, v2);
		if (v1->y > v3->y)
			std::swap(v1, v3);
		if (v2->y > v3->y)
			std::swap(v2, v3);

		if (v2->y == v3->y){
			if(v2->x < v3->x)
				std::swap(v2, v3);
			fillFlatBottomTriangle(v1, v2, v3);
		}
		else if (v1->y == v2->y){
			if(v1->x < v2->x)
				std::swap(v1, v2);
			fillFlatTopTriangle(v1, v2, v3);
		}
		else
		{
			Vertice v4(
				(v1->x + (((float)(v2->y - v1->y) / (float)(v3->y - v1->y)) * (v3->x - v1->x))), v2->y, 0.0f, 0, 0, 0);
			if(v1->x < v4.x){
				fillFlatBottomTriangle(v1, v2, &v4);
				fillFlatTopTriangle(v2, &v4, v3);
			}
			else{
				fillFlatBottomTriangle(v1, &v4, v2);
				fillFlatTopTriangle(&v4, v2, v3);
			}
		}

	}
	//-----------------------------------------------------------------------------
};

int main()
{
	Image image(800, 600);
	image.fillPixels(255, 255, 255);

	Vertice a(0.2f, 0.5f, 0.0f, 255, 0, 0);
	Vertice b(-0.5f, 0.0f, 0.0f, 0, 255, 0);
	Vertice c(0.5f, -0.5f, 0.0f, 0, 0, 255);

	a.fitToImage(image.imageWidth, image.imageHeight);
	b.fitToImage(image.imageWidth, image.imageHeight);
	c.fitToImage(image.imageWidth, image.imageHeight);
	
	image.fillTriangle(&a, &b, &c);

	std::ofstream imageFile;
	imageFile.open("./image.ppm");
	imageFile << "P6 " << image.imageWidth << " " << image.imageHeight << " 255 ";
	imageFile.write((char*)image.pixels, image.imageWidth * image.imageHeight * 3);
	imageFile.close();

	return 0;
}
