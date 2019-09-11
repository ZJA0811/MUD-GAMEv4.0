#ifndef TEXT_PARSER_
#define TEXT_PARSER_
#include<fstream>
using namespace std;
class TextParser {
private:
	ifstream ParserFile; 
	short nRecordNum;		//记录有多少个任务、人物、物品、技能、剧本等
	char cRecordHead;		//记录记号，作为记录总数的一个关键标识

public:
	TextParser(const char * stFileName, const char cHeadFlag);
	~TextParser();
	short GetRecordNum();//返回任务、人物、物品、剧本等的数量
	bool FindRecordHead(); //找到标记的开头
	void ReadString(char * stString); //读一个字符串
	void ReadInteger(short * nInt);	// 读一个整数
	void GotoNextLine();	//往下一行
	bool GetWholeLine(char * stString);	//读一整行
	void GotoFileHead();		//去到文件开头
};
#endif

