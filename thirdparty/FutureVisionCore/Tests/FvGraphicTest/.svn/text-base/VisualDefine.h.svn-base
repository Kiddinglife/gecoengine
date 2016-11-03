
#ifndef __VisualDefine_H__
#define  __VisualDefine_H__

#include <FvKernel.h>

class MoveType
{
public:
	enum Idx
	{
		WALKING,
		RUNING,
		SWINING,
	};
};

class ActDir
{
public:
	enum Idx
	{
		FRONT,
		BACK,
		LEFT,
		RIGHT,
		FRONT_LEFT,
		FRONT_RIGHT,
		BACK_LEFT,
		BACK_RIGHT,
		DIR_TOTAL,
	};
};


class Act
{
public:
	static bool GetMoveDirAct(const ActDir::Idx eDir, const MoveType::Idx eType, FvUInt32& idx)
	{
		if(eDir >= ActDir::DIR_TOTAL)
		{
			return false;
		}
		else
		{
			if(eType == MoveType::WALKING)
			{
				idx = Act::WALK_FRONT + eDir;
				return true;
			}
			else if(eType == MoveType::RUNING)
			{
				idx = Act::MOVE_FRONT + eDir;
				return true;
			}
			else if(eType == MoveType::SWINING)
			{
				idx = Act::SWIN_FRONT + eDir;
				return true;
			}
		}
		return false;
	}

	static FvUInt32 GetJumpDirAct(const ActDir::Idx eDir, const FvUInt32 idx)
	{
		if(eDir >= ActDir::DIR_TOTAL)
		{
			return idx;
		}
		else
		{
			return idx + (eDir + 1)*4;
		}
	}

	enum _Idx
	{
		IDEL = 100,
		IDEL_1,
		IDEL_2,

		MOVE_FRONT = 200,
		MOVE_BACK,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_FRONT_LEFT,
		MOVE_FRONT_RIGHT,
		MOVE_BACK_LEFT,
		MOVE_BACK_RIGHT,

		WALK_FRONT = 250,
		WALK_BACK,
		WALK_LEFT,
		WALK_RIGHT,
		WALK_FRONT_LEFT,
		WALK_FRONT_RIGHT,
		WALK_BACK_LEFT,
		WALK_BACK_RIGHT,

		TURN_LEFT = 300,
		TURN_RIGHT,

		SWIN = 400,
		SWIN_FRONT,
		SWIN_BACK,
		SWIN_LEFT,
		SWIN_RIGHT,
		SWIN_FRONT_LEFT,
		SWIN_FRONT_RIGHT,
		SWIN_BACK_LEFT,
		SWIN_BACK_RIGHT,

		SWING_TURN_LEFT = 500,
		SWING_TURN_RIGHT,

		JUMP_START = 600,
		JUMP_CONTINUE,
		JUMP_SECOND,
		JUMP_LAND,

		JUMP_START_FRONT,
		JUMP_CONTINUE_FRONT,
		JUMP_SECOND_FRONT,
		JUMP_LAND_FRONT,

		JUMP_START_BACK,
		JUMP_CONTINUE_BACK,
		JUMP_SECOND_BACK,
		JUMP_LAND__BACK,

		JUMP_START_LEFT,
		JUMP_CONTINUE_LEFT,
		JUMP_SECOND_LEFT,
		JUMP_LAND__LEFT,

		JUMP_START_RIGHT,
		JUMP_CONTINUE_RIGHT,
		JUMP_SECOND_RIGHT,
		JUMP_LAND__RIGHT,

		JUMP_START_FRONT_LEFT,
		JUMP_CONTINUE_FRONT_LEFT,
		JUMP_SECOND_FRONT_LEFT,
		JUMP_LAND__FRONT_LEFT,

		JUMP_START_FRONT_RIGHT,
		JUMP_CONTINUE_FRONT_RIGHT,
		JUMP_SECOND_FRONT_RIGHT,
		JUMP_LAND__FRONT_RIGHT,

		MOUNT = 700,

		FLY = 800,
		FLY_1,

		ATTACK= 1000,
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,

		ATTACK_HIT_0 = 1100,
		ATTACK_HIT_1,
		ATTACK_HIT_2,
		TRHOW = 1200,

		SPELL = 1500,

		AURA_STATE = 1600,
		HIT_EFFECT = 1700,

		DIE = 2000,
	};
};

class ActLayer
{
public:

	enum Idx
	{
		LAYER_DEFAULT,
		LAYER_ROTATE,
		LAYER_MOVE,
		LAYER_JUMP,
		LAYER_FLY,
		LAYER_STATE,
		LAYER_ACTION,
		LAYER_EFFECT,
		LAYER_LOCK,
		LAYER_TOTAL,
	};

};

class ExpressAttachType
{
public:


};


class MouseCursorShape
{
public:
	enum _Idx
	{
		NONE,
		TALK,
		HOSTILE,
		LOOT,
		NPC_START,
		NPC_END = NPC_START + 14
	};
};


class DisplayConfig
{
public:
	enum Idx
	{
		RESOLUTION	,//	分辨率	0:800*600,1:1024*768,2:1152*864,3:1280*1024,4:1280*960,5:1600*1200,6:1280*720,7:1280*768,8:1360*768,9:1440*900,10:1600*1050
		REFRESH	,//	刷新率	0:60HZ,1:70HZ,2:72HZ,3:75HZ,4:85HZ
		MANIFESTATION_MODE	,//	显示模式	0：全屏,1：窗口
		WINDOW_BIGGEST	,//	窗口最大化	
		PERPENDICULAR_SYNCHRONOUS	,//	垂直同步	
		BUFFER_3D	,//	三维缓冲	
		BRIGHT_DEGREE	,//	亮度	0-255
		APPEARANCE_STYLE	,//	画面风格	0:自然,1:柔和,2:鲜艳
		OVERALL_EXCELLENT_TURN	,//	全局优化	
		CAMERA_FOLLOW_BEHIND	,//	镜头跟随	
		MOUSE_TURN	,//	鼠标反转	
		CAMERA_TURN_SPEED	,//	镜头旋转速度	0-100
		CAMERA_DISTANCE	,//	设置相机距离	0.5M-50M
		OVERALL_ADJUST	,//	整体调整	0:低配置,1:中配置,2:高配置,3:完美配置
		SAW_TOOTH	,//	抗锯齿	0:2X,1:4X,2:8X
		PICTURE_ACCURACY	,//	贴图精度	0:高,1:中,2:低
		MATERIAL_ACCURACY	,//	材质精度	0:高,1:中,2:低
		MODEL_ACCURACY	,//	模型精度	0:高,1:中,2:低
		GROUND_ACCURACY	,//	地表精度	0:高,1:中,2:低
		GROUND_DETAIL	,//	地表细节	0:高,1:中,2:低
		GROUND_ANIMATION	,//	地表动画	
		GROUND_MIXTURE_EXAGGERATE	,//	透明混合渲染	
		ENVIRONMENT_EFFECT	,//	环境特效	0:高,1:中,2:低
		SPELL_EFFECT	,//	技能特效	0:高,1:中,2:低
		SHADOW_EFFECT	,//	阴影效果	0:高,1:中,2:低
		SURFACE_REFLECT	,//	水面反射	0:高,1:中,2:低
		SHOW_WEATHER_EFFECT	,//	天气效果	
		SHOW_TIME_ALTERNATION	,//	昼夜交替	
		SHOW_DEATH_EFFECT	,//	死亡效果	
		MUTE	,//	静音	
		BACKGROUND_MUSIC	,//	背景音乐	
		SCENE_SOUND	,//	场景音效	
		ROLE_SOUND	,//	角色音效	
		SOUND_3D	,//	3D音效	
		UI_SOUND_	,//	界面音效	
		MAIN_VOLUME	,//	主音量	0～100
		BACKGROUND_MUSIC_VOLUME	,//	背景音乐音量	0～100
		SCENE_SOUND_VOLUME	,//	场景音效音量	0～100
		ROLE_SOUND_VOLUME	,//	角色音效音量	0～100
		SOUND_3D_VOLUME	,//	3D音效音量	0～100
		UI_SOUND_VOLUME	,//	界面音效音量	0～100
		TOTAL,
	};
};
//
//class ActionBarType
//{
//public:
//	enum Type
//	{
//		NONE,
//		SPELL, 
//		ITEM,
//	};
//};
//

class AnimType
{
public:
	enum Type
	{
		NONE,
		MOVE_BREAK,
	};

};

class ClientAvatarRenderType
{
public:
	enum Idx
	{
		COLOR,
		BLEND,
		LIGHT,
	};
};


class AuraDiplayType
{
public:
	enum Idx
	{
		DEFAULT,
		HIDE,
	};
};


#endif //__VisualDefine_H__


