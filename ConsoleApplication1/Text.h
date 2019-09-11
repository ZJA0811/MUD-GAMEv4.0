#ifndef TEXT_PARSER_
#define TEXT_PARSER_
#include<fstream>
using namespace std;
class TextParser {
private:
	ifstream ParserFile; 
	short nRecordNum;		//��¼�ж��ٸ����������Ʒ�����ܡ��籾��
	char cRecordHead;		//��¼�Ǻţ���Ϊ��¼������һ���ؼ���ʶ

public:
	TextParser(const char * stFileName, const char cHeadFlag);
	~TextParser();
	short GetRecordNum();//�������������Ʒ���籾�ȵ�����
	bool FindRecordHead(); //�ҵ���ǵĿ�ͷ
	void ReadString(char * stString); //��һ���ַ���
	void ReadInteger(short * nInt);	// ��һ������
	void GotoNextLine();	//����һ��
	bool GetWholeLine(char * stString);	//��һ����
	void GotoFileHead();		//ȥ���ļ���ͷ
};
#endif

