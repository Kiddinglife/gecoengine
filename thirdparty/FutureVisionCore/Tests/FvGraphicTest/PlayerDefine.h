
#ifndef __PlayerDefine_H__
#define __PlayerDefine_H__

#include "ScriptEntityDefine.h"
#include "GameUnitDefine.h"
//#include "ScriptFvDataType.h"
#include <string>
#include <FvDataObj.h>
#include <CEGUIInputEvent.h>

class PlayerDefine
{
public:
	enum _SettingConfig
	{
		HOTKEY_MOVING_FRONT	,//	前进			移动按键
		HOTKEY_MOVING_BACK	,//	后退	
		HOTKEY_TURNING_LEFT	,//	左转	
		HOTKEY_TURNING_RIGHT	,//	右转	
		HOTKEY_MOVING_LEFT	,//	向左平移	
		HOTKEY_MOVING_RIGHT	,//	向右平移	
		HOTKEY_JUMPED	,//	跳跃	
		HOTKEY_SWITCH_RUN_WALK	,//	跑/走	
		HOTKEY_KEEPON_MOVE	,//	持续移动	
		HOTKEY_SITDOWN	,//	坐下	
		HOTKEY_ACTION_RESERVE_0	,//	预留	
		HOTKEY_ACTION_RESERVE_1	,//	预留	
		HOTKEY_ACTION_RESERVE_2	,//	预留	
		HOTKEY_ACTION_RESERVE_3	,//	预留	
		HOTKEY_ACTION_RESERVE_4	,//	预留	
		HOTKEY_ACTION_RESERVE_5	,//	预留	
		HOTKEY_ACTION_RESERVE_6	,//	预留	
		HOTKEY_ACTION_RESERVE_7	,//	预留	
		HOTKEY_ACTION_RESERVE_8	,//	预留	
		HOTKEY_ACTION_RESERVE_9	,//	预留	
		HOTKEY_PET_ACTIVE	,//	宠物主动攻击			宠物按键
		HOTKEY_PET_PASSIVE	,//	宠物被动攻击	
		HOTKEY_PET_FOLLOW	,//	宠物跟随	
		HOTKEY_PET_RESERVE_0	,//	预留	
		HOTKEY_PET_RESERVE_1	,//	预留	
		HOTKEY_PET_RESERVE_2	,//	预留	
		HOTKEY_PET_RESERVE_3	,//	预留	
		HOTKEY_PET_RESERVE_4	,//	预留	
		HOTKEY_PET_RESERVE_5	,//	预留	
		HOTKEY_PET_RESERVE_6	,//	预留	
		HOTKEY_PET_RESERVE_7	,//	预留	
		HOTKEY_PET_RESERVE_8	,//	预留	
		HOTKEY_PET_RESERVE_9	,//	预留	
		HOTKEY_DANCE	,//	跳舞				 表情按键
		HOTKEY_CRY	,//	大叫	
		HOTKEY_ANGRY	,//	生气	
		HOTKEY_KNEEL	,//	跪下	
		HOTKEY_WORSHIP	,//	朝拜	
		HOTKEY_ROLL	,//	翻跟头	
		HOTKEY_RAGE	,//	愤怒	
		HOTKEY_DREAD	,//	恐惧	
		HOTKEY_EMOTION_RESERVE_0	,//	预留	
		HOTKEY_EMOTION_RESERVE_1	,//	预留	
		HOTKEY_EMOTION_RESERVE_2	,//	预留	
		HOTKEY_EMOTION_RESERVE_3	,//	预留	
		HOTKEY_EMOTION_RESERVE_4	,//	预留	
		HOTKEY_EMOTION_RESERVE_5	,//	预留	
		HOTKEY_EMOTION_RESERVE_6	,//	预留	
		HOTKEY_EMOTION_RESERVE_7	,//	预留	
		HOTKEY_EMOTION_RESERVE_8	,//	预留	
		HOTKEY_EMOTION_RESERVE_9	,//	预留	
		HOTKEY_EMOTION_RESERVE_10	,//	预留	
		HOTKEY_EMOTION_RESERVE_11	,//	预留	
		HOTKEY_EMOTION_RESERVE_12	,//	预留	
		HOTKEY_EMOTION_RESERVE_13	,//	预留	
		HOTKEY_EMOTION_RESERVE_14	,//	预留	
		HOTKEY_EMOTION_RESERVE_15	,//	预留	
		HOTKEY_EMOTION_RESERVE_16	,//	预留	
		HOTKEY_EMOTION_RESERVE_17	,//	预留	
		HOTKEY_EMOTION_RESERVE_18	,//	预留	
		HOTKEY_EMOTION_RESERVE_19	,//	预留	
		HOTKEY_UI_MAIN	,//	打开/关闭主界面				界面按键
		HOTKEY_UI_CHARATAR	,//	打开/关闭角色界面	
		HOTKEY_UI_PET	,//	打开/关闭宠物界面	
		HOTKEY_UI_BAG	,//	打开/关闭道具界面	
		HOTKEY_UI_SKILL	,//	打开/关闭技能界面	
		HOTKEY_UI_RELIGION	,//	打开/关闭信仰界面	
		HOTKEY_UI_QUEST	,//	打开/关闭任务界面	
		HOTKEY_UI_HELP	,//	打开/关闭帮助界面	
		HOTKEY_UI_MAP	,//	打开/关闭地图界面	
		HOTKEY_UI_MARKET	,//	打开/关闭商城界面	
		HOTKEY_UI_PROCESSION	,//	打开/关闭团队界面	
		HOTKEY_UI_SOCIALITY	,//	打开/关闭社交界面	
		HOTKEY_UI_GUIILD	,//	打开/关闭公会界面	
		HOTKEY_UI_RANKING	,//	打开/关闭排名界面	
		HOTKEY_UI_SYSTEM	,//	打开/关闭系统菜单	
		HOTKEY_UI_CHAT	,//	打开/关闭聊天界面	
		HOTKEY_UI_RESERVE_0	,//	预留	
		HOTKEY_UI_RESERVE_1	,//	预留	
		HOTKEY_UI_RESERVE_2	,//	预留	
		HOTKEY_UI_RESERVE_3	,//	预留	
		HOTKEY_UI_RESERVE_4	,//	预留	
		HOTKEY_UI_RESERVE_5	,//	预留	
		HOTKEY_UI_RESERVE_6	,//	预留	
		HOTKEY_UI_RESERVE_7	,//	预留	
		HOTKEY_UI_RESERVE_8	,//	预留	
		HOTKEY_UI_RESERVE_9	,//	预留	
		HOTKEY_PRINTSCREEN	,//	截图键	
		HOTKEY_MUSIC	,//	打开/关闭背景音乐			声音按键
		HOTKEY_SOUND	,//	打开/关闭游戏音效	
		HOTKEY_VOLUME_UP	,//	提高主音量	
		HOTKEY_VOLUME_DOWN	,//	降低主音量	
		HOTKEY_SOUND_RESERVE_0	,//	预留	
		HOTKEY_SOUND_RESERVE_1	,//	预留	
		HOTKEY_SOUND_RESERVE_2	,//	预留	
		HOTKEY_SOUND_RESERVE_3	,//	预留	
		HOTKEY_SOUND_RESERVE_4	,//	预留	
		HOTKEY_SOUND_RESERVE_5	,//	预留	
		HOTKEY_SOUND_RESERVE_6	,//	预留	
		HOTKEY_SOUND_RESERVE_7	,//	预留	
		HOTKEY_SOUND_RESERVE_8	,//	预留	
		HOTKEY_SOUND_RESERVE_9	,//	预留	
		HOTKEY_SHORTCUT_0	,//	快捷键1				快捷按键
		HOTKEY_SHORTCUT_1	,//	快捷键2	
		HOTKEY_SHORTCUT_2	,//	快捷键3	
		HOTKEY_SHORTCUT_3	,//	快捷键4	
		HOTKEY_SHORTCUT_4	,//	快捷键5	
		HOTKEY_SHORTCUT_5	,//	快捷键6	
		HOTKEY_SHORTCUT_6	,//	快捷键7	
		HOTKEY_SHORTCUT_7	,//	快捷键8	
		HOTKEY_SHORTCUT_8	,//	快捷键9	
		HOTKEY_SHORTCUT_9	,//	快捷键10	
		HOTKEY_SHORTCUT_10	,//	快捷键11	
		HOTKEY_SHORTCUT_11	,//	快捷键12	
		HOTKEY_SHORTCUT_12	,//	快捷键13	
		HOTKEY_SHORTCUT_13	,//	快捷键14	
		HOTKEY_SHORTCUT_14	,//	快捷键15	
		HOTKEY_SHORTCUT_15	,//	快捷键16	
		HOTKEY_SHORTCUT_16	,//	快捷键17	
		HOTKEY_SHORTCUT_17	,//	快捷键18	
		HOTKEY_SHORTCUT_18	,//	快捷键19	
		HOTKEY_SHORTCUT_19	,//	快捷键20	
		HOTKEY_SHORTCUT_20	,//	快捷键21	
		HOTKEY_SHORTCUT_21	,//	快捷键22	
		HOTKEY_SHORTCUT_22	,//	快捷键23	
		HOTKEY_SHORTCUT_23	,//	快捷键24	
		HOTKEY_SHORTCUT_24	,//	快捷键25	
		HOTKEY_SHORTCUT_25	,//	快捷键26	
		HOTKEY_SHORTCUT_26	,//	快捷键27	
		HOTKEY_SHORTCUT_27	,//	快捷键28	
		HOTKEY_SHORTCUT_28	,//	快捷键29	
		HOTKEY_SHORTCUT_29	,//	快捷键30	
		HOTKEY_SHORTCUT_30	,//	快捷键31	
		HOTKEY_SHORTCUT_31	,//	快捷键32	
		HOTKEY_SHORTCUT_32	,//	快捷键33	
		HOTKEY_SHORTCUT_33	,//	快捷键34	
		HOTKEY_SHORTCUT_34	,//	快捷键35	
		HOTKEY_SHORTCUT_35	,//	快捷键36	
		HOTKEY_SHORTCUT_36	,//	快捷键37	
		HOTKEY_SHORTCUT_37	,//	快捷键38	
		HOTKEY_SHORTCUT_38	,//	快捷键39	
		HOTKEY_SHORTCUT_39	,//	快捷键40	
		HOTKEY_SHORTCUT_40	,//	快捷键41	
		HOTKEY_SHORTCUT_41	,//	快捷键42	
		HOTKEY_SHORTCUT_42	,//	快捷键43	
		HOTKEY_SHORTCUT_43	,//	快捷键44	
		HOTKEY_SHORTCUT_44	,//	快捷键45	
		HOTKEY_SHORTCUT_45	,//	快捷键46	
		HOTKEY_SHORTCUT_46	,//	快捷键47	
		HOTKEY_SHORTCUT_47	,//	快捷键48	
		HOTKEY_SHORTCUT_48	,//	快捷键49	
		HOTKEY_SHORTCUT_49	,//	快捷键50	
		HOTKEY_SHORTCUT_50	,//	快捷键51	
		HOTKEY_SHORTCUT_51	,//	快捷键52	
		HOTKEY_SHORTCUT_52	,//	快捷键53	
		HOTKEY_SHORTCUT_53	,//	快捷键54	
		HOTKEY_SHORTCUT_54	,//	快捷键55	
		HOTKEY_SHORTCUT_55	,//	快捷键56	
		HOTKEY_SHORTCUT_56	,//	快捷键57	
		HOTKEY_SHORTCUT_57	,//	快捷键58	
		HOTKEY_SHORTCUT_58	,//	快捷键59	
		HOTKEY_SHORTCUT_59	,//	快捷键60	
		HOTKEY_SHORTCUT_60	,//	快捷键61	
		HOTKEY_SHORTCUT_61	,//	快捷键62	
		HOTKEY_SHORTCUT_62	,//	快捷键63	
		HOTKEY_SHORTCUT_63	,//	快捷键64	
		HOTKEY_SHORTCUT_RESERVE_0	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_1	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_2	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_3	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_4	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_5	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_6	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_7	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_8	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_9	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_10	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_11	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_12	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_13	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_14	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_15	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_16	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_17	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_18	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_19	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_20	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_21	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_22	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_23	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_24	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_25	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_26	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_27	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_28	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_29	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_30	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_31	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_32	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_33	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_34	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_35	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_36	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_37	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_38	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_39	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_40	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_41	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_42	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_43	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_44	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_45	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_46	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_47	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_48	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_49	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_50	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_51	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_52	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_53	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_54	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_55	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_56	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_57	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_58	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_59	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_60	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_61	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_62	,//	预留	
		HOTKEY_SHORTCUT_RESERVE_63	,//	预留	
		HOTKEY_ENTER	,//	激活聊天输入栏				聊天按键
		HOTKEY_RESPOND	,//	快速回复密语玩家	
		HOTKEY_INPUT_TEXT_PAGE_UP	,//	最近输入文本上翻	
		HOTKEY_INPUT_TEXT_PAGE_DOWN	,//	最近输入文本下翻	
		HOTKEY_LOG_PAGE_UP	,//	聊天记录向上翻页	
		HOTKEY_LOG_PAGE_DOWN	,//	聊天记录向下翻页	
		HOTKEY_LOG_PAGE_LAST	,//	聊天记录翻至最下端	
		HOTKEY_FIGHT_LOG	,//	打开/关闭战斗日志	
		HOTKEY_FIGHT_LOG_PAGE_UP	,//	战斗日志向上翻页	
		HOTKEY_FIGHT_LOG_PAGE_DOWN	,//	战斗日志向下翻页	
		HOTKEY_FIGHT_LOG_PAGE_LAST	,//	战斗日志翻至最下端	
		HOTKEY_SELECT_CLOSEST_ENEMY	,//	选择最近的敌人				目标按键
		HOTKEY_SELECT_CLOSEST_FRIEND	,//	选择最近的友军	
		HOTKEY_SELECT_TARGET_TARGET	,//	选择目标的目标	
		HOTKEY_SELECT_CLOSEST_ENEMY_PLAYER	,//	选择最近的敌对玩家	
		HOTKEY_SELECT_PREVIOUS_ENEMY_PLAYER	,//	选择上一个敌对玩家	
		HOTKEY_SELECT_CLOSEST_FRIEND_PLAYER	,//	选择最近的友方玩家	
		HOTKEY_SELECT_PREVIOUS_FRIEND_PLAYER	,//	选择上一个友方玩家	
		HOTKEY_SELECT_MEMBER_SELF	,//	选择自己	
		HOTKEY_SELECT_MEMBER_FIRST	,//	选择第一个队友	
		HOTKEY_SELECT_MEMBER_SECOND	,//	选择第二个队友	
		HOTKEY_SELECT_MEMBER_THIRD	,//	选择第三个队友	
		HOTKEY_SELECT_MEMBER_LAST	,//	选择第四个队友	
		HOTKEY_SELECT_PET_SELF	,//	选择自己宠物	
		HOTKEY_SELECT_PET_FIRST	,//	选择第一队友宠物	
		HOTKEY_SELECT_PET_SECOND	,//	选择第二队友宠物	
		HOTKEY_SELECT_PET_THIRD	,//	选择第三队友宠物	
		HOTKEY_SELECT_PET_LAST	,//	选择第四队友宠物	
		HOTKEY_SHOW_PLAYER	,//	显示/隐藏其他玩家	
		HOTKEY_ZOOM_IN	,//	缩进视角					视角按键
		HOTKEY_ZOOM_OUT	,//	拉远视角	
		SET_UI_ZOOM	,//	界面缩放	70-120
		SET_AUTO_PICK_UP	,//	自动拾取	
		SET_HIDE_CHAT_FRAME	,//	自动隐藏聊天输入框	
		SET_SHOW_LOGGING_TIP	,//	自动显示上线提示	
		SET_SHOW_NEWBIE_HELP	,//	显示新手帮助	
		SET_MOUSE_WALK	,//	鼠标走路	
		SET_SHOW_QUEST_FOLLOW	,//	启用任务跟踪	
		SET_SHOW_HP_SELF	,//	显示自己血条	
		SET_SHOW_HP_OTHER_PLAYER	,//	显示其他玩家血条	
		SET_SHOW_HP_NPC	,//	显示NPC血条	
		SET_SHOW_TITLE_SELF	,//	显示其他玩家称号	
		SET_SHOW_TITLE_OTHER_PLAYER	,//	显示NPC称号	
		SET_SHOW_TITLE_NPC	,//	显示自己称号	
		SET_SHOW_NAME_SELF	,//	显示自己名字	
		SET_SHOW_NAME_OTHER_PLAYER	,//	显示其他玩家名字	
		SET_SHOW_NAME_NPC	,//	显示NPC名字	
		SET_SHOW_PET_NAME_SELF	,//	显示自己宠物名字	
		SET_SHOW_PET_NAME_OTHER_PLAYER	,//	显示其他玩家宠物名字	
		SET_SHOW_GUILD_NAME_SELF	,//	显示自己公会名	
		SET_SHOW_GUILD_NAME_OTHER_PLAYER	,//	显示他人公会名	
		SET_SHOW_DIALOGUE_PLAYER	,//	显示玩家对话泡泡	
		SET_SHOW_DIALOGUE_NPC	,//	显示NPC对话泡泡	
		SET_SHOW_LOWER_LV_QUEST	,//	显示低等级任务	
		SET_HIDE_HELM	,//	隐藏帽子	
		SET_HIDE_MANTLE	,//	隐藏披风	
		SET_SHOW_AREA_MAP	,//	显示区域地图	
		SET_SHOW_TEAMMATE_QUEST_PROGRESS	,//	显示队友任务状况	
		SET_SHOW_BAG_SPACE	,//	显示背包空间	
		SET_SHOW_MAIN_SHORTCUTBAR	,//	打开主快捷栏	
		SET_SHOW_MAIN_SHORTCUTBAR_BACKGROUND	,//	显示主快捷栏背景	
		SET_SHOW_SUB_SHORTCUTBAR	,//	打开副快捷栏	
		SET_SHOW_SUB_SHORTCUTBAR_BACKGROUND	,//	显示副快捷栏背景1	
		SET_SHOW_USER1_SHORTCUTBAR	,//	打开自定义快捷栏1	
		SET_SHOW_USER1_SHORTCUTBAR_BACKGROUND	,//	显示快捷栏背景1	
		SET_SHOW_USER2_SHORTCUTBAR	,//	打开自定义快捷栏2	
		SET_SHOW_USER2_SHORTCUTBAR_BACKGROUND	,//	显示快捷栏背景2	
		SET_SHOW_SELF	,//	显示自己	
		SET_SHOW_TARGET	,//	显示目标	
		SET_SHOW_TARGET_TARGET	,//	显示目标的目标	
		SET_SHOW_PET_SELF	,//	显示自己宠物	
		SET_SHOW_PET_TEAMMATE	,//	显示队友宠物	
		SET_SHOW_PROCESSION	,//	小队	
		SET_SHOW_EXPBAR	,//	经验条数值	
		SET_SHOW_GROUP	,//	团队	
		SET_PERCENTAGE_SHOW_VALUE	,//	百分比显示	
		SET_SHOW_FIGHT_LOG_SELF	,//	开启自身战斗信息	
		SET_SHOW_FIGHT_LOG_TARGET	,//	开启自身对目标战斗信息	
		SET_SHOW_FIGHT_LOG_TARGET_TARGET	,//	开启目标对目标战斗信息	
		SET_SHOW_BUFF_TIME	,//	显示增益效果剩余时间	
		SET_BUFF_ORDER	,//	增益效果排序	0：从右到左，1:从左到右
		SET_SHOW_DEBUFF_TIME	,//	显示减益效果剩余时间	
		SET_DEBUFF_ORDER	,//	减益效果排序	0：从右到左，1:从左到右
		//
		SET_CHAT_FLAG_NORMAL,	 //	预留	
		SET_CHAT_PANEL_ALPHA,	 //	预留	
		SET_CHAT_PARTY_COLOUR,	 //	预留	
		SET_CHAT_GUILD_COLOUR,	 //	预留	
		SET_CHAT_CAMP_COLOUR,	 //	预留	
		SET_CHAT_SAY_COLOUR,	 //	预留	
		SET_CHAT_CONSTELLATION_COLOUR,	 //	预留	
		SET_CHAT_WHISPER_COLOUR, // 预留
		SET_CHAT_WORLD_COLOUR,	 //	预留	
		SET_OTHER_RESERVE_9,	 //	预留	
		SET_OTHER_RESERVE_10	,//	预留	
		SET_OTHER_RESERVE_11	,//	预留	
		SET_OTHER_RESERVE_12	,//	预留	
		SET_OTHER_RESERVE_13	,//	预留	
		SET_OTHER_RESERVE_14	,//	预留	
		SET_OTHER_RESERVE_15	,//	预留	
		SET_OTHER_RESERVE_16	,//	预留	
		SET_OTHER_RESERVE_17	,//	预留	
		SET_OTHER_RESERVE_18	,//	预留	
		SET_OTHER_RESERVE_19	,//	预留	
		SET_OTHER_RESERVE_20	,//	预留	
		SET_OTHER_RESERVE_21	,//	预留	
		SET_OTHER_RESERVE_22	,//	预留	
		SET_OTHER_RESERVE_23	,//	预留	
		SET_OTHER_RESERVE_24	,//	预留	
		SET_OTHER_RESERVE_25	,//	预留	
		SET_OTHER_RESERVE_26	,//	预留	
		SET_OTHER_RESERVE_27	,//	预留	
		SET_OTHER_RESERVE_28	,//	预留	
		SET_OTHER_RESERVE_29	,//	预留	
		SET_OTHER_RESERVE_30	,//	预留	
		SET_OTHER_RESERVE_31	,//	预留	
		SET_OTHER_RESERVE_32	,//	预留	
		SET_OTHER_RESERVE_33	,//	预留	
		SET_OTHER_RESERVE_34	,//	预留	
		SET_OTHER_RESERVE_35	,//	预留	
		SET_OTHER_RESERVE_36	,//	预留	
		SET_OTHER_RESERVE_37	,//	预留	
		SET_OTHER_RESERVE_38	,//	预留	
		SET_OTHER_RESERVE_39	,//	预留	
		SET_OTHER_RESERVE_40	,//	预留	
		SET_OTHER_RESERVE_41	,//	预留	
		SET_OTHER_RESERVE_42	,//	预留	
		SET_OTHER_RESERVE_43	,//	预留	
		SET_OTHER_RESERVE_44	,//	预留	
		SET_OTHER_RESERVE_45	,//	预留	
		SET_OTHER_RESERVE_46	,//	预留	
		SET_OTHER_RESERVE_47	,//	预留	
		SET_OTHER_RESERVE_48	,//	预留	
		SET_OTHER_RESERVE_49	,//	预留	
		SET_CONFIG_TOTAL,
	};

};

class ShortCutButton
{
public:
	enum Idx
	{
		SLOT_0,
		SLOT_1,
		SLOT_2,
		SLOT_3,
		SLOT_4,
		SLOT_5,
		SLOT_6,
		SLOT_7,
		SLOT_8,
		SLOT_9,
		SLOT_10,
		SLOT_11,
		SLOT_12,
		SLOT_13,
		SLOT_14,
		SLOT_15,
		SLOT_16,
		SLOT_17,
		SLOT_18,
		SLOT_19,
		SLOT_20,
		SLOT_21,
		SLOT_22,
		SLOT_23,
		SLOT_24,
		SLOT_25,
		SLOT_26,
		SLOT_27,
		SLOT_28,
		SLOT_29,
		SLOT_30,
		SLOT_31,
		SLOT_32,
		SLOT_33,
		SLOT_34,
		SLOT_35,
		SLOT_36,
		SLOT_37,
		SLOT_38,
		SLOT_39,
		SLOT_40,
		SLOT_41,
		SLOT_42,
		SLOT_43,
		SLOT_44,
		SLOT_45,
		SLOT_46,
		SLOT_47,
		SLOT_48,
		SLOT_49,
		SLOT_50,
		SLOT_51,
		SLOT_52,
		SLOT_53,
		SLOT_54,
		SLOT_55,
		SLOT_56,
		SLOT_57,
		SLOT_58,
		SLOT_59,
		SLOT_60,
		SLOT_61,
		SLOT_62,
		SLOT_63,
		//
		RESERVE_0,
		RESERVE_1,
		RESERVE_2,
		RESERVE_3,
		RESERVE_4,
		RESERVE_5,
		RESERVE_6,
		RESERVE_7,
		RESERVE_8,
		RESERVE_9,
		RESERVE_10,
		RESERVE_11,
		RESERVE_12,
		RESERVE_13,
		RESERVE_14,
		RESERVE_15,
		RESERVE_16,
		RESERVE_17,
		RESERVE_18,
		RESERVE_19,
		RESERVE_20,
		RESERVE_21,
		RESERVE_22,
		RESERVE_23,
		RESERVE_24,
		RESERVE_25,
		RESERVE_26,
		RESERVE_27,
		RESERVE_28,
		RESERVE_29,
		RESERVE_30,
		RESERVE_31,
		RESERVE_32,
		RESERVE_33,
		RESERVE_34,
		RESERVE_35,
		RESERVE_36,
		RESERVE_37,
		RESERVE_38,
		RESERVE_39,
		RESERVE_40,
		RESERVE_41,
		RESERVE_42,
		RESERVE_43,
		RESERVE_44,
		RESERVE_45,
		RESERVE_46,
		RESERVE_47,
		RESERVE_48,
		RESERVE_49,
		RESERVE_50,
		RESERVE_51,
		RESERVE_52,
		RESERVE_53,
		RESERVE_54,
		RESERVE_55,
		RESERVE_56,
		RESERVE_57,
		RESERVE_58,
		RESERVE_59,
		RESERVE_60,
		RESERVE_61,
		RESERVE_62,
		RESERVE_63,
		//
		TOTAL,
	};
};

class ShortCutButtonType
{
public:
	enum Idx
	{
		NONE,
		SPELL,
		ITEM_TEMPLATE,
		ITEM_INSTANCE,
		TOTAL,
	};
};


struct HotKey
{
	HotKey()
	{
		m_uiKey[0] = 0;
		m_uiKey[1] = 0;
		m_uiKey[2] = 0;
		m_uiKey[3] = 0;
	}
	HotKey(FvUInt8 uiKeyA,FvUInt8 uiSysKeyA,
		FvUInt8 uiKeyB,FvUInt8 uiSysKeyB)
	{
		m_uiKey[0] = uiKeyA;
		m_uiKey[1] = uiSysKeyA;
		m_uiKey[2] = uiKeyB;
		m_uiKey[3] = uiSysKeyB;
	}
	FvUInt8 m_uiKey[4];
};

static const HotKey s_akDefaultHotKey[PlayerDefine::HOTKEY_ZOOM_OUT + 1] = 
{
	HotKey(CEGUI::Key::W,0,CEGUI::Key::ArrowUp,0),//HOTKEY_MOVING_FRONT	,//	前进			移动按键
	HotKey(CEGUI::Key::S,0,CEGUI::Key::ArrowDown,0),//HOTKEY_MOVING_BACK	,//	后退	
	HotKey(CEGUI::Key::A,0,CEGUI::Key::ArrowLeft,0),//HOTKEY_TURNING_LEFT	,//	左转	
	HotKey(CEGUI::Key::D,0,CEGUI::Key::ArrowRight,0),//HOTKEY_TURNING_RIGHT	,//	右转	
	HotKey(CEGUI::Key::Q,0,0,0),//HOTKEY_MOVING_LEFT	,//	向左平移	
	HotKey(CEGUI::Key::E,0,0,0),//HOTKEY_MOVING_RIGHT	,//	向右平移	
	HotKey(CEGUI::Key::Space,0,0,0),//HOTKEY_JUMPED	,//	跳跃	
	HotKey(CEGUI::Key::Grave,0,0,0),//HOTKEY_SWITCH_RUN_WALK	,//	跑/走	
	HotKey(CEGUI::Key::NumLock,0,CEGUI::Key::MediaSelect+1,0),//HOTKEY_KEEPON_MOVE	,//	持续移动	
	HotKey(CEGUI::Key::X,0,0,0),//HOTKEY_SITDOWN	,//	坐下	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_0	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_1	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_2	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_3	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_4	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_5	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_6	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_7	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_8	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_ACTION_RESERVE_9	,//	预留	
	HotKey(CEGUI::Key::T,CEGUI::Shift,0,0),//HOTKEY_PET_ACTIVE	,//	宠物主动攻击			宠物按键
	HotKey(CEGUI::Key::D,CEGUI::Shift,0,0),//HOTKEY_PET_PASSIVE	,//	宠物被动攻击	
	HotKey(CEGUI::Key::F,CEGUI::Shift,0,0),//HOTKEY_PET_FOLLOW	,//	宠物跟随	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_0	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_1	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_2	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_3	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_4	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_5	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_6	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_7	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_8	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_PET_RESERVE_9	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_DANCE	,//	跳舞				 表情按键
	HotKey(0,0,0,0),//HOTKEY_CRY	,//	大叫	
	HotKey(0,0,0,0),//HOTKEY_ANGRY	,//	生气	
	HotKey(0,0,0,0),//HOTKEY_KNEEL	,//	跪下	
	HotKey(0,0,0,0),//HOTKEY_WORSHIP	,//	朝拜	
	HotKey(0,0,0,0),//HOTKEY_ROLL	,//	翻跟头	
	HotKey(0,0,0,0),//HOTKEY_RAGE	,//	愤怒	
	HotKey(0,0,0,0),//HOTKEY_DREAD	,//	恐惧	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_0	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_1	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_2	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_3	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_4	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_5	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_6	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_7	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_8	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_9	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_10	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_11	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_12	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_13	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_14	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_15	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_16	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_17	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_18	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_EMOTION_RESERVE_19	,//	预留	
	HotKey(CEGUI::Key::Z,0,0,0),//HOTKEY_UI_MAIN	,//	打开/关闭主界面				界面按键
	HotKey(CEGUI::Key::C,0,0,0),//HOTKEY_UI_CHARATAR	,//	打开/关闭角色界面	
	HotKey(CEGUI::Key::P,0,0,0),//HOTKEY_UI_PET	,//	打开/关闭宠物界面	
	HotKey(CEGUI::Key::B,0,0,0),//HOTKEY_UI_BAG	,//	打开/关闭道具界面	
	HotKey(CEGUI::Key::K,0,0,0),//HOTKEY_UI_SKILL	,//	打开/关闭技能界面	
	HotKey(CEGUI::Key::I,0,0,0),//HOTKEY_UI_RELIGION	,//	打开/关闭信仰界面	
	HotKey(CEGUI::Key::L,0,0,0),//HOTKEY_UI_QUEST	,//	打开/关闭任务界面	
	HotKey(CEGUI::Key::H,0,0,0),//HOTKEY_UI_HELP	,//	打开/关闭帮助界面	
	HotKey(CEGUI::Key::M,0,0,0),//HOTKEY_UI_MAP	,//	打开/关闭地图界面	
	HotKey(CEGUI::Key::T,0,0,0),//HOTKEY_UI_MARKET	,//	打开/关闭商城界面	
	HotKey(CEGUI::Key::N,0,0,0),//HOTKEY_UI_PROCESSION	,//	打开/关闭团队界面	
	HotKey(CEGUI::Key::O,0,0,0),//HOTKEY_UI_SOCIALITY	,//	打开/关闭社交界面	
	HotKey(CEGUI::Key::G,0,0,0),//HOTKEY_UI_GUIILD	,//	打开/关闭公会界面	
	HotKey(CEGUI::Key::V,0,0,0),//HOTKEY_UI_RANKING	,//	打开/关闭排名界面	
	HotKey(CEGUI::Key::Escape,0,0,0),//HOTKEY_UI_SYSTEM	,//	打开/关闭系统菜单	
	HotKey(CEGUI::Key::C,CEGUI::Alt,0,0),//HOTKEY_UI_CHAT	,//	打开/关闭聊天界面	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_0	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_1	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_2	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_3	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_4	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_5	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_6	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_7	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_8	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_UI_RESERVE_9	,//	预留	
	HotKey(CEGUI::Key::SysRq,0,0,0),//HOTKEY_PRINTSCREEN	,//	截图键	
	HotKey(0,0,0,0),//HOTKEY_MUSIC	,//	打开/关闭背景音乐			声音按键
	HotKey(0,0,0,0),//HOTKEY_SOUND	,//	打开/关闭游戏音效	
	HotKey(0,0,0,0),//HOTKEY_VOLUME_UP	,//	提高主音量	
	HotKey(0,0,0,0),//HOTKEY_VOLUME_DOWN	,//	降低主音量	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_0	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_1	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_2	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_3	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_4	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_5	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_6	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_7	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_8	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SOUND_RESERVE_9	,//	预留	
	HotKey(CEGUI::Key::One,0,0,0),//HOTKEY_SHORTCUT_0	,//	快捷键1				快捷按键
	HotKey(CEGUI::Key::Two,0,0,0),//HOTKEY_SHORTCUT_1	,//	快捷键2	
	HotKey(CEGUI::Key::Three,0,0,0),//HOTKEY_SHORTCUT_2	,//	快捷键3	
	HotKey(CEGUI::Key::Four,0,0,0),//HOTKEY_SHORTCUT_3	,//	快捷键4	
	HotKey(CEGUI::Key::Five,0,0,0),//HOTKEY_SHORTCUT_4	,//	快捷键5	
	HotKey(CEGUI::Key::Six,0,0,0),//HOTKEY_SHORTCUT_5	,//	快捷键6	
	HotKey(CEGUI::Key::Seven,0,0,0),//HOTKEY_SHORTCUT_6	,//	快捷键7	
	HotKey(CEGUI::Key::Eight,0,0,0),//HOTKEY_SHORTCUT_7	,//	快捷键8	
	HotKey(CEGUI::Key::Nine,0,0,0),//HOTKEY_SHORTCUT_8	,//	快捷键9	
	HotKey(CEGUI::Key::Zero,0,0,0),//HOTKEY_SHORTCUT_9	,//	快捷键10	
	HotKey(CEGUI::Key::Minus,0,0,0),//HOTKEY_SHORTCUT_10	,//	快捷键11	
	HotKey(CEGUI::Key::Equals,0,0,0),//HOTKEY_SHORTCUT_11	,//	快捷键12	
	HotKey(CEGUI::Key::One,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_12	,//	快捷键13	
	HotKey(CEGUI::Key::Two,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_13	,//	快捷键14	
	HotKey(CEGUI::Key::Three,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_14	,//	快捷键15	
	HotKey(CEGUI::Key::Four,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_15	,//	快捷键16	
	HotKey(CEGUI::Key::Five,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_16	,//	快捷键17	
	HotKey(CEGUI::Key::Six,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_17	,//	快捷键18	
	HotKey(CEGUI::Key::Seven,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_18	,//	快捷键19	
	HotKey(CEGUI::Key::Eight,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_19	,//	快捷键20	
	HotKey(CEGUI::Key::Nine,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_20	,//	快捷键21	
	HotKey(CEGUI::Key::Zero,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_21	,//	快捷键22	
	HotKey(CEGUI::Key::Minus,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_22	,//	快捷键23	
	HotKey(CEGUI::Key::Equals,CEGUI::Alt,0,0),//HOTKEY_SHORTCUT_23	,//	快捷键24	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_24	,//	快捷键25	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_25	,//	快捷键26	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_26	,//	快捷键27	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_27	,//	快捷键28	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_28	,//	快捷键29	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_29	,//	快捷键30	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_30	,//	快捷键31	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_31	,//	快捷键32	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_32	,//	快捷键33	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_33	,//	快捷键34	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_34	,//	快捷键35	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_35	,//	快捷键36	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_36	,//	快捷键37	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_37	,//	快捷键38	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_38	,//	快捷键39	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_39	,//	快捷键40	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_40	,//	快捷键41	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_41	,//	快捷键42	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_42	,//	快捷键43	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_43	,//	快捷键44	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_44	,//	快捷键45	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_45	,//	快捷键46	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_46	,//	快捷键47	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_47	,//	快捷键48	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_48	,//	快捷键49	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_49	,//	快捷键50	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_50	,//	快捷键51	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_51	,//	快捷键52	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_52	,//	快捷键53	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_53	,//	快捷键54	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_54	,//	快捷键55	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_55	,//	快捷键56	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_56	,//	快捷键57	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_57	,//	快捷键58	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_58	,//	快捷键59	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_59	,//	快捷键60	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_60	,//	快捷键61	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_61	,//	快捷键62	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_62	,//	快捷键63	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_63	,//	快捷键64	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_0	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_1	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_2	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_3	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_4	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_5	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_6	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_7	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_8	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_9	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_10	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_11	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_12	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_13	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_14	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_15	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_16	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_17	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_18	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_19	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_20	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_21	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_22	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_23	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_24	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_25	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_26	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_27	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_28	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_29	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_30	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_31	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_32	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_33	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_34	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_35	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_36	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_37	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_38	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_39	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_40	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_41	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_42	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_43	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_44	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_45	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_46	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_47	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_48	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_49	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_50	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_51	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_52	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_53	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_54	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_55	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_56	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_57	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_58	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_59	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_60	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_61	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_62	,//	预留	
	HotKey(0,0,0,0),//HOTKEY_SHORTCUT_RESERVE_63	,//	预留	
	HotKey(CEGUI::Key::Return,0,0,0),//HOTKEY_ENTER	,//	激活聊天输入栏				聊天按键
	HotKey(CEGUI::Key::R,0,0,0),//HOTKEY_RESPOND	,//	快速回复密语玩家	
	HotKey(CEGUI::Key::ArrowUp,CEGUI::Alt,0,0),//HOTKEY_INPUT_TEXT_PAGE_UP	,//	最近输入文本上翻	
	HotKey(CEGUI::Key::ArrowDown,CEGUI::Alt,0,0),//HOTKEY_INPUT_TEXT_PAGE_DOWN	,//	最近输入文本下翻	
	HotKey(CEGUI::Key::PageUp,0,0,0),//HOTKEY_LOG_PAGE_UP	,//	聊天记录向上翻页	
	HotKey(CEGUI::Key::PageDown,0,0,0),//HOTKEY_LOG_PAGE_DOWN	,//	聊天记录向下翻页	
	HotKey(CEGUI::Key::PageDown,CEGUI::Shift,0,0),//HOTKEY_LOG_PAGE_LAST	,//	聊天记录翻至最下端	
	HotKey(CEGUI::Key::C,CEGUI::Shift,0,0),//HOTKEY_FIGHT_LOG	,//	打开/关闭战斗日志	
	HotKey(CEGUI::Key::PageUp,CEGUI::Control,0,0),//HOTKEY_FIGHT_LOG_PAGE_UP	,//	战斗日志向上翻页	
	HotKey(CEGUI::Key::PageDown,CEGUI::Control,0,0),//HOTKEY_FIGHT_LOG_PAGE_DOWN	,//	战斗日志向下翻页	
	HotKey(CEGUI::Key::PageDown,CEGUI::Control|CEGUI::Shift,0,0),//HOTKEY_FIGHT_LOG_PAGE_LAST	,//	战斗日志翻至最下端	
	HotKey(CEGUI::Key::Tab,0,0,0),//HOTKEY_SELECT_CLOSEST_ENEMY	,//	选择最近的敌人				目标按键
	HotKey(CEGUI::Key::Tab,CEGUI::Control,0,0),//HOTKEY_SELECT_CLOSEST_FRIEND	,//	选择最近的友军	
	HotKey(CEGUI::Key::F,0,0,0),//HOTKEY_SELECT_TARGET_TARGET	,//	选择目标的目标	
	HotKey(0,0,0,0),//HOTKEY_SELECT_CLOSEST_ENEMY_PLAYER	,//	选择最近的敌对玩家	
	HotKey(0,0,0,0),//HOTKEY_SELECT_PREVIOUS_ENEMY_PLAYER	,//	选择上一个敌对玩家	
	HotKey(0,0,0,0),//HOTKEY_SELECT_CLOSEST_FRIEND_PLAYER	,//	选择最近的友方玩家	
	HotKey(0,0,0,0),//HOTKEY_SELECT_PREVIOUS_FRIEND_PLAYER	,//	选择上一个友方玩家	
	HotKey(CEGUI::Key::F1,0,0,0),//HOTKEY_SELECT_MEMBER_SELF	,//	选择自己	
	HotKey(CEGUI::Key::F2,0,0,0),//HOTKEY_SELECT_MEMBER_FIRST	,//	选择第一个队友	
	HotKey(CEGUI::Key::F3,0,0,0),//HOTKEY_SELECT_MEMBER_SECOND	,//	选择第二个队友	
	HotKey(CEGUI::Key::F4,0,0,0),//HOTKEY_SELECT_MEMBER_THIRD	,//	选择第三个队友	
	HotKey(CEGUI::Key::F5,0,0,0),//HOTKEY_SELECT_MEMBER_LAST	,//	选择第四个队友	
	HotKey(CEGUI::Key::F1,CEGUI::Shift,0,0),//HOTKEY_SELECT_PET_SELF	,//	选择自己宠物	
	HotKey(CEGUI::Key::F2,CEGUI::Shift,0,0),//HOTKEY_SELECT_PET_FIRST	,//	选择第一队友宠物	
	HotKey(CEGUI::Key::F3,CEGUI::Shift,0,0),//HOTKEY_SELECT_PET_SECOND	,//	选择第二队友宠物	
	HotKey(CEGUI::Key::F4,CEGUI::Shift,0,0),//HOTKEY_SELECT_PET_THIRD	,//	选择第三队友宠物	
	HotKey(CEGUI::Key::F5,CEGUI::Shift,0,0),//HOTKEY_SELECT_PET_LAST	,//	选择第四队友宠物	
	HotKey(CEGUI::Key::F12,0,0,0),//HOTKEY_SHOW_PLAYER	,//	显示/隐藏其他玩家	
	HotKey(CEGUI::Key::MediaSelect+2,0,CEGUI::Key::Home,0),//HOTKEY_ZOOM_IN	,//	缩进视角					视角按键
	HotKey(CEGUI::Key::MediaSelect+3,0,CEGUI::Key::End,0)//HOTKEY_ZOOM_OUT	,//	拉远视角	
};

class ChatSettingFlag
{
public:
	enum Idx
	{
		PANEL_SHOW				= 0x00000001,
		PANEL_LOCK				= 0x00000002,
		BATTLE_SHOW				= 0x00000004,
		BATTLE_LOCK				= 0x00000008,
		CHANNEL_PARTY			= 0x00000010,
		CHANNEL_GUILD			= 0x00000020,
		CHANNEL_CAMP			= 0x00000040,
		CHANNEL_SAY				= 0x00000080,
		CHANNEL_CONSTELLATION	= 0x00000100,
		CHANNEL_WHISPER			= 0x00000200,
		CHANNEL_WORLD			= 0x00000400,
	};
};

static const FvUInt32 s_auiDefaultChatSetting[PlayerDefine::SET_CHAT_WORLD_COLOUR - PlayerDefine::SET_CHAT_FLAG_NORMAL + 1] = 
{
	ChatSettingFlag::PANEL_SHOW |
	ChatSettingFlag::PANEL_LOCK |
	ChatSettingFlag::BATTLE_SHOW |
	ChatSettingFlag::BATTLE_LOCK |
	ChatSettingFlag::CHANNEL_PARTY |
	ChatSettingFlag::CHANNEL_GUILD |
	ChatSettingFlag::CHANNEL_CAMP |
	ChatSettingFlag::CHANNEL_SAY |
	ChatSettingFlag::CHANNEL_CONSTELLATION |
	ChatSettingFlag::CHANNEL_WHISPER |
	ChatSettingFlag::CHANNEL_WORLD,
	10000,
	0xff84a9e9,
	0xff5deb36,
	0xff49bcf1,
	0xffffffff,
	0xfffeffa7,
	0xffca7eff,
	0xfffccccc
};

class Constellation
{
public:
	enum Idx
	{
		ARIES, //白羊座 ARIES 牧羊座
		TAURUS, //	金牛座 TAURUS 
		GEMINI, //	双子座 GEMINI
		CANCER, //	巨蟹座 CANCER
		LEO, //	狮子座 LEO
		VIRGO, //	室女座 VIRGO 处女座
		LIBRA, //	天秤座 LIBRA
		SCORPIUS, //	天蝎座 SCORPIUS
		SAGITTARIUS, //	人马座 SAGITTARIUS 射手座
		CAPRICORNUS, //	摩羯座 CAPRICORNUS 山羊座
		AQUARIUS, //	宝瓶座 AQUARIUS 水瓶座
		PISCES, //	双鱼座 PISCES
		MAX
	};
};

class FriendFeild
{
public:
	enum Idx
	{
		DBID,///<FvInt64>
		NAME,///<String>
		STATE,///<FvUInt8><FriendState::Idx>
		GROUP_ID,///<FvUInt8>
	};
};
class FriendState
{
public:
	enum Idx
	{
		OFFLINE,
		ONLINE,
	};
};
class BlackMemberrFeild
{
public:
	enum Idx
	{
		DBID,///<FvInt64>
		NAME,///<String>
	};
};
class GroupFeild
{
public:
	enum Idx
	{
		GROUP_ID,///<FvUInt8>
		NAME,///<String>
	};
};

class PostBoxFeild
{
public:
	//Enum define
	enum MailType
	{
		SYSTEM				= 0x80,
		SOCIATY				= 0x40,
		SYSTEM_SEND_BACK	= 0xC0,
		HAS_READED			= 0x20,
		TEMPLATE			= 0x0,
		NORMAL				= 0x1,
		SELL				= 0x2,
		TYPE_MASK			= 0x3
	};

	struct MailOperateResult
	{
		enum
		{
			SUCCEED,
			WRONG_UNIT,
			WRONG_MAIL_ID,
			WRONG_TYPE,
			WRONG_EXTRA
		};
	};

	enum Result
	{
		SUCCEED,
		WRONG_UNIT,
		WRONG_TYPE,
		WRONG_EXTRA

	};

	struct SendMailResult
	{
		enum
		{
			SUCCEED,
			RECEIVER_NOT_EXIST,
			RECEIVER_SAME_ACCOUNT,
			RECEIVER_IS_FULL,
			RECEIVER_FULL,
			GET_ITEM_FAILED,
			UNKNOWN_ERROR
		};
	};

	struct GetMailItemResult
	{
		enum
		{
			SUCCEED,
			BAG_FAILED,
			MAIL_DIRTY,
			NOT_ENOUGH_MONEY,
			UNKNOWN_FAILED
		};
	};

	enum SystemTemplateID
	{
		RECEIVER_POST_FULL,
		SELL_SUCCEED,
		SELL_FAILED,
		MAIL_READ_TIME_OUT,
		AUCTION_ITEM_CANCEL,
		AUCTION_ITEM_BUY_IT_NOW,
		AUCTION_GET_SELL_MONEY,
		AUCTION_ITEM_TURNOVER,
		AUCTION_GET_AUCTION_MONEY,
		AUCTION_SEND_BACK_BID_MONEY,
		AUCTION_ITEM_TIME_OUT,
		AUCTION_ITEM_SEND_BACK_FULL
	};

	enum PostBoxState
	{
		HAS_NEW_MAIL = 0x1,
		POST_BOX_FULL = 0x2
	};

	struct Expire
	{
		enum ExpireType
		{
			SYSTEM,
			TRADE,
			NORMAL_DELETE,
			NORMAL_SEND_BACK
		};
	};
};


#define PAGE_MAIL_NUMBER (8)
#define SYSTEM_MAIL_SENDER_ID (-1)

#define MAX_MAIL_LIST_LENGTH (64)

#endif //__PlayerDefine_H__
