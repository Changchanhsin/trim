// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string.h>

void trim(char*);

int main(int argc, char* argv[], char* envp[])
{
	if (argc >= 2)
	{
		// 如果有参数，对每个参数作为文件名进行处理
		for (int i = 1; i < argc; i++) 
		{
			trim(argv[i]);
		}		
	}
	else
	{
		// 否则进入测试程序或输出提示信息
		//char s[] = "d:\\GB1K1718.HZ";
		//trim(s);
		std::cout << "Usage: trim filename1 [filename2] ..." << std::endl;
	}
}

void backup(char *f1)
{
	char* backfilename = (char*)malloc(500 * sizeof(char));

	memset(backfilename, 0, sizeof(backfilename));
	strcat_s(backfilename, strlen(f1) + 1, f1);
	char ext[] = ".bak";
	strcat_s(backfilename, strlen(backfilename) + strlen(ext) + 1, ext);
	while (rename(f1, backfilename))
	{
		strcat_s(backfilename, strlen(backfilename) + strlen(ext) + 1, ext);
	}
}

void trim(char* f1)
{
	int err;
	int tail;
	unsigned char mask;

	// 分析文件名，设置掩码
	// 文件名：GB1K1718.HZ或GB1K1718
	int maxlen = strnlen_s(f1, 500); // 文件名长
	int widestart = 0;  // 从那个位置取字型宽度信息
	int width; // 字型的宽度
	if (f1[maxlen - 3] == '.')
	{
		// 有扩展名
		if ((f1[maxlen - 2] == 'H' or f1[maxlen - 2] == 'h') and
			(f1[maxlen - 1] == 'Z' or f1[maxlen - 1] == 'z'))
		{
			widestart=7;
		}
		else
		{
			// 有扩展名，但不是HZ，文件名不符合要求，不继续处理.
			fprintf(stdout, "[ERR0001]: Illegal file name.\n");
			return;
		}
	}
	else
	{
		// 无扩展名
		widestart = 4;
	}
	if ((f1[maxlen - widestart] >= '0' and f1[maxlen - widestart] <= '9') and
		(f1[maxlen - widestart + 1] >= '0' and f1[maxlen - widestart + 1] <= '9'))
	{
		
		width = (f1[maxlen - widestart] - '0') * 10 + (f1[maxlen - widestart + 1] - '0');
		// 行尾字节实际有效位数
		mask = width % 8;
		if (mask == 0)
		{
			// 如果满字节，就直接返回，不作处理
			fprintf(stdout, "[ERR0002]: No need to process.\n");
			return;
		}
		// 从左边留1
		mask = 0xFF << (8-mask);
		// 每行字节数
		tail = (int)((width+7)/ 8);
	}
	else
	{
		// 该位置不是数字，文件名不符合要求，不继续处理。
		fprintf(stdout, "[ERR0001]: Illegal file name.\n");
		return;
	}

	// 读文件
	FILE *pFile;
	err = fopen_s(&pFile, f1, "rb");
	if (err != 0)
	{
		// 无法打开文件，不继续处理
		fprintf(stdout, "[ERR0003]: Unable to open file.\n");
		return;
	}
		
	char *pBuf;
	fseek(pFile, 0, SEEK_END);  //定位到文件末尾
	int len = ftell(pFile);  //求文件长度
	pBuf = new char[len + 1];  // 将整个文件读入内存
	rewind(pFile);  //重新定位指针到文件开始处
	fread(pBuf, 1, len, pFile);
	fclose(pFile);

	// 备份旧文件
	backup(f1);

	// 修改文件，将尾部位置零
	for (int i=0;i < len/tail;i++)
	{
		// 一次处理一行，将行尾字节用mask取and
		pBuf[(i+1)*tail-1] = pBuf[(i + 1)*tail - 1] & mask;
	}

	// 写入修改后的文件
	FILE *pFileOut;
	err = fopen_s(&pFileOut, f1, "wb"); 
	if (err != 0)
	{
		fprintf(stdout, "[ERR0003]: Unable to create file.\n");
		return;
	}
	fwrite(pBuf, 1, len, pFileOut);
	fclose(pFileOut);
	free(pBuf); //释放
}