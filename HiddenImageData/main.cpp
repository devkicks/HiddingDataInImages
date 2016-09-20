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
				bool ch2 = ((fileSize >> bitCount) & 1);

				// encode the fileSize

				// clear the last bit
				ch1 = ch1 & (0xFE);

				// store the last bit
				ch1 = ch1 | (ch2);
				curPoint[0] = ch1;
			}
			else
			{

				int curPos = bitCount-32;
				int curBit = curPos%8;
				int curChar = curPos/8;
				if( curChar < fileSize)
				{
					bool ch2 = ((textToEncode[curChar] >> curBit) & 1);

					ch1 = ch1 & 0xFE;

					ch1 = ch1 | ch2;
					curPoint[0] = ch1;
				}
			}

			inImage.at<cv::Vec3b>(j, i) = curPoint;
			bitCount++;
		}
	}
}

void decodeImage(cv::Mat &inImage, char *&textEncoded, int &fileSize)
{
	int bitCount = 0;
	fileSize = 0;
	char c = 0;
	for(int i = 0; i < inImage.cols; i++)
	{
		for(int j = 0; j < inImage.rows; j++)
		{
			cv::Vec3b curPoint = inImage.at<cv::Vec3b>(j, i);
			//uchar ch1 = curPoint[0];
			int ch1 = (int) curPoint[0];
			if(bitCount < 32)
			{
				// encode the fileSize

				fileSize = fileSize | ((ch1 & 1) << bitCount);
				//std::cout << ((ch1 & 1)) << std::endl;
				//curPoint[0] = ch1;
			}
			else
			{
				int curPos = bitCount-32;
				if(curPos == 0 )
				{
					// initialize string
					textEncoded = new char[fileSize+1];
					textEncoded[fileSize] = '\0';
				}

				int curBit = curPos%8;
				int curChar = curPos/8;
				if( curChar < fileSize)
				{
					if( curBit == 0)
						c = 0;
					c = c | ((ch1 & 1) << curBit);
					textEncoded[curChar] = c;
				}
			}

			bitCount++;
		}
	}

}

int main()
{
	int optionSelect = 0;

	std::cout << "Select one of the following options: " << std::endl;
	std::cout << "1. Encode message in an Image." << std::endl;
	std::cout << "2. Decode message from an Image." << std::endl;
	std::cin >> optionSelect;

	if( optionSelect == 1)
	{
		int fileSize;
		char *textToEncode;

		readTextFile("text.txt", textToEncode, fileSize, 0);

		cv::Mat inImage;
		inImage = cv::imread("TheMartian.jpg");

		// encode the data in the image
		encodeImage(inImage, textToEncode, fileSize);

		// saving as png as it is lossless
		cv::imwrite("MessageEncodedImage.png", inImage);
		
	}
	else
	{
		cv::Mat inImage;
		inImage = cv::imread("MessageEncodedImage.png");

		
		int encodedFileSize = 0;
		char *textEncoded;
		std::cout << std::endl;
		decodeImage(inImage, textEncoded, encodedFileSize);

		std::cout << "Encoded FileSize was: " << encodedFileSize <<std::endl;

		std::cout << "Encoded Text was: " << textEncoded << std::endl;
	}
	return 0;
}
