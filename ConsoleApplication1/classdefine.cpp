#include<iostream>
#include<assert.h>
#include"class.h"
#include"global.h"
#include"Text.h"
using namespace std;
bool Game::ReadWholeTextFile(char * FileName, char * Buffer) {
	ifstream TextFile(FileName, ios::binary);
	if (!TextFile) {
		cerr << "open whole text file failed !" << endl;
		return false;
	}
	int FileLength;
	TextFile.seekg(0, ios::end);//���Ƶ��ļ�ĩβ��Ȼ����tellg������ʾ��ǰλ�ã����ļ�����
	FileLength = static_cast<int>(TextFile.tellg());
	TextFile.seekg(0);
	try {
		TextFile.read(Buffer, FileLength);
	}
	catch (...) {//��׽�������͵��쳣
		cout << "read whole text file error !" << endl;
		return false;
	}
	Buffer[FileLength] = '\0';
	return true;
}
bool Game::LoadResFileName(const char* pstFileName, const char cHeadFlag) {
	TextParser ResNameFile(pstFileName, cHeadFlag);
	ResNameFile.FindRecordHead();
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.SkillFile);//��ȡ�����ļ���
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.GoodsFile);//��ȡ��Ʒ�ļ���
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.RoleFile);//��ȡ�����ļ���
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.SpotFile);//��ȡ�ص��ļ���
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.QuestFile);//��ȡ�����ļ���
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.ScriptFile);//��ȡ�ű��ļ���
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.MapFile);//��ȡ��ͼ�ļ���
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.LogoFile);//��ȡlogo�ļ���
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.NickFile);//��ȡ�ǳ��ļ���
	return true;
}
bool Game::LoadPlayerData(bool bNewGame) {
	//װ��npc�ͳ�������
	if (!LoadNpcsData(oResFiles.RoleFile, '#')) {
		SystemWarning("װ��NPC����ʧ�ܣ�");
		return false;
	}
	if (!LoadSpotsData(oResFiles.SpotFile, '#')) {
		SystemWarning("װ�س�����Ϣʧ�ܣ�");
		return false;
	}
	if (bNewGame) {//���������Ϸ����ʼ�ص�������
		oGameEnv.nCurrentMap = 0;
	}
	else {
		//�򿪴浵�ļ�
		ifstream SaveFile("save.dat");
		if (!SaveFile) {
			cerr << "open save file failed !" << endl;
			return false;
		}
		//��ȡ�������
		SaveFile.read(reinterpret_cast<char *>(&pNpcs[0]), sizeof(Npc));
		SaveFile.read(reinterpret_cast<char *>(&oGameEnv.nCurrentMap), sizeof(short));
		//��ȡ�����־
		for (short i = 0; i < QuestsNum; ++i)
			SaveFile.read(reinterpret_cast<char *>(&pQuests[i].Flag), sizeof(short));
		//��ȡ����
		for (int i = 0; i < VAR_NUM; ++i)
			SaveFile.read(reinterpret_cast<char *>(&Var[i]), sizeof(short));
		//��ȡ����������npc����Ʒ
		 for(short i= 0; i < SpotsNum; ++i )
		 {
		 SaveFile.read( reinterpret_cast<char *>(&pSpots[i].NpcNum), sizeof(short));
		 for(short j = 0; j < pSpots[i].NpcNum; ++j )
		 SaveFile.read( reinterpret_cast<char *>(&pSpots[i].Npcs[j]), sizeof(short) );
		 SaveFile.read( reinterpret_cast<char *>(&pSpots[i].GoodsNum), sizeof(short));
		 for(short j = 0; j < pSpots[i].GoodsNum; ++j )
		 SaveFile.read( reinterpret_cast<char *>(&pSpots[i].Goods[j]), sizeof(short) );
		 }
	}
	UpdateNick(0);
	return true;
}
bool Game::LoadSpotsData(const char * pstFileName, const char cHeadFlag) {
	short i;
	if (pSpots) {
		delete[] pSpots;
		pSpots = NULL;
	}
	TextParser SpotsFile(pstFileName, cHeadFlag);	//�ļ���
	SpotsNum = SpotsFile.GetRecordNum();			//��õص�����
	pSpots = new Spot[SpotsNum]; 

	for (short nSpotsIndex = 0; nSpotsIndex < SpotsNum; ++nSpotsIndex) {
		SpotsFile.FindRecordHead();
		SpotsFile.GotoNextLine();
		SpotsFile.ReadString(pSpots[nSpotsIndex].Name);//��ȡ������
		SpotsFile.GotoNextLine();
		SpotsFile.ReadString(pSpots[nSpotsIndex].Scene);//��ȡ��������
		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].ExitNum);//��ȡ��������
		SpotsFile.GotoNextLine();

		for (i = 0; i < pSpots[nSpotsIndex].ExitNum; ++i)//���ζ�ȡ���ڶ�Ӧ��λ��
			SpotsFile.ReadInteger(&pSpots[nSpotsIndex].Exits[i]);

		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].GoodsNum);//��ȡ��Ʒ����
		SpotsFile.GotoNextLine();

		for (i = 0; i < pSpots[nSpotsIndex].GoodsNum; ++i)//���ζ�ȡ��Ʒ��Ӧλ��
			SpotsFile.ReadInteger(&pSpots[nSpotsIndex].Goods[i]);

		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].NpcNum);//��ȡnpc����
		SpotsFile.GotoNextLine();

		for (i = 0; i < pSpots[nSpotsIndex].NpcNum; ++i)//���ζ�ȡnpc����Ӧ��λ��
			SpotsFile.ReadInteger(&pSpots[nSpotsIndex].Npcs[i]);

		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].EnterScript);//��ȡ����ص�ʱ��Ӧ�ű�λ��
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].LeaveScript);//��ȡ�뿪�ص�ʱ��Ӧ�ű�λ��
	}
	return true;
}
bool Game::LoadNpcsData(char * pstFileName, char cHeadFlag) {
	short i;
	if (pNpcs) {
		delete[] pNpcs;
		pNpcs = NULL;
	}
	TextParser NpcsFile(pstFileName, cHeadFlag);//�ļ���
	NpcsNum = NpcsFile.GetRecordNum();		//�õ�npc����	
	pNpcs = new Npc[NpcsNum];
	if (!pNpcs) {
		SystemWarning("Ϊ��ɫ�����ڴ�ʧ�ܣ���Ϸ���˳���");
		return false;
	}
	for (short NpcsIndex = 0; NpcsIndex < NpcsNum; ++NpcsIndex) {
		NpcsFile.FindRecordHead();
		NpcsFile.GotoNextLine();
		NpcsFile.ReadString(pNpcs[NpcsIndex].Name);//��������
		NpcsFile.GotoNextLine();
		NpcsFile.ReadString(pNpcs[NpcsIndex].Desc);//��������
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Experience[0]));	//���ڵ��ľ���
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Experience[1]));	//���ڽ��ľ���
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Experience[2]));	//����ǹ���ľ���
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Experience[3]));	//����ȭ�ľ���
		UpdateNick(NpcsIndex); //�����ǳ�
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].HP));	//��ȡ����ֵ
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MaxHP));	//��ȡ����ֵ����
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MP));	//��ȡ����ֵ
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MaxMP));	//��ȡ����ֵ����
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Attack));	//��ȡ������
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Defend));	//��ȡ������
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Speed));	//��ȡ�ٶ�
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].SkillNum));	//��ȡӵ�м�������
		NpcsFile.GotoNextLine();

		for (i = 0; i < pNpcs[NpcsIndex].SkillNum; i++) {//���ζ�ȡÿ�����ܶ�Ӧ��λ�á��ȼ����Լ���ǰ�ȼ�������ʹ�ô���
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MySkills[i].SkillIndex));
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MySkills[i].SkillLevel));
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MySkills[i].Times));//����ʹ��һ���������������
		}

		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].GoodsNum));	//��ȡ��Ʒ����
		NpcsFile.GotoNextLine();

		for (i = 0; i < pNpcs[NpcsIndex].GoodsNum; i++) {//���ζ�ȡ��Ʒ
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MyGoods[i]));
		}

		NpcsFile.GotoNextLine();
		for (i = 0; i < 4; ++i)//��ȡ�ĸ��ط���װ��
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MyEquipment[i]));

		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Money));//��ȡ����
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].TalkScript)); //��ȡ�����ｻ̸�Ľű����
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].FailScript));	//��ȡ����ʧ��ʱ�Ľű����
	}
	return true;
}//����
bool Game::LoadSkillsData(const char * pstFileName, const char cHeadFlag) {
	if (pSkills) {
		delete[] pSkills;
		pSkills = NULL;
	}
	TextParser SkillsFile(pstFileName, cHeadFlag);//�ļ���
	SkillsNum = SkillsFile.GetRecordNum();//ͨ��HeadFlag��#��ü�������
	pSkills = new Skill[SkillsNum];
	if (!pSkills) {
		SystemWarning("Ϊ���ܷ����ڴ�ʧ�ܣ���Ϸ���˳���");
		return false;
	}
	for (short Skilllndex = 0; Skilllndex < SkillsNum; ++Skilllndex) {
		SkillsFile.FindRecordHead();
		SkillsFile.GotoNextLine();
		SkillsFile.ReadString(pSkills[Skilllndex].Name);//������
		SkillsFile.GotoNextLine();
		SkillsFile.ReadString(pSkills[Skilllndex].Desc);//��ע��
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].Power));//��������
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].Level));//���ܵȼ�
		/*SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].NeedLev));*/
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].LevelUp));//ÿ���ȼ��ļ�����Ҫ��ϰ�Ĵ���
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].NeedExp));//��Ҫ���پ�������������ּ���
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].SkillKind)); //�������� 0����1����2ǹ����3ȭ
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].MpCost));//ʹ�ü������ĵ�ħ��ֵ
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddAttack));//�����˺�
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddDefence));//���ӷ���
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddHp));//��������ֵ
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddMaxHp));//��������ֵ����
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddMp));//����ħ��ֵ
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddMaxMp));//��������ֵ����
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddSpeed));//�����ٶ�
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddExp));//���Ӿ���
	}
	return true;
}
bool Game::LoadGoodsData(char * pstFileName, char cHeadFlag) {
	if (pGoods) {
		delete[] pGoods;
		pGoods = NULL;
	}
	TextParser GoodsFile(pstFileName, cHeadFlag);//�ļ���
	GoodsNum = GoodsFile.GetRecordNum();//�����Ϸ�ܵ���Ʒ��
	pGoods = new Goods[GoodsNum];
	if (!pGoods) {
		SystemWarning("Ϊ��Ʒ�����ڴ�ʧ�ܣ���Ϸ���˳���");
		return false;
	}
	for (short GoodsIndex = 0; GoodsIndex < GoodsNum; GoodsIndex++) {
		GoodsFile.FindRecordHead();
		GoodsFile.GotoNextLine();
		GoodsFile.ReadString(pGoods[GoodsIndex].Name);			//����
		GoodsFile.GotoNextLine();
		GoodsFile.ReadString(pGoods[GoodsIndex].Desc);		//����
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].Use);//0��ʾ�� 1��ʾװ�� 2��ʾѧϰ 3 ������Ʒ
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddAttack);	//���ӵĹ�����
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddDefence);//���ӵķ�����
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddSpeed);	//���ӵ��ٶ�ֵ
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddHP);			//��������ֵ
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddMaxHP);		//��������ֵ����
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddMP);			//��������ֵ
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddMaxMP);		//��������ֵ����
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].SkillIndex);//��ѧϰ�ļ��ܱ�ţ�û����Ϊ-1
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].Location);//�����װ����װ���ĵط�0-ͷ��1-��2-�֣�3-��,-1û��
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].Cost);		//ֵ����Ǯ
	}
	return true;
}
bool Game::LoadScripts(char * pstFileName, char cHeadFlag) {
	if (pScripts) {
		delete[] pScripts;
		pScripts = NULL;
	}
	TextParser ScriptsFile(pstFileName, cHeadFlag);	//�ļ���
	ScriptsNumber = ScriptsFile.GetRecordNum();//ͨ��#�ŵõ��м����ű�
	char Temp[512];
	short ScriptsIndex = -1;
	short ValidLineIndex = -1;
	short LineNumber = 0;
	short ValidLineNum = 0;
	while (ScriptsFile.GetWholeLine(Temp)) {	//�����м��о籾
		LineNumber++;
		if (Temp[0] != cHeadFlag)
			ValidLineNum++;//��Ч�ľ籾���������ǲ�����#�ŵ�
	}
	ScriptLineNumber = ValidLineNum;
	pScripts = new ScriptLine[ValidLineNum];//�籾������Ϊ��λ��
	if (!pScripts) {
		SystemWarning("Ϊ�籾�����ڴ�ʧ�ܣ���Ϸ���˳���");
		return false;
	}
	ScriptsFile.GotoFileHead();
	short i;
	for (i = 0; i < LineNumber; ++i) {
		ScriptsFile.GetWholeLine(Temp);
		if (Temp[0] == cHeadFlag) {
			ScriptsIndex++;
		}
		else {
			ValidLineIndex++;
			if (ValidLineIndex < ValidLineNum) {
				pScripts[ValidLineIndex].ScriptIndex = ScriptsIndex;//��ʾ�ڼ��о籾������һ���籾
				strcpy_s(pScripts[ValidLineIndex].ScriptLine, Temp);
			}
		}
	}
	return true;
}
void Game::LoadNickData(const char * pstFileName, const char cHeadFlag) {
	TextParser NickFile(pstFileName, cHeadFlag);
	NickFile.FindRecordHead();
	NickFile.GotoNextLine();
	short i;
	for (i = 0; i < SKILL_KIND; ++i)
		NickFile.ReadString(SkillNicks[i]);//���� ���� ǹ�� ȭ��     [����]

	NickFile.GotoNextLine();
	for (i = 0; i < SKILL_LVL; ++i)
		NickFile.ReadString(LvlNicks[i]);//���� ���� ���� ר�� ��ʦ[�ƺ�]

	NickFile.GotoNextLine();
	for (i = 0; i < SKILL_LVL; ++i)
		NickFile.ReadInteger(&NickExp[i]);//		0 100 300 500 1000[����]
}
bool Game::LoadQuestData(char * pstFileName, char cHeadFlag) {
	if (pQuests) {
		delete[] pQuests;
		pQuests = NULL;
	}
	TextParser QuestsFile(pstFileName, cHeadFlag);//�ļ���
	QuestsNum = QuestsFile.GetRecordNum();//��������
	pQuests = new Quest[QuestsNum];
	if (!pQuests) {
		SystemWarning("Ϊ��������ڴ�ʧ�ܣ���Ϸ���˳���");
		return false;
	}
	for (short QuestIndex = 0; QuestIndex < QuestsNum; ++QuestIndex) {
		QuestsFile.FindRecordHead();
		QuestsFile.GotoNextLine();
		QuestsFile.ReadString(pQuests[QuestIndex].Desc);//����
		QuestsFile.GotoNextLine();
		QuestsFile.ReadInteger(&pQuests[QuestIndex].RoleIndex);//������
		QuestsFile.GotoNextLine();
		QuestsFile.ReadInteger(&pQuests[QuestIndex].MapIndex);//��ͼ���
		pQuests[QuestIndex].Flag = 0;//0δ���� 1����δ��� 2�����
	}
	return true;
}
bool Game::SavePlayerData() {
	ofstream SaveFile("save.dat");
	if (!SaveFile) {
		cerr << "open save file failed !" << endl;
		return false;
	}
	SaveFile.write(reinterpret_cast<char *>(&pNpcs[0]), sizeof(Npc));//д��СϺ����Ϣ
	SaveFile.write(reinterpret_cast<char *>(&oGameEnv.nCurrentMap),sizeof(short));//��ǰ��ͼ��Ϣ
	//д�������־
	for (int i = 0; i < QuestsNum; ++i)
		SaveFile.write(reinterpret_cast<char *>(&pQuests[i].Flag),sizeof(short));
	//д�������ʶ
	for (int i = 0; i < VAR_NUM; ++i)
		SaveFile.write(reinterpret_cast<char *>(&Var[i]), sizeof(short));
	//д���������npc�� ��Ʒ�����
	 for( short i= 0; i < SpotsNum; ++i )
	 {
	 SaveFile.write( reinterpret_cast<char *>(&pSpots[i].NpcNum), sizeof(short) );
	 for(short j = 0; j < pSpots[i].NpcNum; ++j )
	 SaveFile.write( reinterpret_cast<char *>(&pSpots[i].Npcs[j]), sizeof(short) );
	 SaveFile.write( reinterpret_cast<char *>(&pSpots[i].GoodsNum), sizeof(short));
	 for(short j = 0; j < pSpots[i].GoodsNum; ++j )
	 SaveFile.write( reinterpret_cast<char *>(&pSpots[i].Goods[j]), sizeof(short) );
	 }
	return true;
}
short Game::NumOfGoods(short RoleIDx, short nGoodsIdx) {
	short nNumber = 0;
	for (short i = 0; i < pNpcs[RoleIDx].GoodsNum; ++i) {//����ĳ��������ĳ��Ʒ����
		if (pNpcs[RoleIDx].MyGoods[i] == nGoodsIdx)
			nNumber++;
	}
	return nNumber;
}
short Game::GetUserInput() {
	char str[1];
	str[0] = getchar();//��ȡһ���ַ�
	return atoi(str);//���ַ�ת��ΪInt
}
short Game::GetUserInput(short nMin, short nMax) {
	short n = GetUserInput();

	while (n < nMin || n > nMax)//��n�����ֵ����Сֵ֮�����
		n = GetUserInput();

	return n;
}
void Game::ShowNpcName(short nNpcIndex) {
#ifdef _WIN32_//������������Ϊ��ɫ����ӡ����
	SetConsoleTextAttribute(hConsoleHandle, NPC_NAME_COLOR | 0);
	if (nNpcIndex == 0)
		cout << pNpcs[nNpcIndex].Name << "[" << pNpcs[nNpcIndex].Nick << "] ";
	else
		cout << pNpcs[nNpcIndex].Name;
#else
	if (nNpcIndex == 0)
	{
		printf(NPC_NAME_STRING, pNpcs[nNpcIndex].Name);
		cout << "[" << pNpcs[nNpcIndex].Nick << "] ";
	}
	else
		printf(NPC_NAME_STRING, pNpcs[nNpcIndex].Name);
#endif
}
void Game::ShowNpcState(short nNpcIndex) {
	ShowNpcName(nNpcIndex);//��ʾ��������
	//cout << pNpcs[nNpcIndex].Nick << endl;
	//������ﵱǰ����������ֵ���Լ�����
	cout << "( ������" << pNpcs[nNpcIndex].HP << "/" << pNpcs[nNpcIndex].MaxHP
		<< "�� ������" << pNpcs[nNpcIndex].MP << "/" << pNpcs[nNpcIndex].MaxMP
		<< "�� " << pNpcs[nNpcIndex].Money << "�� )" << endl;
}
void Game::ShowNpcDetail(short nNpcIndex) {
	ShowNpcName(nNpcIndex);
#ifdef _WIN32_//����������ɫΪ��ɫ���������
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
	cout << pNpcs[nNpcIndex].Desc << endl;
#else
	printf(DESCRIPTION_STRING, pNpcs[nNpcIndex].Desc);
	cout << endl;
#endif
}
void Game::ShowNpcDetailX(short nNpcIndex) {
	ShowNpcDetail(nNpcIndex);
#ifdef _WIN32_//����Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, ITEM_NAME_COLOR | 0);
#endif
	cout << "����: " << pNpcs[nNpcIndex].Money << endl;
	cout << endl;

#ifdef _WIN32_//����Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
#endif
	for (short i = 0; i < SKILL_KIND; ++i) {
		if(i==0)
		cout << SkillNicks[i] << "������Ϊ ��" << pNpcs[nNpcIndex].Experience[i] << endl;
		if (i == 1)
			cout << SkillNicks[i] << "������Ϊ ��" << pNpcs[nNpcIndex].Experience[i] << endl;
		if (i == 2)
			cout << SkillNicks[i] << "ǹ����Ϊ ��" << pNpcs[nNpcIndex].Experience[i] << endl;
		if (i == 3)
			cout << SkillNicks[i] << "ȭ����Ϊ ��" << pNpcs[nNpcIndex].Experience[i] << endl;
	}
	cout << "����  ��" << pNpcs[nNpcIndex].HP << " / "
		<< pNpcs[nNpcIndex].MaxHP << endl;
	cout << "����  ��" << pNpcs[nNpcIndex].MP << " / "
		<< pNpcs[nNpcIndex].MaxMP << endl;
	cout << "����  ��" << pNpcs[nNpcIndex].Attack << " + "
		<< GetAddedAttack(nNpcIndex) << endl;
	cout << "����  ��" << pNpcs[nNpcIndex].Defend << " + "
		<< GetAddedDefend(nNpcIndex) << endl;
	cout << "�ٶ�  ��" << pNpcs[nNpcIndex].Speed << " + "
		<< GetAddedSpeed(nNpcIndex) << endl << endl;
}
void Game::ShowGoodsName(short GoodsIndex) {
#ifdef _WIN32_//����Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, ITEM_NAME_COLOR | 0);
	cout << pGoods[GoodsIndex].Name;
#else
	printf(ITEM_NAME_STRING, pGoods[GoodsIndex].Name);
#endif
}
void Game::ShowGoodsDetail(short GoodsIndex) {
	if (GoodsIndex >= GoodsNum) {
		SystemWarning("��Ʒ����������Χ����Ϸ���˳���");
		Running = false;
		return;
}
	ShowGoodsName(GoodsIndex);//�����Ʒ����
	cout << endl;
#ifdef _WIN32_//����Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
	cout << pGoods[GoodsIndex].Desc << endl;
#else
	printf(DESCRIPTION_STRING, pGoods[GoodsIndex].Desc);//�����Ʒ����
	cout << endl;
#endif

	//��Ʒ��Ч��
	switch (pGoods[GoodsIndex].Use) {
	case 0:
		cout << "���Է��ã�";
		break;
	case 1:
		cout << "����װ����";
		break;
	case 2:
		cout << "������ϰ��";
		break;
	default:
		cout << "������Ʒ!";
	}

	if (pGoods[GoodsIndex].AddHP)
		cout << "���� + " << pGoods[GoodsIndex].AddHP << "��\t";
	if (pGoods[GoodsIndex].AddMaxHP)
		cout << "�������� + " << pGoods[GoodsIndex].AddMaxHP << "��\t";
	if (pGoods[GoodsIndex].AddMP)
		cout << "���� + " << pGoods[GoodsIndex].AddMP << "��\t";
	if (pGoods[GoodsIndex].AddMaxMP)
		cout << "�������� + " << pGoods[GoodsIndex].AddMaxMP << "��\t";
	if (pGoods[GoodsIndex].AddAttack)
		cout << "���� + " << pGoods[GoodsIndex].AddAttack << "��\t";
	if (pGoods[GoodsIndex].AddDefence)
		cout << "���� + " << pGoods[GoodsIndex].AddDefence << "��\t";
	if (pGoods[GoodsIndex].AddSpeed)
		cout << "�ٶ� + " << pGoods[GoodsIndex].AddSpeed << "��\t";
	if (pGoods[GoodsIndex].SkillIndex != -1) {
		cout << "�������书��";
		ShowSkillName(pGoods[GoodsIndex].SkillIndex);
		cout << "��";
	}

	cout << endl;
}
void Game::ShowSpotName(short nSpotIndex) {
#ifdef _WIN32_//����Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, SPOT_NAME_COLOR | 0);
	cout << pSpots[nSpotIndex].stName;
#else
	printf(SPOT_NAME_STRING, pSpots[nSpotIndex].Name);
#endif
}
void Game::ShowSpotDetail(short nSpotIndex) {
	short i;

	ShowSpotName(nSpotIndex);
	cout << endl;
#ifdef _WIN32_//����Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
	cout << pSpots[nSpotIndex].stScene << endl;
#else
	printf(DESCRIPTION_STRING, pSpots[nSpotIndex].Scene);//��ӡ��������
	cout << endl;
#endif

	//��ʾ�õص������
	if (pSpots[nSpotIndex].NpcNum) {
		cout << "����������У�";
		for (i = 0; i < pSpots[nSpotIndex].NpcNum; ++i) {
			ShowNpcName(pSpots[nSpotIndex].Npcs[i]);
			cout << "\t";
		}
		cout << endl;
	}

	// ��ʾ�õص����Ʒ
	if (pSpots[nSpotIndex].GoodsNum) {
		cout << "������Щ���Լ�ȡ�Ķ�����";
		for (i = 0; i < pSpots[nSpotIndex].GoodsNum; ++i) {
			ShowGoodsName(pSpots[nSpotIndex].Goods[i]);
			cout << "\t";
		}
		cout << endl;
	}

	//��ʾ�õص�ĳ���
	if (pSpots[nSpotIndex].ExitNum) {
		cout << "�������ͨ����";
		for (i = 0; i < pSpots[nSpotIndex].ExitNum; ++i) {
			ShowSpotName(pSpots[nSpotIndex].Exits[i]);
			cout << " ";
		}
		cout << endl;
	}
	//RunScripts( pSpots[nSpotIndex].nEnterScript );
}
void Game::ShowSkillName(short Skilllndex) {
#ifdef _WIN32_//����Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, SKILL_NAME_COLOR | 0);
	cout << pSkills[Skilllndex].Name;
#else
	printf(SKILL_NAME_STRING, pSkills[Skilllndex].Name);
#endif
}
void Game::ShowSkillDetail(short Skilllndex) {
	ShowSkillName(Skilllndex);
	cout << endl;
#ifdef _WIN32_
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
	cout << pSkills[Skilllndex].Desc << endl;
#else
	printf(DESCRIPTION_STRING, pSkills[Skilllndex].Desc);//��������
	cout << endl;
#endif
	cout << "ÿ��һ����������" << pSkills[Skilllndex].Power << "��" << endl;
}
void Game::ShowQuests(bool bCompleted) {
	short i, n = 0;

	if (bCompleted) {
		cout << "����ɵ�����" << endl;

		for (i = 0; i < QuestsNum; ++i) {
			if (pQuests[i].Flag == 2) {//��ʾ�����
				n++;
#ifdef _WIN32_//����Ϊ��ɫ
				SetConsoleTextAttribute(hConsoleHandle, QUEST_COLOR | 0);
				cout << "[" << n << "] ";
				cout << pQuests[i].Desc << endl;
#else
				cout << "[" << n << "]";
				printf(QUEST_STRING, pQuests[i].Desc);//��������
				cout << endl;
#endif
			}
		}
	}

	else {
		cout << "δ��ɵ�����" << endl;

		for (i = 0; i < QuestsNum; ++i) {
			if (pQuests[i].Flag == 1) {//1��ʾδ���
				n++;
#ifdef _WIN32_
				SetConsoleTextAttribute(hConsoleHandle, QUEST_COLOR | 0);
				cout << "[" << n << "] ";
				cout << pQuests[i].Desc << endl;
#else
				cout << "[" << n << "] ";
				printf(QUEST_STRING, pQuests[i].Desc);
				cout << endl;
#endif
			}
		}
	}

	if (!n)//���û�������ӡ��
		cout << "�ޡ�" << endl;

	SystemPause();//ϵͳ��ͣ ���û�����
}
void Game::ShowEquipment(short nRoleIndex, short nLocation) {
	if (pNpcs[nRoleIndex].MyEquipment[nLocation] != -1)//��ʾ����ĳһλ�õ�װ��
		ShowGoodsName(pNpcs[nRoleIndex].MyEquipment[nLocation]);
	else
		cout << "��";
}
void Game::ReadSubString(char * stString, char * stSubString) {
	short nIndex = 0;//��һ�����ַ�����һ��С���ַ���
	while (stString[IndexInCurLine] == ' ' || stString[IndexInCurLine] == '\t') {
		IndexInCurLine++;
	}
	while (IndexInCurLine < (short)strlen(stString)) {
		if (stString[IndexInCurLine] == ' ' || stString[IndexInCurLine] == '\t' || stString[IndexInCurLine] == '\r')
			break;
		else {
			stSubString[nIndex] = stString[IndexInCurLine];
			nIndex++;
		}
		IndexInCurLine++;
	}
	stSubString[nIndex] = '\0';
}
bool Game::HaveSkill(short nRoleIndex, short Skilllndex) {
	short i;
	for (i = 0; i < pNpcs[nRoleIndex].SkillNum; ++i) {
		if (pNpcs[nRoleIndex].MySkills[i].SkillIndex == Skilllndex)//�ж������Ƿ���ĳ�ּ���
			return true;
	}

	return false;
}
bool Game::AddSkill(short nRoleIndex, short Skilllndex) {
	if (Skilllndex < 0 || Skilllndex >= SkillsNum)//����λ�ñ����ڹ涨��Χ��
		return false;

	if (pNpcs[nRoleIndex].SkillNum == MAX_SKILL_NUM) {
		ShowNpcName(nRoleIndex);
		SystemWarning("�������ﵽ���ޣ�����ѧϰ�µļ��ܣ�");
		cout << endl;
		return false;
	}
	if (HaveSkill(nRoleIndex, Skilllndex)) {
		ShowNpcName(nRoleIndex);
		SystemWarning("�������ּ��ܣ�");
		cout << endl;
		return false;
	}
	short nSkillKind = pSkills[Skilllndex].SkillKind;
	if (pNpcs[nRoleIndex].Experience[nSkillKind] < pSkills[Skilllndex].NeedExp) {//�ж����ﾭ���Ƿ�ﵽ��׼
		ShowNpcName(nRoleIndex);
		SystemWarning("����Ϊ���������������书��");
		cout << endl;
		return false;
	}
	//��������������� ��������Ӹ����
	pNpcs[nRoleIndex].MySkills[pNpcs[nRoleIndex].SkillNum].SkillIndex = Skilllndex;
	pNpcs[nRoleIndex].MySkills[pNpcs[nRoleIndex].SkillNum].SkillLevel = 1;
	pNpcs[nRoleIndex].MySkills[pNpcs[nRoleIndex].SkillNum].Times = 0;
	pNpcs[nRoleIndex].SkillNum++;
	ShowNpcName(nRoleIndex);
	cout << "ϰ��";
	ShowSkillName(Skilllndex);
	cout << "��" << endl;
	cout << "������������" << pSkills[Skilllndex].AddMaxHp<<endl<< "������������" << pSkills[Skilllndex].AddMaxMp << endl;
	pNpcs[nRoleIndex].MaxHP += pSkills[Skilllndex].AddMaxHp;
	pNpcs[nRoleIndex].MaxMP += pSkills[Skilllndex].AddMaxMp;
	SystemPause();
	return true;
}
void Game::AddGoodsToSpot(short nSpotIndex, short GoodsIndex) {
	if (pSpots[nSpotIndex].GoodsNum == MAX_GOODS_NUM) {//����õص���Ʒ�ﵽ���ޡ���ȥ���������Ʒ���������Ʒ���õص�
		short i;
		for (i = 0; i < pSpots[nSpotIndex].GoodsNum - 1; ++i)
			pSpots[nSpotIndex].Goods[i] = pSpots[nSpotIndex].Goods[i + 1];
		pSpots[nSpotIndex].Goods[i] = GoodsIndex;
	}
	else {//�����������Ʒ���õص�
		pSpots[nSpotIndex].Goods[pSpots[nSpotIndex].GoodsNum] = GoodsIndex;
		pSpots[nSpotIndex].GoodsNum++;
	}
}
bool Game::AddGoods(short nRoleIndex, short GoodsIndex, bool bVisible) {
	assert(GoodsIndex >= 0 && GoodsIndex <= GoodsNum);//�жϸ���Ʒ�Ƿ��ڷ�Χ��

	if (bVisible) {//���Ҫ��ʾ
		ShowNpcName(nRoleIndex);
		cout << "�õ�";
		ShowGoodsName(GoodsIndex);
		cout << "��" << endl;
		SystemPause();
	}

	if (pNpcs[nRoleIndex].GoodsNum == MAX_GOODS_NUM) {//��Ʒ�����������Ʒ����
		ShowNpcName(nRoleIndex);
		cout << "������������";
		ShowGoodsName(GoodsIndex);
		cout << "�����˵��ϡ�" << endl;
		SystemPause();

		AddGoodsToSpot(oGameEnv.nCurrentMap, GoodsIndex);//����Ʒ��ӵ��õص�
		return false;
	}
	else {
		pNpcs[nRoleIndex].GoodsNum++;
		pNpcs[nRoleIndex].MyGoods[pNpcs[nRoleIndex].GoodsNum - 1] = GoodsIndex;
		return true;
	}
}
short Game::HaveGoods(short nRoleIndex, short GoodsIndex) {
	short i;
	for (i = 0; i < pNpcs[nRoleIndex].GoodsNum; ++i) {
		if (pNpcs[nRoleIndex].MyGoods[i] == GoodsIndex)
			break;
	}

	if (i == pNpcs[nRoleIndex].GoodsNum)
		return -1;
	else
		return i;//������Ʒ��������λ��
}
void Game::RemoveGoodsFromSpot(short nSpotIndex, short nGoodsLocation) {
	short i;
	//����Ʒ�Ӹõص��Ƴ�
	for (i = nGoodsLocation; i < pSpots[nSpotIndex].GoodsNum - 1; ++i)
		pSpots[nSpotIndex].Goods[i] = pSpots[nSpotIndex].Goods[i + 1];
	pSpots[nSpotIndex].GoodsNum--;
}
void Game::PickGoods(short nRoleIndex, short nGoodsLocation) {
	ShowNpcName(nRoleIndex);
	if (pNpcs[nRoleIndex].GoodsNum == MAX_GOODS_NUM)//��������
		cout << "�������������Ų����κζ����ˡ�" << endl;
	else {//�������
		cout << "����";
		ShowGoodsName(pSpots[oGameEnv.nCurrentMap].Goods[nGoodsLocation]);
		cout << "��" << endl;
		AddGoods(nRoleIndex, pSpots[oGameEnv.nCurrentMap].Goods[nGoodsLocation], false);
		RemoveGoodsFromSpot(oGameEnv.nCurrentMap, nGoodsLocation);
	}

	SystemPause();
}
bool Game::RemoveGoods(short nRoleIndex, short nGoodsLocation, bool bVisible) {
	if (nGoodsLocation < 0 || nGoodsLocation >= pNpcs[nRoleIndex].GoodsNum)
		return false;
	if (bVisible) {//�Ƿ���ʾ
		ShowNpcName(nRoleIndex);
		cout << "ʧȥ";
		ShowGoodsName(pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
		cout << "��" << endl;
		SystemPause();
	}
	short i;
	for (i = nGoodsLocation; i < pNpcs[nRoleIndex].GoodsNum - 1; ++i)
		pNpcs[nRoleIndex].MyGoods[i] = pNpcs[nRoleIndex].MyGoods[i + 1];
	pNpcs[nRoleIndex].GoodsNum--;
	return true;
}
void Game::SellGoods(short nRoleIndex, short nGoodsLocation) {
	ShowNpcName(nRoleIndex);
	cout << "������";
	ShowGoodsName(pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
	cout << "��" << endl;
	SystemPause();
	pNpcs[nRoleIndex].Money += pGoods[pNpcs[nRoleIndex].MyGoods[nGoodsLocation]].Cost / 2;//������ֻ���õ�һ������
	RemoveGoods(nRoleIndex, nGoodsLocation, false);
}
void Game::BuyGoods(short nRoleIndex, short GoodsIndex) {
	if (pNpcs[nRoleIndex].Money > pGoods[GoodsIndex].Cost) {//���Ǯ��
		ShowNpcName(nRoleIndex);
		cout << "������";
		ShowGoodsName(GoodsIndex);
		cout << "��" << endl;
		AddGoods(nRoleIndex, GoodsIndex, false);
		pNpcs[nRoleIndex].Money -= pGoods[GoodsIndex].Cost;
	}
	else {//����
		ShowNpcName(oGameEnv.nCurrentNpc);
		cout << ": ���Ǯ������" << endl;
	}

	SystemPause();
}
void Game::UseGoods(short nRoleIndex, short nGoodsLocation) {
	switch (pGoods[pNpcs[nRoleIndex].MyGoods[nGoodsLocation]].Use) {
	case 0:	//���û�ʹ��
		EatOrUse(nRoleIndex, pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
		RemoveGoods(nRoleIndex, nGoodsLocation, false);
		break;
	case 1: //װ��
		Equip(nRoleIndex, nGoodsLocation);
		break;
	case 2:  //ѧϰ
		LearnSkill(nRoleIndex, pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
		break;
	default:	//������Ʒ
		SystemWarning("����������Ʒ��");
		cout << endl;
	}
}
void Game::EatOrUse(short nRoleIndex, short GoodsIndex) {
	ShowNpcName(nRoleIndex);//�����������
	cout << "����";
	ShowGoodsName(GoodsIndex);//�����Ʒ����
	cout << "��" << endl;
	SystemPause();
	//��ʾ��Ʒ��������
	if (pGoods[GoodsIndex].AddMaxHP) {
		cout << "�������ޣ�" << pNpcs[nRoleIndex].MaxHP << "\t---->\t";
		pNpcs[nRoleIndex].MaxHP += pGoods[GoodsIndex].AddMaxHP;
		cout << pNpcs[nRoleIndex].MaxHP << endl;
	}
	if (pGoods[GoodsIndex].AddHP) {
		cout << "��ǰ������" << pNpcs[nRoleIndex].HP << "\t---->\t";
		pNpcs[nRoleIndex].HP += pGoods[GoodsIndex].AddHP;
		if (pNpcs[nRoleIndex].HP > pNpcs[nRoleIndex].MaxHP)
			pNpcs[nRoleIndex].HP = pNpcs[nRoleIndex].MaxHP;
		cout << pNpcs[nRoleIndex].HP << endl;
	}

	if (pGoods[GoodsIndex].AddMaxMP) {
		cout << "�������ޣ�" << pNpcs[nRoleIndex].MaxMP << "\t---->\t";
		pNpcs[nRoleIndex].MaxMP += pGoods[GoodsIndex].AddMaxMP;
		cout << pNpcs[nRoleIndex].MaxMP << endl;
	}

	if (pGoods[GoodsIndex].AddMP) {
		cout << "��ǰ������" << pNpcs[nRoleIndex].MP << "\t---->\t";
		pNpcs[nRoleIndex].MP += pGoods[GoodsIndex].AddMP;
		if (pNpcs[nRoleIndex].MP > pNpcs[nRoleIndex].MaxMP)
			pNpcs[nRoleIndex].MP = pNpcs[nRoleIndex].MaxMP;
		cout << pNpcs[nRoleIndex].MP << endl;
	}

	SystemPause();
}
void Game::UnEquip(short nRoleIndex, short nLocation) {
	short nGoosIndex = pNpcs[nRoleIndex].MyEquipment[nLocation];
	if (nGoosIndex == -1)
		return;

	ShowNpcName(nRoleIndex);
	cout << "ж����";
	ShowGoodsName(nGoosIndex);
	cout << "��" << endl;
	SystemPause();

	AddGoods(nRoleIndex, nGoosIndex, false);//ж����װ������ӵ���Ʒ��
	pNpcs[nRoleIndex].MyEquipment[nLocation] = -1;
}
void Game::Equip(short nRoleIndex, short nGoodsLocation) {
	short oldAttack, oldDefend, oldSpeed;
	oldAttack = GetAddedAttack(nRoleIndex);
	oldDefend = GetAddedDefend(nRoleIndex);
	oldSpeed = GetAddedSpeed(nRoleIndex);

	short nIndex = pNpcs[nRoleIndex].MyGoods[nGoodsLocation];
	UnEquip(nRoleIndex, pGoods[nIndex].Location);//��ж��ԭ��װ��

	ShowNpcName(nRoleIndex);
	cout << "װ����";
	ShowGoodsName(pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
	cout << "��" << endl;
	SystemPause();

	pNpcs[nRoleIndex].MyEquipment[pGoods[nIndex].Location] = nIndex;//װ��
	RemoveGoods(nRoleIndex, nGoodsLocation, false);//����Ʒ��ȥ����װ��
	//��ʾ��ǰ����������
	cout << "������" << pNpcs[nRoleIndex].Attack << " + " << oldAttack
		<< "\t---->\t" << pNpcs[nRoleIndex].Attack << " + "
		<< GetAddedAttack(nRoleIndex) << "\t("
		<< GetAddedAttack(nRoleIndex) - oldAttack << ")" << endl;

	cout << "���壺" << pNpcs[nRoleIndex].Defend << " + " << oldDefend
		<< "\t---->\t" << pNpcs[nRoleIndex].Defend << " + "
		<< GetAddedDefend(nRoleIndex) << "\t("
		<< GetAddedDefend(nRoleIndex) - oldDefend << ")" << endl;

	cout << "���ݣ�" << pNpcs[nRoleIndex].Speed << " + " << oldSpeed
		<< "\t---->\t" << pNpcs[nRoleIndex].Speed << " + "
		<< GetAddedSpeed(nRoleIndex) << "\t("
		<< GetAddedSpeed(nRoleIndex) - oldSpeed << ")" << endl;

	SystemPause();
}
void Game::LearnSkill(short nRoleIndex, short GoodsIndex) {
	AddSkill(nRoleIndex, pGoods[GoodsIndex].SkillIndex);//���Ӽ���
}
void Game::RandomDrop(short nRoleIdx, short nGoodsIdx, short nProb) {
	short nRan = rand() % 100;//���������Ʒ
	if (nRan <= nProb) {
		AddGoodsToSpot(oGameEnv.nCurrentMap, nGoodsIdx);
		ShowNpcName(nRoleIdx);
		cout << "������";
		ShowGoodsName(nGoodsIdx);
		cout << endl;
		SystemPause();
	}
}
void Game::AddMoney(short nRoleIdx, short nMoney) {
	pNpcs[nRoleIdx].Money += nMoney;
	ShowNpcName(nRoleIdx);
	cout << "�õ���Ǯ" << nMoney << "�ġ�" << endl;
	SystemPause();
}
bool Game::DelMoney(short nRoleIdx, short nMoney) {
	if (pNpcs[nRoleIdx].Money >= nMoney) {
		pNpcs[nRoleIdx].Money -= nMoney;
		ShowNpcName(nRoleIdx);
		cout << "ʧȥ��Ǯ" << nMoney << "�ġ�" << endl;
		SystemPause();
		return true;
	}
	else {
		return false;
	}
}
bool Game::AddNpc(short nSpotIndex, short nNpcIndex) {
	assert(nSpotIndex >= 0 && nSpotIndex < SpotsNum && nNpcIndex >= 0&& nNpcIndex < NpcsNum);

	if (pSpots[nSpotIndex].NpcNum == MAX_NPC_NUM) {//�õص����������ﵽ����
		cout << "�õ�ͼ��npc���������npc����" << endl;
		return false;
	}

	pSpots[nSpotIndex].Npcs[pSpots[nSpotIndex].NpcNum] = nNpcIndex;
	pSpots[nSpotIndex].NpcNum += 1;
	return true;
}
bool Game::RemoveNpc(short nSpotIndex, short nNpcIndex) {
	assert(nSpotIndex >= 0 && nSpotIndex < SpotsNum && nNpcIndex >= 0 && nNpcIndex < NpcsNum);

	if (pSpots[nSpotIndex].NpcNum == 0)
		return false;

	//�ҵ����npc�Ǹõ�ͼ�еĵڼ���
	short nNpcLocation;
	for (nNpcLocation = 0; nNpcLocation < pSpots[nSpotIndex].NpcNum; ++nNpcLocation) {
		if (pSpots[nSpotIndex].Npcs[nNpcLocation] == nNpcIndex)
			break;
	}

	if (nNpcLocation == pSpots[nSpotIndex].NpcNum)//û���ҵ�������
		return false;

	if (nNpcLocation < pSpots[nSpotIndex].NpcNum - 1) {//����ȥ��������
		short i;
		for (i = nNpcLocation; i < pSpots[nSpotIndex].NpcNum - 1; ++i) {
			pSpots[nSpotIndex].Npcs[i] = pSpots[nSpotIndex].Npcs[i + 1];
		}
	}
	pSpots[nSpotIndex].NpcNum--;

	return true;
}
short Game::GetAddedSpeed(short nRoleIndex) {
	short nAdd = 0;
	short i;
	for (i = 0; i < 4; ++i) {//����װ�����ӵ��ٶ�
		if (pNpcs[nRoleIndex].MyEquipment[i] != -1)
			nAdd += pGoods[pNpcs[nRoleIndex].MyEquipment[i]].AddSpeed;
	}
	return nAdd;
}
short Game::GetAddedAttack(short nRoleIndex) {
	short Add = 0;
	short i;
	for (i = 0; i < 4; ++i) {//����װ�����ӵĹ�����
		if (pNpcs[nRoleIndex].MyEquipment[i] != -1)
			Add += pGoods[pNpcs[nRoleIndex].MyEquipment[i]].AddAttack;
	}

	return Add;
}
short Game::GetAddedDefend(short nRoleIndex) {
	short Add = 0;
	short i;
	for (i = 0; i < 4; ++i) {//����װ�����ӵķ�����
		if (pNpcs[nRoleIndex].MyEquipment[i] != -1)
			Add += pGoods[pNpcs[nRoleIndex].MyEquipment[i]].AddDefence;
	}

	return Add;
}
short Game::SkillDamage(short nRoleIndex, short Skilllndex) {
	return (pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].Power
		* pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel);//���㼼���˺�
}
short Game::SkillMpCost(short nRoleIndex, short Skilllndex) {
	return pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].MpCost
		* pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel;//���㼼����������ֵ
}
void Game::InitFight(short nNpcIndex) {
	oGameEnv.nCurrentEnemy = nNpcIndex;
	oGameEnv.nCurrentNpc = nNpcIndex;

	ActSlot[0].RoleID = 0;//СϺ��
	ActSlot[0].ActSpeed = pNpcs[0].Speed + GetAddedSpeed(0);//�����ٶ�
	ActSlot[1].RoleID = oGameEnv.nCurrentEnemy;
	ActSlot[1].ActSpeed = pNpcs[oGameEnv.nCurrentEnemy].Speed + GetAddedSpeed(oGameEnv.nCurrentEnemy);

	oGameEnv.bFighting = true;
	oGameEnv.State = fighting;//�л���ս��״̬
}
void Game::EndFight(short nNpcIndex) {
	//��ʾ��Ϣ
	if (nNpcIndex == 0){//�����Ӯ��
		cout << "��սʤ��";
		ShowNpcName(oGameEnv.nCurrentEnemy);
		cout << "��" << endl;
		SystemPause();
		RunScripts(pNpcs[oGameEnv.nCurrentEnemy].FailScript);	//���е���ʧ�ܵĽű�
		Refresh(oGameEnv.nCurrentEnemy);//�ָ�����
		oGameEnv.bFighting = false;
	}
	else{
		cout << "�������";
		ShowNpcName(oGameEnv.nCurrentEnemy);
		cout << "��" << endl;
		SystemPause();
		oGameEnv.bFighting = false;
	}
	if (nNpcIndex == 0) {//���СϺ��Ӯ�ˣ��ɻ�þ���
		for (short i = 0; i < 4; i++) {
			short experience = GetExperience(nNpcIndex, oGameEnv.nCurrentEnemy);
			ShowNpcName(nNpcIndex);
			if(i==0)
			cout << "�����õ�" << experience << "�㾭�顣" << endl;
			if (i == 1)
				cout << "�����õ�" << experience << "�㾭�顣" << endl;
			if (i == 2)
				cout << "ǹ���õ�" << experience << "�㾭�顣" << endl;
			if (i == 3)
				cout << "ȭ���õ�" << experience << "�㾭�顣" << endl;
			pNpcs[nNpcIndex].Experience[i] += experience;
		}
		SystemPause();
		if (oGameEnv.bFighting)
			oGameEnv.State = fighting;
		else
			oGameEnv.State = continue_game;
	}
	else {//���СϺ�����ˣ���ʾ�ű������˳�
		RunScripts(pNpcs[0].FailScript);
		oGameEnv.State = system_menu;
	}
}
void Game::EscapeOut(short nRoleIndex, short nEnemyIndex) {
	short r = rand() % 100;
	if (pNpcs[nRoleIndex].Speed + GetAddedSpeed(nRoleIndex)
		< pNpcs[nEnemyIndex].Speed + GetAddedSpeed(nEnemyIndex)) {//�ж�˭����
		if (r < 30) {
			ShowNpcName(nRoleIndex);
			cout << "������ս����" << endl;
			oGameEnv.State = continue_game;
			oGameEnv.bFighting = false;
		}
		else {
			ShowNpcName(nRoleIndex);
			cout << "����ʧ�ܣ�" << endl;
			oGameEnv.State = fighting;
		}
	}
	else {
		if (r < 90) {
			ShowNpcName(nRoleIndex);
			cout << "������ս����" << endl;
			oGameEnv.State = continue_game;
			oGameEnv.bFighting = false;
		}
		else {
			ShowNpcName(nRoleIndex);
			cout << "����ʧ�ܣ�" << endl;
			oGameEnv.State = fighting;
		}
	}
	SystemPause();
}
short Game::DecideWhoAct() {
	if (ActSlot[0].ActSpeed >= ActSlot[1].ActSpeed) {//�ж�˭���췵���ж���������
		ActSlot[1].ActSpeed += (pNpcs[oGameEnv.nCurrentEnemy].Speed
			+ GetAddedSpeed(oGameEnv.nCurrentEnemy));
		return ActSlot[0].RoleID;
	}
	else {
		ActSlot[0].ActSpeed += pNpcs[0].Speed + GetAddedSpeed(0);
		return ActSlot[1].RoleID;
	}
}
short Game::UseWhichSkill(short nRoleIndex) {
	short SelectedSkill = -1;
	short nSkillPower = 0;
	short i;
	//�ж�ʹ����һ�ּ���
	for (i = 0; i < pNpcs[nRoleIndex].SkillNum; ++i) {
		short nPower = SkillDamage(nRoleIndex, i);
		if (nSkillPower < nPower && pNpcs[nRoleIndex].MP >= SkillMpCost(nRoleIndex, i)) {
			nSkillPower = nPower;
			SelectedSkill = i;
		}
	}
	return SelectedSkill;
}
short Game::Damage(short nRoleIndex, short nEnemyIndex) {
	short attack = pNpcs[nRoleIndex].Attack + GetAddedAttack(nRoleIndex);
	short defend = pNpcs[nEnemyIndex].Defend + GetAddedDefend(nEnemyIndex);
	if (attack > defend)//��������˺�ֵ
		//return attack;
		return (attack - defend + rand() % 60);
	else
		return rand() % 60+20;
}
short Game::GetExperience(short nRoleIndex, short nEnemyIndex) {
	short experience;
	experience = rand() % 30+10;
	return experience;//���������������
}
void Game::UpdateNick(short nRoleIdx) {
	/*	#       [�ƺ�]
		���� ���� ǹ�� ȭ��[����]
		���� ���� ���� ר�� ��ʦ[�ƺ�]
		0 100 300 500 1000[����]*/
	short MaxIdx = -1;
	short MaxExp = -1;
	for (short i = 0; i < SKILL_KIND; ++i) {
		if (pNpcs[nRoleIdx].Experience[i] > MaxExp) {
			MaxExp = pNpcs[nRoleIdx].Experience[i];
			MaxIdx = i;
		}
	}
	strcpy_s(pNpcs[nRoleIdx].Nick, SkillNicks[MaxIdx]);//��skillNicks����׷�ӵ�Nicks����
	for (short i = SKILL_LVL - 1; i >= 0; --i) {
		if (MaxExp >= NickExp[i]) {
			strcat_s(pNpcs[nRoleIdx].Nick, LvlNicks[i]);//��LvlNicks����׷�ӵ�Nick����
			break;
		}
	}
}
void Game::SkillLevelUp(short nRoleIndex, short Skilllndex) {
	//��������
	pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel++;
	pNpcs[nRoleIndex].MySkills[Skilllndex].Times = 0;
	short nHpAdded =pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel* pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].AddHp;
	pNpcs[nRoleIndex].MaxHP += nHpAdded;
	short nMpAdded =pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel* pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].AddMp;
	pNpcs[nRoleIndex].MaxMP += nMpAdded;
	short nAtkAdded =pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel* pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].AddAttack;
	pNpcs[nRoleIndex].Attack += nAtkAdded;
	short nDefAdded =pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel* pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].AddDefence;
	pNpcs[nRoleIndex].Defend += nDefAdded;
	short nSpeedAdded =pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel* pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].AddSpeed;
	pNpcs[nRoleIndex].Speed += nSpeedAdded;
	cout << endl;
	ShowNpcName(nRoleIndex);
	cout << "��";
	ShowSkillName(pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex);
	cout << "������" << pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel << "����"<< endl;
	cout << "������������" << nHpAdded << "��" << endl;
	cout << "������������" << nMpAdded << "��" << endl;
	cout << "��������" << nAtkAdded << "��" << endl;
	cout << "��������" << nDefAdded << "��" << endl;
	cout << "�ٶ�����" << nSpeedAdded << "��" << endl;
	SystemPause();
}
void Game::UpdateSkillLevel(short nRoleIndex, short Skilllndex) {
	short global_skill_index = pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex;
	short local_skill_level = pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel;

	short nExpAdded = local_skill_level * pSkills[global_skill_index].AddExp;
	pNpcs[nRoleIndex].Experience[pSkills[global_skill_index].SkillKind] += nExpAdded;

	cout << SkillNicks[pSkills[Skilllndex].SkillKind] << "��Ϊ����" << nExpAdded
		<< endl;
	UpdateNick(nRoleIndex);//�����º�

	if (local_skill_level == MAX_SKILL_LEVEL)
		return;
	if (pNpcs[nRoleIndex].MySkills[Skilllndex].Times 
		== local_skill_level * pSkills[global_skill_index].LevelUp) {//�ж�ʹ�ô����Ƿ�ﵽ��׼
		SkillLevelUp(nRoleIndex, Skilllndex);
	}
}
void Game::Act(short nRoleIndex, short Skilllndex, short nEnemyIndex) {
	short nDamage;
#ifdef _WIN32_
	SetConsoleTextAttribute(hConsoleHandle, SYSTEM_COLOR | 0);
#endif

	//��ͨ����
	if (Skilllndex == -1) {
		nDamage = Damage(nRoleIndex, nEnemyIndex);
		ShowNpcName(nRoleIndex);
		cout << "����";
		ShowNpcName(nEnemyIndex);
		cout << "�� " << endl;
	}

	//ʹ�ü��ܹ���
	else {
		pNpcs[nRoleIndex].MP -= SkillMpCost(nRoleIndex, Skilllndex);
		nDamage = Damage(nRoleIndex, nEnemyIndex) + SkillDamage(nRoleIndex, Skilllndex);
		ShowNpcName(nRoleIndex);
		cout << "ʹ��";
		ShowSkillName(pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex);
		cout << "����";
		ShowNpcName(nEnemyIndex);
		cout << "�� " << endl;
		if (nRoleIndex == 0){
			pNpcs[nRoleIndex].MySkills[Skilllndex].Times++;//����ʹ�ô�������
			UpdateSkillLevel(nRoleIndex, Skilllndex);//���¼��ܵȼ�
		}
	}

	ShowNpcName(nEnemyIndex);
	cout << "�ܵ�" << nDamage << "���˺���";

	//����֮��
	pNpcs[nEnemyIndex].HP -= nDamage;
	pNpcs[nEnemyIndex].HP =pNpcs[nEnemyIndex].HP > 0 ? pNpcs[nEnemyIndex].HP : 0;
	SystemPause();
	//����ս������սʤ��
	if (pNpcs[nEnemyIndex].HP == 0)
		EndFight(nRoleIndex);
	else if(pNpcs[nRoleIndex].HP==0)
		EndFight(nEnemyIndex);
	else {
		//RemoveNpc(oGameEnv.nCurrentMap, nEnemyIndex);
		oGameEnv.State = fighting;
	}
}
void Game::ExecuteScriptLine(char * stScriptLine) {//ִ��һ���ű����
	IndexInCurLine = 0;
	char stCommand[32];
	ReadSubString(stScriptLine, stCommand);// ��ȡ����
	char stStringBuffer[256];
	char stNumberBuffer[4];
	if (!strcmp(stCommand, "RETURN")) {//����
		RunningScripts = false;
	}
	//��Ļ���
	else if (!strcmp(stCommand, "PRINT")) {
		ReadSubString(stScriptLine, stStringBuffer);
		SystemOut(stStringBuffer);
		SystemPause();
	}
	//��ͣ
	else if (!strcmp(stCommand, "PAUSE"))
		SystemPause();
	//���
	else if (!strcmp(stCommand, "CLEAR"))
		system("cls");
	//����
	else if (!strcmp(stCommand, "TALK")) {
		//��ȡNPC�ı��
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNpcIndex = atoi(stNumberBuffer);
		//��ȡ��������
		ReadSubString(stScriptLine, stStringBuffer);
		//���
		ShowNpcName(nNpcIndex);
		cout << ": ";
#ifdef _WIN32_//����Ϊ��ɫ
		short nRandomColor = 1 + rand() % 15;
		SetConsoleTextAttribute(hConsoleHandle, SYSTEM_HINT_COLOR | 0);
		cout << stStringBuffer << endl << endl;
#else
		printf(SYSTEM_HINT_STRING, stStringBuffer);
		cout << endl;
#endif
		SystemPause();
	}

	//����ĳ������ֵ��������ڲ���ֵ��ű���ת��ָ���ط�
	else if (!strcmp(stCommand, "QUEST_JUMP")) {
		//��ȡ������
		ReadSubString(stScriptLine, stNumberBuffer);
		short QuestIndex = atoi(stNumberBuffer);

		//��ȡ����ֵ
		ReadSubString(stScriptLine, stNumberBuffer);
		short nTestValue = atoi(stNumberBuffer);

		//������Գ�������ת
		if (pQuests[QuestIndex].Flag == nTestValue) {
			//��ȡ��ת�Ľű����
			ReadSubString(stScriptLine, stNumberBuffer);
			short nScriptIndex = atoi(stNumberBuffer);
			FindScripts(nScriptIndex);//���Ҹýű�
			CurLineIndex--;
		}
	}
	//���������־
	else if (!strcmp(stCommand, "SET_FLAG")) {
		//��ȡ������
		ReadSubString(stScriptLine, stNumberBuffer);
		short QuestIndex = atoi(stNumberBuffer);
		//��ȡ�趨ֵ
		ReadSubString(stScriptLine, stNumberBuffer);
		short nSetValue = atoi(stNumberBuffer);
		pQuests[QuestIndex].Flag = nSetValue;//�趨������
	}

	//�����ɫ����ӵ��ָ��������ĳ����
	else if (!strcmp(stCommand, "HAVE_JUMP")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nGoodsIdx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNumber = atoi(stNumberBuffer);
		if (NumOfGoods(RoleIDx, nGoodsIdx) >= nNumber) {//numuofgoods:ӵ��ĳ��Ʒ������
			ReadSubString(stScriptLine, stNumberBuffer);
			short nScriptIndex = atoi(stNumberBuffer);
			FindScripts(nScriptIndex);//Ѱ�ҽű�
			CurLineIndex--;
		}
	}

	//����������ڵ�������ת
	else if (!strcmp(stCommand, "MORE_JUMP")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short NpcIndex= atoi(stNumberBuffer);
		//short nVarIndex = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nTestValue = atoi(stNumberBuffer);
		if (pNpcs[NpcIndex].Money >= nTestValue) {
			ReadSubString(stScriptLine, stNumberBuffer);
			short nScriptIndex = atoi(stNumberBuffer);
			FindScripts(nScriptIndex);//Ѱ������ű�
			CurLineIndex--;
		}
	}

	//���������������ת
	else if (!strcmp(stCommand, "EQ_JUMP")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short nVarIndex = atoi(stNumberBuffer);//����λ��
		ReadSubString(stScriptLine, stNumberBuffer);
		short nTestValue = atoi(stNumberBuffer);
		if (Var[nVarIndex] == nTestValue) {
			ReadSubString(stScriptLine, stNumberBuffer);
			short nScriptIndex = atoi(stNumberBuffer);
			FindScripts(nScriptIndex);//Ѱ�ҽű�
			CurLineIndex--;
		}
	}

	//�趨������ֵ
	else if (!strcmp(stCommand, "SET_VAR")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short nVarIndex = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nValue = atoi(stNumberBuffer);
		Var[nVarIndex] = nValue;
	}

	// YES/NO
	else if (!strcmp(stCommand, "YES_NO")) {
		SystemOut("1.��  2.��");
		short nSel = GetUserInput(1, 2);
		Var[0] = nSel;
	}
	else if (!strcmp(stCommand, "SELECT")) {
	//SystemOut("1.ţ����:30��2.����:20��3.���:50��4.����:20");
	short Sellect = GetUserInput(1, 4);
	Var[2] = Sellect;
	}

	//����ð��
	else if (!strcmp(stCommand, "CONTINUE"))
		oGameEnv.State = continue_game;
	//���н���
	else if (!strcmp(stCommand, "TRADE")) {
		oGameEnv.State = trading;
	}

	//ս��
	else if (!strcmp(stCommand, "FIGHT")) {
		//��ȡNPC���
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNpcIndex = atoi(stNumberBuffer);

		//��ʼ��ս��
		InitFight(nNpcIndex);
	}
	//������Ʒ
	else if (!strcmp(stCommand, "ADD_GOODS")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short GoodsIndex = atoi(stNumberBuffer);
		AddGoods(RoleIDx, GoodsIndex, true);
	}
	//ȥ����Ʒ
	else if (!strcmp(stCommand, "DEL_GOODS")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nGoodsIdx = atoi(stNumberBuffer);
		RemoveGoods(RoleIDx, HaveGoods(RoleIDx, nGoodsIdx), true);
	}
	//��������
	else if (!strcmp(stCommand, "ADD_MONEY")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nMoney = atoi(stNumberBuffer);
		AddMoney(RoleIDx, nMoney);
	}
	//��������
	else if (!strcmp(stCommand, "DEL_MONEY")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nMoney = atoi(stNumberBuffer);
		if (DelMoney(RoleIDx, nMoney))
			Var[1] = 0;  //�ɹ�
		else
			Var[1] = 1;  //ʧ��
	}

	//��ָ����ͼ���npc
	else if (!strcmp(stCommand, "ADD_NPC")) {
		//��ȡ��ͼ���
		ReadSubString(stScriptLine, stNumberBuffer);
		short nSpotIndex = atoi(stNumberBuffer);

		//��ȡnpc���
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNpcIndex = atoi(stNumberBuffer);

		//���npc
		if (!AddNpc(nSpotIndex, nNpcIndex)) {
			SystemWarning("���NPCʧ�ܣ���Ϸ���˳���");
			Running = false;
		}
	}

	//�Ƴ�ָ����ͼ��npc
	else if (!strcmp(stCommand, "REMOVE_NPC")) {
		//��ȡ��ͼ���
		ReadSubString(stScriptLine, stNumberBuffer);
		short nSpotIndex = atoi(stNumberBuffer);

		//��ȡnpc���
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNpcIndex = atoi(stNumberBuffer);

		//�Ƴ�npc
		if (!RemoveNpc(nSpotIndex, nNpcIndex)) {
			SystemWarning("ɾ��NPC������Ϸ���˳���");
			Running = false;
		}
	}

	else if (!strcmp(stCommand, "RANDOM_DROP")) {//���������Ʒ
		// ��ȡ��Ʒ���
		ReadSubString(stScriptLine, stNumberBuffer);
		short GoodsIndex = atoi(stNumberBuffer);

		// ��ȡ�����Դ�С
		ReadSubString(stScriptLine, stNumberBuffer);
		short nProb = atoi(stNumberBuffer);

		RandomDrop(oGameEnv.nCurrentNpc, GoodsIndex, nProb);
	}
	//�ָ�ȫ��
	else if (!strcmp(stCommand, "REFRESH")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleID = atoi(stNumberBuffer);
		Refresh(RoleID);
	}
	//�ָ�һ��
	else if (!strcmp(stCommand, "REFRESHSOME")) {
	ReadSubString(stScriptLine, stNumberBuffer);
	short RoleID = atoi(stNumberBuffer);
	RefreshSome(RoleID);
	}
	//ʳ��
	else if (!strcmp(stCommand, "EAT")) {
	ReadSubString(stScriptLine, stNumberBuffer);
	short NpcIndex = atoi(stNumberBuffer);
	ReadSubString(stScriptLine, stNumberBuffer);
	short GoodsIndex = atoi(stNumberBuffer);
	EatOrUse(NpcIndex, GoodsIndex);
	}
}
bool Game::FindScripts(short nScriptsID) {
	if (!pScripts || nScriptsID >= ScriptsNumber || nScriptsID < 0) {
		return false;
	}
	CurLineIndex = 0;
	while (pScripts[CurLineIndex].ScriptIndex < nScriptsID) {//�ҵ��籾
		CurLineIndex++;
	}
	return true;
}
void Game::RunScripts(short nScriptID) {
	if (!FindScripts(nScriptID))
		return;
	RunningScripts = true;
	while (RunningScripts) {
		ExecuteScriptLine(pScripts[CurLineIndex].ScriptLine);//һ��һ�����о籾
		CurLineIndex++;
	}
}
void Game::Refresh(short nRoleIndex) {
	//�ָ�
	pNpcs[nRoleIndex].HP = pNpcs[nRoleIndex].MaxHP;
	pNpcs[nRoleIndex].MP = pNpcs[nRoleIndex].MaxMP;
}
void Game::RefreshSome(short nRoleIndex) {
	//�ָ�һ��
	if(pNpcs[nRoleIndex].HP< pNpcs[nRoleIndex].MaxHP-10)
		pNpcs[nRoleIndex].HP = pNpcs[nRoleIndex].HP+10;
	else
		pNpcs[nRoleIndex].HP = pNpcs[nRoleIndex].MaxHP;

	if (pNpcs[nRoleIndex].MP < pNpcs[nRoleIndex].MaxMP - 5)
		pNpcs[nRoleIndex].MP = pNpcs[nRoleIndex].MP+5;
	else
		pNpcs[nRoleIndex].MP = pNpcs[nRoleIndex].MaxMP;
}
void Game::SystemPause() {
	system("pause>nul");//ϵͳ��ͣ ���û�����
	//system("pause");
	//cout << endl;
}
void Game::SystemOut(const char * stOutString) {
#ifdef _WIN32_//��Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, SYSTEM_COLOR | 0);
	cout << stOutString << endl << endl;
#else
	printf(SYSTEM_STRING, stOutString);
	cout << endl << endl;
#endif
}
void Game::SystemHint(const char * stHintString) {
	cout << endl;
#ifdef _WIN32_//��Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, SYSTEM_HINT_COLOR | 0);
	cout << stHintString << endl << endl;
#else
	printf(SYSTEM_HINT_STRING, stHintString);
	cout << endl << endl;
#endif
}
void Game::SystemWarning(const char * stWarningString) {
#ifdef _WIN32_//��Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, WARNING_COLOR | 0);
	cout << stWarningString << endl;
#else
	printf(WARNING_STRING, stWarningString);
	cout << endl;
#endif
	SystemPause();
}
void Game::PrintNpcs(short nSpotIndex) {
	short i;
	for (i = 0; i < pSpots[nSpotIndex].NpcNum; ++i) {//��ӡ����������
		cout << i + 1 << ".";
		ShowNpcName(pSpots[nSpotIndex].Npcs[i]);
		cout << endl;
	}
	cout << i + 1 << "." << "ȡ��" << endl << endl;
}
void Game::PrintGoodsOfNpc(short nNpcIndex) {
	short i;
	//��ӡ������Ʒ
	for (i = 0; i < pNpcs[nNpcIndex].GoodsNum; ++i) {
		cout << i + 1 << ".";
		ShowGoodsName(pNpcs[nNpcIndex].MyGoods[i]);
		cout << "\t";
		if (oGameEnv.State == buy_goods)
			cout << pGoods[pNpcs[nNpcIndex].MyGoods[i]].Cost << "��";
		else if (oGameEnv.State == sell_goods)
			cout << pGoods[pNpcs[nNpcIndex].MyGoods[i]].Cost / 2 << "��";
		cout << endl;
	}
	cout << i + 1 << ".ȡ��" << endl << endl;
}
void Game::PrintLogo() {
	if (!ReadWholeTextFile(oResFiles.LogoFile, LogoBuffer))
		return;
	/*cout << LogoBuffer << endl;
	return;*/
#ifdef _WIN32_//��Ϊ��ɫ
	SetConsoleTextAttribute(hConsoleHandle, SPOT_NAME_COLOR | 0);//�����ɫ
	cout << LogoBuffer << endl << endl;
#else
	printf(SPOT_NAME_STRING, LogoBuffer);
	cout<< endl << endl;
#endif
}
void Game::PrintMap() {
	if (!ReadWholeTextFile(oResFiles.MapFile, MapBuffer))
		return;
	/*cout << MapBuffer << endl;
	return;*/
	#ifdef _WIN32_//��Ϊ��ɫ
		SetConsoleTextAttribute(hConsoleHandle, ITEM_NAME_COLOR | 0);//�����ɫ
		cout << MapBuffer << endl << endl;
	#else
		printf(ITEM_NAME_STRING, MapBuffer);
		cout << endl << endl;
	#endif
}
void Game::PrintMenu() {
	cout << "1.�µ���; 2.��ս���� 3.��������" << endl;
}
void Game::PlayGame() {
	short input;
	if (!LoadResFileName("res_index.txt", '#'))//��ȡ�����ļ������ļ���ʼ��ʶΪ#
		return;
	if (!LoadSkillsData(oResFiles.SkillFile, '#'))//���ؼ����ļ�
		return;
	if (!LoadGoodsData(oResFiles.GoodsFile, '#'))//������Ʒ�ļ�
		return;
	if (!LoadQuestData(oResFiles.QuestFile, '#'))//���������ļ�
		return;
	if (!LoadScripts(oResFiles.ScriptFile, '#'))//���ؾ籾�ļ�
		return;
	if (!ReadWholeTextFile(oResFiles.MapFile, MapBuffer))//��ȡ��ͼ
		return;
	if (!ReadWholeTextFile(oResFiles.LogoFile, LogoBuffer))//��ȡlogo
		return;
	LoadNickData(oResFiles.NickFile, '#');//�����ǳ�
	Running = true;
	oGameEnv.State = system_menu;
	while (Running) {
		switch (oGameEnv.State) {
		case system_menu:
			system("cls");//����
			PrintLogo();//��ӡlogo
			PrintMenu();//��ӡ�˵�
			input = GetUserInput(1, 3);
			if (input == 1) {
				SystemHint("��ȡ�������");
				if (!LoadPlayerData(true)) {
					Running = false;
				}
				cout << "������";
				ShowNpcDetail(0);//��ʾСϺ��ϸ��
				SystemPause();
				oGameEnv.State = continue_game;
			}
			else if (input == 2) {
				SystemHint("��ȡ�������");
				if (!LoadPlayerData(false)) {
					Running = false;
				}
				oGameEnv.State = continue_game;
			}
			else if (input == 3)
					return;
			else
			SystemWarning("û����ѡ�����������ѡ��");

			break;
			
		case continue_game:
			system("cls");	//����
			ShowSpotDetail(oGameEnv.nCurrentMap);//��ʾ��ǰ�ص�ϸ��
			ShowNpcState(0);
			SystemOut("1.��̸	2.�۲�	3.����	4.ʰȡ	5.��·	6.״̬	7.ϵͳ	8.��ͼ");
			input = GetUserInput(1, 8);
			//���˽�̸
			if (input == 1) {
				if (pSpots[oGameEnv.nCurrentMap].NpcNum == 0)
					SystemWarning("����û���˿��Խ�̸��");
				else
					oGameEnv.State = talk_with_npc;
			}
			//�۲����
			else if (input == 2) {
				if (pSpots[oGameEnv.nCurrentMap].NpcNum == 0)
					SystemWarning("����û���˿��Թ۲죡");
				else
					oGameEnv.State = watch_npc;
			}
			//��������
			else if (input == 3) {
				if (pSpots[oGameEnv.nCurrentMap].NpcNum == 0)
					SystemWarning("����û���˿��Թ�����");
				else
					oGameEnv.State = attack_npc;
			}
			//����
			else if (input == 4) {
				if (pSpots[oGameEnv.nCurrentMap].GoodsNum == 0)
					SystemWarning("����û�пɼ�Ķ�����");
				else
					oGameEnv.State = list_free_item;
			}
			//��·
			else if (input == 5) {
				if (pSpots[oGameEnv.nCurrentMap].LeaveScript >= 0) {
					RunScripts(pSpots[oGameEnv.nCurrentMap].LeaveScript);
					oGameEnv.State = continue_game;
				}
				else
					oGameEnv.State = show_exits;
			}
			//��Ҳ쿴״̬
			else if (input == 6)
				oGameEnv.State = watch_state;
			//ϵͳ�˵�
			else if (input == 7) {
				oGameEnv.State = playing_system_menu;
			}
			//�鿴��ͼ
			else if (input == 8) {
				PrintMap();
				SystemPause();
				oGameEnv.State = continue_game;
			}
			else
			SystemWarning("û����ѡ�����������ѡ��");
			break;
			//��Ҳ쿴״̬
		case watch_state:
			SystemHint("Ҫ�쿴ʲô��");
			SystemOut("1.����	2.��Ʒ	3.װ��	4.����	5.����	6.ȡ��");
			input = GetUserInput(1, 6);
			//���Ҫ�쿴����
			if (input == 1) {
				ShowNpcDetailX(0);
				SystemPause();
				oGameEnv.State = watch_state;
			}
			//�鿴���ϵ���Ʒ
			else if (input == 2) {
				if (pNpcs[0].GoodsNum)
					oGameEnv.State = list_goods;
				else {
					SystemWarning("������û���κ���Ʒ.");
					oGameEnv.State = watch_state;
				}
			}
			//�鿴װ��
			else if (input == 3)
				oGameEnv.State = check_equipment;
			//�鿴ӵ�еļ���
			else if (input == 4) {
				if (pNpcs[0].SkillNum)
					oGameEnv.State = list_skills;
				else {
					SystemWarning("�㻹ûѧ���κμ��ܡ�");
					oGameEnv.State = watch_state;
				}
			}
			//�鿴����
			else if (input == 5)
				oGameEnv.State = list_quests;
			//ȡ��
			else if(input==6)
				oGameEnv.State = continue_game;
			else
			{
				SystemWarning("û����ѡ�������������Ϸ��");
				oGameEnv.State = continue_game;
			}
			break;
			//ѡ���ƶ���Ŀ�ĵ�
		case show_exits:
			SystemHint("Ҫȥ���");
			short i;
			//��ʾ����ȥ�ĵط�
			for (i = 0; i < pSpots[oGameEnv.nCurrentMap].ExitNum; ++i) {
				cout << (i + 1) << ".";
				ShowSpotName(pSpots[oGameEnv.nCurrentMap].Exits[i]);
				cout << endl;
			}
			cout << i + 1 << ".ȡ��" << endl << endl;
			input = GetUserInput(1, i + 1);
			//ȡ��
			if (input == i + 1)
				oGameEnv.State = continue_game;
			//ת����Ŀ�ĵ�
			else {
				oGameEnv.nCurrentMap = pSpots[oGameEnv.nCurrentMap].Exits[input - 1];
				RunScripts(pSpots[oGameEnv.nCurrentMap].EnterScript);
				oGameEnv.State = continue_game;
			}
			break;
			//���˽�̸
		case talk_with_npc:
			SystemHint("Ҫ��˭��̸��");
			PrintNpcs(oGameEnv.nCurrentMap);
			input = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].NpcNum + 1);
			//ȡ��
			if (input == pSpots[oGameEnv.nCurrentMap].NpcNum + 1)
				oGameEnv.State = continue_game;
			//��ѡ�е�����Ի�
			else {
				oGameEnv.nCurrentNpc = pSpots[oGameEnv.nCurrentMap].Npcs[input - 1];
				RunScripts(pNpcs[oGameEnv.nCurrentNpc].TalkScript);
				if (oGameEnv.State == talk_with_npc)
					oGameEnv.State = continue_game;
			}
			break;
			//�۲����
		case watch_npc:
			SystemHint("Ҫ�۲�˭��");
			PrintNpcs(oGameEnv.nCurrentMap);
			input = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].NpcNum + 1);
			//ȡ��
			if (input == pSpots[oGameEnv.nCurrentMap].NpcNum + 1)
				oGameEnv.State = continue_game;
			//��ʾ�����˵�����
			else {
				//ShowNpcDetail(pSpots[oGameEnv.nCurrentMap].Npcs[input - 1]);
				ShowNpcDetailX(pSpots[oGameEnv.nCurrentMap].Npcs[input - 1]);
				SystemPause();
				oGameEnv.State = continue_game;
			}
			break;
			//��������
		case attack_npc:
			SystemHint("Ҫ����˭��");
			PrintNpcs(oGameEnv.nCurrentMap);//��ʾ��ǰ��ͼ����npc
			input = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].NpcNum + 1);
			//ȡ��
			if (input == pSpots[oGameEnv.nCurrentMap].NpcNum + 1)
				oGameEnv.State = continue_game;
			//��ս
			else {
				cout << "����";
				ShowNpcName(pSpots[oGameEnv.nCurrentMap].Npcs[input - 1]);
				SystemWarning("��ʼս����");
				InitFight(pSpots[oGameEnv.nCurrentMap].Npcs[input - 1]);
				oGameEnv.State = fighting;
			}
			break;
			//ս������
		case fighting:
			short ActRole;
			short SelectedSkill;
			ActRole = DecideWhoAct();
			//���������ж�
			if (ActRole == 0) {
				oGameEnv.State = player_act;
			}
			//�����npc�ж�
			else {
				SelectedSkill = UseWhichSkill(ActRole);
				Act(ActRole, SelectedSkill, 0);
			}
			break;
			//�����ս���е��ж�
		case player_act:
			SystemHint("�ֵ����ж��ˣ�");
			ShowNpcState(0);//��ʾ����ֵ������
			//cout << endl;
			ShowNpcState(oGameEnv.nCurrentEnemy);
			SystemOut("1.����	2.����	3.��Ʒ	4.����	");
			input = GetUserInput(1, 4);
			//ѡ�񹥻�
			if (input == 1)
			{
				Act(0, -1, oGameEnv.nCurrentEnemy);//�ж�
				cout << endl;
			}
			//ʹ�ü���
			else if (input == 2) {
				if (pNpcs[0].SkillNum == 0) {
					SystemWarning("�㻹ûѧ���κμ��ܡ�");
				}
				else
					oGameEnv.State = list_skills;
			}
			//ʹ����Ʒ
			else if (input == 3) {
				if (pNpcs[0].GoodsNum == 0) {
					SystemWarning("������û���κ���Ʒ��");
				}
				else
					oGameEnv.State = list_goods;
			}
			//����
			else if (input == 4)
				EscapeOut(0, oGameEnv.nCurrentEnemy);
			//����
			else
				SystemWarning("û����ѡ�����������ѡ��");
			break;
			//��Ϸ�����е���ϵͳ�˵�
		case playing_system_menu:

			SystemHint("Ҫ��ʲô��");
			SystemOut("1.�洢����\n2.��ȡ����\n3.���ر���\n4.������Ϸ");

			input = GetUserInput(1, 4);

			//����Ǵ洢����
			if (input == 1) {
				if (!SavePlayerData()) {
					SystemWarning("���ȴ洢ʧ�ܡ�");
					Running = false;
				}
				else {
					SystemHint("���ȴ洢��ϡ�");
					SystemPause();
				}
				oGameEnv.State = playing_system_menu;
			}

			//��ȡ����
			else if (input == 2) {
				if (!LoadPlayerData(false)) {
					SystemWarning("���ȶ�ȡʧ�ܣ�");
					Running = false;
				}
				else {
					SystemHint("���ȶ�ȡ�ɹ���");
					SystemPause();
				}
				oGameEnv.State = continue_game;
			}

			else if (input == 3)
			{
				oGameEnv.State = system_menu;
				SystemPause();
			}
			else if (input == 4)
			{
				SystemOut("������Ϸ��");
				oGameEnv.State = continue_game;
				SystemPause();
			}
			break;

			//�鿴����
		case list_quests:
			SystemHint("Ҫ�鿴��������");
			SystemOut("1.δ�������		2.���������	3.����");
			input = GetUserInput(1, 3);
			if (input == 1) {
				ShowQuests(false);
			}
			else if (input == 2) {
				ShowQuests(true);
			}
			else if (input == 3)
				oGameEnv.State = continue_game;
			break;
			//�鿴ӵ����Ʒ
		case list_goods:
			SystemHint("Ҫʹ��ʲô��Ʒ��");
			PrintGoodsOfNpc(0);//��ӡ������Ʒ
			input = GetUserInput(1, pNpcs[0].GoodsNum + 1);
			//ȡ��
			if (input == pNpcs[0].GoodsNum + 1) {
				if (oGameEnv.bFighting)
					oGameEnv.State = player_act;
				else
					oGameEnv.State = continue_game;
			}
			//׼��ʹ��ѡ�е���Ʒ
			else {
				oGameEnv.nCurrentGoods = input - 1;
				oGameEnv.State = use_goods;
			}
			break;
			//ʹ����Ʒ
		case use_goods:
			ShowGoodsDetail(pNpcs[0].MyGoods[oGameEnv.nCurrentGoods]);
			SystemOut("1.ʹ��	2.����	3.����");
			input = GetUserInput(1, 3);
			//ʹ����Ʒ
			if (input == 1) {
				UseGoods(0, oGameEnv.nCurrentGoods);
				if (oGameEnv.bFighting)
					oGameEnv.State = fighting;
				else
					oGameEnv.State = list_goods;
			}
			else if (input == 2) {
				ShowNpcName(0);
				cout << "�ӵ���";
				ShowGoodsName(pNpcs[0].MyGoods[oGameEnv.nCurrentGoods]);
				cout << "��" << endl;
				SystemPause();
				AddGoodsToSpot(oGameEnv.nCurrentMap, pNpcs[0].MyGoods[oGameEnv.nCurrentGoods]);//��������Ʒ���뵱ǰ�ص�
				RemoveGoods(0, oGameEnv.nCurrentGoods, false);//ȥ��СϺ�����ϵĸ���Ʒ
				oGameEnv.State = list_goods;
			}
			//ȡ��
			else
				oGameEnv.State = list_goods;
			break;

			//�鿴ӵ�еļ���
		case list_skills:
			SystemHint("Ҫʹ��ʲô���ܣ�");
			for (i = 0; i < pNpcs[0].SkillNum; ++i) {
				cout << i + 1 << ".";
				ShowSkillName(pNpcs[0].MySkills[i].SkillIndex);
				cout << "\t" << pNpcs[0].MySkills[i].SkillLevel << "��" << endl;
			}
			cout << i + 1 << ".ȡ��" << endl << endl;
			input = GetUserInput(1, i + 1);
			//ȡ��
			if (input == i + 1) {
				if (oGameEnv.bFighting)
					oGameEnv.State = player_act;
				else
					oGameEnv.State = continue_game;
			}
			//׼��ʹ�ü���
			else {
				oGameEnv.nCurrentSkill = input - 1;
				oGameEnv.State = use_skill;
			}
			break;
			//ʹ�ü���
		case use_skill:
			//������������
			short nMpCost;
			nMpCost = SkillMpCost(0, oGameEnv.nCurrentSkill);
			//��ʾ
			ShowSkillDetail(pNpcs[0].MySkills[oGameEnv.nCurrentSkill].SkillIndex);//��ʾ����ϸ��
			cout << "��ǰ����" << pNpcs[0].MySkills[oGameEnv.nCurrentSkill].SkillLevel << '\t';
			cout << "�ķ�������" << nMpCost << '\t';
			cout << "�����˺���" << SkillDamage(0, oGameEnv.nCurrentSkill) << endl;
			SystemOut("1.ʹ��	2.����");
			input = GetUserInput(1, 2);
			//ʹ�ü���
			if (input == 1) {
				if (oGameEnv.bFighting) {
					if (pNpcs[0].MP >= nMpCost) {
						Act(0, oGameEnv.nCurrentSkill, oGameEnv.nCurrentEnemy);
					}
					else {
						SystemWarning("�������㡣");
						oGameEnv.State = player_act;
					}
				}
				else {
					SystemWarning("ֻ����ս��ʱʹ�ã�");
					oGameEnv.State = list_skills;
				}
			}
			//ȡ��
			else if (input == 2) {
				oGameEnv.State = list_skills;
			}
			break;
			//�г����Լ�Ķ���
		case list_free_item:
			SystemHint("Ҫ����ʲô��");
			for (i = 0; i < pSpots[oGameEnv.nCurrentMap].GoodsNum; ++i) {
				cout << i + 1 << ".";
				ShowGoodsName(pSpots[oGameEnv.nCurrentMap].Goods[i]);
				cout << "\t";
			}
			cout << i + 1 << ".ȡ��" << endl << endl;
			input = GetUserInput(1, i + 1);
			if (input == i + 1)
				oGameEnv.State = continue_game;
			else {
				oGameEnv.nCurrentGoods = input - 1;
				oGameEnv.State = pick_item;
			}
			break;
			//��ȡ��Ʒ
		case pick_item:
			ShowGoodsDetail(
				pSpots[oGameEnv.nCurrentMap].Goods[oGameEnv.nCurrentGoods]);
			SystemOut("1.��ȡ	2.ȡ��");
			input = GetUserInput(1, 2);
			if (input == 1) {
				PickGoods(0, oGameEnv.nCurrentGoods);
				if (pSpots[oGameEnv.nCurrentMap].GoodsNum == 0)
					oGameEnv.State = continue_game;
				else
					oGameEnv.State = list_free_item;
			}
			else
				oGameEnv.State = list_free_item;
			break;
		case check_equipment:
			ShowNpcName(0);
			cout << "��װ�������" << endl;

#ifdef _WIN32_//����Ϊ��ɫ
			SetConsoleTextAttribute(hConsoleHandle, ITEM_NAME_COLOR | 0);
#endif
			cout << "1.ͷ����";
			ShowEquipment(0, 0);
			cout << endl;
			cout << "2.���ţ�";
			ShowEquipment(0, 1);
			cout << endl;
			cout << "3.�ֳ֣�";
			ShowEquipment(0, 2);
			cout << endl;
			cout << "4.��̤��";
			ShowEquipment(0, 3);
			cout << endl;
			cout << "5.ȡ��";
			cout << endl << endl;
			input = GetUserInput(1, 5);
			if (input == 5)
				oGameEnv.State = continue_game;
			else if (pNpcs[0].MyEquipment[input - 1] == -1)
				SystemWarning("��ûʲô�ÿ��ġ�");
			else {
				oGameEnv.nCurrentEquipment = input - 1;
				oGameEnv.State = unloading_equipment;
			}
			break;
		case unloading_equipment:
			ShowGoodsDetail(pNpcs[0].MyEquipment[oGameEnv.nCurrentEquipment]);
			SystemOut("1.ж��	2.����");
			input = GetUserInput(1, 2);
			if (input == 1) {
				UnEquip(0, oGameEnv.nCurrentEquipment);
				oGameEnv.State = check_equipment;
			}
			else
				oGameEnv.State = continue_game;
			break;
		//����
		case trading:
			ShowNpcName(oGameEnv.nCurrentNpc);
			cout << "����ӭ���٣�������ʲô��Ҫ�İɣ�" << endl;
			SystemOut("1.����	2.����	3.ȡ��");
			input = GetUserInput(1, 3);
			if (input == 1)
				oGameEnv.State = buy_goods;
			else if (input == 2)
				oGameEnv.State = sell_goods;
			else
				oGameEnv.State = continue_game;
			break;
		//����
		case buy_goods:
			SystemHint("Ҫ��Щʲô��");
			PrintGoodsOfNpc(oGameEnv.nCurrentNpc);//�г�������Ʒ
			input = GetUserInput(1, pNpcs[oGameEnv.nCurrentNpc].GoodsNum + 1);
			if (input == pNpcs[oGameEnv.nCurrentNpc].GoodsNum + 1)
				oGameEnv.State = continue_game;
			else {
				BuyGoods(0, pNpcs[oGameEnv.nCurrentNpc].MyGoods[input - 1]);
				oGameEnv.State = buy_goods;
			}
			break;
		//������
		case sell_goods:
			SystemHint("Ҫ��ʲô��");
			PrintGoodsOfNpc(0);//�г�������Ʒ
			input = GetUserInput(1, pNpcs[0].GoodsNum + 1);
			if (input == pNpcs[0].GoodsNum + 1)
				oGameEnv.State = continue_game;
			else {
				SellGoods(0, input - 1);
				oGameEnv.State = sell_goods;
			}
			break;
		default:
			oGameEnv.State = continue_game;
		}
	}
}