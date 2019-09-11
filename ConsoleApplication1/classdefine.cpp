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
	TextFile.seekg(0, ios::end);//先移到文件末尾，然后用tellg函数显示当前位置，即文件长度
	FileLength = static_cast<int>(TextFile.tellg());
	TextFile.seekg(0);
	try {
		TextFile.read(Buffer, FileLength);
	}
	catch (...) {//捕捉所有类型的异常
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
	ResNameFile.ReadString(oResFiles.SkillFile);//读取技能文件名
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.GoodsFile);//读取物品文件名
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.RoleFile);//读取人物文件名
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.SpotFile);//读取地点文件名
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.QuestFile);//读取任务文件名
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.ScriptFile);//读取脚本文件名
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.MapFile);//读取地图文件名
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.LogoFile);//读取logo文件名
	ResNameFile.GotoNextLine();
	ResNameFile.ReadString(oResFiles.NickFile);//读取昵称文件名
	return true;
}
bool Game::LoadPlayerData(bool bNewGame) {
	//装载npc和场景数据
	if (!LoadNpcsData(oResFiles.RoleFile, '#')) {
		SystemWarning("装载NPC数据失败！");
		return false;
	}
	if (!LoadSpotsData(oResFiles.SpotFile, '#')) {
		SystemWarning("装载场景信息失败！");
		return false;
	}
	if (bNewGame) {//如果是新游戏，起始地点在宿舍
		oGameEnv.nCurrentMap = 0;
	}
	else {
		//打开存档文件
		ifstream SaveFile("save.dat");
		if (!SaveFile) {
			cerr << "open save file failed !" << endl;
			return false;
		}
		//读取玩家数据
		SaveFile.read(reinterpret_cast<char *>(&pNpcs[0]), sizeof(Npc));
		SaveFile.read(reinterpret_cast<char *>(&oGameEnv.nCurrentMap), sizeof(short));
		//读取任务标志
		for (short i = 0; i < QuestsNum; ++i)
			SaveFile.read(reinterpret_cast<char *>(&pQuests[i].Flag), sizeof(short));
		//读取变量
		for (int i = 0; i < VAR_NUM; ++i)
			SaveFile.read(reinterpret_cast<char *>(&Var[i]), sizeof(short));
		//读取各个场景的npc和物品
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
	TextParser SpotsFile(pstFileName, cHeadFlag);	//文件流
	SpotsNum = SpotsFile.GetRecordNum();			//获得地点总数
	pSpots = new Spot[SpotsNum]; 

	for (short nSpotsIndex = 0; nSpotsIndex < SpotsNum; ++nSpotsIndex) {
		SpotsFile.FindRecordHead();
		SpotsFile.GotoNextLine();
		SpotsFile.ReadString(pSpots[nSpotsIndex].Name);//读取场景名
		SpotsFile.GotoNextLine();
		SpotsFile.ReadString(pSpots[nSpotsIndex].Scene);//读取场景描述
		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].ExitNum);//读取出口数量
		SpotsFile.GotoNextLine();

		for (i = 0; i < pSpots[nSpotsIndex].ExitNum; ++i)//依次读取出口对应的位置
			SpotsFile.ReadInteger(&pSpots[nSpotsIndex].Exits[i]);

		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].GoodsNum);//读取物品数量
		SpotsFile.GotoNextLine();

		for (i = 0; i < pSpots[nSpotsIndex].GoodsNum; ++i)//依次读取物品对应位置
			SpotsFile.ReadInteger(&pSpots[nSpotsIndex].Goods[i]);

		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].NpcNum);//读取npc数量
		SpotsFile.GotoNextLine();

		for (i = 0; i < pSpots[nSpotsIndex].NpcNum; ++i)//依次读取npc所对应的位置
			SpotsFile.ReadInteger(&pSpots[nSpotsIndex].Npcs[i]);

		SpotsFile.GotoNextLine();
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].EnterScript);//读取进入地点时对应脚本位置
		SpotsFile.ReadInteger(&pSpots[nSpotsIndex].LeaveScript);//读取离开地点时对应脚本位置
	}
	return true;
}
bool Game::LoadNpcsData(char * pstFileName, char cHeadFlag) {
	short i;
	if (pNpcs) {
		delete[] pNpcs;
		pNpcs = NULL;
	}
	TextParser NpcsFile(pstFileName, cHeadFlag);//文件流
	NpcsNum = NpcsFile.GetRecordNum();		//得到npc数量	
	pNpcs = new Npc[NpcsNum];
	if (!pNpcs) {
		SystemWarning("为角色分配内存失败，游戏将退出！");
		return false;
	}
	for (short NpcsIndex = 0; NpcsIndex < NpcsNum; ++NpcsIndex) {
		NpcsFile.FindRecordHead();
		NpcsFile.GotoNextLine();
		NpcsFile.ReadString(pNpcs[NpcsIndex].Name);//人物姓名
		NpcsFile.GotoNextLine();
		NpcsFile.ReadString(pNpcs[NpcsIndex].Desc);//任务描述
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Experience[0]));	//关于刀的经验
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Experience[1]));	//关于剑的经验
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Experience[2]));	//关于枪棍的经验
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Experience[3]));	//关于拳的经验
		UpdateNick(NpcsIndex); //更新昵称
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].HP));	//读取生命值
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MaxHP));	//读取生命值上限
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MP));	//读取内力值
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MaxMP));	//读取内力值上限
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Attack));	//读取攻击力
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Defend));	//读取防御力
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Speed));	//读取速度
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].SkillNum));	//读取拥有技能数量
		NpcsFile.GotoNextLine();

		for (i = 0; i < pNpcs[NpcsIndex].SkillNum; i++) {//依次读取每个技能对应的位置、等级、以及当前等级技能已使用次数
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MySkills[i].SkillIndex));
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MySkills[i].SkillLevel));
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MySkills[i].Times));//技能使用一定次数后可以升级
		}

		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].GoodsNum));	//读取物品数量
		NpcsFile.GotoNextLine();

		for (i = 0; i < pNpcs[NpcsIndex].GoodsNum; i++) {//依次读取物品
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MyGoods[i]));
		}

		NpcsFile.GotoNextLine();
		for (i = 0; i < 4; ++i)//读取四个地方的装备
			NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].MyEquipment[i]));

		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].Money));//读取银两
		NpcsFile.GotoNextLine();
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].TalkScript)); //读取与人物交谈的脚本序号
		NpcsFile.ReadInteger(&(pNpcs[NpcsIndex].FailScript));	//读取人物失败时的脚本序号
	}
	return true;
}//看过
bool Game::LoadSkillsData(const char * pstFileName, const char cHeadFlag) {
	if (pSkills) {
		delete[] pSkills;
		pSkills = NULL;
	}
	TextParser SkillsFile(pstFileName, cHeadFlag);//文件流
	SkillsNum = SkillsFile.GetRecordNum();//通过HeadFlag即#获得技能数量
	pSkills = new Skill[SkillsNum];
	if (!pSkills) {
		SystemWarning("为技能分配内存失败，游戏将退出！");
		return false;
	}
	for (short Skilllndex = 0; Skilllndex < SkillsNum; ++Skilllndex) {
		SkillsFile.FindRecordHead();
		SkillsFile.GotoNextLine();
		SkillsFile.ReadString(pSkills[Skilllndex].Name);//读名字
		SkillsFile.GotoNextLine();
		SkillsFile.ReadString(pSkills[Skilllndex].Desc);//读注释
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].Power));//技能威力
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].Level));//技能等级
		/*SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].NeedLev));*/
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].LevelUp));//每个等级的技能需要练习的次数
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].NeedExp));//需要多少经验才能领悟这种技能
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].SkillKind)); //技能类型 0刀、1剑、2枪棍、3拳
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].MpCost));//使用技能消耗的魔法值
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddAttack));//增加伤害
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddDefence));//增加防御
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddHp));//增加生命值
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddMaxHp));//增加生命值上限
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddMp));//增加魔法值
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddMaxMp));//增加内力值上限
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddSpeed));//增加速度
		SkillsFile.GotoNextLine();
		SkillsFile.ReadInteger(&(pSkills[Skilllndex].AddExp));//增加经验
	}
	return true;
}
bool Game::LoadGoodsData(char * pstFileName, char cHeadFlag) {
	if (pGoods) {
		delete[] pGoods;
		pGoods = NULL;
	}
	TextParser GoodsFile(pstFileName, cHeadFlag);//文件流
	GoodsNum = GoodsFile.GetRecordNum();//获得游戏总的物品量
	pGoods = new Goods[GoodsNum];
	if (!pGoods) {
		SystemWarning("为物品分配内存失败，游戏将退出！");
		return false;
	}
	for (short GoodsIndex = 0; GoodsIndex < GoodsNum; GoodsIndex++) {
		GoodsFile.FindRecordHead();
		GoodsFile.GotoNextLine();
		GoodsFile.ReadString(pGoods[GoodsIndex].Name);			//姓名
		GoodsFile.GotoNextLine();
		GoodsFile.ReadString(pGoods[GoodsIndex].Desc);		//描述
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].Use);//0表示吃 1表示装备 2表示学习 3 任务物品
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddAttack);	//增加的攻击力
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddDefence);//增加的防御力
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddSpeed);	//增加的速度值
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddHP);			//增加生命值
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddMaxHP);		//增加生命值上限
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddMP);			//增加内力值
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].AddMaxMP);		//增加内力值上限
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].SkillIndex);//可学习的技能标号，没有则为-1
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].Location);//如果是装备，装备的地方0-头，1-身，2-手，3-脚,-1没有
		GoodsFile.GotoNextLine();
		GoodsFile.ReadInteger(&pGoods[GoodsIndex].Cost);		//值多少钱
	}
	return true;
}
bool Game::LoadScripts(char * pstFileName, char cHeadFlag) {
	if (pScripts) {
		delete[] pScripts;
		pScripts = NULL;
	}
	TextParser ScriptsFile(pstFileName, cHeadFlag);	//文件流
	ScriptsNumber = ScriptsFile.GetRecordNum();//通过#号得到有几个脚本
	char Temp[512];
	short ScriptsIndex = -1;
	short ValidLineIndex = -1;
	short LineNumber = 0;
	short ValidLineNum = 0;
	while (ScriptsFile.GetWholeLine(Temp)) {	//计算有几行剧本
		LineNumber++;
		if (Temp[0] != cHeadFlag)
			ValidLineNum++;//有效的剧本行数，就是不包括#号的
	}
	ScriptLineNumber = ValidLineNum;
	pScripts = new ScriptLine[ValidLineNum];//剧本是以行为单位的
	if (!pScripts) {
		SystemWarning("为剧本分配内存失败，游戏将退出！");
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
				pScripts[ValidLineIndex].ScriptIndex = ScriptsIndex;//表示第几行剧本属于那一个剧本
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
		NickFile.ReadString(SkillNicks[i]);//剑术 刀法 枪棍 拳掌     [技能]

	NickFile.GotoNextLine();
	for (i = 0; i < SKILL_LVL; ++i)
		NickFile.ReadString(LvlNicks[i]);//新手 入门 新秀 专精 大师[称号]

	NickFile.GotoNextLine();
	for (i = 0; i < SKILL_LVL; ++i)
		NickFile.ReadInteger(&NickExp[i]);//		0 100 300 500 1000[经验]
}
bool Game::LoadQuestData(char * pstFileName, char cHeadFlag) {
	if (pQuests) {
		delete[] pQuests;
		pQuests = NULL;
	}
	TextParser QuestsFile(pstFileName, cHeadFlag);//文件流
	QuestsNum = QuestsFile.GetRecordNum();//任务数量
	pQuests = new Quest[QuestsNum];
	if (!pQuests) {
		SystemWarning("为任务分配内存失败，游戏将退出！");
		return false;
	}
	for (short QuestIndex = 0; QuestIndex < QuestsNum; ++QuestIndex) {
		QuestsFile.FindRecordHead();
		QuestsFile.GotoNextLine();
		QuestsFile.ReadString(pQuests[QuestIndex].Desc);//描述
		QuestsFile.GotoNextLine();
		QuestsFile.ReadInteger(&pQuests[QuestIndex].RoleIndex);//人物标号
		QuestsFile.GotoNextLine();
		QuestsFile.ReadInteger(&pQuests[QuestIndex].MapIndex);//地图标号
		pQuests[QuestIndex].Flag = 0;//0未接受 1接受未完成 2已完成
	}
	return true;
}
bool Game::SavePlayerData() {
	ofstream SaveFile("save.dat");
	if (!SaveFile) {
		cerr << "open save file failed !" << endl;
		return false;
	}
	SaveFile.write(reinterpret_cast<char *>(&pNpcs[0]), sizeof(Npc));//写入小虾米信息
	SaveFile.write(reinterpret_cast<char *>(&oGameEnv.nCurrentMap),sizeof(short));//当前地图信息
	//写入任务标志
	for (int i = 0; i < QuestsNum; ++i)
		SaveFile.write(reinterpret_cast<char *>(&pQuests[i].Flag),sizeof(short));
	//写入变量标识
	for (int i = 0; i < VAR_NUM; ++i)
		SaveFile.write(reinterpret_cast<char *>(&Var[i]), sizeof(short));
	//写入各个场景npc和 物品的情况
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
	for (short i = 0; i < pNpcs[RoleIDx].GoodsNum; ++i) {//计算某人物身上某物品数量
		if (pNpcs[RoleIDx].MyGoods[i] == nGoodsIdx)
			nNumber++;
	}
	return nNumber;
}
short Game::GetUserInput() {
	char str[1];
	str[0] = getchar();//获取一个字符
	return atoi(str);//将字符转换为Int
}
short Game::GetUserInput(short nMin, short nMax) {
	short n = GetUserInput();

	while (n < nMin || n > nMax)//当n在最大值、最小值之间结束
		n = GetUserInput();

	return n;
}
void Game::ShowNpcName(short nNpcIndex) {
#ifdef _WIN32_//设置人物名字为黄色并打印名字
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
	ShowNpcName(nNpcIndex);//显示人物姓名
	//cout << pNpcs[nNpcIndex].Nick << endl;
	//输出人物当前生命、内力值、以及银两
	cout << "( 生命：" << pNpcs[nNpcIndex].HP << "/" << pNpcs[nNpcIndex].MaxHP
		<< "； 内力：" << pNpcs[nNpcIndex].MP << "/" << pNpcs[nNpcIndex].MaxMP
		<< "； " << pNpcs[nNpcIndex].Money << "文 )" << endl;
}
void Game::ShowNpcDetail(short nNpcIndex) {
	ShowNpcName(nNpcIndex);
#ifdef _WIN32_//设置描述颜色为绿色并输出描述
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
	cout << pNpcs[nNpcIndex].Desc << endl;
#else
	printf(DESCRIPTION_STRING, pNpcs[nNpcIndex].Desc);
	cout << endl;
#endif
}
void Game::ShowNpcDetailX(short nNpcIndex) {
	ShowNpcDetail(nNpcIndex);
#ifdef _WIN32_//设置为绿色
	SetConsoleTextAttribute(hConsoleHandle, ITEM_NAME_COLOR | 0);
#endif
	cout << "银两: " << pNpcs[nNpcIndex].Money << endl;
	cout << endl;

#ifdef _WIN32_//设置为绿色
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
#endif
	for (short i = 0; i < SKILL_KIND; ++i) {
		if(i==0)
		cout << SkillNicks[i] << "刀法修为 ：" << pNpcs[nNpcIndex].Experience[i] << endl;
		if (i == 1)
			cout << SkillNicks[i] << "剑法修为 ：" << pNpcs[nNpcIndex].Experience[i] << endl;
		if (i == 2)
			cout << SkillNicks[i] << "枪棍修为 ：" << pNpcs[nNpcIndex].Experience[i] << endl;
		if (i == 3)
			cout << SkillNicks[i] << "拳法修为 ：" << pNpcs[nNpcIndex].Experience[i] << endl;
	}
	cout << "生命  ：" << pNpcs[nNpcIndex].HP << " / "
		<< pNpcs[nNpcIndex].MaxHP << endl;
	cout << "内力  ：" << pNpcs[nNpcIndex].MP << " / "
		<< pNpcs[nNpcIndex].MaxMP << endl;
	cout << "攻击  ：" << pNpcs[nNpcIndex].Attack << " + "
		<< GetAddedAttack(nNpcIndex) << endl;
	cout << "防御  ：" << pNpcs[nNpcIndex].Defend << " + "
		<< GetAddedDefend(nNpcIndex) << endl;
	cout << "速度  ：" << pNpcs[nNpcIndex].Speed << " + "
		<< GetAddedSpeed(nNpcIndex) << endl << endl;
}
void Game::ShowGoodsName(short GoodsIndex) {
#ifdef _WIN32_//设置为绿色
	SetConsoleTextAttribute(hConsoleHandle, ITEM_NAME_COLOR | 0);
	cout << pGoods[GoodsIndex].Name;
#else
	printf(ITEM_NAME_STRING, pGoods[GoodsIndex].Name);
#endif
}
void Game::ShowGoodsDetail(short GoodsIndex) {
	if (GoodsIndex >= GoodsNum) {
		SystemWarning("物品索引超出范围，游戏将退出！");
		Running = false;
		return;
}
	ShowGoodsName(GoodsIndex);//输出物品名字
	cout << endl;
#ifdef _WIN32_//设置为绿色
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
	cout << pGoods[GoodsIndex].Desc << endl;
#else
	printf(DESCRIPTION_STRING, pGoods[GoodsIndex].Desc);//输出物品描述
	cout << endl;
#endif

	//物品的效果
	switch (pGoods[GoodsIndex].Use) {
	case 0:
		cout << "可以服用！";
		break;
	case 1:
		cout << "可以装备。";
		break;
	case 2:
		cout << "可以修习！";
		break;
	default:
		cout << "任务物品!";
	}

	if (pGoods[GoodsIndex].AddHP)
		cout << "生命 + " << pGoods[GoodsIndex].AddHP << "。\t";
	if (pGoods[GoodsIndex].AddMaxHP)
		cout << "生命上限 + " << pGoods[GoodsIndex].AddMaxHP << "。\t";
	if (pGoods[GoodsIndex].AddMP)
		cout << "内力 + " << pGoods[GoodsIndex].AddMP << "。\t";
	if (pGoods[GoodsIndex].AddMaxMP)
		cout << "内力上限 + " << pGoods[GoodsIndex].AddMaxMP << "。\t";
	if (pGoods[GoodsIndex].AddAttack)
		cout << "攻击 + " << pGoods[GoodsIndex].AddAttack << "。\t";
	if (pGoods[GoodsIndex].AddDefence)
		cout << "护体 + " << pGoods[GoodsIndex].AddDefence << "。\t";
	if (pGoods[GoodsIndex].AddSpeed)
		cout << "速度 + " << pGoods[GoodsIndex].AddSpeed << "。\t";
	if (pGoods[GoodsIndex].SkillIndex != -1) {
		cout << "记载有武功：";
		ShowSkillName(pGoods[GoodsIndex].SkillIndex);
		cout << "。";
	}

	cout << endl;
}
void Game::ShowSpotName(short nSpotIndex) {
#ifdef _WIN32_//设置为蓝色
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
#ifdef _WIN32_//设置为绿色
	SetConsoleTextAttribute(hConsoleHandle, DESCRIPTION_COLOR | 0);
	cout << pSpots[nSpotIndex].stScene << endl;
#else
	printf(DESCRIPTION_STRING, pSpots[nSpotIndex].Scene);//打印场景描述
	cout << endl;
#endif

	//显示该地点的人物
	if (pSpots[nSpotIndex].NpcNum) {
		cout << "这里的人物有：";
		for (i = 0; i < pSpots[nSpotIndex].NpcNum; ++i) {
			ShowNpcName(pSpots[nSpotIndex].Npcs[i]);
			cout << "\t";
		}
		cout << endl;
	}

	// 显示该地点的物品
	if (pSpots[nSpotIndex].GoodsNum) {
		cout << "这里有些可以拣取的东西：";
		for (i = 0; i < pSpots[nSpotIndex].GoodsNum; ++i) {
			ShowGoodsName(pSpots[nSpotIndex].Goods[i]);
			cout << "\t";
		}
		cout << endl;
	}

	//显示该地点的出口
	if (pSpots[nSpotIndex].ExitNum) {
		cout << "这里可以通往：";
		for (i = 0; i < pSpots[nSpotIndex].ExitNum; ++i) {
			ShowSpotName(pSpots[nSpotIndex].Exits[i]);
			cout << " ";
		}
		cout << endl;
	}
	//RunScripts( pSpots[nSpotIndex].nEnterScript );
}
void Game::ShowSkillName(short Skilllndex) {
#ifdef _WIN32_//设置为蓝色
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
	printf(DESCRIPTION_STRING, pSkills[Skilllndex].Desc);//技能描述
	cout << endl;
#endif
	cout << "每升一级增加威力" << pSkills[Skilllndex].Power << "。" << endl;
}
void Game::ShowQuests(bool bCompleted) {
	short i, n = 0;

	if (bCompleted) {
		cout << "已完成的任务：" << endl;

		for (i = 0; i < QuestsNum; ++i) {
			if (pQuests[i].Flag == 2) {//表示已完成
				n++;
#ifdef _WIN32_//设置为绿色
				SetConsoleTextAttribute(hConsoleHandle, QUEST_COLOR | 0);
				cout << "[" << n << "] ";
				cout << pQuests[i].Desc << endl;
#else
				cout << "[" << n << "]";
				printf(QUEST_STRING, pQuests[i].Desc);//任务描述
				cout << endl;
#endif
			}
		}
	}

	else {
		cout << "未完成的任务：" << endl;

		for (i = 0; i < QuestsNum; ++i) {
			if (pQuests[i].Flag == 1) {//1表示未完成
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

	if (!n)//如果没有任务打印无
		cout << "无。" << endl;

	SystemPause();//系统暂停 等用户输入
}
void Game::ShowEquipment(short nRoleIndex, short nLocation) {
	if (pNpcs[nRoleIndex].MyEquipment[nLocation] != -1)//显示人物某一位置的装备
		ShowGoodsName(pNpcs[nRoleIndex].MyEquipment[nLocation]);
	else
		cout << "无";
}
void Game::ReadSubString(char * stString, char * stSubString) {
	short nIndex = 0;//从一个大字符串读一个小的字符串
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
		if (pNpcs[nRoleIndex].MySkills[i].SkillIndex == Skilllndex)//判断人物是否有某种技能
			return true;
	}

	return false;
}
bool Game::AddSkill(short nRoleIndex, short Skilllndex) {
	if (Skilllndex < 0 || Skilllndex >= SkillsNum)//技能位置必须在规定范围内
		return false;

	if (pNpcs[nRoleIndex].SkillNum == MAX_SKILL_NUM) {
		ShowNpcName(nRoleIndex);
		SystemWarning("技能数达到上限，不能学习新的技能！");
		cout << endl;
		return false;
	}
	if (HaveSkill(nRoleIndex, Skilllndex)) {
		ShowNpcName(nRoleIndex);
		SystemWarning("已有这种技能！");
		cout << endl;
		return false;
	}
	short nSkillKind = pSkills[Skilllndex].SkillKind;
	if (pNpcs[nRoleIndex].Experience[nSkillKind] < pSkills[Skilllndex].NeedExp) {//判断人物经验是否达到标准
		ShowNpcName(nRoleIndex);
		SystemWarning("的修为还不能领悟这种武功！");
		cout << endl;
		return false;
	}
	//如果符合以上条件 便可以增加该项技能
	pNpcs[nRoleIndex].MySkills[pNpcs[nRoleIndex].SkillNum].SkillIndex = Skilllndex;
	pNpcs[nRoleIndex].MySkills[pNpcs[nRoleIndex].SkillNum].SkillLevel = 1;
	pNpcs[nRoleIndex].MySkills[pNpcs[nRoleIndex].SkillNum].Times = 0;
	pNpcs[nRoleIndex].SkillNum++;
	ShowNpcName(nRoleIndex);
	cout << "习得";
	ShowSkillName(Skilllndex);
	cout << "！" << endl;
	cout << "生命上限增加" << pSkills[Skilllndex].AddMaxHp<<endl<< "内力上限增加" << pSkills[Skilllndex].AddMaxMp << endl;
	pNpcs[nRoleIndex].MaxHP += pSkills[Skilllndex].AddMaxHp;
	pNpcs[nRoleIndex].MaxMP += pSkills[Skilllndex].AddMaxMp;
	SystemPause();
	return true;
}
void Game::AddGoodsToSpot(short nSpotIndex, short GoodsIndex) {
	if (pSpots[nSpotIndex].GoodsNum == MAX_GOODS_NUM) {//如果该地点物品达到上限、则去除最早的物品，并添加物品到该地点
		short i;
		for (i = 0; i < pSpots[nSpotIndex].GoodsNum - 1; ++i)
			pSpots[nSpotIndex].Goods[i] = pSpots[nSpotIndex].Goods[i + 1];
		pSpots[nSpotIndex].Goods[i] = GoodsIndex;
	}
	else {//否则则添加物品到该地点
		pSpots[nSpotIndex].Goods[pSpots[nSpotIndex].GoodsNum] = GoodsIndex;
		pSpots[nSpotIndex].GoodsNum++;
	}
}
bool Game::AddGoods(short nRoleIndex, short GoodsIndex, bool bVisible) {
	assert(GoodsIndex >= 0 && GoodsIndex <= GoodsNum);//判断该物品是否在范围内

	if (bVisible) {//如果要显示
		ShowNpcName(nRoleIndex);
		cout << "得到";
		ShowGoodsName(GoodsIndex);
		cout << "。" << endl;
		SystemPause();
	}

	if (pNpcs[nRoleIndex].GoodsNum == MAX_GOODS_NUM) {//物品已满，则该物品掉落
		ShowNpcName(nRoleIndex);
		cout << "的行囊已满，";
		ShowGoodsName(GoodsIndex);
		cout << "掉在了地上。" << endl;
		SystemPause();

		AddGoodsToSpot(oGameEnv.nCurrentMap, GoodsIndex);//将物品添加到该地点
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
		return i;//返回物品在人物中位置
}
void Game::RemoveGoodsFromSpot(short nSpotIndex, short nGoodsLocation) {
	short i;
	//将物品从该地点移除
	for (i = nGoodsLocation; i < pSpots[nSpotIndex].GoodsNum - 1; ++i)
		pSpots[nSpotIndex].Goods[i] = pSpots[nSpotIndex].Goods[i + 1];
	pSpots[nSpotIndex].GoodsNum--;
}
void Game::PickGoods(short nRoleIndex, short nGoodsLocation) {
	ShowNpcName(nRoleIndex);
	if (pNpcs[nRoleIndex].GoodsNum == MAX_GOODS_NUM)//行囊已满
		cout << "的行囊已满，放不下任何东西了。" << endl;
	else {//否则捡起
		cout << "拣起";
		ShowGoodsName(pSpots[oGameEnv.nCurrentMap].Goods[nGoodsLocation]);
		cout << "。" << endl;
		AddGoods(nRoleIndex, pSpots[oGameEnv.nCurrentMap].Goods[nGoodsLocation], false);
		RemoveGoodsFromSpot(oGameEnv.nCurrentMap, nGoodsLocation);
	}

	SystemPause();
}
bool Game::RemoveGoods(short nRoleIndex, short nGoodsLocation, bool bVisible) {
	if (nGoodsLocation < 0 || nGoodsLocation >= pNpcs[nRoleIndex].GoodsNum)
		return false;
	if (bVisible) {//是否显示
		ShowNpcName(nRoleIndex);
		cout << "失去";
		ShowGoodsName(pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
		cout << "。" << endl;
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
	cout << "卖掉了";
	ShowGoodsName(pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
	cout << "。" << endl;
	SystemPause();
	pNpcs[nRoleIndex].Money += pGoods[pNpcs[nRoleIndex].MyGoods[nGoodsLocation]].Cost / 2;//卖东西只能拿到一半银两
	RemoveGoods(nRoleIndex, nGoodsLocation, false);
}
void Game::BuyGoods(short nRoleIndex, short GoodsIndex) {
	if (pNpcs[nRoleIndex].Money > pGoods[GoodsIndex].Cost) {//如果钱够
		ShowNpcName(nRoleIndex);
		cout << "购买了";
		ShowGoodsName(GoodsIndex);
		cout << "。" << endl;
		AddGoods(nRoleIndex, GoodsIndex, false);
		pNpcs[nRoleIndex].Money -= pGoods[GoodsIndex].Cost;
	}
	else {//否则
		ShowNpcName(oGameEnv.nCurrentNpc);
		cout << ": 你的钱不够。" << endl;
	}

	SystemPause();
}
void Game::UseGoods(short nRoleIndex, short nGoodsLocation) {
	switch (pGoods[pNpcs[nRoleIndex].MyGoods[nGoodsLocation]].Use) {
	case 0:	//服用或使用
		EatOrUse(nRoleIndex, pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
		RemoveGoods(nRoleIndex, nGoodsLocation, false);
		break;
	case 1: //装备
		Equip(nRoleIndex, nGoodsLocation);
		break;
	case 2:  //学习
		LearnSkill(nRoleIndex, pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
		break;
	default:	//任务物品
		SystemWarning("这是任务物品！");
		cout << endl;
	}
}
void Game::EatOrUse(short nRoleIndex, short GoodsIndex) {
	ShowNpcName(nRoleIndex);//输出人物名字
	cout << "用了";
	ShowGoodsName(GoodsIndex);//输出物品名字
	cout << "。" << endl;
	SystemPause();
	//显示物品各种作用
	if (pGoods[GoodsIndex].AddMaxHP) {
		cout << "生命上限：" << pNpcs[nRoleIndex].MaxHP << "\t---->\t";
		pNpcs[nRoleIndex].MaxHP += pGoods[GoodsIndex].AddMaxHP;
		cout << pNpcs[nRoleIndex].MaxHP << endl;
	}
	if (pGoods[GoodsIndex].AddHP) {
		cout << "当前生命：" << pNpcs[nRoleIndex].HP << "\t---->\t";
		pNpcs[nRoleIndex].HP += pGoods[GoodsIndex].AddHP;
		if (pNpcs[nRoleIndex].HP > pNpcs[nRoleIndex].MaxHP)
			pNpcs[nRoleIndex].HP = pNpcs[nRoleIndex].MaxHP;
		cout << pNpcs[nRoleIndex].HP << endl;
	}

	if (pGoods[GoodsIndex].AddMaxMP) {
		cout << "内力上限：" << pNpcs[nRoleIndex].MaxMP << "\t---->\t";
		pNpcs[nRoleIndex].MaxMP += pGoods[GoodsIndex].AddMaxMP;
		cout << pNpcs[nRoleIndex].MaxMP << endl;
	}

	if (pGoods[GoodsIndex].AddMP) {
		cout << "当前内力：" << pNpcs[nRoleIndex].MP << "\t---->\t";
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
	cout << "卸除了";
	ShowGoodsName(nGoosIndex);
	cout << "。" << endl;
	SystemPause();

	AddGoods(nRoleIndex, nGoosIndex, false);//卸除的装备需添加到物品中
	pNpcs[nRoleIndex].MyEquipment[nLocation] = -1;
}
void Game::Equip(short nRoleIndex, short nGoodsLocation) {
	short oldAttack, oldDefend, oldSpeed;
	oldAttack = GetAddedAttack(nRoleIndex);
	oldDefend = GetAddedDefend(nRoleIndex);
	oldSpeed = GetAddedSpeed(nRoleIndex);

	short nIndex = pNpcs[nRoleIndex].MyGoods[nGoodsLocation];
	UnEquip(nRoleIndex, pGoods[nIndex].Location);//先卸除原有装备

	ShowNpcName(nRoleIndex);
	cout << "装备了";
	ShowGoodsName(pNpcs[nRoleIndex].MyGoods[nGoodsLocation]);
	cout << "。" << endl;
	SystemPause();

	pNpcs[nRoleIndex].MyEquipment[pGoods[nIndex].Location] = nIndex;//装备
	RemoveGoods(nRoleIndex, nGoodsLocation, false);//从物品中去除该装备
	//显示当前的人物属性
	cout << "攻击：" << pNpcs[nRoleIndex].Attack << " + " << oldAttack
		<< "\t---->\t" << pNpcs[nRoleIndex].Attack << " + "
		<< GetAddedAttack(nRoleIndex) << "\t("
		<< GetAddedAttack(nRoleIndex) - oldAttack << ")" << endl;

	cout << "护体：" << pNpcs[nRoleIndex].Defend << " + " << oldDefend
		<< "\t---->\t" << pNpcs[nRoleIndex].Defend << " + "
		<< GetAddedDefend(nRoleIndex) << "\t("
		<< GetAddedDefend(nRoleIndex) - oldDefend << ")" << endl;

	cout << "敏捷：" << pNpcs[nRoleIndex].Speed << " + " << oldSpeed
		<< "\t---->\t" << pNpcs[nRoleIndex].Speed << " + "
		<< GetAddedSpeed(nRoleIndex) << "\t("
		<< GetAddedSpeed(nRoleIndex) - oldSpeed << ")" << endl;

	SystemPause();
}
void Game::LearnSkill(short nRoleIndex, short GoodsIndex) {
	AddSkill(nRoleIndex, pGoods[GoodsIndex].SkillIndex);//增加技能
}
void Game::RandomDrop(short nRoleIdx, short nGoodsIdx, short nProb) {
	short nRan = rand() % 100;//随机掉落物品
	if (nRan <= nProb) {
		AddGoodsToSpot(oGameEnv.nCurrentMap, nGoodsIdx);
		ShowNpcName(nRoleIdx);
		cout << "掉落了";
		ShowGoodsName(nGoodsIdx);
		cout << endl;
		SystemPause();
	}
}
void Game::AddMoney(short nRoleIdx, short nMoney) {
	pNpcs[nRoleIdx].Money += nMoney;
	ShowNpcName(nRoleIdx);
	cout << "得到金钱" << nMoney << "文。" << endl;
	SystemPause();
}
bool Game::DelMoney(short nRoleIdx, short nMoney) {
	if (pNpcs[nRoleIdx].Money >= nMoney) {
		pNpcs[nRoleIdx].Money -= nMoney;
		ShowNpcName(nRoleIdx);
		cout << "失去金钱" << nMoney << "文。" << endl;
		SystemPause();
		return true;
	}
	else {
		return false;
	}
}
bool Game::AddNpc(short nSpotIndex, short nNpcIndex) {
	assert(nSpotIndex >= 0 && nSpotIndex < SpotsNum && nNpcIndex >= 0&& nNpcIndex < NpcsNum);

	if (pSpots[nSpotIndex].NpcNum == MAX_NPC_NUM) {//该地点人物数量达到上限
		cout << "该地图中npc已满，添加npc错误！" << endl;
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

	//找到这个npc是该地图中的第几个
	short nNpcLocation;
	for (nNpcLocation = 0; nNpcLocation < pSpots[nSpotIndex].NpcNum; ++nNpcLocation) {
		if (pSpots[nSpotIndex].Npcs[nNpcLocation] == nNpcIndex)
			break;
	}

	if (nNpcLocation == pSpots[nSpotIndex].NpcNum)//没有找到该人物
		return false;

	if (nNpcLocation < pSpots[nSpotIndex].NpcNum - 1) {//否则去除该人物
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
	for (i = 0; i < 4; ++i) {//返回装备增加的速度
		if (pNpcs[nRoleIndex].MyEquipment[i] != -1)
			nAdd += pGoods[pNpcs[nRoleIndex].MyEquipment[i]].AddSpeed;
	}
	return nAdd;
}
short Game::GetAddedAttack(short nRoleIndex) {
	short Add = 0;
	short i;
	for (i = 0; i < 4; ++i) {//返回装备增加的攻击力
		if (pNpcs[nRoleIndex].MyEquipment[i] != -1)
			Add += pGoods[pNpcs[nRoleIndex].MyEquipment[i]].AddAttack;
	}

	return Add;
}
short Game::GetAddedDefend(short nRoleIndex) {
	short Add = 0;
	short i;
	for (i = 0; i < 4; ++i) {//返回装备增加的防御力
		if (pNpcs[nRoleIndex].MyEquipment[i] != -1)
			Add += pGoods[pNpcs[nRoleIndex].MyEquipment[i]].AddDefence;
	}

	return Add;
}
short Game::SkillDamage(short nRoleIndex, short Skilllndex) {
	return (pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].Power
		* pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel);//计算技能伤害
}
short Game::SkillMpCost(short nRoleIndex, short Skilllndex) {
	return pSkills[pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex].MpCost
		* pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel;//计算技能消耗内力值
}
void Game::InitFight(short nNpcIndex) {
	oGameEnv.nCurrentEnemy = nNpcIndex;
	oGameEnv.nCurrentNpc = nNpcIndex;

	ActSlot[0].RoleID = 0;//小虾米
	ActSlot[0].ActSpeed = pNpcs[0].Speed + GetAddedSpeed(0);//计算速度
	ActSlot[1].RoleID = oGameEnv.nCurrentEnemy;
	ActSlot[1].ActSpeed = pNpcs[oGameEnv.nCurrentEnemy].Speed + GetAddedSpeed(oGameEnv.nCurrentEnemy);

	oGameEnv.bFighting = true;
	oGameEnv.State = fighting;//切换到战斗状态
}
void Game::EndFight(short nNpcIndex) {
	//显示消息
	if (nNpcIndex == 0){//如果你赢了
		cout << "你战胜了";
		ShowNpcName(oGameEnv.nCurrentEnemy);
		cout << "！" << endl;
		SystemPause();
		RunScripts(pNpcs[oGameEnv.nCurrentEnemy].FailScript);	//运行敌人失败的脚本
		Refresh(oGameEnv.nCurrentEnemy);//恢复敌人
		oGameEnv.bFighting = false;
	}
	else{
		cout << "你输给了";
		ShowNpcName(oGameEnv.nCurrentEnemy);
		cout << "！" << endl;
		SystemPause();
		oGameEnv.bFighting = false;
	}
	if (nNpcIndex == 0) {//如果小虾米赢了，可获得经验
		for (short i = 0; i < 4; i++) {
			short experience = GetExperience(nNpcIndex, oGameEnv.nCurrentEnemy);
			ShowNpcName(nNpcIndex);
			if(i==0)
			cout << "刀法得到" << experience << "点经验。" << endl;
			if (i == 1)
				cout << "剑法得到" << experience << "点经验。" << endl;
			if (i == 2)
				cout << "枪棍得到" << experience << "点经验。" << endl;
			if (i == 3)
				cout << "拳法得到" << experience << "点经验。" << endl;
			pNpcs[nNpcIndex].Experience[i] += experience;
		}
		SystemPause();
		if (oGameEnv.bFighting)
			oGameEnv.State = fighting;
		else
			oGameEnv.State = continue_game;
	}
	else {//如果小虾米输了，显示脚本，后退出
		RunScripts(pNpcs[0].FailScript);
		oGameEnv.State = system_menu;
	}
}
void Game::EscapeOut(short nRoleIndex, short nEnemyIndex) {
	short r = rand() % 100;
	if (pNpcs[nRoleIndex].Speed + GetAddedSpeed(nRoleIndex)
		< pNpcs[nEnemyIndex].Speed + GetAddedSpeed(nEnemyIndex)) {//判断谁更快
		if (r < 30) {
			ShowNpcName(nRoleIndex);
			cout << "逃离了战斗！" << endl;
			oGameEnv.State = continue_game;
			oGameEnv.bFighting = false;
		}
		else {
			ShowNpcName(nRoleIndex);
			cout << "逃跑失败！" << endl;
			oGameEnv.State = fighting;
		}
	}
	else {
		if (r < 90) {
			ShowNpcName(nRoleIndex);
			cout << "逃离了战斗！" << endl;
			oGameEnv.State = continue_game;
			oGameEnv.bFighting = false;
		}
		else {
			ShowNpcName(nRoleIndex);
			cout << "逃跑失败！" << endl;
			oGameEnv.State = fighting;
		}
	}
	SystemPause();
}
short Game::DecideWhoAct() {
	if (ActSlot[0].ActSpeed >= ActSlot[1].ActSpeed) {//判断谁更快返回行动的人物标号
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
	//判断使用哪一种技能
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
	if (attack > defend)//产生随机伤害值
		//return attack;
		return (attack - defend + rand() % 60);
	else
		return rand() % 60+20;
}
short Game::GetExperience(short nRoleIndex, short nEnemyIndex) {
	short experience;
	experience = rand() % 30+10;
	return experience;//返回随机经验增加
}
void Game::UpdateNick(short nRoleIdx) {
	/*	#       [称号]
		剑术 刀法 枪棍 拳掌[技能]
		新手 入门 新秀 专精 大师[称号]
		0 100 300 500 1000[经验]*/
	short MaxIdx = -1;
	short MaxExp = -1;
	for (short i = 0; i < SKILL_KIND; ++i) {
		if (pNpcs[nRoleIdx].Experience[i] > MaxExp) {
			MaxExp = pNpcs[nRoleIdx].Experience[i];
			MaxIdx = i;
		}
	}
	strcpy_s(pNpcs[nRoleIdx].Nick, SkillNicks[MaxIdx]);//将skillNicks内容追加到Nicks后面
	for (short i = SKILL_LVL - 1; i >= 0; --i) {
		if (MaxExp >= NickExp[i]) {
			strcat_s(pNpcs[nRoleIdx].Nick, LvlNicks[i]);//将LvlNicks内容追加到Nick后面
			break;
		}
	}
}
void Game::SkillLevelUp(short nRoleIndex, short Skilllndex) {
	//升级技能
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
	cout << "的";
	ShowSkillName(pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex);
	cout << "升到第" << pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel << "级！"<< endl;
	cout << "生命上限上升" << nHpAdded << "；" << endl;
	cout << "内力上限上升" << nMpAdded << "；" << endl;
	cout << "攻击上升" << nAtkAdded << "；" << endl;
	cout << "防御上升" << nDefAdded << "；" << endl;
	cout << "速度上升" << nSpeedAdded << "；" << endl;
	SystemPause();
}
void Game::UpdateSkillLevel(short nRoleIndex, short Skilllndex) {
	short global_skill_index = pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex;
	short local_skill_level = pNpcs[nRoleIndex].MySkills[Skilllndex].SkillLevel;

	short nExpAdded = local_skill_level * pSkills[global_skill_index].AddExp;
	pNpcs[nRoleIndex].Experience[pSkills[global_skill_index].SkillKind] += nExpAdded;

	cout << SkillNicks[pSkills[Skilllndex].SkillKind] << "修为增加" << nExpAdded
		<< endl;
	UpdateNick(nRoleIndex);//升级绰号

	if (local_skill_level == MAX_SKILL_LEVEL)
		return;
	if (pNpcs[nRoleIndex].MySkills[Skilllndex].Times 
		== local_skill_level * pSkills[global_skill_index].LevelUp) {//判断使用次数是否达到标准
		SkillLevelUp(nRoleIndex, Skilllndex);
	}
}
void Game::Act(short nRoleIndex, short Skilllndex, short nEnemyIndex) {
	short nDamage;
#ifdef _WIN32_
	SetConsoleTextAttribute(hConsoleHandle, SYSTEM_COLOR | 0);
#endif

	//普通攻击
	if (Skilllndex == -1) {
		nDamage = Damage(nRoleIndex, nEnemyIndex);
		ShowNpcName(nRoleIndex);
		cout << "攻击";
		ShowNpcName(nEnemyIndex);
		cout << "。 " << endl;
	}

	//使用技能攻击
	else {
		pNpcs[nRoleIndex].MP -= SkillMpCost(nRoleIndex, Skilllndex);
		nDamage = Damage(nRoleIndex, nEnemyIndex) + SkillDamage(nRoleIndex, Skilllndex);
		ShowNpcName(nRoleIndex);
		cout << "使用";
		ShowSkillName(pNpcs[nRoleIndex].MySkills[Skilllndex].SkillIndex);
		cout << "攻击";
		ShowNpcName(nEnemyIndex);
		cout << "。 " << endl;
		if (nRoleIndex == 0){
			pNpcs[nRoleIndex].MySkills[Skilllndex].Times++;//技能使用次数增加
			UpdateSkillLevel(nRoleIndex, Skilllndex);//更新技能等级
		}
	}

	ShowNpcName(nEnemyIndex);
	cout << "受到" << nDamage << "的伤害。";

	//攻击之后
	pNpcs[nEnemyIndex].HP -= nDamage;
	pNpcs[nEnemyIndex].HP =pNpcs[nEnemyIndex].HP > 0 ? pNpcs[nEnemyIndex].HP : 0;
	SystemPause();
	//结束战斗的是战胜者
	if (pNpcs[nEnemyIndex].HP == 0)
		EndFight(nRoleIndex);
	else if(pNpcs[nRoleIndex].HP==0)
		EndFight(nEnemyIndex);
	else {
		//RemoveNpc(oGameEnv.nCurrentMap, nEnemyIndex);
		oGameEnv.State = fighting;
	}
}
void Game::ExecuteScriptLine(char * stScriptLine) {//执行一条脚本语句
	IndexInCurLine = 0;
	char stCommand[32];
	ReadSubString(stScriptLine, stCommand);// 读取命令
	char stStringBuffer[256];
	char stNumberBuffer[4];
	if (!strcmp(stCommand, "RETURN")) {//返回
		RunningScripts = false;
	}
	//屏幕输出
	else if (!strcmp(stCommand, "PRINT")) {
		ReadSubString(stScriptLine, stStringBuffer);
		SystemOut(stStringBuffer);
		SystemPause();
	}
	//暂停
	else if (!strcmp(stCommand, "PAUSE"))
		SystemPause();
	//清除
	else if (!strcmp(stCommand, "CLEAR"))
		system("cls");
	//讲话
	else if (!strcmp(stCommand, "TALK")) {
		//读取NPC的标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNpcIndex = atoi(stNumberBuffer);
		//读取讲话内容
		ReadSubString(stScriptLine, stStringBuffer);
		//输出
		ShowNpcName(nNpcIndex);
		cout << ": ";
#ifdef _WIN32_//设置为白色
		short nRandomColor = 1 + rand() % 15;
		SetConsoleTextAttribute(hConsoleHandle, SYSTEM_HINT_COLOR | 0);
		cout << stStringBuffer << endl << endl;
#else
		printf(SYSTEM_HINT_STRING, stStringBuffer);
		cout << endl;
#endif
		SystemPause();
	}

	//测试某个任务值，如果等于测试值则脚本跳转到指定地方
	else if (!strcmp(stCommand, "QUEST_JUMP")) {
		//读取任务标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short QuestIndex = atoi(stNumberBuffer);

		//读取测试值
		ReadSubString(stScriptLine, stNumberBuffer);
		short nTestValue = atoi(stNumberBuffer);

		//如果测试成立则跳转
		if (pQuests[QuestIndex].Flag == nTestValue) {
			//读取跳转的脚本标号
			ReadSubString(stScriptLine, stNumberBuffer);
			short nScriptIndex = atoi(stNumberBuffer);
			FindScripts(nScriptIndex);//查找该脚本
			CurLineIndex--;
		}
	}
	//设置任务标志
	else if (!strcmp(stCommand, "SET_FLAG")) {
		//读取任务标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short QuestIndex = atoi(stNumberBuffer);
		//读取设定值
		ReadSubString(stScriptLine, stNumberBuffer);
		short nSetValue = atoi(stNumberBuffer);
		pQuests[QuestIndex].Flag = nSetValue;//设定的任务
	}

	//如果角色身上拥有指定数量的某道具
	else if (!strcmp(stCommand, "HAVE_JUMP")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nGoodsIdx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNumber = atoi(stNumberBuffer);
		if (NumOfGoods(RoleIDx, nGoodsIdx) >= nNumber) {//numuofgoods:拥有某物品的数量
			ReadSubString(stScriptLine, stNumberBuffer);
			short nScriptIndex = atoi(stNumberBuffer);
			FindScripts(nScriptIndex);//寻找脚本
			CurLineIndex--;
		}
	}

	//如果变量大于等于则跳转
	else if (!strcmp(stCommand, "MORE_JUMP")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short NpcIndex= atoi(stNumberBuffer);
		//short nVarIndex = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nTestValue = atoi(stNumberBuffer);
		if (pNpcs[NpcIndex].Money >= nTestValue) {
			ReadSubString(stScriptLine, stNumberBuffer);
			short nScriptIndex = atoi(stNumberBuffer);
			FindScripts(nScriptIndex);//寻找这个脚本
			CurLineIndex--;
		}
	}

	//如果变量等于则跳转
	else if (!strcmp(stCommand, "EQ_JUMP")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short nVarIndex = atoi(stNumberBuffer);//变量位置
		ReadSubString(stScriptLine, stNumberBuffer);
		short nTestValue = atoi(stNumberBuffer);
		if (Var[nVarIndex] == nTestValue) {
			ReadSubString(stScriptLine, stNumberBuffer);
			short nScriptIndex = atoi(stNumberBuffer);
			FindScripts(nScriptIndex);//寻找脚本
			CurLineIndex--;
		}
	}

	//设定变量的值
	else if (!strcmp(stCommand, "SET_VAR")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short nVarIndex = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nValue = atoi(stNumberBuffer);
		Var[nVarIndex] = nValue;
	}

	// YES/NO
	else if (!strcmp(stCommand, "YES_NO")) {
		SystemOut("1.是  2.否");
		short nSel = GetUserInput(1, 2);
		Var[0] = nSel;
	}
	else if (!strcmp(stCommand, "SELECT")) {
	//SystemOut("1.牛肉汤:30，2.炒饭:20，3.火锅:50，4.自助:20");
	short Sellect = GetUserInput(1, 4);
	Var[2] = Sellect;
	}

	//继续冒险
	else if (!strcmp(stCommand, "CONTINUE"))
		oGameEnv.State = continue_game;
	//进行交易
	else if (!strcmp(stCommand, "TRADE")) {
		oGameEnv.State = trading;
	}

	//战斗
	else if (!strcmp(stCommand, "FIGHT")) {
		//读取NPC标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNpcIndex = atoi(stNumberBuffer);

		//初始化战斗
		InitFight(nNpcIndex);
	}
	//增加物品
	else if (!strcmp(stCommand, "ADD_GOODS")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short GoodsIndex = atoi(stNumberBuffer);
		AddGoods(RoleIDx, GoodsIndex, true);
	}
	//去除物品
	else if (!strcmp(stCommand, "DEL_GOODS")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nGoodsIdx = atoi(stNumberBuffer);
		RemoveGoods(RoleIDx, HaveGoods(RoleIDx, nGoodsIdx), true);
	}
	//增加银两
	else if (!strcmp(stCommand, "ADD_MONEY")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nMoney = atoi(stNumberBuffer);
		AddMoney(RoleIDx, nMoney);
	}
	//减少银两
	else if (!strcmp(stCommand, "DEL_MONEY")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleIDx = atoi(stNumberBuffer);
		ReadSubString(stScriptLine, stNumberBuffer);
		short nMoney = atoi(stNumberBuffer);
		if (DelMoney(RoleIDx, nMoney))
			Var[1] = 0;  //成功
		else
			Var[1] = 1;  //失败
	}

	//向指定地图添加npc
	else if (!strcmp(stCommand, "ADD_NPC")) {
		//读取地图标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short nSpotIndex = atoi(stNumberBuffer);

		//读取npc标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNpcIndex = atoi(stNumberBuffer);

		//添加npc
		if (!AddNpc(nSpotIndex, nNpcIndex)) {
			SystemWarning("添加NPC失败，游戏将退出！");
			Running = false;
		}
	}

	//移除指定地图的npc
	else if (!strcmp(stCommand, "REMOVE_NPC")) {
		//读取地图标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short nSpotIndex = atoi(stNumberBuffer);

		//读取npc标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short nNpcIndex = atoi(stNumberBuffer);

		//移除npc
		if (!RemoveNpc(nSpotIndex, nNpcIndex)) {
			SystemWarning("删除NPC出错，游戏将退出！");
			Running = false;
		}
	}

	else if (!strcmp(stCommand, "RANDOM_DROP")) {//随机掉落物品
		// 读取物品标号
		ReadSubString(stScriptLine, stNumberBuffer);
		short GoodsIndex = atoi(stNumberBuffer);

		// 读取可能性大小
		ReadSubString(stScriptLine, stNumberBuffer);
		short nProb = atoi(stNumberBuffer);

		RandomDrop(oGameEnv.nCurrentNpc, GoodsIndex, nProb);
	}
	//恢复全部
	else if (!strcmp(stCommand, "REFRESH")) {
		ReadSubString(stScriptLine, stNumberBuffer);
		short RoleID = atoi(stNumberBuffer);
		Refresh(RoleID);
	}
	//恢复一点
	else if (!strcmp(stCommand, "REFRESHSOME")) {
	ReadSubString(stScriptLine, stNumberBuffer);
	short RoleID = atoi(stNumberBuffer);
	RefreshSome(RoleID);
	}
	//食用
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
	while (pScripts[CurLineIndex].ScriptIndex < nScriptsID) {//找到剧本
		CurLineIndex++;
	}
	return true;
}
void Game::RunScripts(short nScriptID) {
	if (!FindScripts(nScriptID))
		return;
	RunningScripts = true;
	while (RunningScripts) {
		ExecuteScriptLine(pScripts[CurLineIndex].ScriptLine);//一行一行运行剧本
		CurLineIndex++;
	}
}
void Game::Refresh(short nRoleIndex) {
	//恢复
	pNpcs[nRoleIndex].HP = pNpcs[nRoleIndex].MaxHP;
	pNpcs[nRoleIndex].MP = pNpcs[nRoleIndex].MaxMP;
}
void Game::RefreshSome(short nRoleIndex) {
	//恢复一点
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
	system("pause>nul");//系统暂停 等用户输入
	//system("pause");
	//cout << endl;
}
void Game::SystemOut(const char * stOutString) {
#ifdef _WIN32_//设为白色
	SetConsoleTextAttribute(hConsoleHandle, SYSTEM_COLOR | 0);
	cout << stOutString << endl << endl;
#else
	printf(SYSTEM_STRING, stOutString);
	cout << endl << endl;
#endif
}
void Game::SystemHint(const char * stHintString) {
	cout << endl;
#ifdef _WIN32_//设为白色
	SetConsoleTextAttribute(hConsoleHandle, SYSTEM_HINT_COLOR | 0);
	cout << stHintString << endl << endl;
#else
	printf(SYSTEM_HINT_STRING, stHintString);
	cout << endl << endl;
#endif
}
void Game::SystemWarning(const char * stWarningString) {
#ifdef _WIN32_//设为红色
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
	for (i = 0; i < pSpots[nSpotIndex].NpcNum; ++i) {//打印出所有人物
		cout << i + 1 << ".";
		ShowNpcName(pSpots[nSpotIndex].Npcs[i]);
		cout << endl;
	}
	cout << i + 1 << "." << "取消" << endl << endl;
}
void Game::PrintGoodsOfNpc(short nNpcIndex) {
	short i;
	//打印所有物品
	for (i = 0; i < pNpcs[nNpcIndex].GoodsNum; ++i) {
		cout << i + 1 << ".";
		ShowGoodsName(pNpcs[nNpcIndex].MyGoods[i]);
		cout << "\t";
		if (oGameEnv.State == buy_goods)
			cout << pGoods[pNpcs[nNpcIndex].MyGoods[i]].Cost << "文";
		else if (oGameEnv.State == sell_goods)
			cout << pGoods[pNpcs[nNpcIndex].MyGoods[i]].Cost / 2 << "文";
		cout << endl;
	}
	cout << i + 1 << ".取消" << endl << endl;
}
void Game::PrintLogo() {
	if (!ReadWholeTextFile(oResFiles.LogoFile, LogoBuffer))
		return;
	/*cout << LogoBuffer << endl;
	return;*/
#ifdef _WIN32_//设为篮色
	SetConsoleTextAttribute(hConsoleHandle, SPOT_NAME_COLOR | 0);//设计颜色
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
	#ifdef _WIN32_//设为绿色
		SetConsoleTextAttribute(hConsoleHandle, ITEM_NAME_COLOR | 0);//设计颜色
		cout << MapBuffer << endl << endl;
	#else
		printf(ITEM_NAME_STRING, MapBuffer);
		cout << endl << endl;
	#endif
}
void Game::PrintMenu() {
	cout << "1.新的征途 2.再战海大 3.归隐江湖" << endl;
}
void Game::PlayGame() {
	short input;
	if (!LoadResFileName("res_index.txt", '#'))//读取所有文件名，文件开始标识为#
		return;
	if (!LoadSkillsData(oResFiles.SkillFile, '#'))//下载技能文件
		return;
	if (!LoadGoodsData(oResFiles.GoodsFile, '#'))//下载物品文件
		return;
	if (!LoadQuestData(oResFiles.QuestFile, '#'))//下载任务文件
		return;
	if (!LoadScripts(oResFiles.ScriptFile, '#'))//下载剧本文件
		return;
	if (!ReadWholeTextFile(oResFiles.MapFile, MapBuffer))//读取地图
		return;
	if (!ReadWholeTextFile(oResFiles.LogoFile, LogoBuffer))//读取logo
		return;
	LoadNickData(oResFiles.NickFile, '#');//下载昵称
	Running = true;
	oGameEnv.State = system_menu;
	while (Running) {
		switch (oGameEnv.State) {
		case system_menu:
			system("cls");//清屏
			PrintLogo();//打印logo
			PrintMenu();//打印菜单
			input = GetUserInput(1, 3);
			if (input == 1) {
				SystemHint("读取玩家数据");
				if (!LoadPlayerData(true)) {
					Running = false;
				}
				cout << "你名叫";
				ShowNpcDetail(0);//显示小虾米细节
				SystemPause();
				oGameEnv.State = continue_game;
			}
			else if (input == 2) {
				SystemHint("读取玩家数据");
				if (!LoadPlayerData(false)) {
					Running = false;
				}
				oGameEnv.State = continue_game;
			}
			else if (input == 3)
					return;
			else
			SystemWarning("没有您选的那项，请重新选择！");

			break;
			
		case continue_game:
			system("cls");	//清屏
			ShowSpotDetail(oGameEnv.nCurrentMap);//显示当前地点细节
			ShowNpcState(0);
			SystemOut("1.交谈	2.观察	3.攻击	4.拾取	5.赶路	6.状态	7.系统	8.地图");
			input = GetUserInput(1, 8);
			//和人交谈
			if (input == 1) {
				if (pSpots[oGameEnv.nCurrentMap].NpcNum == 0)
					SystemWarning("这里没有人可以交谈！");
				else
					oGameEnv.State = talk_with_npc;
			}
			//观察别人
			else if (input == 2) {
				if (pSpots[oGameEnv.nCurrentMap].NpcNum == 0)
					SystemWarning("这里没有人可以观察！");
				else
					oGameEnv.State = watch_npc;
			}
			//攻击别人
			else if (input == 3) {
				if (pSpots[oGameEnv.nCurrentMap].NpcNum == 0)
					SystemWarning("这里没有人可以攻击。");
				else
					oGameEnv.State = attack_npc;
			}
			//拣东西
			else if (input == 4) {
				if (pSpots[oGameEnv.nCurrentMap].GoodsNum == 0)
					SystemWarning("这里没有可拣的东西。");
				else
					oGameEnv.State = list_free_item;
			}
			//赶路
			else if (input == 5) {
				if (pSpots[oGameEnv.nCurrentMap].LeaveScript >= 0) {
					RunScripts(pSpots[oGameEnv.nCurrentMap].LeaveScript);
					oGameEnv.State = continue_game;
				}
				else
					oGameEnv.State = show_exits;
			}
			//玩家察看状态
			else if (input == 6)
				oGameEnv.State = watch_state;
			//系统菜单
			else if (input == 7) {
				oGameEnv.State = playing_system_menu;
			}
			//查看地图
			else if (input == 8) {
				PrintMap();
				SystemPause();
				oGameEnv.State = continue_game;
			}
			else
			SystemWarning("没有您选的那项，请重新选择！");
			break;
			//玩家察看状态
		case watch_state:
			SystemHint("要察看什么？");
			SystemOut("1.属性	2.物品	3.装备	4.技能	5.任务	6.取消");
			input = GetUserInput(1, 6);
			//如果要察看属性
			if (input == 1) {
				ShowNpcDetailX(0);
				SystemPause();
				oGameEnv.State = watch_state;
			}
			//查看身上的物品
			else if (input == 2) {
				if (pNpcs[0].GoodsNum)
					oGameEnv.State = list_goods;
				else {
					SystemWarning("你身上没有任何物品.");
					oGameEnv.State = watch_state;
				}
			}
			//查看装备
			else if (input == 3)
				oGameEnv.State = check_equipment;
			//查看拥有的技能
			else if (input == 4) {
				if (pNpcs[0].SkillNum)
					oGameEnv.State = list_skills;
				else {
					SystemWarning("你还没学会任何技能。");
					oGameEnv.State = watch_state;
				}
			}
			//查看任务
			else if (input == 5)
				oGameEnv.State = list_quests;
			//取消
			else if(input==6)
				oGameEnv.State = continue_game;
			else
			{
				SystemWarning("没有您选的那项，将继续游戏！");
				oGameEnv.State = continue_game;
			}
			break;
			//选择移动的目的地
		case show_exits:
			SystemHint("要去哪里？");
			short i;
			//显示可以去的地方
			for (i = 0; i < pSpots[oGameEnv.nCurrentMap].ExitNum; ++i) {
				cout << (i + 1) << ".";
				ShowSpotName(pSpots[oGameEnv.nCurrentMap].Exits[i]);
				cout << endl;
			}
			cout << i + 1 << ".取消" << endl << endl;
			input = GetUserInput(1, i + 1);
			//取消
			if (input == i + 1)
				oGameEnv.State = continue_game;
			//转换到目的地
			else {
				oGameEnv.nCurrentMap = pSpots[oGameEnv.nCurrentMap].Exits[input - 1];
				RunScripts(pSpots[oGameEnv.nCurrentMap].EnterScript);
				oGameEnv.State = continue_game;
			}
			break;
			//和人交谈
		case talk_with_npc:
			SystemHint("要和谁交谈？");
			PrintNpcs(oGameEnv.nCurrentMap);
			input = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].NpcNum + 1);
			//取消
			if (input == pSpots[oGameEnv.nCurrentMap].NpcNum + 1)
				oGameEnv.State = continue_game;
			//与选中的人物对话
			else {
				oGameEnv.nCurrentNpc = pSpots[oGameEnv.nCurrentMap].Npcs[input - 1];
				RunScripts(pNpcs[oGameEnv.nCurrentNpc].TalkScript);
				if (oGameEnv.State == talk_with_npc)
					oGameEnv.State = continue_game;
			}
			break;
			//观察别人
		case watch_npc:
			SystemHint("要观察谁？");
			PrintNpcs(oGameEnv.nCurrentMap);
			input = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].NpcNum + 1);
			//取消
			if (input == pSpots[oGameEnv.nCurrentMap].NpcNum + 1)
				oGameEnv.State = continue_game;
			//显示对那人的描述
			else {
				//ShowNpcDetail(pSpots[oGameEnv.nCurrentMap].Npcs[input - 1]);
				ShowNpcDetailX(pSpots[oGameEnv.nCurrentMap].Npcs[input - 1]);
				SystemPause();
				oGameEnv.State = continue_game;
			}
			break;
			//攻击别人
		case attack_npc:
			SystemHint("要攻击谁？");
			PrintNpcs(oGameEnv.nCurrentMap);//显示当前地图所有npc
			input = GetUserInput(1, pSpots[oGameEnv.nCurrentMap].NpcNum + 1);
			//取消
			if (input == pSpots[oGameEnv.nCurrentMap].NpcNum + 1)
				oGameEnv.State = continue_game;
			//开战
			else {
				cout << "你与";
				ShowNpcName(pSpots[oGameEnv.nCurrentMap].Npcs[input - 1]);
				SystemWarning("开始战斗！");
				InitFight(pSpots[oGameEnv.nCurrentMap].Npcs[input - 1]);
				oGameEnv.State = fighting;
			}
			break;
			//战斗过程
		case fighting:
			short ActRole;
			short SelectedSkill;
			ActRole = DecideWhoAct();
			//如果是玩家行动
			if (ActRole == 0) {
				oGameEnv.State = player_act;
			}
			//如果是npc行动
			else {
				SelectedSkill = UseWhichSkill(ActRole);
				Act(ActRole, SelectedSkill, 0);
			}
			break;
			//玩家在战斗中的行动
		case player_act:
			SystemHint("轮到你行动了！");
			ShowNpcState(0);//显示生命值、内力
			//cout << endl;
			ShowNpcState(oGameEnv.nCurrentEnemy);
			SystemOut("1.攻击	2.技能	3.物品	4.逃走	");
			input = GetUserInput(1, 4);
			//选择攻击
			if (input == 1)
			{
				Act(0, -1, oGameEnv.nCurrentEnemy);//行动
				cout << endl;
			}
			//使用技能
			else if (input == 2) {
				if (pNpcs[0].SkillNum == 0) {
					SystemWarning("你还没学会任何技能。");
				}
				else
					oGameEnv.State = list_skills;
			}
			//使用物品
			else if (input == 3) {
				if (pNpcs[0].GoodsNum == 0) {
					SystemWarning("你身上没有任何物品！");
				}
				else
					oGameEnv.State = list_goods;
			}
			//逃跑
			else if (input == 4)
				EscapeOut(0, oGameEnv.nCurrentEnemy);
			//出错
			else
				SystemWarning("没有您选的那项，请重新选择！");
			break;
			//游戏过程中调出系统菜单
		case playing_system_menu:

			SystemHint("要做什么？");
			SystemOut("1.存储进度\n2.读取进度\n3.返回标题\n4.继续游戏");

			input = GetUserInput(1, 4);

			//如果是存储进度
			if (input == 1) {
				if (!SavePlayerData()) {
					SystemWarning("进度存储失败。");
					Running = false;
				}
				else {
					SystemHint("进度存储完毕。");
					SystemPause();
				}
				oGameEnv.State = playing_system_menu;
			}

			//读取进度
			else if (input == 2) {
				if (!LoadPlayerData(false)) {
					SystemWarning("进度读取失败！");
					Running = false;
				}
				else {
					SystemHint("进度读取成功！");
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
				SystemOut("继续游戏！");
				oGameEnv.State = continue_game;
				SystemPause();
			}
			break;

			//查看任务
		case list_quests:
			SystemHint("要查看哪类任务？");
			SystemOut("1.未完成任务		2.已完成任务	3.返回");
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
			//查看拥有物品
		case list_goods:
			SystemHint("要使用什么物品？");
			PrintGoodsOfNpc(0);//打印所有物品
			input = GetUserInput(1, pNpcs[0].GoodsNum + 1);
			//取消
			if (input == pNpcs[0].GoodsNum + 1) {
				if (oGameEnv.bFighting)
					oGameEnv.State = player_act;
				else
					oGameEnv.State = continue_game;
			}
			//准备使用选中的物品
			else {
				oGameEnv.nCurrentGoods = input - 1;
				oGameEnv.State = use_goods;
			}
			break;
			//使用物品
		case use_goods:
			ShowGoodsDetail(pNpcs[0].MyGoods[oGameEnv.nCurrentGoods]);
			SystemOut("1.使用	2.丢弃	3.返回");
			input = GetUserInput(1, 3);
			//使用物品
			if (input == 1) {
				UseGoods(0, oGameEnv.nCurrentGoods);
				if (oGameEnv.bFighting)
					oGameEnv.State = fighting;
				else
					oGameEnv.State = list_goods;
			}
			else if (input == 2) {
				ShowNpcName(0);
				cout << "扔掉了";
				ShowGoodsName(pNpcs[0].MyGoods[oGameEnv.nCurrentGoods]);
				cout << "。" << endl;
				SystemPause();
				AddGoodsToSpot(oGameEnv.nCurrentMap, pNpcs[0].MyGoods[oGameEnv.nCurrentGoods]);//将丢掉物品加入当前地点
				RemoveGoods(0, oGameEnv.nCurrentGoods, false);//去除小虾米身上的该物品
				oGameEnv.State = list_goods;
			}
			//取消
			else
				oGameEnv.State = list_goods;
			break;

			//查看拥有的技能
		case list_skills:
			SystemHint("要使用什么技能？");
			for (i = 0; i < pNpcs[0].SkillNum; ++i) {
				cout << i + 1 << ".";
				ShowSkillName(pNpcs[0].MySkills[i].SkillIndex);
				cout << "\t" << pNpcs[0].MySkills[i].SkillLevel << "级" << endl;
			}
			cout << i + 1 << ".取消" << endl << endl;
			input = GetUserInput(1, i + 1);
			//取消
			if (input == i + 1) {
				if (oGameEnv.bFighting)
					oGameEnv.State = player_act;
				else
					oGameEnv.State = continue_game;
			}
			//准备使用技能
			else {
				oGameEnv.nCurrentSkill = input - 1;
				oGameEnv.State = use_skill;
			}
			break;
			//使用技能
		case use_skill:
			//计算内力消耗
			short nMpCost;
			nMpCost = SkillMpCost(0, oGameEnv.nCurrentSkill);
			//显示
			ShowSkillDetail(pNpcs[0].MySkills[oGameEnv.nCurrentSkill].SkillIndex);//显示技能细节
			cout << "当前级别：" << pNpcs[0].MySkills[oGameEnv.nCurrentSkill].SkillLevel << '\t';
			cout << "耗费内力：" << nMpCost << '\t';
			cout << "附加伤害：" << SkillDamage(0, oGameEnv.nCurrentSkill) << endl;
			SystemOut("1.使用	2.返回");
			input = GetUserInput(1, 2);
			//使用技能
			if (input == 1) {
				if (oGameEnv.bFighting) {
					if (pNpcs[0].MP >= nMpCost) {
						Act(0, oGameEnv.nCurrentSkill, oGameEnv.nCurrentEnemy);
					}
					else {
						SystemWarning("内力不足。");
						oGameEnv.State = player_act;
					}
				}
				else {
					SystemWarning("只能在战斗时使用！");
					oGameEnv.State = list_skills;
				}
			}
			//取消
			else if (input == 2) {
				oGameEnv.State = list_skills;
			}
			break;
			//列出可以捡的东西
		case list_free_item:
			SystemHint("要捡起什么？");
			for (i = 0; i < pSpots[oGameEnv.nCurrentMap].GoodsNum; ++i) {
				cout << i + 1 << ".";
				ShowGoodsName(pSpots[oGameEnv.nCurrentMap].Goods[i]);
				cout << "\t";
			}
			cout << i + 1 << ".取消" << endl << endl;
			input = GetUserInput(1, i + 1);
			if (input == i + 1)
				oGameEnv.State = continue_game;
			else {
				oGameEnv.nCurrentGoods = input - 1;
				oGameEnv.State = pick_item;
			}
			break;
			//捡取物品
		case pick_item:
			ShowGoodsDetail(
				pSpots[oGameEnv.nCurrentMap].Goods[oGameEnv.nCurrentGoods]);
			SystemOut("1.捡取	2.取消");
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
			cout << "的装备情况：" << endl;

#ifdef _WIN32_//设置为绿色
			SetConsoleTextAttribute(hConsoleHandle, ITEM_NAME_COLOR | 0);
#endif
			cout << "1.头戴：";
			ShowEquipment(0, 0);
			cout << endl;
			cout << "2.身着：";
			ShowEquipment(0, 1);
			cout << endl;
			cout << "3.手持：";
			ShowEquipment(0, 2);
			cout << endl;
			cout << "4.脚踏：";
			ShowEquipment(0, 3);
			cout << endl;
			cout << "5.取消";
			cout << endl << endl;
			input = GetUserInput(1, 5);
			if (input == 5)
				oGameEnv.State = continue_game;
			else if (pNpcs[0].MyEquipment[input - 1] == -1)
				SystemWarning("这没什么好看的。");
			else {
				oGameEnv.nCurrentEquipment = input - 1;
				oGameEnv.State = unloading_equipment;
			}
			break;
		case unloading_equipment:
			ShowGoodsDetail(pNpcs[0].MyEquipment[oGameEnv.nCurrentEquipment]);
			SystemOut("1.卸除	2.返回");
			input = GetUserInput(1, 2);
			if (input == 1) {
				UnEquip(0, oGameEnv.nCurrentEquipment);
				oGameEnv.State = check_equipment;
			}
			else
				oGameEnv.State = continue_game;
			break;
		//交易
		case trading:
			ShowNpcName(oGameEnv.nCurrentNpc);
			cout << "：欢迎光临，看看有什么需要的吧！" << endl;
			SystemOut("1.购买	2.贩卖	3.取消");
			input = GetUserInput(1, 3);
			if (input == 1)
				oGameEnv.State = buy_goods;
			else if (input == 2)
				oGameEnv.State = sell_goods;
			else
				oGameEnv.State = continue_game;
			break;
		//买东西
		case buy_goods:
			SystemHint("要买些什么？");
			PrintGoodsOfNpc(oGameEnv.nCurrentNpc);//列出所有物品
			input = GetUserInput(1, pNpcs[oGameEnv.nCurrentNpc].GoodsNum + 1);
			if (input == pNpcs[oGameEnv.nCurrentNpc].GoodsNum + 1)
				oGameEnv.State = continue_game;
			else {
				BuyGoods(0, pNpcs[oGameEnv.nCurrentNpc].MyGoods[input - 1]);
				oGameEnv.State = buy_goods;
			}
			break;
		//卖东西
		case sell_goods:
			SystemHint("要卖什么？");
			PrintGoodsOfNpc(0);//列出所有物品
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