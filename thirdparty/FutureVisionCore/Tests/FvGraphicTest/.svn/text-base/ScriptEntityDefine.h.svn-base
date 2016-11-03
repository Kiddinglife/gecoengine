
#ifndef __ScriptEntityDefine_H__
#define  __ScriptEntityDefine_H__


#include <FvGameUnitDefine.h>



class FvUnitNature
{
public:

	enum Idx
	{
		Human, //人型		
		Beast, //野兽		
		Plant, //植物		
		Insect, //虫类		
		Aves, //鸟类		
		Element, //元素		
		Soul, //亡灵		
		Dragon, //龙类		
		Demon, //恶魔	
		Titan, //巨人	
		Machine, //机械
	};
};

class Occupation
{
public:

	enum Idx
	{
		NONE,
		WARRIOR,//战士
		SNIPER,//弓手
		SEER,//先知
		FLAMEN,//祭司
		TOTAL,
	};
};

class Gender
{
public:
	enum Idx
	{
		NONE,
		MALE = 1,
		FEMALE = 2,
		MAX = FEMALE
	};
};

class TargetRaliation
{
public:
	enum _Idx
	{
		SELF,
		TARGET,
		FRIEND,
		ENEMY,
		NEUTRAL,
	};
};

class UnitRelation
{
public:
	enum Idx
	{
		FRIEND,
		ENEMY,
		NEUTRAL,
	};
};

class AttackMode
{
public:

	enum Idx
	{
		PEACE,//和平模式	
		FREE,//自由模式	
		JUSTICE,//正义模式	
		VILLAINY,//邪恶模式	
		TEAM_CONSIDER,
		//TEAM_IGNORE,
		//SAME_GUILD,
		LEAGUE_GUILD,
		//ANTAGONIZE_GUILD,
		TOTAL,
	};

};

class PKStateDefine
{
public:
	enum Idx
	{
		WHITE,//白名	
		GRAY,//灰名	
		DARK,//暗灰名	
		RED,//红名	
		TOTAL,
	};
};

class AttackRalation
{
public:

	enum Idx
	{
		REA_NAME,//红名	
		GRAY_NAME,//暗灰名	
		DARK_NAME,//灰名	
		WHITE_NAME,//白名	

		TEAM_MEMBER,//同组队成员	
		//NOT_TEAM_MEMBER,//非组队成员	

		GUILD_MEMBER,//同公会成员	
		LEAGUE_MEMBER,//同盟公会成员	
		ANTAGONIZE_MEMBER,//敌对公会成员	
		//其它公会成员	
		TOTAL,
	};
};




//+-----------------------------------------------------------------------------
class FvMoveState
{
public:
	enum _Mask
	{
		MOVING_FRONT = 0X01,
		MOVING_BACK = 0X02,
		MOVING_LEFT = 0X04,
		MOVING_RIGHT = 0X08,
		MOVING_DIR_MASK = MOVING_FRONT + MOVING_BACK + MOVING_LEFT +MOVING_RIGHT,
		MOVING_TO = 0X10,

		TURNING_LEFT = 0X100,
		TURNING_RIGHT = 0X200,	
		TURNING_DIR_MASK = TURNING_LEFT + TURNING_RIGHT,
		JUMPED = 0X800,

		MASK_MOVE_ALL = MOVING_DIR_MASK + MOVING_TO + TURNING_DIR_MASK + JUMPED,

		//! 移动方式
		WALKING = 0X1000,
		SWAMMING = 0X2000,
		FLAYING = 0X4000,
		MOUNTING = 0X8000,
		//!
		MOVE_MASK = 0X100 - 1,
	};
};

class ActionState
{
public:
	enum _Idx
	{
		AUTO_ATTACK,
		ATTACKING,
		DEAD,
		ATTACKED,
		SOUL,

		DIS_AUTO_ATTACK = 16,
		DIS_ATTACKING,
		DIS_DEAD,
		DIS_ATTACKED,
		DIS_MOVE,
		DIS_TURN,
		DIS_SWIN,
		DIS_MOUNT,
		DIS_FLY,  
		DIS_SAY,
		DIS_NPC_TALK,
		DIS_TRADE,//即时交易、拍卖、邮寄
	};
};

//+-----------------------------------------------------------------------------
//#define IS_UNIT32_UF(uiIdx) ((uiIdx) < UF_UINT32_TOTAL)
//#define IS_FLOAT_UF(uiIdx) ((uiIdx) < UF_FLOAT_SUP && (uiIdx) >= CONST_UF_FLOAT_INF)

//! 显示标记
class FvVisibility
{
public:
	enum _Mask
	{
		VISIBILITY_FRIEND = 0X01,
		VISIBILITY_ENEMY	= 0X02,

	};
};

class NPCFunction
{
public:
	enum Mask
	{
		GOSSIP					= 0X00000001, //	聊天
		QUESTGIVE			= 0X00000002, //	任务
		TRAINE					= 0X00000004, //	训练
		VENDOR					= 0X00000008, //	交易
		REPAIR					= 0X00000010, //	修理
		REFINE					= 0X00000020, //	合成
		PROFESSION			= 0X00000040, //	专业
		BATTLE					= 0X00000080, //	战斗
		BANKE					= 0X00000100, //	仓库
		TRANSPORT			= 0X00000200, //	传送
		AUCTIONE				= 0X00000400, //	拍卖
		PROXY_SELL			= 0X00000800, //	代售
		GUARD					= 0X00001000, //	卫兵
		PETITION				= 0X00002000, //	公会请求
		INNKEEPER				= 0X00004000, //	旅馆
		DOCTOR				= 0X00008000, //	医生
		MATCHMAKER			= 0X00010000, //	公告版
		POWWOW				= 0X00020000, //	祭祀
		SPIRITHEALER		= 0X00040000, //	灵魂医者
		SPIRITGUIDE			= 0X00080000, //	灵魂引导
		TABARDDESIGN		= 0X00100000, //	徽章设计
		MASTER_MANAGE	= 0X00200000, //	师徒管理
		GROUP_MANAGE		= 0X00400000, //	公会管理
		STRENGTHEN			= 0X00800000, //	强化

	};

	enum VendorMaskDerive
	{
		//! 最大支持15个
		VENDOR_MASK			= 0X0F000000, //	买卖掩码
		VENDOR_WEAPON		= 0X01000000, //	武器店
		VENDOR_ARMOR			= 0X02000000, //	防具店
		VENDOR_CLOTHES		= 0X03000000, //	衣服店
		VENDOR_GROCERY		= 0X04000000, //	杂货店
		VENDOR_FURNITURE	= 0X05000000, //	家具店
		VENDOR_COMPOUND	= 0X06000000, //	合成屋
		VENDOR_BLACKSMITH	= 0X07000000, //	铁匠铺
		VENDOR_WALK			= 0X08000000, //	行商人
		VENDOR_MAP				= 0X09000000, //	地图屋
	};
	enum RefineMaskDerive
	{
		//! 最大支持15个
		REFINER_MASK			= 0XF0000000, //	合成掩码
		REFINER_ENCHANT		= 0X10000000, //	雕金
		REFINER_SEW				= 0X20000000, //	裁縫
		REFINER_FORGE			= 0X30000000, //	锻冶
		REFINER_ALCHEMY		= 0X40000000, //	炼金
		REFINER_CRAFT			= 0X50000000, //	工艺
		REFINER_DECOMPOSE	= 0X60000000, //	分解
	};

};

class NPCQuestState
{
public:
	enum Mask
	{
		COMMIT		= 0x01,
		CNT_COMMIT	= 0x02,
		GIVE		= 0x04,
		CNT_GIVE	= 0x08,
		MAIN		= 0x10,
		COMMON		= 0x20,
		DAILY		= 0x40
	};
};

class NPCEliteLevel
{
public:
	enum Idx
	{
		NORMAL,
		RARE,
		ELITE,
		SMALL_BOSS,
		BIG_BOSS,		
		TOTAL,
	};
};



class Camp
{
public: 
	enum Idx
	{
		NEUTRAL,
		ATHENA,
		POSEIDON,
		HADES,
		MAX
	};
};



class AIType
{
public:
	enum Idx
	{
		NONE,
		PASSIVE,
		ACTIVE,
		TOTAL,
	};
};


class GameUnitActionType
{
public:
	enum Idx
	{
		NONE,
		FOLLOW,
		RANDOM_MOVE,
	};
};

class AIActionType
{
public:
	enum Idx
	{
		NONE,
		///<必备>
		PATROL,
		CHASE,
		CONTROLLED,///<被控制>
		///<扩展>
		FLEE,
		HOMING,
		TALK,
		
		
		TOTAL,
	};
};


#endif //__ScriptEntityDefine_H__


