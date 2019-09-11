#include<string>
#include<iostream>
using namespace std;
#define MAX_SKILL_LEVEL	 5		//每项技能最高5级
#define MAX_GOODS_NUM	5
#define MAX_SKILL_NUM	5
#define MAX_NPC_NUM		5
#define MAX_EXIT_NUM	4
#define VAR_NUM 32//var
#define SKILL_KIND 4//kind
#define SKILL_LVL 5

enum GAMState {
	system_menu,		//新的故事，旧的回忆，封建归隐，关于
	playing_system_menu,	//存储进度，读取进度，离开游戏
	continue_game,		//继续游戏：赶路，交谈，攻击，状态，物品，进度
	show_exits,			//让玩家选择去哪里
	talk_with_npc,		//让玩家选择和谁交谈
	attack_npc,			//让玩家选择攻击谁
	watch_npc,			//让玩家选择观察谁
	watch_state,		//玩家察看状态、技能、物品
	list_goods,			//列出所拥有物品
	list_skills,		//列出所拥有的技能
	list_quests,			//查看任务
	use_goods,			//使用物品
	use_skill,			//使用技能
	fighting,			//战斗中
	player_act,			//轮到玩家行动	
	list_free_item,		//列出可以拾取的物品
	pick_item,			//捡取物品
	check_equipment,	//检查装备
	unloading_equipment,			//卸除装备
	trading,			//交易
	buy_goods,			//买东西
	sell_goods			//卖东西
};
struct Skill {
	char Name[32];//技能名称
	char Desc[256];//技能描述
	short Power;//威力
	short Level;  // 技能等级
	short LevelUp;//每个等级的技能需要练习的次数
	//short NeedLev;//技能需要人物到达的等级
	short NeedExp;//需要多少经验才能领悟这种技能
	short SkillKind;//刀、剑、枪棍、拳
	short MpCost; //释放技能消耗魔法值
	short AddHp;//增加生命值
	short AddMaxHp;//增加生命值上限
	short AddMaxMp;//增加内力值上限
	short AddMp;//增加内力值
	short AddAttack;//增加攻击
	short AddSpeed;//增加速度
	short AddDefence;//增加防御
	short AddExp;//增加经验
};
struct Quest {
	char Desc[128];	//任务描述
	short Flag;	//0-未接受，1-接受但未完成，2-已经完成
	short RoleIndex;   //委托者
	short MapIndex;    //委托人所在地图
};

struct Npc {
	char Name[32];//姓名
	char Nick[32];//绰号
	char Desc[256];//描述
	short HP;//生命值
	short MaxHP;//生命上限
	short MaxMP;//内力上限
	short MP;//内力
	short Attack;//攻击力
	short Defend;//防御力
	short Speed;//速度
	short Money;//银两
	short Experience[4];//刀、剑、枪、拳
	short TalkScript;//与该人物对话时的脚本
	short FailScript;//该人物失败时的脚本
	struct HaveSkill {
		short SkillIndex;//技能编号
		short SkillLevel;//技能等级
		short Times;//使用技能到一定次数，可以升级技能
	};
	short SkillNum;//技能数量
	HaveSkill MySkills[MAX_SKILL_NUM];
	short GoodsNum;
	short MyGoods[MAX_GOODS_NUM];
	short MyEquipment[4];// 可以装备的位置。0 - 头，1 - 身，2 - 手，3 - 脚
};
struct Spot {
	char Name[24];//地点名字
	char Scene[256];//场景描述
	short NpcNum;//NPC数量
	short Npcs[MAX_NPC_NUM];	//最多5个
	short ExitNum;	//出口数目
	short Exits[MAX_EXIT_NUM];	//最多有4个出口
	short GoodsNum;	//可以捡的东西
	short Goods[MAX_GOODS_NUM];
	short EnterScript;		//进入时触发的脚本
	short LeaveScript;		//离开时触发的脚本
};
struct Goods {
	char Name[32];	//物品名称
	char Desc[256];	//物品描述
	short Use;	//物品用途：能吃或直接用(0)，能装备(1)，或者可以学习(2)，任务物品(3)
	short AddAttack;	//增加攻击
	short AddDefence;	//增加防御
	short AddHP;		//增加生命值
	short AddMaxHP;//增加生命上限
	short AddMP;//增加内力值
	short AddMaxMP;//增加内力上限
	short AddSpeed;	//增加速度
	short SkillIndex;	//技能书对应的技能
	short Location;	//可以装备的位置。0-头，1-身，2-手，3-脚
	short Cost;		//值多少钱
};
struct ScriptLine {
	short ScriptIndex;//脚本所在位置
	char ScriptLine[256];//包含一行剧本
};
class Game {
public:
	Game() {};
	~Game() {};
	void PlayGame();//开始游戏
	void PrintLogo();//打印logo
	void PrintMap();//打印地图
	void PrintMenu();//打印菜单
	bool ReadWholeTextFile(char * stFileName, char * stBuffer);//读取整个文本文件
	bool LoadResFileName(const char* pstFileName, const char cHeadFlag);//下载与游戏相关的文件名
	bool LoadSpotsData(const char * pstFileName, const char cHeadFlag);	//下载地点文本
	bool LoadNpcsData(char * pstFileName, char cHeadFlag);	// 下载npc文本
	bool LoadSkillsData(const char * pstFileName, const char cHeadFlag);//下载技能文本
	bool LoadGoodsData(char * pstFileName, char cHeadFlag);	//下载物品文本
	bool LoadQuestData(char * pstFileName, char cHeadFlag);	//装载任务数据
	bool LoadScripts(char * pstFileName, char cHeadFlag);	// 下载剧本
	void LoadNickData(const char * pstFileName, const char cHeadFlag);//下载昵称文本
	bool LoadPlayerData(bool bNewGame);//下载npc和地点数据
	bool SavePlayerData();//保存数据
	short NumOfGoods(short RoleIDx, short nGoodsIdx);//计算相应人物具有的相应物品数量
	short GetUserInput();//获取用户输入
	short GetUserInput(short nMin, short nMax);//用户输入需要在最大值与最小值之间
	void ShowNpcName(short nNpcIndex);//显示Npc姓名
	void ShowNpcState(short nNpcIndex);//显示人物的生命值和内力
	void ShowNpcDetail(short nNpcIndex);//显示npc名字和描述
	void ShowNpcDetailX(short nNpcIndex);//显示npc具体细节
	void ShowGoodsName(short GoodsIndex);//显示物品名字
	void ShowGoodsDetail(short GoodsIndex);//显示物品细节
	void ShowSpotName(short nSpotIndex);//显示场景名字
	void ShowSpotDetail(short nSpotIndex);//显示场景细节
	void ShowSkillName(short Skilllndex);//显示技能名字
	void ShowSkillDetail(short Skilllndex);//显示技能细节
	void ShowQuests(bool bCompleted);//显示人物
	void ShowEquipment(short nRoleIndex, short nLocation);//显示装备
	void Refresh(short nRoleIndex);//恢复生命、内力
	void RefreshSome(short nRoleIndex);//恢复一点生命和内力
	void ReadSubString(char * stString, char * stSubString);//读取一句小句子，不能包含空格、换行符
	short SkillDamage(short nRoleIndex, short Skilllndex);//使用技能伤害
	short SkillMpCost(short nRoleIndex, short Skilllndex);//使用技能消耗内力
	void InitFight(short nNpcIndex);//初始化战斗
	void EndFight(short nNpcIndex);//结束战斗
	void EscapeOut(short nRoleIndex, short nEnemyIndex);//逃跑
	short DecideWhoAct();//决定战斗时轮到谁行动
	short UseWhichSkill(short nRoleIndex);//选择npc要使用的技能
	short Damage(short nRoleIndex, short nEnemyIndex);//计算伤害
	short GetExperience(short nRoleIndex, short nEnemyIndex);//获得经验
	void UpdateNick(short nRoleIdx);//升级昵称
	void SkillLevelUp(short nRoleIndex, short Skilllndex);//升级技能
	void UpdateSkillLevel(short nRoleIndex, short Skilllndex);//更新技能等级
	void Act(short nRoleIndex, short Skilllndex, short nEnemyIndex);//战斗
	void ExecuteScriptLine(char * stScriptLine);//执行一行脚本
	bool FindScripts(short nScriptsID);//找脚本
	void RunScripts(short nScriptID);//跑脚本
	void SystemPause();//停住屏幕
	void SystemOut(const char * stOutString);//输出系统信息
	void SystemHint(const char * stHintString);//输出提示信息
	void SystemWarning(const char * stWarningString);//输出警告信息
	bool HaveSkill(short nRoleIndex, short Skilllndex);//判断人物是否具有该技能
	bool AddSkill(short nRoleIndex, short Skilllndex);//给人物添加技能
	void AddGoodsToSpot(short nSpotIndex, short GoodsIndex);//把物品放到该地点
	bool AddGoods(short nRoleIndex, short GoodsIndex, bool bVisible);//增加物品
	short HaveGoods(short nRoleIndex, short GoodsIndex);//返回该物品在人物物品中的位置
	void RemoveGoodsFromSpot(short nSpotIndex, short nGoodsLocation);//从该地点去除该物品
	void PickGoods(short nRoleIndex, short nGoodsLocation);//捡起物品
	void SellGoods(short nRoleIndex, short nGoodsLocation);//卖物品
	void BuyGoods(short nRoleIndex, short GoodsIndex);//买物品
	void UseGoods(short nRoleIndex, short nGoodsLocation);//使用物品
	void EatOrUse(short nRoleIndex, short GoodsIndex);//吃或使用物品
	void UnEquip(short nRoleIndex, short nLocation);//卸载装备
	void Equip(short nRoleIndex, short nGoodsLocation);//装上装备
	void LearnSkill(short nRoleIndex, short GoodsIndex);//学习技能
	bool RemoveGoods(short nRoleIndex, short nGoodsLocation, bool bVisible);
	void RandomDrop(short nRoleIdx, short nGoodsIdx, short nProb);//随机掉落物品
	void AddMoney(short nRoleIdx, short nMoney);//增加银两
	bool DelMoney(short nRoleIdx, short nMoney);//减少银两
	bool AddNpc(short nSpotIndex, short nNpcIndex);//增加npc
	bool RemoveNpc(short nSpotIndex, short nNpcIndex);//去除npc
	short GetAddedSpeed(short nRoleIndex);//获得速度增长
	short GetAddedAttack(short nRoleIndex);//获得攻击增长
	short GetAddedDefend(short nRoleIndex);//获得防御增长
	void PrintNpcs(short nSpotIndex);//打印npc
	void PrintGoodsOfNpc(short nNpcIndex);//打印npc有的物品
	struct ResFiles {//文件名
		char SkillFile[128];
		char GoodsFile[128];
		char RoleFile[128];
		char SpotFile[128];
		char QuestFile[128];
		char ScriptFile[128];
		char MapFile[128];
		char LogoFile[128];
		char NickFile[128];

	} oResFiles;
	struct CGameEnvironment {
		short nCurrentMap;			//当前位置
		short nCurrentNpc;			//当前npc
		short nCurrentEnemy;		//当前敌人
		short nCurrentSkill;		//当前技能
		short nCurrentGoods;		//当前物品
		short nCurrentEquipment;	//当前装备
		bool bFighting;				//是否在战斗
		GAMState State;			//game state

	} oGameEnv;
	struct CActSlot {
		short RoleID;
		short ActSpeed;
	} ActSlot[2];
private:
	char LogoBuffer[2048];//存放logo
	char MapBuffer[1024];//存放地图
	short NpcsNum;//npc数量
	short SkillsNum;//技能数量
	short GoodsNum;//物品数量
	short QuestsNum;//人物数量
	short SpotsNum;//地点数量
	short ScriptsNumber;//脚本数量
	short ScriptLineNumber;	//脚本行数
	short CurLineIndex;		//当前在第几行脚本
	short IndexInCurLine;//在当前脚本的第几行
	ScriptLine * pScripts;
	Npc * pNpcs;
	Spot * pSpots;
	Skill * pSkills;
	Goods * pGoods;
	Quest *pQuests;
	bool Running;
	bool RunningScripts;
	short Var[32];
	char SkillNicks[4][16];
	char LvlNicks[5][16];
	short NickExp[5];
};