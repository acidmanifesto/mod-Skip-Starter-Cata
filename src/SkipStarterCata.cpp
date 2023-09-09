/*
 * Copyright (C) 2023+ Firelands Core Project , released under AGPL v3 license: https://github.com/FirelandsProject/firelands-cata/blob/master/LICENSE
 */

#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Common.h"
#include "Chat.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SharedDefines.h"
#include "World.h"
#include "WorldSession.h"

constexpr auto YESSKIP = 10;

class Firelandscore_consolidated_skip_announce : public PlayerScript
{
  public:
    Firelandscore_consolidated_skip_announce() : PlayerScript("Firelandscore_consolidated_skip_announce") {}

    void OnLogin(Player* Player, bool /*firstLogin*/) override
    {
        if (sConfigMgr->GetBoolDefault("Consolidated.Skip.Starter.Announce.enable", true))
        {
            ChatHandler(Player->GetSession()).SendSysMessage("This server is running the |cff4CFF00Firelands Consolidated Skip Starter Area |rmodule.");
        }
    }
};

void Firelandscore_skip_deathknight_HandleSkip(Player* player)
{
    // Not sure where DKs were supposed to pick this up from, leaving as the one manual add
    player->AddItem(6948, true); // Hearthstone

    // these are all the starter quests that award talent points, quest items, or spells
    int STARTER_QUESTS[33] = {12593, 12619, 12842, 12848, 12636, 12641, 12657, 12678, 12679, 12680, 12687, 12698, 12701, 12706, 12716, 12719, 12720, 12722, 12724, 12725, 12727, 12733, -1, 12751,
        12754, 12755, 12756, 12757, 12779, 12801, 13165, 13166};

    int specialSurpriseQuestId = -1;
    switch (player->getRace())
    {
    case RACE_TAUREN:
        specialSurpriseQuestId = 12739;
        break;
    case RACE_HUMAN:
        specialSurpriseQuestId = 12742;
        break;
    case RACE_NIGHTELF:
        specialSurpriseQuestId = 12743;
        break;
    case RACE_DWARF:
        specialSurpriseQuestId = 12744;
        break;
    case RACE_GNOME:
        specialSurpriseQuestId = 12745;
        break;
    case RACE_DRAENEI:
        specialSurpriseQuestId = 12746;
        break;
    case RACE_BLOODELF:
        specialSurpriseQuestId = 12747;
        break;
    case RACE_ORC:
        specialSurpriseQuestId = 12748;
        break;
    case RACE_TROLL:
        specialSurpriseQuestId = 12749;
        break;
    case RACE_UNDEAD_PLAYER:
        specialSurpriseQuestId = 12750;
        break;
    case RACE_GOBLIN:
        specialSurpriseQuestId = 28650;
        break;
    case RACE_WORGEN:
        specialSurpriseQuestId = 28649;
    }

    STARTER_QUESTS[22] = specialSurpriseQuestId;
    STARTER_QUESTS[32] = player->GetTeam() == ALLIANCE ? 13188 : 13189;

    for (int questId : STARTER_QUESTS)
    {
        if (player->GetQuestStatus(questId) == QUEST_STATUS_NONE)
        {
            player->AddQuest(sObjectMgr->GetQuestTemplate(questId), nullptr);
            player->RewardQuest(sObjectMgr->GetQuestTemplate(questId), 0, player, false);
        }
    }

    // these are alternate reward items from quest 12679, item 39320 is chosen by default as the reward
    player->AddItem(38664, true); // Sky Darkener's Shroud of the Unholy
    player->AddItem(39322, true); // Shroud of the North Wind

    // these are alternate reward items from quest 12801, item 38633 is chosen by default as the reward
    player->AddItem(38632, true); // Greatsword of the Ebon Blade

    int DKL = sConfigMgr->GetFloatDefault("Skip.Deathknight.Start.Level", 58);
    if (player->getLevel() <= DKL)
    {
        // GiveLevel updates character properties more thoroughly than SetLevel
        player->GiveLevel(DKL);
    }

    // Don't need to save all players, just current
    player->SaveToDB();

    WorldLocation Aloc = WorldLocation(0, -8866.55f, 671.39f, 97.90f, 5.27f);  // Stormwind
    WorldLocation Hloc = WorldLocation(1, 1637.62f, -4440.22f, 15.78f, 2.42f); // Orgrimmar

    if (player->GetTeam() == ALLIANCE)
    {
        player->TeleportTo(Aloc);       // Stormwind
        player->SetHomebind(Aloc, 1637);                          // Stormwind Homebind location
    }
    else
    {
        player->TeleportTo(Hloc);        // Orgrimmar
        player->SetHomebind(Hloc, 1653);                          // Orgrimmar Homebind location
    }
}

class Firelandscore_skip_deathknight : public PlayerScript
{
  public:
    Firelandscore_skip_deathknight() : PlayerScript("Firelandscore_skip_deathknight") {}

    void OnLogin(Player* player, bool firstLogin) override
    {
        if (firstLogin && player->GetAreaId() == 4342)
        {
            // These changes make it so user mistakes in the configuration file don't cause this to run 2x
            if ((sConfigMgr->GetBoolDefault("Skip.Deathknight.Starter.Enable", true) && player->GetSession()->GetSecurity() == SEC_PLAYER) ||
                (sConfigMgr->GetBoolDefault("GM.Skip.Deathknight.Starter.Enable", true) && player->GetSession()->GetSecurity() >= SEC_MODERATOR))
            {
                Firelandscore_skip_deathknight_HandleSkip(player);
            }
        }
    }
};

#define LOCALE_LICHKING_0 "I wish to skip the Death Knight starter questline."
#define LOCALE_LICHKING_1 "죽음의 기사 스타터 퀘스트 라인을 건너뛰고 싶습니다."
#define LOCALE_LICHKING_2 "Je souhaite sauter la série de quêtes de démarrage du Chevalier de la mort."
#define LOCALE_LICHKING_3 "Ich möchte die Todesritter-Starter-Questreihe überspringen."
#define LOCALE_LICHKING_4 "我想跳過死亡騎士新手任務線。"
#define LOCALE_LICHKING_5 "我想跳過死亡騎士新手任務線。"
#define LOCALE_LICHKING_6 "Deseo saltarme la línea de misiones de inicio del Caballero de la Muerte."
#define LOCALE_LICHKING_7 "Deseo saltarme la línea de misiones de inicio del Caballero de la Muerte."
#define LOCALE_LICHKING_8 "Я хочу пропустить начальную цепочку заданий Рыцаря Смерти."

class Firelandscore_optional_deathknight_skip : public CreatureScript
{
  public:
    Firelandscore_optional_deathknight_skip() : CreatureScript("npc_fl_skip_lich") {}

    struct npc_SkipLichAI : public ScriptedAI
    {
        npc_SkipLichAI(Creature* creature) : ScriptedAI(creature) {}

        bool GossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
            {
                player->PrepareQuestMenu(me->GetGUID());
            }

            if (sConfigMgr->GetBoolDefault("Skip.Deathknight.Optional.Enable", true))
            {
                char const* localizedEntry;
                switch (player->GetSession()->GetSessionDbcLocale())
                {
                case LOCALE_koKR:
                    localizedEntry = LOCALE_LICHKING_1;
                    break;
                case LOCALE_frFR:
                    localizedEntry = LOCALE_LICHKING_2;
                    break;
                case LOCALE_deDE:
                    localizedEntry = LOCALE_LICHKING_3;
                    break;
                case LOCALE_zhCN:
                    localizedEntry = LOCALE_LICHKING_4;
                    break;
                case LOCALE_zhTW:
                    localizedEntry = LOCALE_LICHKING_5;
                    break;
                case LOCALE_esES:
                    localizedEntry = LOCALE_LICHKING_6;
                    break;
                case LOCALE_esMX:
                    localizedEntry = LOCALE_LICHKING_7;
                    break;
                case LOCALE_ruRU:
                    localizedEntry = LOCALE_LICHKING_8;
                    break;
                case LOCALE_enUS:
                    localizedEntry = LOCALE_LICHKING_0;
                    break;
                default:
                    localizedEntry = LOCALE_LICHKING_0;
                }
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, localizedEntry, GOSSIP_SENDER_MAIN, YESSKIP);
                SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            }

            player->TalkedToCreature(me->GetEntry(), me->GetGUID());
            return true;
        }

        bool GossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            if (action == YESSKIP)
            {
                CloseGossipMenuFor(player);
                Firelandscore_skip_deathknight_HandleSkip(player);
            }

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override { return new npc_SkipLichAI(creature); }
};

void Firelandscore_skip_worgan_HandleSkip(Player* player)
{
    // these are all the starter quests that award talent points, quest items, or spells
    int STARTER_WORG_QUESTS[26] = {
        14093, 14094, -1, 24930, 14154, 14204, 14218, 14347, 14348, 14369, 14368, 14382, 14386, 14395, 14401, 14404,
        14412, 14416, 24468, 24501, 24616, 24593, 24592, 24674, 24904, 24681};

    int safetyInNumbersId = -1;
    switch (player->getClass())
    {
    case CLASS_DRUID:
        safetyInNumbersId = 14291;
        break;
    case CLASS_HUNTER:
        safetyInNumbersId = 14290;
        break;
    case CLASS_MAGE:
        safetyInNumbersId = 14288;
        break;
    case CLASS_PRIEST:
        safetyInNumbersId = 14289;
        break;
    case CLASS_ROGUE:
        safetyInNumbersId = 14285;
        break;
    case CLASS_WARLOCK:
        safetyInNumbersId = 14287;
        break;
    case CLASS_WARRIOR:
        safetyInNumbersId = 14286;
        break;
    }

    STARTER_WORG_QUESTS[2] = safetyInNumbersId;

    for (int questId : STARTER_WORG_QUESTS)
    {
        if (player->GetQuestStatus(questId) == QUEST_STATUS_NONE)
        {
            player->AddQuest(sObjectMgr->GetQuestTemplate(questId), nullptr);
            player->RewardQuest(sObjectMgr->GetQuestTemplate(questId), 0, player, false);
        }
    }

    int WGL = sConfigMgr->GetFloatDefault("Skip.Worgan.Start.Level", 16);
    if (player->getLevel() <= WGL)
    {
        // GiveLevel updates character properties more thoroughly than SetLevel
        player->GiveLevel(WGL);
    }

    // Learn The Racials in case the quests didnt propperly reward it
    player->LearnSpell(72792, false); // Learn Racials
    player->LearnSpell(72857, false); // Learn Two Forms
    player->LearnSpell(95759, false); // Learn Darkflight
    player->LearnSpell(87840, false); // Lear Running wild
    player->LearnSpell(94293, false); // Learn Altered Form

    // Don't need to save all players, just current
    player->SaveToDB();

    WorldLocation Wloc = WorldLocation(1, 8181.060059f, 999.103027f, 7.253240f, 6.174160f);  // Howling Oak

    player->TeleportTo(Wloc); // Howling Oak
    player->SetHomebind(Wloc, 702); // Howling Oak Homebind location
}

class Firelandscore_skip_worgan : public PlayerScript
{
  public:
    Firelandscore_skip_worgan() : PlayerScript("Firelandscore_skip_worgan") {}

    void OnLogin(Player* player, bool firstLogin) override
    {
        if (firstLogin && player->GetAreaId() == 4756)
        {
            // These changes make it so user mistakes in the configuration file don't cause this to run 2x
            if ((sConfigMgr->GetBoolDefault("Skip.Worgan.Starter.Enable", true) && player->GetSession()->GetSecurity() == SEC_PLAYER) ||
                (sConfigMgr->GetBoolDefault("GM.Skip.Worgan.Starter.Enable", true) && player->GetSession()->GetSecurity() >= SEC_MODERATOR))
            {
                Firelandscore_skip_worgan_HandleSkip(player);
            }
        }
    }
};

#define LOCALE_WORSKIP_0 "I wish to skip the Worgen starter questline."
#define LOCALE_WORSKIP_1 "늑대인간 스타터 퀘스트 라인을 건너뛰고 싶습니다."
#define LOCALE_WORSKIP_2 "Je souhaite sauter la série de quêtes de démarrage Worgen."
#define LOCALE_WORSKIP_3 "Ich möchte die Worgen-Starter-Questreihe überspringen."
#define LOCALE_WORSKIP_4 "我想跳過狼人新手任務線。"
#define LOCALE_WORSKIP_5 "我想跳過狼人新手任務線。"
#define LOCALE_WORSKIP_6 "Deseo omitir la línea de misiones de inicio de los huargen."
#define LOCALE_WORSKIP_7 "Deseo omitir la línea de misiones de inicio de los huargen."
#define LOCALE_WORSKIP_8 "Я хочу пропустить начальную цепочку заданий воргенов."

class Firelandscore_optional_worgan_skip : public CreatureScript
{
  public:
    Firelandscore_optional_worgan_skip() : CreatureScript("npc_fl_skip_worg") {}

    struct npc_SkipWorgAI : public ScriptedAI
    {
        npc_SkipWorgAI(Creature* creature) : ScriptedAI(creature) {}

        bool GossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
            {
                player->PrepareQuestMenu(me->GetGUID());
            }

            if (sConfigMgr->GetBoolDefault("Skip.Worgan.Optional.Enable", true))
            {
                char const* localizedWorgEntry;
                switch (player->GetSession()->GetSessionDbcLocale())
                {
                case LOCALE_koKR:
                    localizedWorgEntry = LOCALE_WORSKIP_1;
                    break;
                case LOCALE_frFR:
                    localizedWorgEntry = LOCALE_WORSKIP_2;
                    break;
                case LOCALE_deDE:
                    localizedWorgEntry = LOCALE_WORSKIP_3;
                    break;
                case LOCALE_zhCN:
                    localizedWorgEntry = LOCALE_WORSKIP_4;
                    break;
                case LOCALE_zhTW:
                    localizedWorgEntry = LOCALE_WORSKIP_5;
                    break;
                case LOCALE_esES:
                    localizedWorgEntry = LOCALE_WORSKIP_6;
                    break;
                case LOCALE_esMX:
                    localizedWorgEntry = LOCALE_WORSKIP_7;
                    break;
                case LOCALE_ruRU:
                    localizedWorgEntry = LOCALE_WORSKIP_8;
                    break;
                case LOCALE_enUS:
                    localizedWorgEntry = LOCALE_WORSKIP_0;
                    break;
                default:
                    localizedWorgEntry = LOCALE_WORSKIP_0;
                }
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, localizedWorgEntry, GOSSIP_SENDER_MAIN, YESSKIP);
                SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            }

            player->TalkedToCreature(me->GetEntry(), me->GetGUID());
            return true;
        }

        bool GossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            if (action == YESSKIP)
            {
                CloseGossipMenuFor(player);
                Firelandscore_skip_worgan_HandleSkip(player);
            }

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override { return new npc_SkipWorgAI(creature); }
};

void Firelandscore_skip_goblin_HandleSkip(Player* player)
{
    // these are all the starter quests that award talent points, quest items, or spells
    int STARTER_GOB_QUESTS[39] = {14075, 14069, 25473, 14071, 14070, 28414, 14115, 14122, 14125, 14019, 14021, 14234, 14234,
        14238, 14241, 14243, 24671, 24744, 24817, 24858, 24868, 24901, 24929, 24942, 24945, 24958, 25066, 25093, 25109, 25123,
        25184, 25202, 25203, 25207, 25214, 25243, 25244, 25265, 25267};

    for (int questId : STARTER_GOB_QUESTS)
    {
        if (player->GetQuestStatus(questId) == QUEST_STATUS_NONE)
        {
            player->AddQuest(sObjectMgr->GetQuestTemplate(questId), nullptr);
            player->RewardQuest(sObjectMgr->GetQuestTemplate(questId), 0, player, false);
        }
    }

    int GBL = sConfigMgr->GetFloatDefault("Skip.Goblin.Start.Level", 16);
    if (player->getLevel() <= GBL)
    {
        // GiveLevel updates character properties more thoroughly than SetLevel
        player->GiveLevel(GBL);
    }

    // Learn The Racials in case the quests didnt propperly reward it
    player->LearnSpell(69070, false); // Learn Rocket Jump
    player->LearnSpell(69041, false); // Learn Rocket Barrage
    player->LearnSpell(69042, false); // Learn Time is Money
    player->LearnSpell(69044, false); // Learn Best Deals Anywhere
    player->LearnSpell(69045, false); // Learn Better Living Through Chemisty
    player->LearnSpell(69046, false); // Learn Pack Hobgoblin

    // Don't need to save all players, just current
    player->SaveToDB();

    WorldLocation Gloc = WorldLocation(1, 1569.59f, -4397.63f, 16.06f, 0.54f); //  Orgrimmar

    player->TeleportTo(Gloc); // Orgrimmar
    player->SetHomebind(Gloc, 1653); // Orgrimmar Homebind location
}

class Firelandscore_skip_goblin : public PlayerScript
{
  public:
    Firelandscore_skip_goblin() : PlayerScript("Firelandscore_skip_goblin") {}

    void OnLogin(Player* player, bool firstLogin) override
    {
        if (firstLogin && player->GetAreaId() == 4765)
        {
            // These changes make it so user mistakes in the configuration file don't cause this to run 2x
            if ((sConfigMgr->GetBoolDefault("Skip.Goblin.Starter.Enable", true) && player->GetSession()->GetSecurity() == SEC_PLAYER) ||
                (sConfigMgr->GetBoolDefault("GM.Skip.Goblin.Starter.Enable", true) && player->GetSession()->GetSecurity() >= SEC_MODERATOR))
            {
                Firelandscore_skip_goblin_HandleSkip(player);
            }
        }
    }
};

#define LOCALE_GOBSKIP_0 "I wish to skip the Goblin starter questline."
#define LOCALE_GOBSKIP_1 "고블린 스타터 퀘스트 라인을 건너뛰고 싶습니다."
#define LOCALE_GOBSKIP_2 "Je souhaite sauter la série de quêtes de démarrage des gobelins."
#define LOCALE_GOBSKIP_3 "Ich möchte die Goblin-Starter-Questreihe überspringen."
#define LOCALE_GOBSKIP_4 "我想跳過地精初學者任務線。"
#define LOCALE_GOBSKIP_5 "我想跳過地精初學者任務線。"
#define LOCALE_GOBSKIP_6 "Deseo omitir la línea de misiones de inicio de goblin."
#define LOCALE_GOBSKIP_7 "Deseo omitir la línea de misiones de inicio de goblin."
#define LOCALE_GOBSKIP_8 "Я хочу пропустить стартовую цепочку заданий гоблинов."

class Firelandscore_optional_goblin_skip : public CreatureScript
{
  public:
    Firelandscore_optional_goblin_skip() : CreatureScript("npc_fl_skip_gob") {}

    struct npc_SkipGobAI : public ScriptedAI
    {
        npc_SkipGobAI(Creature* creature) : ScriptedAI(creature) {}

        bool GossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
            {
                player->PrepareQuestMenu(me->GetGUID());
            }

            if (sConfigMgr->GetBoolDefault("Skip.Goblin.Optional.Enable", true))
            {
                char const* localizedGobEntry;
                switch (player->GetSession()->GetSessionDbcLocale())
                {
                case LOCALE_koKR:
                    localizedGobEntry = LOCALE_GOBSKIP_1;
                    break;
                case LOCALE_frFR:
                    localizedGobEntry = LOCALE_GOBSKIP_2;
                    break;
                case LOCALE_deDE:
                    localizedGobEntry = LOCALE_GOBSKIP_3;
                    break;
                case LOCALE_zhCN:
                    localizedGobEntry = LOCALE_GOBSKIP_4;
                    break;
                case LOCALE_zhTW:
                    localizedGobEntry = LOCALE_GOBSKIP_5;
                    break;
                case LOCALE_esES:
                    localizedGobEntry = LOCALE_GOBSKIP_6;
                    break;
                case LOCALE_esMX:
                    localizedGobEntry = LOCALE_GOBSKIP_7;
                    break;
                case LOCALE_ruRU:
                    localizedGobEntry = LOCALE_GOBSKIP_8;
                    break;
                case LOCALE_enUS:
                    localizedGobEntry = LOCALE_GOBSKIP_0;
                    break;
                default:
                    localizedGobEntry = LOCALE_GOBSKIP_0;
                }
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, localizedGobEntry, GOSSIP_SENDER_MAIN, YESSKIP);
                SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            }

            player->TalkedToCreature(me->GetEntry(), me->GetGUID());
            return true;
        }

        bool GossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            if (action == YESSKIP)
            {
                CloseGossipMenuFor(player);
                Firelandscore_skip_goblin_HandleSkip(player);
            }

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override { return new npc_SkipGobAI(creature); }
};

void Addmod_Skip_Starter_Cata()
{
    new Firelandscore_consolidated_skip_announce;
    new Firelandscore_skip_deathknight;
    new Firelandscore_optional_deathknight_skip;
    new Firelandscore_skip_worgan;
    new Firelandscore_optional_worgan_skip;
    new Firelandscore_skip_goblin;
    new Firelandscore_optional_goblin_skip;
}
