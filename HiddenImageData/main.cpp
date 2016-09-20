#include "include\opencv\cv.h"
#include "include\opencv\highgui.h"

#include <iostream>
#include <fstream>

//void 

// reads the text from a txt file
void readTextFile(const char *inFileName, char *&outText, int &sizeFile, bool ver = false)
{
	std::ifstream inFile;
	inFile.open("text.txt", std::ios::in|std::ios::binary);

	inFile.seekg(0, inFile.end);
	sizeFile = inFile.tellg();
	inFile.seekg(0, inFile.beg);

	outText = new char[sizeFile];

	inFile.read(outText, sizeFile);
	if(ver)
	{
		std::cout << "Size of file" << sizeFile <<std::endl;
		std::cout << "Contents: " << std::endl;
		for(int i = 0; i < sizeFile; i++)
		{
			std::cout << outText[i];
		}
		std::cout << std::endl;
	}
	inFile.close();
}

void encodeImage(cv::Mat &inImage, char *textToEncode, int fileSize)
{
	int totalBitsToWrite = fileSize*8 + 32;
	int bitCount = 0;
	for(int i = 0; i < inImage.cols; i++)
	{
		for(int j = 0; j < inImage.rows; j++)
		{
			cv::Vec3b curPoint = inImage.at<cv::Vec3b>(j, i);
			uchar ch1 = curPoint[0];
			
			if(bitCount < 32)
			{
				// encode the fileSize
				ch1 = ch1 & ((fileSize >> bitCount) & 1);
				curPoint[0] = ch1;
			}
			else
			{
				int curPos = bitCount-31;
				int curBit = curPos%8;
				int curChar = curPos/8;

				ch1 = ch1 & (textToEncode[curChar] >> curBit & 1);
			}
			bitCount++;
		}
	}
}

void decodeImage(cv::Mat &inImage, char *&textEncoded, int &fileSize)
{
	int bitCount = 0;
	fileSize = 0;
	for(int i = 0; i < inImage.cols; i++)
	{
		for(int j = 0; j < inImage.rows; j++)
		{
			cv::Vec3b curPoint = inImage.at<cv::Vec3b>(j, i);
			uchar ch1 = curPoint[0];
			
			if(bitCount < 32)
			{
				// encode the fileSize
				fileSize = fileSize | ((ch1 & 1) << bitCount);
				//curPoint[0] = ch1;
			}
			/*else
			{
				int curPos = bitCount-31;
				int curBit = curPos%8;
				int curChar = curPos/8;

				ch1 = ch1 | ((textToEncode[curChar] >> curBit | 0) << curBit);
			}*/

			bitCount++;
		}
	}

}

int main()
{
	int testing = 0;
	int testing2 = 121;

	for(int i = 0; i < 32; i++)
	{
		testing = testing | ((testing2 >> i) & 1) << i;
	}
	std::cout << testing <<std::endl;

	int fileSize;
	char *textToEncode;

	readTextFile("text.txt", textToEncode, fileSize, 0);

	cv::Mat inImage;
	inImage = cv::imread("TheMartian.jpg");

	cv::imshow("Show Image", inImage);
	//cv::waitKey(0);

	// encode the data in the image
	
	encodeImage(inImage, textToEncode, fileSize);
	int encodedFileSize = 0;
	char *textEncoded;
	decodeImage(inImage, textEncoded, encodedFileSize);

	std::cout << "Encoded FileSize was: " << encodedFileSize <<std::endl;
 	return 0;
}
