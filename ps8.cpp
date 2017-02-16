/* ////////////////////////////////////////////////////////////

File Name: ps8.cpp
Copyright (c) 2016 Anchit Sood (sood.anchit@gmail.com).  All rights reserved.


Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

//////////////////////////////////////////////////////////// */



/*
Author: Anchit Sood
Andrew ID: anchits
Course: 24-787 - Engineering Computation


This C++ program takes a file name as input from the user, and shows the picture described in the specified text file.
It consists of a class that reads the file content, and draws lines on the screen. The overall picture window size is 800x600.

Content of each file must be as follows:

1st line: Number of lines
2nd line: x1 y1 x2 y2 r g b (line coordinates and color)
3nd line: x1 y1 x2 y2 r g b (line coordinates and color)
:
:

Y-coordinate in the data file is taken such that the bottom of the window is zero.
Therefore, Y-coordinates are flipped upside down to get a correct picture.
*/



#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "fssimplewindow.h"



///////////////////////////////////////////////////////////////////
//Helper functions
///////////////////////////////////////////////////////////////////

int XScr(double x);	//Converts real coordinate to screen coordinate, truncates decimal
int YScr(double y);	//Converts real coordinate to screen coordinate, truncates decimal
void DrawLine(double x1, double y1, double x2, double y2, int r, int g, int b);	//Draws a line from (x1,y1) to (x2,y2) in the specified color r, g, b

///////////////////////////////////////////////////////////////////
//end of helper functions
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//StringParser class - refer comments for rundown on each function
///////////////////////////////////////////////////////////////////

class StringParser
{
protected:
	char *str;
	int *wordTop, *wordLength;
	int nw;

public:
	StringParser();
	~StringParser();
	void CleanUp(void);
	//Cleans up the pointers above

	
	int Parse(char s[]);	//Usable function!!!

	//Parses the string using the protected Parse function below
	//Use as is, everything is already accounted for!

protected:
	int Parse(int wordTop[], int wordLength[], int maxNumWord, char str[]);	
	//Parses the string, fills wordTop with the index of each new word, 
	//wordLength with the length of each new word, and takes str as input
	//Uses maxNumWord to as upper bound to the number of words being parsed (maxNumWord is automatically calculated assuming worst case scenario below)
	//Can handle worst case scenario, automatically compensates even if input string is something like "t h i s" (where each letter is a word)

public:
	int NW(void);	//Usable function!!!

	//Returns number of words in the string

	
	void GetWord(char word[], int sizeofthisword, int atposition);	//Usable function!!!

	//Populates word[] (specified outside of class) with the word at position 'atposition' (indexed at zero) in the input string
	//Must specify the length of word[] (specified outside of class) in the variable 'sizeofthisword'
	//'sizeofthisword' variable must be large enough to also accomodate '0' at the end of string word[]
	//Similarly, array word[] must be able to accomodate a '0' at the end, so its size must be atleast 1 more than size of the word intended,
	//or there will be an indexoutofbounds error
};

StringParser::StringParser()
{
	str = nullptr;
	wordTop = nullptr;
	wordLength = nullptr;
	nw = 0;
}

StringParser::~StringParser()
{
	CleanUp();
}

void StringParser::CleanUp(void)
{
	if (nullptr != str)
	{
		delete[] str;
		str = nullptr;
	}
	if (nullptr != wordTop)
	{
		delete[] wordTop;
		wordTop = nullptr;
	}
	if (nullptr != wordLength)
	{
		delete[] wordLength;
		wordTop = nullptr;
	}
}

int StringParser::NW(void)
{
	return nw;
}

int StringParser::Parse(char s[])
{
	CleanUp();

	int n = strlen(s);
	str = new char[n + 1];
	strcpy(str, s);

	wordTop = new int[1 + n / 2];
	wordLength = new int[1 + n / 2];
	nw = Parse(wordTop, wordLength, 1 + n / 2, s);
	return nw;
}

int StringParser::Parse(int wordTop[], int wordLength[], int maxNumWord, char str[])
{
	int state = 0;  // 0:Blank   1:Visible Letter
	int nw = 0;
	for (int i = 0; 0 != str[i]; ++i)
	{
		switch (state)
		{
		case 0:
			if (' ' != str[i] && isprint(str[i]))
			{
				if (nw < maxNumWord)
				{
					wordTop[nw] = i;
					wordLength[nw] = 1;
				}
				else
				{
					goto LOOPOUT;
				}
				++nw;
				state = 1;
			}
			else
			{
				//Do nothing
			}
			break;

		case 1:
			if (' ' != str[i] && isprint(str[i]))
			{
				wordLength[nw - 1]++;
			}
			else
			{
				state = 0;
			}
			break;
		}
	}
LOOPOUT:

	return nw;
}

void StringParser::GetWord(char word[], int sizeofthisword, int atposition)
{
	word[0] = 0;
	if (0 <= atposition && atposition < nw)
	{
		for (int i = 0; i < wordLength[atposition] && i < sizeofthisword; ++i)
		{
			word[i] = str[wordTop[atposition] + i];
			word[i + 1] = 0;
		}
	}
}

///////////////////////////////////////////////////////////////////
//end of StringParser class
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//NeverForgetFileClose class - refer comments
///////////////////////////////////////////////////////////////////

class NeverForgetFileClose	//to ensure file is always closed at end of scope of relevant call
{
protected:
	FILE *fp;

public:
	NeverForgetFileClose();
	~NeverForgetFileClose();

	void Open(char fn[], char mode[]);	//Usable function!!!

	//Opens file referred to in the character string fn[], in the mode referred to in mode[] - eg "r" for read, "w" for write

	
	FILE *Fp(void);	//Usable function!!!

	//Used to refer to file pointer, eg - if (nullptr != File.Fp()), do <something>
	//where File is the name of an instance of this class


	void Close(void);
	//Closes file and destroys pointers, used in destructor		
};

NeverForgetFileClose::NeverForgetFileClose()
{
	fp = nullptr;
}

NeverForgetFileClose::~NeverForgetFileClose()
{
	Close();
}

void NeverForgetFileClose::Open(char fn[], char mode[])
{
	printf("\nOpening file...\n");
	fp = fopen(fn, mode);
}

void NeverForgetFileClose::Close(void)
{
	if (nullptr != fp)
	{
		printf("\nClosing file.\n");
		fclose(fp);
		fp = nullptr;
	}
}

FILE *NeverForgetFileClose::Fp(void)
{
	return fp;
}

///////////////////////////////////////////////////////////////////
//end of NeverForgetFileClose class
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//Line2D class - refer comments
///////////////////////////////////////////////////////////////////

class Line2D
{
protected:
	short int startX, startY, endX, endY;	//store coordinates of one line object, coordinates are updated by when Move is called
	short int initX1, initY1, initX2, initY2;	//store coordinates of one line object, do not change once assigned
	short int r, g, b;	//store color values for line object

public:
	Line2D();
	~Line2D();

	void Draw(void) const;	//Usable function!!!

	//Draws a line object using coordinates and color specified


	void MakeFromString(char lineread[]);
	//Takes a string with 7 words and uses it to populate coordinates and color specifications

	void MoveLine(int deltamouseX, int deltamouseY);
	//Calculates new coordinates based on movement of mouse

	void Reset(void);
	//Resets the values of coordinates when called
};

Line2D::Line2D()
{
	startX = 0;
	startY = 0;
	endX = 0;
	endY = 0;
	r = 0;
	g = 0;
	b = 0;
}

Line2D::~Line2D()
{

}

void Line2D::Draw(void) const
{
	DrawLine(startX, startY, endX, endY, r, g, b);
}

void Line2D::MoveLine(int deltamouseX, int deltamouseY)
{
	startX += deltamouseX;
	startY += deltamouseY;

	endX += deltamouseX;
	endY += deltamouseY;
}

void Line2D::Reset(void)
{
	startX = initX1;
	startY = initY1;
	endX = initX2;
	endY = initY2;
}

void Line2D::MakeFromString(char lineread[])
{
	StringParser checkline;
	checkline.Parse(lineread);
	if (checkline.NW() == 7)
	{
		char str[16];

		checkline.GetWord(str, 15, 0);
		startX = atoi(str);
		initX1 = startX;

		checkline.GetWord(str, 15, 1);
		startY = atoi(str);
		initY1 = startY;

		checkline.GetWord(str, 15, 2);
		endX = atoi(str);
		initX2 = endX;

		checkline.GetWord(str, 15, 3);
		endY = atoi(str);
		initY2 = endY;

		checkline.GetWord(str, 15, 4);
		r = atoi(str);

		checkline.GetWord(str, 15, 5);
		g = atoi(str);

		checkline.GetWord(str, 15, 6);
		b = atoi(str);
	}
	else
	{
		//Do nothing, the string does not have 7 words, so do not read it
	}
}

///////////////////////////////////////////////////////////////////
//end of Line2D class
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//Drawing2D class - refer comments
///////////////////////////////////////////////////////////////////

class Drawing2D
{
protected:
	int nLines;
	Line2D* Line;

public:
	Drawing2D();
	~Drawing2D();
	void CleanUp(void);
	Drawing2D(const Drawing2D &other);
	//copy constructor for this class

	Drawing2D &operator=(const Drawing2D &other);
	//copy operator for this class

	void ReadFile(NeverForgetFileClose File);
	//Reads a file for coordinates of each line object. Takes a NeverForgetFileClose object

	void ReadFile(char filename[]);
	//Reads a file for coordinates of each line object. Takes a string filename and converts it to a NeverForgetFileClose object
	
	void Draw(void) const;	//Usable function!!!

	//Draws all the line objects populated by ReadFile functions


	void Move(int deltamouseX, int deltamouseY);
	//Calculates new coordinates of each line object based on mouse movement

	void Reset(void) const;
	//Resets the values of coordinates of each line object when called
};

Drawing2D::Drawing2D()
{
	nLines = 0;
	Line = new Line2D;
}

Drawing2D::Drawing2D(const Drawing2D &other)
{
	Line = nullptr;

	CleanUp();

	Line = new Line2D;
	*Line = *other.Line;
}

Drawing2D &Drawing2D::operator=(const Drawing2D &other)
{
	Line = new Line2D;
	*Line = *other.Line;

	return *this;
}


Drawing2D::~Drawing2D()
{
	CleanUp();
}

void Drawing2D::CleanUp(void)
{
	delete[] Line;
	Line = nullptr;
	nLines = 0;	
}

void Drawing2D::ReadFile(NeverForgetFileClose File)
{
	if (File.Fp() != nullptr)
	{
		printf("File opened.\n");
		int nLinesRead = 0;
		char str[256];
		while (nullptr != fgets(str, 255, File.Fp()))
		{
			StringParser lineparse;
			lineparse.Parse(str);
			if (lineparse.NW() > 0)
			{
				if (lineparse.NW() == 7)
				{
					if (nLinesRead < nLines)
					{
						Line[nLinesRead].MakeFromString(str);
						++nLinesRead;
					}
				}

				else if (lineparse.NW() == 1)
				{
					char numlines[8];
					lineparse.GetWord(numlines, 7, 0);
					nLines = atoi(numlines);
					Line = new Line2D[nLines];
				}

				else
				{
					printf("\nInvalid file format!\n");
					File.Close();
					CleanUp();
				}
			}
			else
			{
				//Do nothing, line has non-positive number of words (like zero)
			}
		}
		printf("%d Line objects read of %d.\n\n", nLinesRead, nLines);
		File.Close();
	}
	else
	{
		//Do nothing. This if else is not really needed, since validity of filename is already checked in main function.
	}
}

void Drawing2D::ReadFile(char filename[])
{
	NeverForgetFileClose File;
	File.Open(filename, "r");

	if (File.Fp() != nullptr)
	{
		printf("File opened.\n");
		int nLinesRead = 0;
		char str[256];
		while (nullptr != fgets(str, 255, File.Fp()))
		{
			StringParser lineparse;
			lineparse.Parse(str);
			if (lineparse.NW() > 0)
			{
				if (lineparse.NW() == 7)
				{
					if (nLinesRead < nLines)
					{
						Line[nLinesRead].MakeFromString(str);
						++nLinesRead;
					}
				}

				else if (lineparse.NW() == 1)
				{
					char numlines[8];
					lineparse.GetWord(numlines, 7, 0);
					nLines = atoi(numlines);
					Line = new Line2D[nLines];
				}
				
				else
				{
					printf("\nInvalid file format!\n");
					File.Close();
					CleanUp();
				}
			}
			else
			{
				//Do nothing, line has non-positive number of words (like zero)
			}
		}
		printf("%d Line objects read out of %d.\n\n", nLinesRead, nLines);
		File.Close();
	}
	else
	{
		//Do nothing. This if else is not really needed, since validity of filename is already checked in main function.
	}
}

void Drawing2D::Draw(void) const
{
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < nLines; ++i)
	{
		Line[i].Draw();
	}
}

void Drawing2D::Move(int deltamouseX, int deltamouseY)
{
	for (int i = 0; i < nLines; ++i)
	{
		Line[i].MoveLine(deltamouseX, -deltamouseY);
	}
}

void Drawing2D::Reset(void) const
{
	for (int i = 0; i < nLines; ++i)
	{
		Line[i].Reset();
	}
}

///////////////////////////////////////////////////////////////////
//end of Drawing2D class
///////////////////////////////////////////////////////////////////



int main(void)
{
	printf("Shape drawing program.\nEnter filename to draw > ");
	char str[256], usableFilename[256];
	fgets(str, 255, stdin);
	StringParser filename;
	filename.Parse(str);
	if (filename.NW() != 1)	//Check if single word string has been entered, if not, then terminate
	{
		printf("\nIllegal entry! Only single word filename (without spaces) accepted.\nAborting...\n");
		return -1;
	}
	filename.GetWord(usableFilename, 256, 0);
	
	
	FILE *fp = fopen(usableFilename, "r");	//Open a graphics window only if file is found in the CWD
	if (fp != nullptr)
	{
		FsOpenWindow(16, 16, 800, 600, 1);
	}
	else
	{
		printf("\nFile not found.\nAborting...\n");
		return -1;
	}
	fclose(fp);	//Can close the file, because it will be opened again by Drawing2D class, this was just to check if graphics window needs to be opened
	
	Drawing2D Drawing;
	Drawing.ReadFile(usableFilename);	//Read from file and create line objects

	int terminate = 0;
	int lb, mb, rb, mx = 0, my = 0;
	FsGetMouseState(lb, mb, rb, mx, my);
	int oldmx = mx, oldmy = my;
	int deltamouseX = 0, deltamouseY = 0;
	
	//int lineX1 = 100, lineY1 = 100, lineX2 = 700, lineY2 = 500;
	//test code

	while (0 == terminate)
	{
		FsPollDevice();
		int key = FsInkey();

		switch (key)
		{
		case FSKEY_ESC:
			terminate = 1;
			break;

		case FSKEY_SPACE:
			Drawing.Reset();	//Move back to home position if SPACE is pressed
			break;
			
			//lineX1 = 100, lineY1 = 100, lineX2 = 700, lineY2 = 500;
			//test code
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		oldmx = mx;
		oldmy = my;

		FsGetMouseState(lb, mb, rb, mx, my);
		if (lb == 1)
		{
			deltamouseX = mx - oldmx;
			deltamouseY = my - oldmy;
			
			//lineX1 += deltamouseX;
			//lineY1 += deltamouseY;
			//lineX2 += deltamouseX;
			//lineY2 += deltamouseY;
			//test code
		}

		Drawing.Move(deltamouseX, deltamouseY);		//Move all the line objects
		Drawing.Draw();								//Draw all the line objects
		deltamouseX = 0, deltamouseY = 0;

		//glBegin(GL_LINES);
		//glColor3ub(0, 0, 0);
		//glVertex2i(lineX1, lineY1);
		//glVertex2i(lineX2, lineY2);
		//glEnd();
		//test code

		FsSwapBuffers();
		FsSleep(25);
	}
	return 0;
}



int XScr(double x)
{
	return int(x);
}

int YScr(double y)
{
	return int(-y + 600);
}

void DrawLine(double x1, double y1, double x2, double y2, int r, int g, int b)
{
	glColor3ub(r % 256, g % 256, b % 256);

	glBegin(GL_LINES);
	glVertex2i(XScr(x1), YScr(y1));
	glVertex2i(XScr(x2), YScr(y2));
	glEnd();
}