#include<string>
#include<iostream>
using namespace std;
#define MAX_SKILL_LEVEL	 5		//ÿ������5��
#define MAX_GOODS_NUM	5
#define MAX_SKILL_NUM	5
#define MAX_NPC_NUM		5
#define MAX_EXIT_NUM	4
#define VAR_NUM 32//var
#define SKILL_KIND 4//kind
#define SKILL_LVL 5

enum GAMState {
	system_menu,		//�µĹ��£��ɵĻ��䣬�⽨����������
	playing_system_menu,	//�洢���ȣ���ȡ���ȣ��뿪��Ϸ
	continue_game,		//������Ϸ����·����̸��������״̬����Ʒ������
	show_exits,			//�����ѡ��ȥ����
	talk_with_npc,		//�����ѡ���˭��̸
	attack_npc,			//�����ѡ�񹥻�˭
	watch_npc,			//�����ѡ��۲�˭
	watch_state,		//��Ҳ쿴״̬�����ܡ���Ʒ
	list_goods,			//�г���ӵ����Ʒ
	list_skills,		//�г���ӵ�еļ���
	list_quests,			//�鿴����
	use_goods,			//ʹ����Ʒ
	use_skill,			//ʹ�ü���
	fighting,			//ս����
	player_act,			//�ֵ�����ж�	
	list_free_item,		//�г�����ʰȡ����Ʒ
	pick_item,			//��ȡ��Ʒ
	check_equipment,	//���װ��
	unloading_equipment,			//ж��װ��
	trading,			//����
	buy_goods,			//����
	sell_goods			//������
};
struct Skill {
	char Name[32];//��������
	char Desc[256];//��������
	short Power;//����
	short Level;  // ���ܵȼ�
	short LevelUp;//ÿ���ȼ��ļ�����Ҫ��ϰ�Ĵ���
	//short NeedLev;//������Ҫ���ﵽ��ĵȼ�
	short NeedExp;//��Ҫ���پ�������������ּ���
	short SkillKind;//��������ǹ����ȭ
	short MpCost; //�ͷż�������ħ��ֵ
	short AddHp;//��������ֵ
	short AddMaxHp;//��������ֵ����
	short AddMaxMp;//��������ֵ����
	short AddMp;//��������ֵ
	short AddAttack;//���ӹ���
	short AddSpeed;//�����ٶ�
	short AddDefence;//���ӷ���
	short AddExp;//���Ӿ���
};
struct Quest {
	char Desc[128];	//��������
	short Flag;	//0-δ���ܣ�1-���ܵ�δ��ɣ�2-�Ѿ����
	short RoleIndex;   //ί����
	short MapIndex;    //ί�������ڵ�ͼ
};

struct Npc {
	char Name[32];//����
	char Nick[32];//�º�
	char Desc[256];//����
	short HP;//����ֵ
	short MaxHP;//��������
	short MaxMP;//��������
	short MP;//����
	short Attack;//������
	short Defend;//������
	short Speed;//�ٶ�
	short Money;//����
	short Experience[4];//��������ǹ��ȭ
	short TalkScript;//�������Ի�ʱ�Ľű�
	short FailScript;//������ʧ��ʱ�Ľű�
	struct HaveSkill {
		short SkillIndex;//���ܱ��
		short SkillLevel;//���ܵȼ�
		short Times;//ʹ�ü��ܵ�һ��������������������
	};
	short SkillNum;//��������
	HaveSkill MySkills[MAX_SKILL_NUM];
	short GoodsNum;
	short MyGoods[MAX_GOODS_NUM];
	short MyEquipment[4];// ����װ����λ�á�0 - ͷ��1 - ��2 - �֣�3 - ��
};
struct Spot {
	char Name[24];//�ص�����
	char Scene[256];//��������
	short NpcNum;//NPC����
	short Npcs[MAX_NPC_NUM];	//���5��
	short ExitNum;	//������Ŀ
	short Exits[MAX_EXIT_NUM];	//�����4������
	short GoodsNum;	//���Լ�Ķ���
	short Goods[MAX_GOODS_NUM];
	short EnterScript;		//����ʱ�����Ľű�
	short LeaveScript;		//�뿪ʱ�����Ľű�
};
struct Goods {
	char Name[32];	//��Ʒ����
	char Desc[256];	//��Ʒ����
	short Use;	//��Ʒ��;���ܳԻ�ֱ����(0)����װ��(1)�����߿���ѧϰ(2)��������Ʒ(3)
	short AddAttack;	//���ӹ���
	short AddDefence;	//���ӷ���
	short AddHP;		//��������ֵ
	short AddMaxHP;//������������
	short AddMP;//��������ֵ
	short AddMaxMP;//������������
	short AddSpeed;	//�����ٶ�
	short SkillIndex;	//�������Ӧ�ļ���
	short Location;	//����װ����λ�á�0-ͷ��1-��2-�֣�3-��
	short Cost;		//ֵ����Ǯ
};
struct ScriptLine {
	short ScriptIndex;//�ű�����λ��
	char ScriptLine[256];//����һ�о籾
};
class Game {
public:
	Game() {};
	~Game() {};
	void PlayGame();//��ʼ��Ϸ
	void PrintLogo();//��ӡlogo
	void PrintMap();//��ӡ��ͼ
	void PrintMenu();//��ӡ�˵�
	bool ReadWholeTextFile(char * stFileName, char * stBuffer);//��ȡ�����ı��ļ�
	bool LoadResFileName(const char* pstFileName, const char cHeadFlag);//��������Ϸ��ص��ļ���
	bool LoadSpotsData(const char * pstFileName, const char cHeadFlag);	//���صص��ı�
	bool LoadNpcsData(char * pstFileName, char cHeadFlag);	// ����npc�ı�
	bool LoadSkillsData(const char * pstFileName, const char cHeadFlag);//���ؼ����ı�
	bool LoadGoodsData(char * pstFileName, char cHeadFlag);	//������Ʒ�ı�
	bool LoadQuestData(char * pstFileName, char cHeadFlag);	//װ����������
	bool LoadScripts(char * pstFileName, char cHeadFlag);	// ���ؾ籾
	void LoadNickData(const char * pstFileName, const char cHeadFlag);//�����ǳ��ı�
	bool LoadPlayerData(bool bNewGame);//����npc�͵ص�����
	bool SavePlayerData();//��������
	short NumOfGoods(short RoleIDx, short nGoodsIdx);//������Ӧ������е���Ӧ��Ʒ����
	short GetUserInput();//��ȡ�û�����
	short GetUserInput(short nMin, short nMax);//�û�������Ҫ�����ֵ����Сֵ֮��
	void ShowNpcName(short nNpcIndex);//��ʾNpc����
	void ShowNpcState(short nNpcIndex);//��ʾ���������ֵ������
	void ShowNpcDetail(short nNpcIndex);//��ʾnpc���ֺ�����
	void ShowNpcDetailX(short nNpcIndex);//��ʾnpc����ϸ��
	void ShowGoodsName(short GoodsIndex);//��ʾ��Ʒ����
	void ShowGoodsDetail(short GoodsIndex);//��ʾ��Ʒϸ��
	void ShowSpotName(short nSpotIndex);//��ʾ��������
	void ShowSpotDetail(short nSpotIndex);//��ʾ����ϸ��
	void ShowSkillName(short Skilllndex);//��ʾ��������
	void ShowSkillDetail(short Skilllndex);//��ʾ����ϸ��
	void ShowQuests(bool bCompleted);//��ʾ����
	void ShowEquipment(short nRoleIndex, short nLocation);//��ʾװ��
	void Refresh(short nRoleIndex);//�ָ�����������
	void RefreshSome(short nRoleIndex);//�ָ�һ������������
	void ReadSubString(char * stString, char * stSubString);//��ȡһ��С���ӣ����ܰ����ո񡢻��з�
	short SkillDamage(short nRoleIndex, short Skilllndex);//ʹ�ü����˺�
	short SkillMpCost(short nRoleIndex, short Skilllndex);//ʹ�ü�����������
	void InitFight(short nNpcIndex);//��ʼ��ս��
	void EndFight(short nNpcIndex);//����ս��
	void EscapeOut(short nRoleIndex, short nEnemyIndex);//����
	short DecideWhoAct();//����ս��ʱ�ֵ�˭�ж�
	short UseWhichSkill(short nRoleIndex);//ѡ��npcҪʹ�õļ���
	short Damage(short nRoleIndex, short nEnemyIndex);//�����˺�
	short GetExperience(short nRoleIndex, short nEnemyIndex);//��þ���
	void UpdateNick(short nRoleIdx);//�����ǳ�
	void SkillLevelUp(short nRoleIndex, short Skilllndex);//��������
	void UpdateSkillLevel(short nRoleIndex, short Skilllndex);//���¼��ܵȼ�
	void Act(short nRoleIndex, short Skilllndex, short nEnemyIndex);//ս��
	void ExecuteScriptLine(char * stScriptLine);//ִ��һ�нű�
	bool FindScripts(short nScriptsID);//�ҽű�
	void RunScripts(short nScriptID);//�ܽű�
	void SystemPause();//ͣס��Ļ
	void SystemOut(const char * stOutString);//���ϵͳ��Ϣ
	void SystemHint(const char * stHintString);//�����ʾ��Ϣ
	void SystemWarning(const char * stWarningString);//���������Ϣ
	bool HaveSkill(short nRoleIndex, short Skilllndex);//�ж������Ƿ���иü���
	bool AddSkill(short nRoleIndex, short Skilllndex);//��������Ӽ���
	void AddGoodsToSpot(short nSpotIndex, short GoodsIndex);//����Ʒ�ŵ��õص�
	bool AddGoods(short nRoleIndex, short GoodsIndex, bool bVisible);//������Ʒ
	short HaveGoods(short nRoleIndex, short GoodsIndex);//���ظ���Ʒ��������Ʒ�е�λ��
	void RemoveGoodsFromSpot(short nSpotIndex, short nGoodsLocation);//�Ӹõص�ȥ������Ʒ
	void PickGoods(short nRoleIndex, short nGoodsLocation);//������Ʒ
	void SellGoods(short nRoleIndex, short nGoodsLocation);//����Ʒ
	void BuyGoods(short nRoleIndex, short GoodsIndex);//����Ʒ
	void UseGoods(short nRoleIndex, short nGoodsLocation);//ʹ����Ʒ
	void EatOrUse(short nRoleIndex, short GoodsIndex);//�Ի�ʹ����Ʒ
	void UnEquip(short nRoleIndex, short nLocation);//ж��װ��
	void Equip(short nRoleIndex, short nGoodsLocation);//װ��װ��
	void LearnSkill(short nRoleIndex, short GoodsIndex);//ѧϰ����
	bool RemoveGoods(short nRoleIndex, short nGoodsLocation, bool bVisible);
	void RandomDrop(short nRoleIdx, short nGoodsIdx, short nProb);//���������Ʒ
	void AddMoney(short nRoleIdx, short nMoney);//��������
	bool DelMoney(short nRoleIdx, short nMoney);//��������
	bool AddNpc(short nSpotIndex, short nNpcIndex);//����npc
	bool RemoveNpc(short nSpotIndex, short nNpcIndex);//ȥ��npc
	short GetAddedSpeed(short nRoleIndex);//����ٶ�����
	short GetAddedAttack(short nRoleIndex);//��ù�������
	short GetAddedDefend(short nRoleIndex);//��÷�������
	void PrintNpcs(short nSpotIndex);//��ӡnpc
	void PrintGoodsOfNpc(short nNpcIndex);//��ӡnpc�е���Ʒ
	struct ResFiles {//�ļ���
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
		short nCurrentMap;			//��ǰλ��
		short nCurrentNpc;			//��ǰnpc
		short nCurrentEnemy;		//��ǰ����
		short nCurrentSkill;		//��ǰ����
		short nCurrentGoods;		//��ǰ��Ʒ
		short nCurrentEquipment;	//��ǰװ��
		bool bFighting;				//�Ƿ���ս��
		GAMState State;			//game state

	} oGameEnv;
	struct CActSlot {
		short RoleID;
		short ActSpeed;
	} ActSlot[2];
private:
	char LogoBuffer[2048];//���logo
	char MapBuffer[1024];//��ŵ�ͼ
	short NpcsNum;//npc����
	short SkillsNum;//��������
	short GoodsNum;//��Ʒ����
	short QuestsNum;//��������
	short SpotsNum;//�ص�����
	short ScriptsNumber;//�ű�����
	short ScriptLineNumber;	//�ű�����
	short CurLineIndex;		//��ǰ�ڵڼ��нű�
	short IndexInCurLine;//�ڵ�ǰ�ű��ĵڼ���
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