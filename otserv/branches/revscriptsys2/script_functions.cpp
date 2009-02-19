/////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////

#include "otpch.h"

#include "lua_manager.h"
#include "script_manager.h"

#include "script_event.h"
#include "script_listener.h"

#include "game.h"
#include "creature.h"
#include "town.h"
#include "chat.h"
#include "house.h"

extern Game g_game;
extern Vocations g_vocations;

using namespace Script;

///////////////////////////////////////////////////////////////////////////////
// Bit lib declarations

int lua_BitNot(lua_State* L);
int lua_BitAnd(lua_State* L);
int lua_BitOr(lua_State* L);
int lua_BitXor(lua_State* L);
int lua_BitLeftShift(lua_State* L);
int lua_BitRightShift(lua_State* L);
// Unsigned
int lua_BitUNot(lua_State* L);
int lua_BitUAnd(lua_State* L);
int lua_BitUOr(lua_State* L);
int lua_BitUXor(lua_State* L);
int lua_BitULeftShift(lua_State* L);
int lua_BitURightShift(lua_State* L);

const luaL_Reg lua_BitReg[] =
{
	{"bnot", lua_BitNot},
	{"band", lua_BitAnd},
	{"bor", lua_BitOr},
	{"bxor", lua_BitXor},
	{"lshift", lua_BitLeftShift},
	{"rshift", lua_BitRightShift},
	// Unsigned
	{"ubnot", lua_BitUNot},
	{"uband", lua_BitUAnd},
	{"ubor", lua_BitUOr},
	{"ubxor", lua_BitUXor},
	{"ulshift", lua_BitULeftShift},
	{"urshift", lua_BitURightShift},
	{NULL,NULL}
};

///////////////////////////////////////////////////////////////////////////////
// Function list

void Manager::registerClasses() {
	// Classes...
	registerClass("Event");
	registerClass("OnSayEvent", "Event");
	registerClass("OnUseItemEvent", "Event");
	registerClass("OnJoinChannelEvent", "Event");
	registerClass("OnLeaveChannelEvent", "Event");
	registerClass("OnLoginEvent", "Event");
	registerClass("OnLogoutEvent", "Event");
	registerClass("OnLookEvent", "Event");
	registerClass("OnTurnEvent", "Event");
	registerClass("OnServerLoad", "Event");

	registerClass("Thing");
	registerClass("Creature", "Thing");
	registerClass("Monster", "Creature");
	registerClass("Player", "Creature");
	registerClass("Item", "Thing");
	registerClass("Teleport", "Item");
	registerClass("Container", "Item");

	registerClass("Tile");

	registerClass("Town");
	registerClass("House");
	registerClass("Waypoint");

	registerClass("Channel");

	// Event classes
	registerMemberFunction("Event", "skip()", &Manager::lua_Event_skip);
	registerMemberFunction("Event", "propagate()", &Manager::lua_Event_propagate);

	// Game classes
	registerMemberFunction("Thing", "getPosition()", &Manager::lua_Thing_getPosition);
	registerMemberFunction("Thing", "getParentTile()", &Manager::lua_Thing_getParentTile);
	registerMemberFunction("Thing", "getParent()", &Manager::lua_Thing_getParent);
	registerMemberFunction("Thing", "isMoveable()", &Manager::lua_Thing_isMoveable);
	registerMemberFunction("Thing", "getPosition()", &Manager::lua_Thing_getPosition);
	registerMemberFunction("Thing", "getName()", &Manager::lua_Thing_getName);
	registerMemberFunction("Thing", "getDescription([int lookdistance])", &Manager::lua_Thing_getDescription);
	registerMemberFunction("Thing", "moveTo(table pos)", &Manager::lua_Thing_moveToPosition);
	registerMemberFunction("Thing", "destroy()", &Manager::lua_Thing_destroy);

	// Creature
	registerMemberFunction("Creature", "getID()", &Manager::lua_Creature_getID);
	registerMemberFunction("Creature", "getHealth()", &Manager::lua_Creature_getHealth);
	registerMemberFunction("Creature", "getHealthMax()", &Manager::lua_Creature_getHealthMax);
	registerMemberFunction("Creature", "setHealth(integer newval)", &Manager::lua_Creature_setHealth);

	registerMemberFunction("Creature", "getOrientation()", &Manager::lua_Creature_getOrientation);
	registerMemberFunction("Creature", "getDirection()", &Manager::lua_Creature_getOrientation);

	registerMemberFunction("Creature", "walk(int direction)", &Manager::lua_Creature_walk);
	registerMemberFunction("Creature", "say(string msg)", &Manager::lua_Creature_say);

	registerGlobalFunction("getCreatureByName(string name)", &Manager::lua_getCreatureByName);
	registerGlobalFunction("getCreaturesByName(string name)", &Manager::lua_getCreaturesByName);

	// Player
	registerMemberFunction("Player", "setStorageValue(string key, string value)", &Manager::lua_Player_setStorageValue);
	registerMemberFunction("Player", "getStorageValue(string key)", &Manager::lua_Player_getStorageValue);

	registerMemberFunction("Player", "getFood()", &Manager::lua_Player_getFood);
	registerMemberFunction("Player", "getMana()", &Manager::lua_Player_getMana);
	registerMemberFunction("Player", "getLevel()", &Manager::lua_Player_getLevel);
	registerMemberFunction("Player", "getMagicLevel()", &Manager::lua_Player_getMagicLevel);
	registerMemberFunction("Player", "getManaMax()", &Manager::lua_Player_getManaMax);
	registerMemberFunction("Player", "setMana(integer newval)", &Manager::lua_Player_setMana);
	registerMemberFunction("Player", "addManaSpent(integer howmuch)", &Manager::lua_Player_addManaSpent);
	registerMemberFunction("Player", "getSoulPoints()", &Manager::lua_Player_getSoulPoints);
	registerMemberFunction("Player", "setSoulPoints()", &Manager::lua_Player_setSoulPoints);
	registerMemberFunction("Player", "getFreeCap()", &Manager::lua_Player_getFreeCap);
	registerMemberFunction("Player", "getMaximumCap()", &Manager::lua_Player_getMaximumCap);
	registerMemberFunction("Player", "getSex()", &Manager::lua_Player_getSex);
	registerMemberFunction("Player", "getVocationID()", &Manager::lua_Player_getVocationID);
	registerMemberFunction("Player", "getTownID()", &Manager::lua_Player_getTownID);
	registerMemberFunction("Player", "getGUID()", &Manager::lua_Player_getGUID);
	registerMemberFunction("Player", "getAccessGroup()", &Manager::lua_Player_getGroup);
	registerMemberFunction("Player", "getPremiumDays()", &Manager::lua_Player_getPremiumDays);
	registerMemberFunction("Player", "getSkull()", &Manager::lua_Player_getSkullType);
	registerMemberFunction("Player", "getLastLogin()", &Manager::lua_Player_getLastLogin);
	registerMemberFunction("Player", "getGuildID()", &Manager::lua_Player_getGuildID);
	registerMemberFunction("Player", "getGuildName()", &Manager::lua_Player_getGuildName);
	registerMemberFunction("Player", "getGuildRank()", &Manager::lua_Player_getGuildRank);
	registerMemberFunction("Player", "getGuildNick()", &Manager::lua_Player_getGuildNick);

	registerMemberFunction("Player", "addItem(Item item)", &Manager::lua_Player_addItem);
	registerMemberFunction("Player", "getInventoryItem(int slot)", &Manager::lua_Player_getInventoryItem);
	registerMemberFunction("Player", "addExperience(int experience)", &Manager::lua_Player_addExperience);
	registerMemberFunction("Player", "setTown(int townid)", &Manager::lua_Player_setTown);
	registerMemberFunction("Player", "setVocation(int vocationid)", &Manager::lua_Player_setVocation);
	registerMemberFunction("Player", "hasGroupFlag(integer flag)", &Manager::lua_Player_hasGroupFlag);

	registerMemberFunction("Player", "getMoney()", &Manager::lua_Player_getMoney);
	registerMemberFunction("Player", "removeMoney(int amount)", &Manager::lua_Player_removeMoney);
	registerMemberFunction("Player", "addMoney(int amount)", &Manager::lua_Player_addMoney);

	registerMemberFunction("Player", "sendMessage(int type, string msg)", &Manager::lua_Player_sendMessage);

	registerGlobalFunction("getOnlinePlayers()", &Manager::lua_getOnlinePlayers);
	registerGlobalFunction("getPlayerByName(string name)", &Manager::lua_getPlayerByName);
	registerGlobalFunction("getPlayerByNameWildcard(string wild)", &Manager::lua_getPlayerByNameWildcard);
	registerGlobalFunction("getPlayersByName(string name)", &Manager::lua_getPlayersByName);
	registerGlobalFunction("getPlayersByNameWildcard(string wild)", &Manager::lua_getPlayersByNameWildcard);

	// Item
	registerGlobalFunction("createItem(int newid[, int count])", &Manager::lua_createItem);
	registerMemberFunction("Item", "getItemID()", &Manager::lua_Item_getItemID);
	registerMemberFunction("Item", "getActionID()", &Manager::lua_Item_getActionID);
	registerMemberFunction("Item", "getUniqueID()", &Manager::lua_Item_getUniqueID);
	registerMemberFunction("Item", "getCount()", &Manager::lua_Item_getCount);
	registerMemberFunction("Item", "getWeight()", &Manager::lua_Item_getWeight);
	registerMemberFunction("Item", "isPickupable()", &Manager::lua_Item_isPickupable);
	registerMemberFunction("Item", "getText()", &Manager::lua_Item_getText);
	registerMemberFunction("Item", "getSpecialDescription()", &Manager::lua_Item_getSpecialDescription);

	registerMemberFunction("Item", "setItemID(int newid)", &Manager::lua_Item_setItemID);
	registerMemberFunction("Item", "setActionID(int id)", &Manager::lua_Item_setActionID);
	registerMemberFunction("Item", "setCount(int newcount)", &Manager::lua_Item_setCount);
	registerMemberFunction("Item", "startDecaying()", &Manager::lua_Item_startDecaying);
	registerMemberFunction("Item", "setText(string text)", &Manager::lua_Item_setText);
	registerMemberFunction("Item", "setSpecialDescription(string text)", &Manager::lua_Item_setSpecialDescription);

	registerGlobalFunction("getItemIDByName(string name)", &Manager::lua_getItemIDByName);
	registerGlobalFunction("isValidItemID(int id)", &Manager::lua_isValidItemID);

	// Tile
	registerMemberFunction("Tile", "getThing(int index)", &Manager::lua_Tile_getThing);
	registerMemberFunction("Tile", "getCreatures()", &Manager::lua_Tile_getCreatures);
	registerMemberFunction("Tile", "getMoveableItems()", &Manager::lua_Tile_getMoveableItems);
	registerMemberFunction("Tile", "getItems()", &Manager::lua_Tile_getItems);
	registerMemberFunction("Tile", "queryAdd()", &Manager::lua_Tile_queryAdd);
	registerMemberFunction("Tile", "hasProperty(integer prop)", &Manager::lua_Tile_hasProperty);

	registerMemberFunction("Tile", "isPZ()", &Manager::lua_Tile_isPZ);
	registerMemberFunction("Tile", "isPVP()", &Manager::lua_Tile_isPVP);
	registerMemberFunction("Tile", "isNoPVP()", &Manager::lua_Tile_isNoPVP);
	registerMemberFunction("Tile", "isNoLogout()", &Manager::lua_Tile_isNoLogout);
	registerMemberFunction("Tile", "doesRefresh()", &Manager::lua_Tile_doesRefresh);

	registerMemberFunction("Tile", "getItemTypeCount(int itemid)", &Manager::lua_Tile_getItemTypeCount);

	registerMemberFunction("Tile", "addItem(Item item)", &Manager::lua_Tile_addItem);

	// Town
	registerMemberFunction("Town", "getTemplePosition()", &Manager::lua_Town_getTemplePosition);
	registerMemberFunction("Town", "getName()", &Manager::lua_Town_getName);
	registerMemberFunction("Town", "getID()", &Manager::lua_Town_getID);
	
	registerGlobalFunction("getAllTowns()", &Manager::lua_getTowns);

	// Waypoint
	registerMemberFunction("Waypoint", "getPosition()", &Manager::lua_Waypoint_getPosition);
	registerMemberFunction("Waypoint", "getName()", &Manager::lua_Waypoint_getName);

	registerGlobalFunction("getWaypointByName(string name)", &Manager::lua_getWaypointByName);

	// House
	registerMemberFunction("House", "cleanHouse()", &Manager::lua_House_cleanHouse);
	registerMemberFunction("House", "getDoors()", &Manager::lua_House_getDoors);
	registerMemberFunction("House", "getExitPosition()", &Manager::lua_House_getExitPosition);
	registerMemberFunction("House", "getID()", &Manager::lua_House_getID);
	registerMemberFunction("House", "getInvitedList()", &Manager::lua_House_getInvitedList);
	registerMemberFunction("House", "getName()", &Manager::lua_House_getName);
	registerMemberFunction("House", "getPaidUntil()", &Manager::lua_House_getPaidUntil);
	registerMemberFunction("House", "getRent()", &Manager::lua_House_getRent);
	registerMemberFunction("House", "getSubownerList()", &Manager::lua_House_getSubownerList);
	registerMemberFunction("House", "getTiles()", &Manager::lua_House_getTiles);
	registerMemberFunction("House", "getTown()", &Manager::lua_House_getTown);
	registerMemberFunction("House", "kickPlayer(Player who)", &Manager::lua_House_kickPlayer);
	registerMemberFunction("House", "isInvited(Player who)", &Manager::lua_House_isInvited);
	registerMemberFunction("House", "setInviteList(table list)", &Manager::lua_House_setInviteList);
	registerMemberFunction("House", "setOwner(int guid)", &Manager::lua_House_setOwner);
	registerMemberFunction("House", "setSubownerList(table list)", &Manager::lua_House_setSubownerList);
	registerMemberFunction("House", "setPaidUntil(int until)", &Manager::lua_House_setPaidUntil);
	
	registerGlobalFunction("getAllHouses()", &Manager::lua_getHouses);

	// Channel
	registerMemberFunction("Channel", "getID()", &Manager::lua_Channel_getID);
	registerMemberFunction("Channel", "getName()", &Manager::lua_Channel_getName);
	registerMemberFunction("Channel", "getUsers()", &Manager::lua_Channel_getUsers);
	registerMemberFunction("Channel", "addUser(Player player)", &Manager::lua_Channel_addUser);
	registerMemberFunction("Channel", "removeUser(Player player)", &Manager::lua_Channel_removeUser);
	registerMemberFunction("Channel", "talk(Player speaker, int type, string msg)", &Manager::lua_Channel_talk);
}

void Manager::registerFunctions() {
	registerGlobalFunction("wait(int delay)", &Manager::lua_wait);

	registerGlobalFunction("registerOnSay(string method, boolean case_sensitive, string filter, function callback)", &Manager::lua_registerGenericEvent_OnSay);
	registerGlobalFunction("registerOnCreatureSay(Creature who, string method, boolean case_sensitive, string filter, function callback)", &Manager::lua_registerSpecificEvent_OnSay);

	registerGlobalFunction("registerOnUseItem(string method, int filter, function callback)", &Manager::lua_registerGenericEvent_OnUseItem);

	registerGlobalFunction("registerOnStepInCreature(string method, int filter, function callback)", &Manager::lua_registerGenericEvent_OnStepInCreature);
	registerGlobalFunction("registerOnStepOutCreature(string method, int filter, function callback)", &Manager::lua_registerGenericEvent_OnStepOutCreature);
	registerGlobalFunction("registerOnCreatureMove(Creature who, function callback)", &Manager::lua_registerSpecificEvent_OnMoveCreature);
	registerGlobalFunction("registerOnMoveCreature(Creature who, function callback)", &Manager::lua_registerSpecificEvent_OnMoveCreature);

	registerGlobalFunction("registerOnAnyCreatureTurn(function callback)", &Manager::lua_registerGenericEvent_OnCreatureTurn);
	registerGlobalFunction("registerOnCreatureTurn(Creature who, function callback)", &Manager::lua_registerSpecificEvent_OnCreatureTurn);

	registerGlobalFunction("registerOnEquipItem(string method, int filter, string slot, function callback)", &Manager::lua_registerGenericEvent_OnEquipItem);
	registerGlobalFunction("registerOnDeEquipItem(string method, int filter, string slot, function callback)", &Manager::lua_registerGenericEvent_OnDeEquipItem);
	
	registerGlobalFunction("registerOnServerLoad(function callback)", &Manager::lua_registerGenericEvent_OnServerLoad);

	// Others are bound in lua
	registerGlobalFunction("registerOnMoveItem(string method, int filter, boolean isadd, boolean isontile, function callback)", &Manager::lua_registerGenericEvent_OnMoveItem);

	registerGlobalFunction("registerOnJoinChannel(function callback)", &Manager::lua_registerGenericEvent_OnJoinChannel);
	registerGlobalFunction("registerOnPlayerJoinChannel(Player player, function callback)", &Manager::lua_registerSpecificEvent_OnJoinChannel);
	registerGlobalFunction("registerOnLeaveChannel(function callback)", &Manager::lua_registerGenericEvent_OnLeaveChannel);
	registerGlobalFunction("registerOnPlayerLeaveChannel(Player player, function callback)", &Manager::lua_registerSpecificEvent_OnLeaveChannel);

	registerGlobalFunction("registerOnLogin(function callback)", &Manager::lua_registerGenericEvent_OnLogin);
	registerGlobalFunction("registerOnLogout(function callback)", &Manager::lua_registerGenericEvent_OnLogout);
	registerGlobalFunction("registerOnPlayerLogout(Player player, function callback)", &Manager::lua_registerSpecificEvent_OnLogout);

	registerGlobalFunction("registerOnLookAtItem(string method, int filter, function callback)", &Manager::lua_registerGenericEvent_OnLookAtItem);
	registerGlobalFunction("registerOnLookAtCreature(Creature creature, function callback)", &Manager::lua_registerGenericEvent_OnLookAtCreature);
	registerGlobalFunction("registerOnPlayerLookAt(Creature creature, function callback)", &Manager::lua_registerSpecificEvent_OnLook);

	registerGlobalFunction("stopListener(string listener_id)", &Manager::lua_stopListener);

	registerGlobalFunction("wait(int delay)", &Manager::lua_wait);

	registerGlobalFunction("getTile(int x, int y, int z)", &Manager::lua_getTile);
	registerGlobalFunction("sendMagicEffect(position where, int type)", &Manager::lua_sendMagicEffect);


	luaL_register(state, "bit", lua_BitReg);
}

///////////////////////////////////////////////////////////////////////////////
// Utility functions

int LuaState::lua_wait() {
	// integer delay is ontop of stack
	return lua_yield(state, 1);
}

///////////////////////////////////////////////////////////////////////////////
// Register Events

int LuaState::lua_registerGenericEvent_OnServerLoad()
{
	OnServerLoad::ScriptInformation si_onload;
	boost::any p(si_onload);
	Listener_ptr listener(new Listener(ON_LOAD_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnLoad.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnSay() {
	// Store callback
	insert(-4);

	std::string filter = popString();
	bool case_sensitive = popBoolean();
	std::string method = popString();

	OnSay::ScriptInformation si_onsay;
	if(method == "all") {
		si_onsay.method = OnSay::FILTER_ALL;
	}
	else if(method == "substring") {
		si_onsay.method = OnSay::FILTER_SUBSTRING;
	}
	else if(method == "beginning") {
		si_onsay.method = OnSay::FILTER_MATCH_BEGINNING;
	}
	else if(method == "exact") {
		si_onsay.method = OnSay::FILTER_EXACT;
	}
	else {
		throw Error("Invalid argument (2) 'method'");
	}
	si_onsay.filter = filter;
	si_onsay.case_sensitive = case_sensitive;

	boost::any p(si_onsay);
	Listener_ptr listener(new Listener(ON_SAY_LISTENER, p, *this->getManager()));

	// OnSay event list is sorted by length, from longest to shortest
	if(si_onsay.method == OnSay::FILTER_EXACT){
		// Insert at beginning
		enviroment.Generic.OnSay.insert(enviroment.Generic.OnSay.begin(), listener);
	}
	else if(si_onsay.method == OnSay::FILTER_ALL){
		// All comes very last
		enviroment.Generic.OnSay.push_back(listener);
	}
	else{
		if(enviroment.Generic.OnSay.empty()){
			enviroment.Generic.OnSay.push_back(listener);
		}
		else{
			bool registered = false;
			for(ListenerList::iterator listener_iter = enviroment.Generic.OnSay.begin(),
				end = enviroment.Generic.OnSay.end();
				listener_iter != end; ++listener_iter)
			{
				OnSay::ScriptInformation info = boost::any_cast<OnSay::ScriptInformation>((*listener_iter)->getData());

				if(si_onsay.method == OnSay::FILTER_MATCH_BEGINNING){
					// We should be inserted before substrings...
					if(info.method == OnSay::FILTER_SUBSTRING || info.method == OnSay::FILTER_ALL){
						enviroment.Generic.OnSay.insert(listener_iter, listener);
						registered = true; 
						break;
					}

					if(info.filter.length() < si_onsay.filter.length()){
						enviroment.Generic.OnSay.insert(listener_iter, listener);
						registered = true; 
						break;
					}
				}
				else{
					assert(si_onsay.method == OnSay::FILTER_SUBSTRING);
					// We should be inserted before generic...
					if(info.method == OnSay::FILTER_ALL){
						enviroment.Generic.OnSay.insert(listener_iter, listener);
						registered = true; 
						break;
					}

					if(info.filter.length() < si_onsay.filter.length()){
						enviroment.Generic.OnSay.insert(listener_iter, listener);
						registered = true; 
						break;
					}
				}
			}
			if(!registered)
				enviroment.Generic.OnSay.push_back(listener);
		}
	}
	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerSpecificEvent_OnSay() {
	// Store callback
	insert(-5);

	std::string filter = popString();
	bool case_sensitive = popBoolean();
	std::string method = popString();
	Creature* who = popCreature();

	// Callback is no the top of the stack

	OnSay::ScriptInformation si_onsay;
	if(method == "all") {
		si_onsay.method = OnSay::FILTER_ALL;
	}
	else if(method == "substring") {
		si_onsay.method = OnSay::FILTER_SUBSTRING;
	}
	else if(method == "beginning") {
		si_onsay.method = OnSay::FILTER_MATCH_BEGINNING;
	}
	else if(method == "exact") {
		si_onsay.method = OnSay::FILTER_EXACT;
	}
	else {
		throw Error("Invalid argument (2) 'method'");
	}
	si_onsay.filter = filter;
	si_onsay.case_sensitive = case_sensitive;

	// This here explains why boost is so awesome, thanks to our custom
	// delete function, the listener is cleanly stopped when all references
	// to it is removed. :)
	boost::any p(si_onsay);
	Listener_ptr listener(
		new Listener(ON_SAY_LISTENER, p, *this->getManager()),
		boost::bind(&Listener::deactivate, _1));

	enviroment.registerSpecificListener(listener);
	who->addListener(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnUseItem() {
	// Store callback
	insert(-3);

	int id = popInteger();
	std::string method = popString();

	OnUseItem::ScriptInformation si_onuse;
	if(method == "itemid") {
		si_onuse.method = OnUseItem::FILTER_ITEMID;
	}
	else if(method == "actionid") {
		si_onuse.method = OnUseItem::FILTER_ACTIONID;
	}
	else if(method == "uniqueid") {
		si_onuse.method = OnUseItem::FILTER_UNIQUEID;
	}
	else {
		throw Error("Invalid argument (1) 'method'");
	}
	si_onuse.id = id;

	boost::any p(si_onuse);
	Listener_ptr listener(new Listener(ON_USE_ITEM_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnUseItem.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnJoinChannel() {
	boost::any p(0);
	Listener_ptr listener(new Listener(ON_OPEN_CHANNEL_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnJoinChannel.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerSpecificEvent_OnJoinChannel() {
	// Store callback
	insert(-2);

	Player* who = popPlayer();

	boost::any p(0);
	Listener_ptr listener(
		new Listener(ON_OPEN_CHANNEL_LISTENER, p, *this->getManager()),
		boost::bind(&Listener::deactivate, _1));

	enviroment.registerSpecificListener(listener);
	who->addListener(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnLeaveChannel() {
	boost::any p(0);
	Listener_ptr listener(new Listener(ON_CLOSE_CHANNEL_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnLeaveChannel.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerSpecificEvent_OnLeaveChannel() {
	// Store callback
	insert(-2);

	Player* who = popPlayer();

	boost::any p(0);
	Listener_ptr listener(
		new Listener(ON_CLOSE_CHANNEL_LISTENER, p, *this->getManager()),
		boost::bind(&Listener::deactivate, _1));

	enviroment.registerSpecificListener(listener);
	who->addListener(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnLogin() {
	boost::any p(0);
	Listener_ptr listener(new Listener(ON_LOGIN_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnLogin.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnLogout() {
	boost::any p(0);
	Listener_ptr listener(new Listener(ON_LOGOUT_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnLogout.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerSpecificEvent_OnLogout() {
	// Store callback
	insert(-2);

	Player* who = popPlayer();

	boost::any p(0);
	Listener_ptr listener(
		new Listener(ON_LOGOUT_LISTENER, p, *this->getManager()),
		boost::bind(&Listener::deactivate, _1));

	enviroment.registerSpecificListener(listener);
	who->addListener(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnLookAtItem() {
	// Store callback
	insert(-3);

	uint32_t id = popInteger();
	std::string method = popString();

	OnLook::ScriptInformation si_onlook;
	if(method == "itemid") {
		si_onlook.method = OnLook::FILTER_ITEMID;
	}
	else if(method == "actionid") {
		si_onlook.method = OnLook::FILTER_ACTIONID;
	}
	else if(method == "uniqueid") {
		si_onlook.method = OnLook::FILTER_UNIQUEID;
	}
	else {
		throw Error("Invalid argument (1) 'method'");
	}
	si_onlook.id = id;

	boost::any p(si_onlook);
	Listener_ptr listener(new Listener(ON_LOOK_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnLook.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnLookAtCreature() {
	// Store callback
	insert(-3);

	Creature* at = popCreature();

	OnLook::ScriptInformation si_onlook;
	si_onlook.method = OnLook::FILTER_CREATUREID;
	si_onlook.id = at->getID();

	boost::any p(si_onlook);
	Listener_ptr listener(new Listener(ON_LOOK_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnLook.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerSpecificEvent_OnLook() {
	// Store callback
	insert(-2);

	Creature* who = popCreature();

	OnLook::ScriptInformation si_onlook;
	si_onlook.method = OnLook::FILTER_NONE;
	si_onlook.id = 0;

	// This here explains why boost is so awesome, thanks to our custom
	// delete function, the listener is cleanly stopped when all references
	// to it is removed. :)
	boost::any p(si_onlook);
	Listener_ptr listener(
		new Listener(ON_LOOK_LISTENER, p, *this->getManager()),
		boost::bind(&Listener::deactivate, _1));

	enviroment.registerSpecificListener(listener);
	who->addListener(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnEquipItem() {
	// Store callback
	insert(-4);

	std::string slot = popString();
	int id = popInteger();
	std::string method = popString();

	OnEquipItem::ScriptInformation si_onequip;

	if(slot == "head") {
		si_onequip.slot = SLOTP_HEAD;
	}
	else if(slot == "neck") {
		si_onequip.slot = SLOTP_NECKLACE;
	}
	else if(slot == "backpack") {
		si_onequip.slot = SLOTP_BACKPACK;
	}
	else if(slot == "armor") {
		si_onequip.slot = SLOTP_ARMOR;
	}
	else if(slot == "right-hand") {
		si_onequip.slot = SLOTP_RIGHT;
	}
	else if(slot == "left-hand") {
		si_onequip.slot = SLOTP_LEFT;
	}
	else if(slot == "legs") {
		si_onequip.slot = SLOTP_LEGS;
	}
	else if(slot == "feet") {
		si_onequip.slot = SLOTP_FEET;
	}
	else if(slot == "ring") {
		si_onequip.slot = SLOTP_RING;
	}
	else if(slot == "ammo") {
		si_onequip.slot = SLOTP_AMMO;
	}
	else if(slot == "hand") {
		si_onequip.slot = (SLOTP_LEFT | SLOTP_RIGHT);
	}
	else if(slot == "any") {
		si_onequip.slot = (SLOTP_HEAD | SLOTP_NECKLACE | SLOTP_BACKPACK |
			SLOTP_ARMOR | SLOTP_RIGHT | SLOTP_LEFT | SLOTP_LEGS |
			SLOTP_FEET | SLOTP_RING | SLOTP_AMMO);
	}
	else{
		throw Error("Invalid argument (3) 'slot'");
	}

	if(method == "itemid") {
		si_onequip.method = OnEquipItem::FILTER_ITEMID;
	}
	else if(method == "actionid") {
		si_onequip.method = OnEquipItem::FILTER_ACTIONID;
	}
	else if(method == "uniqueid") {
		si_onequip.method = OnEquipItem::FILTER_UNIQUEID;
	}
	else {
		throw Error("Invalid argument (1) 'method'");
	}
	si_onequip.id = id;
	si_onequip.equip = true;

	boost::any p(si_onequip);
	Listener_ptr listener(new Listener(ON_EQUIP_ITEM_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnEquipItem.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnDeEquipItem() {
	// Store callback
	insert(-4);

	std::string slot = popString();
	int id = popInteger();
	std::string method = popString();

	OnEquipItem::ScriptInformation si_ondeequip;

	if(slot == "head") {
		si_ondeequip.slot = SLOTP_HEAD;
	}
	else if(slot == "neck") {
		si_ondeequip.slot = SLOTP_NECKLACE;
	}
	else if(slot == "backpack") {
		si_ondeequip.slot = SLOTP_BACKPACK;
	}
	else if(slot == "armor") {
		si_ondeequip.slot = SLOTP_ARMOR;
	}
	else if(slot == "right-hand") {
		si_ondeequip.slot = SLOTP_RIGHT;
	}
	else if(slot == "left-hand") {
		si_ondeequip.slot = SLOTP_LEFT;
	}
	else if(slot == "legs") {
		si_ondeequip.slot = SLOTP_LEGS;
	}
	else if(slot == "feet") {
		si_ondeequip.slot = SLOTP_FEET;
	}
	else if(slot == "ring") {
		si_ondeequip.slot = SLOTP_RING;
	}
	else if(slot == "ammo") {
		si_ondeequip.slot = SLOTP_AMMO;
	}
	else if(slot == "hand") {
		si_ondeequip.slot = (SLOTP_LEFT | SLOTP_RIGHT);
	}
	else if(slot == "any") {
		si_ondeequip.slot = (SLOTP_HEAD | SLOTP_NECKLACE | SLOTP_BACKPACK |
			SLOTP_ARMOR | SLOTP_RIGHT | SLOTP_LEFT | SLOTP_LEGS |
			SLOTP_FEET | SLOTP_RING | SLOTP_AMMO);
	}
	else{
		throw Error("Invalid argument (3) 'slot'");
	}

	if(method == "itemid") {
		si_ondeequip.method = OnEquipItem::FILTER_ITEMID;
	}
	else if(method == "actionid") {
		si_ondeequip.method = OnEquipItem::FILTER_ACTIONID;
	}
	else if(method == "uniqueid") {
		si_ondeequip.method = OnEquipItem::FILTER_UNIQUEID;
	}
	else {
		throw Error("Invalid argument (1) 'method'");
	}
	si_ondeequip.id = id;
	si_ondeequip.equip = false;

	boost::any p(si_ondeequip);
	Listener_ptr listener(new Listener(ON_EQUIP_ITEM_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnEquipItem.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnStepInCreature() {
	// Store callback
	insert(-3);

	int id = popInteger();
	std::string method = popString();

	OnMoveCreature::ScriptInformation si_onmovecreature;
	if(method == "itemid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_ITEMID;
	}
	else if(method == "actionid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_ACTIONID;
	}
	else if(method == "uniqueid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_UNIQUEID;
	}
	else {
		throw Error("Invalid argument (1) 'method'");
	}
	si_onmovecreature.slot = 0;
	si_onmovecreature.id = id;
	si_onmovecreature.moveType = OnMoveCreature::TYPE_STEPIN;

	boost::any p(si_onmovecreature);
	Listener_ptr listener(new Listener(ON_MOVE_CREATURE_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnMoveCreature.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnStepOutCreature() {
	// Store callback
	insert(-3);

	int id = popInteger();
	std::string method = popString();

	OnMoveCreature::ScriptInformation si_onmovecreature;
	if(method == "itemid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_ITEMID;
	}
	else if(method == "actionid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_ACTIONID;
	}
	else if(method == "uniqueid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_UNIQUEID;
	}
	else {
		throw Error("Invalid argument (1) 'method'");
	}
	si_onmovecreature.slot = 0;
	si_onmovecreature.id = id;
	si_onmovecreature.moveType = OnMoveCreature::TYPE_STEPOUT;

	boost::any p(si_onmovecreature);
	Listener_ptr listener(new Listener(ON_MOVE_CREATURE_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnMoveCreature.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

/*
int LuaState::lua_registerGenericEvent_OnMoveCreature() {
	// Store callback
	insert(-3);

	int id = popInteger();
	std::string method = popString();

	OnMoveCreature::ScriptInformation si_onmovecreature;
	if(method == "itemid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_ITEMID;
	}
	else if(method == "actionid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_ACTIONID;
	}
	else if(method == "uniqueid") {
		si_onmovecreature.method = OnMoveCreature::FILTER_UNIQUEID;
	}
	else {
		throw Error("Invalid argument (1) 'method'");
	}
	si_onmovecreature.id = id;
	si_onmovecreature.moveType = OnMoveCreature::TYPE_MOVE;

	boost::any p(si_onmovecreature);
	Listener_ptr listener(new Listener(ON_MOVE_CREATURE_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnMoveCreature.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}
*/

int LuaState::lua_registerSpecificEvent_OnMoveCreature() {
	// Store callback
	insert(-2);

	Creature* who = popCreature();

	OnMoveCreature::ScriptInformation si_onmovecreature;
	si_onmovecreature.method = OnMoveCreature::FILTER_NONE;
	si_onmovecreature.id = 0;
	si_onmovecreature.slot = 0;
	si_onmovecreature.moveType = OnMoveCreature::TYPE_MOVE;

	boost::any p(si_onmovecreature);
	Listener_ptr listener(
		new Listener(ON_MOVE_CREATURE_LISTENER, p, *this->getManager()),
		boost::bind(&Listener::deactivate, _1));

	enviroment.registerSpecificListener(listener);
	who->addListener(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnCreatureTurn()
{
	OnTurn::ScriptInformation si_onturn;
	boost::any p(si_onturn);
	Listener_ptr listener(new Listener(ON_LOOK_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnTurn.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerSpecificEvent_OnCreatureTurn() {
	// Store callback
	insert(-2);

	Creature* who = popCreature();

	OnTurn::ScriptInformation si_onturn;

	boost::any p(si_onturn);
	Listener_ptr listener(
		new Listener(ON_TURN_LISTENER, p, *this->getManager()),
		boost::bind(&Listener::deactivate, _1));

	enviroment.registerSpecificListener(listener);
	who->addListener(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_registerGenericEvent_OnMoveItem() {
	// Store callback
	insert(-5);

	bool isAddItem = popBoolean();
	bool isItemOnTile = popBoolean();
	int id = popInteger();
	std::string method = popString();

	OnMoveItem::ScriptInformation si_onmoveitem;
	if(method == "itemid") {
		si_onmoveitem.method = OnMoveItem::FILTER_ITEMID;
	}
	else if(method == "actionid") {
		si_onmoveitem.method = OnMoveItem::FILTER_ACTIONID;
	}
	else if(method == "uniqueid") {
		si_onmoveitem.method = OnMoveItem::FILTER_UNIQUEID;
	}
	else {
		throw Error("Invalid argument (1) 'method'");
	}
	si_onmoveitem.id = id;
	si_onmoveitem.addItem = isAddItem;
	si_onmoveitem.isItemOnTile = isItemOnTile;

	boost::any p(si_onmoveitem);
	Listener_ptr listener(new Listener(ON_MOVE_ITEM_LISTENER, p, *this->getManager()));

	enviroment.Generic.OnMoveItem.push_back(listener);

	// Register event
	setRegistryItem(listener->getLuaTag());

	// Return listener
	pushString(listener->getLuaTag());
	return 1;
}

int LuaState::lua_stopListener() {
	std::string listener_id = popString();

	size_t _pos = listener_id.find("_") + 1;
	if(_pos == std::string::npos) {
		throw Error("Invalid argument to stopListener");
	}
	size_t _2pos = listener_id.find("_", _pos);
	if(_2pos == std::string::npos) {
		throw Error("Invalid argument to stopListener");
	}

	std::istringstream is(listener_id.substr(_pos, _2pos));
	uint32_t id, type;
	is >> type;
	if(!is) {
		pushBoolean(false);
		return 1;
	}
	is.str(listener_id.substr(_2pos+1));
	is >> id;
	if(!is) {
		pushBoolean(false);
		return 1;
	}
	pushBoolean(enviroment.stopListener((ListenerType)type, id));
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Member functions

///////////////////////////////////////////////////////////////////////////////
// Class Event

int LuaState::lua_Event_skip() {
	// Event table is ontop of stack
	setField(-1, "skipped", false);
	return 1;
}

int LuaState::lua_Event_propagate() {
	// Event table is ontop of stack
	setField(-1, "skipped", true);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Class Thing

int LuaState::lua_Thing_getPosition()
{
	Thing* thing = popThing();
	pushPosition(thing->getPosition());
	return 1;
}

int LuaState::lua_Thing_getParentTile()
{
	Thing* thing = popThing();
	pushThing(thing->getTile());
	return 1;
}

int LuaState::lua_Thing_getParent()
{
	Thing* thing = popThing();
	Cylinder* parent = thing->getParent();
	if(parent->getTile()) {
		pushTile(static_cast<Tile*>(parent));
	} else if(parent->getItem() || parent->getCreature()) {
		pushThing(parent);
	} else {
		pushNil();
	}
	return 1;
}

int LuaState::lua_Thing_moveToPosition()
{
	Position pos = popPosition();
	Thing* thing = popThing();

	pushBoolean(g_game.internalTeleport(NULL, thing, pos) == RET_NOERROR);

	return 1;
}

int LuaState::lua_Thing_isMoveable()
{
	Thing* thing = popThing();
	pushBoolean(thing->isPushable());
	return 1;
}

int LuaState::lua_Thing_getName() {
	Thing* t = popThing();
	if(Creature* c = t->getCreature()) {
		pushString(c->getName());
	} else if(Item* i = t->getItem()) {
		pushString(i->getName());
	} else {
		pushString("");
	}
	return 1;
}

int LuaState::lua_Thing_getDescription()
{
	int lookdistance = 0;
	if(getStackTop() > 1) {
		lookdistance = popInteger();
	}
	Thing* thing = popThing();
	pushString(thing->getDescription(lookdistance));
	return 1;
}

int LuaState::lua_Thing_destroy()
{
	Thing* thing = popThing();
	if(Item* item = thing->getItem()) {
		pushBoolean(g_game.internalRemoveItem(NULL, item) == RET_NOERROR);
	} else if(Creature* creature = thing->getCreature()) {
		if(Player* player = creature->getPlayer()) {
			player->kickPlayer();
			pushBoolean(true);
		} else {
			pushBoolean(g_game.removeCreature(creature));
		}
	} else {
		pushBoolean(false);
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Class Tile

int LuaState::lua_Tile_getThing()
{
	int index = popInteger(); // lua indices start at 1, but we don't give a crap!
	Tile* tile = popTile();

	// -1 is top item
	if(index < 0) {
		index = tile->__getLastIndex() + index;
	}
	if(index > tile->__getLastIndex()) {
		throw Error("Tile:getThing : Index out of range!");
	}

	pushThing(tile->__getThing(index));
	return 1;
}

int LuaState::lua_Tile_getCreatures()
{
	Tile* tile = popTile();

	newTable();
	int n = 1;
	for(CreatureVector::iterator iter = tile->creatures.begin(),
		end_iter = tile->creatures.end();
		iter != end_iter; ++iter, ++n)
	{
		pushThing(*iter);
		setField(-2, n);
	}
	return 1;
}

int LuaState::lua_Tile_getMoveableItems()
{
	Tile* tile = popTile();

	newTable();
	int n = 1;
	for(ItemVector::iterator iter = tile->downItems.begin(),
		end_iter = tile->downItems.end();
		iter != end_iter; ++iter)
	{
		if((*iter)->isNotMoveable() == false) {
			pushThing(*iter);
			setField(-2, n++);
		}
	}
	return 1;
}

int LuaState::lua_Tile_getItems()
{
	Tile* tile = popTile();

	newTable();
	int n = 1;
	if(tile->ground) {
		pushThing(tile->ground);
		setField(-2, n++);
	}

	for(ItemVector::iterator iter = tile->topItems.begin(),
		end_iter = tile->topItems.end();
		iter != end_iter; ++iter, ++n)
	{
		pushThing(*iter);
		setField(-2, n);
	}

	for(ItemVector::iterator iter = tile->downItems.begin(),
		end_iter = tile->downItems.end();
		iter != end_iter; ++iter, ++n)
	{
		pushThing(*iter);
		setField(-2, n);
	}
	return 1;
}

int LuaState::lua_Tile_getItemTypeCount() {
	int type = popInteger();
	Tile* tile = popTile();
	push(((Cylinder*)tile)->__getItemTypeCount(type));
	return 1;
}

int LuaState::lua_Tile_queryAdd()
{
	int flags = 0;
	if(getStackTop() > 2) {
		flags = popInteger();
	}
	Thing* thing = popThing();
	Tile* tile = popTile();

	pushInteger(tile->__queryAdd(0, thing, 1, flags));
	return 1;
}

int LuaState::lua_Tile_isPZ()
{
	Tile* tile = popTile();
	pushBoolean(tile->hasFlag(TILESTATE_PROTECTIONZONE));
	return 1;
}

int LuaState::lua_Tile_isNoPVP()
{
	Tile* tile = popTile();
	pushBoolean(tile->hasFlag(TILESTATE_NOPVPZONE));
	return 1;
}

int LuaState::lua_Tile_isPVP()
{
	Tile* tile = popTile();
	pushBoolean(tile->hasFlag(TILESTATE_PVPZONE));
	return 1;
}

int LuaState::lua_Tile_isNoLogout()
{
	Tile* tile = popTile();
	pushBoolean(tile->hasFlag(TILESTATE_NOLOGOUT));
	return 1;
}

int LuaState::lua_Tile_doesRefresh()
{
	Tile* tile = popTile();
	pushBoolean(tile->hasFlag(TILESTATE_REFRESH));
	return 1;
}

int LuaState::lua_Tile_addItem()
{
	Item* item = popItem(Script::ERROR_PASS);
	if(item == NULL) {
		pushNil();
		return 1;
	}
	Tile* tile = popTile();

	if(item->getParent() != VirtualCylinder::virtualCylinder){
		// Must remove from previous parent...
		g_game.internalRemoveItem(NULL, item);
	}

	ReturnValue ret = g_game.internalAddItem(NULL, tile, item);
	pushBoolean(ret == RET_NOERROR);
	return 1;
}

int LuaState::lua_Tile_hasProperty()
{
	int prop = popInteger();
	Tile* tile = popTile();
	pushBoolean(tile && tile->hasProperty((ITEMPROPERTY)prop));
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Class Creature

int LuaState::lua_Creature_getID() {
	Creature* creature = popCreature();
	pushUnsignedInteger(creature->getID());
	return 1;
}

int LuaState::lua_Creature_getHealth() {
	Creature* creature = popCreature();
	pushInteger(creature->getHealth());
	return 1;
}

int LuaState::lua_Creature_getHealthMax() {
	Creature* creature = popCreature();
	pushInteger(creature->getMaxHealth());
	return 1;
}

int LuaState::lua_Creature_setHealth() {
	int newval = popInteger();
	Creature* c = popCreature();

	if(newval >= c->getHealth()){
		g_game.combatChangeHealth(COMBAT_HEALING, NULL, c, newval - c->getHealth(), false);
	}
	else{
		g_game.combatChangeHealth(COMBAT_UNDEFINEDDAMAGE, NULL, c, newval - c->getHealth(), false);
	}

	push(true);
	return 1;
}

int LuaState::lua_Creature_getOrientation() {
	Creature* creature = popCreature();
	pushInteger(creature->getDirection());
	return 1;
}

int LuaState::lua_Creature_walk()
{
	int ndir = popInteger();
	Creature* creature = popCreature();

	switch(ndir){
		case NORTH:
		case SOUTH:
		case WEST:
		case EAST:
		case SOUTHWEST:
		case NORTHWEST:
		case NORTHEAST:
		case SOUTHEAST:
			break;
		default:
			throw Error("Creature:walk : Invalid direction");
	}

	ReturnValue ret = g_game.internalMoveCreature(NULL, creature, (Direction)ndir, FLAG_NOLIMIT);
	pushBoolean(ret == RET_NOERROR);
	return 1;
}

int LuaState::lua_Creature_say()
{
	std::string msg = popString();
	Creature* creature = popCreature();

	bool ret = g_game.internalCreatureSay(creature, SPEAK_SAY, msg);
	pushBoolean(ret);
	return 1;
}

int LuaState::lua_getCreatureByName()
{
	std::string name = popString();
	pushThing(g_game.getCreatureByName(name));
	return 1 ;
}

int LuaState::lua_getCreaturesByName()
{
	std::string name = popString();
	std::vector<Creature*> creatures = g_game.getCreaturesByName(name);

	int n = 1;
	newTable();
	for(std::vector<Creature*>::iterator i = creatures.begin(); i != creatures.end(); ++i){
		pushThing(*i);
		setField(-2, n++);
	}
	return 1 ;
}

///////////////////////////////////////////////////////////////////////////////
// Class Player

int LuaState::lua_Player_setStorageValue() {
	bool remove = false;
	std::string value;
	if(isNil(-1)) {
		pop();
		remove = true;
	} else {
		value = popString();
	}
	std::string key = popString();
	Player* player = popPlayer();

	if(remove)
		player->eraseStorageValue(key);
	else
		player->addStorageValue(key, value);

	pushBoolean(true);
	return 1;
}

int LuaState::lua_Player_getStorageValue() {
	std::string key = popString();
	Player* player = popPlayer();
	std::string value;
	if(player->getStorageValue(key, value)) {
		pushString(value);
	} else {
		pushNil();
	}
	return 1;
}

int LuaState::lua_Player_getFood() {
	Player* p = popPlayer();
	Condition* condition = p->getCondition(CONDITION_REGENERATION, CONDITIONID_DEFAULT, 0);
	if(condition){
		push(condition->getTicks() / 1000);
	}
	else{
		pushInteger(0);
	}
	return 1;
}

int LuaState::lua_Player_getLevel() {
	Player* p = popPlayer();
	push(p->getLevel());
	return 1;
}

int LuaState::lua_Player_getMagicLevel() {
	Player* p = popPlayer();
	push(p->getMagicLevel());
	return 1;
}

int LuaState::lua_Player_getAccess() {
	Player* p = popPlayer();
	push(p->getAccessLevel());
	return 1;
}

int LuaState::lua_Player_getMana() {
	Player* p = popPlayer();
	push(p->getMana());
	return 1;
}

int LuaState::lua_Player_getManaMax() {
	Player* p = popPlayer();
	push(p->getMaxMana());
	return 1;
}

int LuaState::lua_Player_setMana() {
	int newval = popInteger();
	Player* p = popPlayer();
	g_game.combatChangeMana(NULL, p, newval - p->getMana(), false);
	push(true);
	return 1;
}

int LuaState::lua_Player_addManaSpent() {
	int mana = popInteger();
	Player* p = popPlayer();
	p->addManaSpent(mana);
	push(true);
	return 1;
}

int LuaState::lua_Player_getVocationID() {
	Player* p = popPlayer();
	push(p->getVocationId());
	return 1;
}

int LuaState::lua_Player_getSoulPoints() {
	Player* p = popPlayer();
	push(p->getPlayerInfo(PLAYERINFO_SOUL));
	return 1;
}

int LuaState::lua_Player_setSoulPoints() {
	int soul = popInteger();
	Player* p = popPlayer();
	p->changeSoul(soul - p->getPlayerInfo(PLAYERINFO_SOUL));
	push(true);
	return 1;
}

int LuaState::lua_Player_getFreeCap() {
	Player* p = popPlayer();
	push(p->getFreeCapacity());
	return 1;
}

int LuaState::lua_Player_getMaximumCap() {
	Player* p = popPlayer();
	push(p->getCapacity());
	return 1;
}

int LuaState::lua_Player_getGuildID() {
	Player* p = popPlayer();
	push(p->getGuildId());
	return 1;
}

int LuaState::lua_Player_getGuildName() {
	Player* p = popPlayer();
	push(p->getGuildName());
	return 1;
}

int LuaState::lua_Player_getGuildRank() {
	Player* p = popPlayer();
	push(p->getGuildRank());
	return 1;
}

int LuaState::lua_Player_getGuildNick() {
	Player* p = popPlayer();
	push(p->getGuildNick());
	return 1;
}

int LuaState::lua_Player_getSex() {
	Player* p = popPlayer();
	push(p->getSex() == PLAYERSEX_MALE);
	return 1;
}

int LuaState::lua_Player_getTownID() {
	Player* p = popPlayer();
	push(p->getTown());
	return 1;
}

int LuaState::lua_Player_getGUID() {
	Player* p = popPlayer();
	push(p->getGUID());
	return 1;
}

int LuaState::lua_Player_getGroup() {
	Player* p = popPlayer();
	push(p->getAccessGroup());
	return 1;
}

int LuaState::lua_Player_getPremiumDays() {
	Player* p = popPlayer();
	push(p->getPremiumDays());
	return 1;
}

int LuaState::lua_Player_getLastLogin() {
	Player* p = popPlayer();
	pushUnsignedInteger(p->getLastLoginSaved());
	return 1;
}

int LuaState::lua_Player_getSkullType() {
	Player* p = popPlayer();
#ifdef __SKULLSYSTEM__
	push(p->getSkull());
#else
	pushInteger(0);
#endif
	return 1;
}
int LuaState::lua_Player_hasGroupFlag()
{
	int f = popInteger();
	Player* player = popPlayer();
	if(f < 0 || f >= PlayerFlag_LastFlag)
		throw Error("Invalid player flag passed to function Player.hasGroupFlag!");
	pushBoolean(player->hasFlag((PlayerFlags)f));
	return 1;
}

int LuaState::lua_Player_addExperience() {
	int64_t exp = (int64_t)popFloat();
	Player* p = popPlayer();
	p->addExperience(exp);
	return 1;
}

int LuaState::lua_Player_getInventoryItem()
{
	int slot = popInteger();
	Player* player = popPlayer();

	if(slot < SLOT_FIRST || slot > SLOT_LAST)
	{
		throw Error("Player.getInventoryItem: slot out of range!");
	}

	Item* i = player->getInventoryItem((slots_t)slot);
	if(i)
		pushThing(i);
	else
		pushNil();

	return 1;
}

int LuaState::lua_Player_getItemTypeCount() {
	int type = popInteger();
	Player* p = popPlayer();
	push(((Cylinder*)p)->__getItemTypeCount(type));
	return 1;
}

int LuaState::lua_Player_getMoney()
{
	Player* player = popPlayer();
	pushInteger(g_game.getMoney(player));
	return 1;
}

int LuaState::lua_Player_removeMoney()
{
	int amount = popInteger();
	Player* player = popPlayer();
	pushBoolean(g_game.removeMoney(NULL, player, amount));
	return 1;
}

int LuaState::lua_Player_addMoney()
{
	int amount = popInteger();
	Player* player = popPlayer();
	pushBoolean(g_game.addMoney(NULL, player, amount));
	return 1;
}

int LuaState::lua_Player_setVocation()
{
	int vocationID = popInteger();
	Player* player = popPlayer();

	if(g_vocations.getVocation(vocationID)) {
		player->setVocation(vocationID);
		pushBoolean(true);
	} else {
		pushBoolean(false);
	}
	return 1;
}

int LuaState::lua_Player_setTown()
{
	Town* town = popTown();
	Player* player = popPlayer();

	player->setMasterPos(town->getTemplePosition());
	player->setTown(town->getTownID());
	pushBoolean(true);

	return 1;
}

int LuaState::lua_Player_sendMessage()
{
	std::string text = popString();
	uint32_t messageClass = popUnsignedInteger();
	Player* player = popPlayer();

	player->sendTextMessage((MessageClasses)messageClass, text);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Player_addItem()
{
	bool canDropOnMap = true;

	Item* item = popItem(ERROR_PASS);
	if(item == NULL) {
		pushNil();
		return 1;
	}
	Player* player = popPlayer();

	if(item->getParent() != VirtualCylinder::virtualCylinder){
		// Must remove from previous parent...
		g_game.internalRemoveItem(NULL, item);
	}

	ReturnValue ret = RET_NOERROR;
	if(canDropOnMap){
		ret = g_game.internalPlayerAddItem(player, item);
	}
	else{
		ret = g_game.internalAddItem(NULL, player, item);
	}
	pushBoolean(ret == RET_NOERROR);
	return 1;
}

int LuaState::lua_getOnlinePlayers()
{
	newTable();
	int n = 1;
	for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
		if(!it->second->isRemoved()){
			pushThing(it->second);
			setField(-2, n++);
		}
	}
	return 1 ;
}

int LuaState::lua_getPlayerByName()
{
	std::string name = popString();
	pushThing(g_game.getPlayerByName(name));
	return 1 ;
}

int LuaState::lua_getPlayersByName()
{
	std::string name = popString();
	std::vector<Player*> players = g_game.getPlayersByName(name);

	int n = 1;
	newTable();
	for(std::vector<Player*>::iterator i = players.begin(); i != players.end(); ++i){
		pushThing(*i);
		setField(-2, n++);
	}
	return 1 ;
}

int LuaState::lua_getPlayerByNameWildcard()
{
	std::string name = popString();

	Player* p = NULL;
	ReturnValue ret = g_game.getPlayerByNameWildcard(name, p);

	pushInteger(ret);
	pushThing(p);
	return 2;
}

int LuaState::lua_getPlayersByNameWildcard()
{
	std::string name = popString();
	std::vector<Player*> players = g_game.getPlayersByNameWildcard(name);

	int n = 1;
	newTable();
	for(std::vector<Player*>::iterator i = players.begin(); i != players.end(); ++i){
		pushThing(*i);
		setField(-2, n++);
	}
	return 1 ;
}

///////////////////////////////////////////////////////////////////////////////
// Class Item

int LuaState::lua_createItem()
{
	int count = -1;
	if(getStackTop() > 1) {
		count = popInteger();
	}
	int id = popUnsignedInteger();

	Item* item = Item::CreateItem((uint16_t)id, (count < 0? 0 : count));
	item->useThing2();
	pushThing(item);
	// It will be freed if not assigned to any parent
	g_game.FreeThing(item);
	return 1;
}

int LuaState::lua_Item_getItemID()
{
	Item* item = popItem();
	pushInteger(item->getID());
	return 1;
}

int LuaState::lua_Item_getActionID()
{
	Item* item = popItem();
	pushInteger(item->getActionId());
	return 1;
}

int LuaState::lua_Item_getUniqueID()
{
	Item* item = popItem();
	pushInteger(item->getUniqueId());
	return 1;
}

int LuaState::lua_Item_getCount()
{
	Item* item = popItem();
	if(item->isStackable()) {
		pushInteger(item->getItemCount());
	} else {
		pushInteger(1);
	}
	return 1;
}

int LuaState::lua_Item_getWeight()
{
	Item* item = popItem();
	pushFloat(item->getWeight());
	return 1;
}

int LuaState::lua_Item_isPickupable()
{
	Item* item = popItem();
	pushBoolean(item->isPickupable());
	return 1;
}

int LuaState::lua_Item_getText()
{
	Item* item = popItem();
	pushString(item->getText());
	return 1;
}

int LuaState::lua_Item_getSpecialDescription()
{
	Item* item = popItem();
	pushInteger(item->getID());
	return 1;
}

int LuaState::lua_Item_setItemID()
{
	int newcount = -1;
	if(getStackTop() > 2) {
		newcount = popInteger();
	}

	int newid = popInteger();
	Item* item = popItem();

	if(newid < 0 || newid > 65535) {
		throw Error("Item:setItemID : item ID provided");
	}

	const ItemType& it = Item::items[newid];
	if(it.stackable && newcount > 100 || newcount < -1){
		throw Error("Item:setItemID : Stack count cannot be higher than 100.");
	}

	g_game.transformItem(NULL, item, newid, newcount);

	pushBoolean(true);
	return 1;
}

int LuaState::lua_Item_startDecaying()
{
	Item* item = popItem();
	g_game.startDecay(item);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Item_setCount()
{
	int newcount = popInteger();
	Item* item = popItem();
	if(!item->isStackable()) {
		throw Error("Item.setCount: Item is not stackable!");
	}
	if(newcount < 1) {
		throw Error("Item.setCount: New count out of range!");
	}

	pushBoolean(g_game.transformItem(NULL, item, item->getID(), newcount) != NULL);
	return 1;
}

int LuaState::lua_Item_setText()
{
	std::string text = popString();
	Item* item = popItem();
	item->setText(text);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Item_setSpecialDescription()
{
	std::string text = popString();
	Item* item = popItem();
	item->setSpecialDescription(text);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Item_setActionID()
{
	int actionid = popInteger();
	Item* item = popItem();
	item->setActionId(actionid);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_getItemIDByName()
{
	std::string name = popString();

	int32_t itemid = Item::items.getItemIdByName(name);
	if(itemid == -1){
		pushNil();
	} else {
		pushUnsignedInteger(itemid);
	}
	return 1;
}

int LuaState::lua_isValidItemID()
{
	int32_t id = popInteger();

	const ItemType& it = Item::items[(uint16_t)id];
	pushBoolean(id >= 100 && it.id != 0);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Combat functions

#if 0

int LuaState::lua_Combat_create()
{
	pushCombat(new Combat);
	return 1;
}

bool LuaState::getArea(std::list<uint32_t>& list, uint32_t& rows)
{
	rows = 0;
	uint32_t i = 0, j = 0;
	pushNil();  // first key //

	while(lua_next(L, -2) != 0){
		pushNil();
		while(lua_next(L, -2) != 0){
			list.push_back(popNumber());
			++j;
		}

		++rows;

		j = 0;
		pop();
		++i;
	}

	pop();
	return (rows != 0);
}

int LuaState::lua_CombatArea_create()
{
	//createCombatArea( {area}, <optional> {extArea} )
	int32_t parameters = lua_gettop(m_luaState);

	AreaCombat* area = new AreaCombat;

	if(parameters > 1)
	{
		//has extra parameter with diagonal area information

		uint32_t rowsExtArea;
		std::list<uint32_t> listExtArea;
		getArea(listExtArea, rowsExtArea);

		// setup all possible rotations
		area->setupExtArea(listExtArea, rowsExtArea);
	}

	uint32_t rowsArea = 0;
	std::list<uint32_t> listArea;
	getArea(listArea, rowsArea);

	// setup all possible rotations
	area->setupArea(listArea, rowsArea);

	pushCombatArea(area);
	return 1;
}

int LuaState::lua_Condition_create()
{
	ConditionType_t type = (ConditionType_t)popNumber();

	Condition* condition = Condition::createCondition(CONDITIONID_COMBAT, type, 0, 0);
	if(!condition)
		throw Error("createCondition: Invalid argument 'type', unknown condition type!");

	pushCondition(condition);
	return 1;
}

int LuaState::lua_Combat_setArea()
{
	AreaCombat* area = popCombatArea();
	Combat* combat = popCombat();

	combat->setArea(new AreaCombat(*area));

	pushBoolean(true);
	return 1;
}

int LuaState::lua_Combat_setCondition()
{
	Condition* condition = popCondition();
	Combat* combat = popCombat();

	combat->setCondition(condition);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Combat_setParameter()
{
	uint32_t value = popUnsignedInteger();
	CombatParam_t key = (CombatParam_t)popInteger();
	Combat* combat = popCombat();

	combat->setParam(key, value);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Condition_setParameter()
{
	//setConditionParam(condition, key, value)
	int32_t value = popInteger();
	ConditionParam_t key = (ConditionParam_t)popInteger();
	Condition* condition = popCondition();

	condition->setParam(key, value);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Condition_addDamage()
{
	//addDamageCondition(condition, rounds, time, value)
	int32_t value = popInteger();
	int32_t time = popInteger();
	int32_t rounds  = popInteger();
	ConditionDamage* condition = dynamic_cast<ConditionDamage*>(popCondition());

	if(!condition)
		throw Error("Condition.addDamage: Condition is not a Damage Condition!");
	condition->addDamage(rounds, time, value);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Condition_addOutfit()
{
	Outfit_t outfit = popOutfit();
	ConditionOutfit* condition = dynamic_cast<ConditionOutfit*>(popCondition());

	if(!condition)
		throw Error("Condition.addOutfit: Condition is not an Outfit Condition!");
	condition->addOutfit(outfit);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_Combat_setCallback()
{
	//setCombatCallBack(combat, key, function)
	std::string function = putAwayFunction(function_str); // Save function
	CallBackParam_t key = (CallBackParam_t)popNumber();
	Combat* combat = popCombat();
	
	combat->setCallback(
	//setCombatCallBack(combat, key, function_name)
	const char* function = popString(L);
	std::string function_str(function);
	CallBackParam_t key = (CallBackParam_t)popNumber(L);
	uint32_t combatId = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	if(env->getScriptId() != EVENT_ID_LOADING){
		reportError(__FUNCTION__, "This function can only be used while loading the script.");
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Combat* combat = env->getCombatObject(combatId);

	if(!combat){
		throwLuaException(getErrorDesc(LUA_ERROR_COMBAT_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	LuaScriptInterface* scriptInterface = env->getScriptInterface();

	combat->setCallback(key);
	CallBack* callback = combat->getCallback(key);
	if(!callback){
		std::stringstream ss;
		ss << (uint32_t)key << " is not a valid callback key";
		reportError(__FUNCTION__, ss.str());
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	if(!callback->loadCallBack(scriptInterface, function_str)){
		reportError(__FUNCTION__, "Can not load callback");
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	lua_pushnumber(L, LUA_NO_ERROR);
	return 1;
}

int LuaState::luaSetCombatFormula(lua_State *L)
{
	//setCombatFormula(combat, type, mina, minb, maxa, maxb)
	ScriptEnviroment* env = getScriptEnv();

	if(env->getScriptId() != EVENT_ID_LOADING){
		reportError(__FUNCTION__, "This function can only be used while loading the script.");
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	double maxb = popFloatNumber(L);
	double maxa = popFloatNumber(L);
	double minb = popFloatNumber(L);
	double mina = popFloatNumber(L);

	formulaType_t type = (formulaType_t)popNumber(L);
	uint32_t combatId = popNumber(L);

	Combat* combat = env->getCombatObject(combatId);

	if(combat){
		combat->setPlayerCombatValues(type, mina, minb, maxa, maxb);

		lua_pushnumber(L, LUA_NO_ERROR);
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_COMBAT_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}
	return 1;
}

int LuaState::luaSetConditionFormula(lua_State *L)
{
	//setConditionFormula(condition, mina, minb, maxa, maxb)
	ScriptEnviroment* env = getScriptEnv();

	if(env->getScriptId() != EVENT_ID_LOADING){
		reportError(__FUNCTION__, "This function can only be used while loading the script.");
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	double maxb = popFloatNumber(L);
	double maxa = popFloatNumber(L);
	double minb = popFloatNumber(L);
	double mina = popFloatNumber(L);

	uint32_t conditionId = popNumber(L);

	ConditionSpeed* condition = dynamic_cast<ConditionSpeed*>(env->getConditionObject(conditionId));

	if(condition){
		condition->setFormulaVars(mina, minb, maxa, maxb);

		lua_pushnumber(L, LUA_NO_ERROR);
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_CONDITION_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}
	return 1;
}

int LuaState::luaDoCombat(lua_State *L)
{
	//doCombat(cid, combat, param)
	ScriptEnviroment* env = getScriptEnv();

	LuaVariant var = popVariant(L);
	uint32_t combatId = (uint32_t)popNumber(L);
	uint32_t cid = (uint32_t)popNumber(L);

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);

		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	const Combat* combat = env->getCombatObject(combatId);

	if(!combat){
		throwLuaException(getErrorDesc(LUA_ERROR_COMBAT_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	if(var.type == VARIANT_NONE){
		throwLuaException(getErrorDesc(LUA_ERROR_VARIANT_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	switch(var.type){
		case VARIANT_NUMBER:
		{
			Creature* target = g_game.getCreatureByID(var.number);

			if(!target){
				lua_pushnumber(L, LUA_ERROR);
				return 1;
			}

			if(combat->hasArea()){
				combat->doCombat(creature, target->getPosition());
				//std::cout << "Combat->hasArea()" << std::endl;
			}
			else{
				combat->doCombat(creature, target);
			}
			break;
		}

		case VARIANT_POSITION:
		{
			combat->doCombat(creature, var.pos);
			break;
		}

		case VARIANT_TARGETPOSITION:
		{
			if(combat->hasArea()){
				combat->doCombat(creature, var.pos);
			}
			else{
				combat->postCombatEffects(creature, var.pos);
				g_game.addMagicEffect(var.pos, NM_ME_PUFF);
			}
			break;
		}

		case VARIANT_STRING:
		{
			Player* target = g_game.getPlayerByName(var.text);
			if(!target){
				lua_pushnumber(L, LUA_ERROR);
				return 1;
			}

			combat->doCombat(creature, target);
			break;
		}

		default:
		{
			throwLuaException(getErrorDesc(LUA_ERROR_VARIANT_UNKNOWN));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
			break;
		}
	}

	lua_pushnumber(L, LUA_NO_ERROR);
	return 1;
}

int LuaState::luaDoAreaCombatHealth(lua_State *L)
{
	//doAreaCombatHealth(cid, type, pos, area, min, max, effect)
	uint8_t effect = (uint8_t)popNumber(L);
	int32_t maxChange = (int32_t)popNumber(L);
	int32_t minChange = (int32_t)popNumber(L);
	uint32_t areaId = popNumber(L);

	PositionEx pos;
	popPosition(L, pos);

	CombatType_t combatType = (CombatType_t)popNumber(L);
	uint32_t cid = (uint32_t)popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);

		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	const AreaCombat* area = env->getCombatArea(areaId);
	if(area || areaId == 0){
		CombatParams params;
		params.combatType = combatType;
		params.impactEffect = effect;
		Combat::doCombatHealth(creature, pos, area, minChange, maxChange, params);

		lua_pushnumber(L, LUA_NO_ERROR);
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_AREA_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}

	return 1;
}

int LuaState::luaDoTargetCombatHealth(lua_State *L)
{
	//doTargetCombatHealth(cid, target, type, min, max, effect)
	uint8_t effect = (uint8_t)popNumber(L);
	int32_t maxChange = (int32_t)popNumber(L);
	int32_t minChange = (int32_t)popNumber(L);
	CombatType_t combatType = (CombatType_t)popNumber(L);
	uint32_t targetCid = popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);

		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	Creature* target = env->getCreatureByUID(targetCid);
	if(target){
		CombatParams params;
		params.combatType = combatType;
		params.impactEffect = effect;
		Combat::doCombatHealth(creature, target, minChange, maxChange, params);

		lua_pushnumber(L, LUA_NO_ERROR);
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}
	return 1;
}

int LuaState::luaDoAreaCombatMana(lua_State *L)
{
	//doAreaCombatMana(cid, pos, area, min, max, effect)
	uint8_t effect = (uint8_t)popNumber(L);
	int32_t maxChange = (int32_t)popNumber(L);
	int32_t minChange = (int32_t)popNumber(L);
	uint32_t areaId = popNumber(L);

	PositionEx pos;
	popPosition(L, pos);

	uint32_t cid = (uint32_t)popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);

		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	const AreaCombat* area = env->getCombatArea(areaId);
	if(area || areaId == 0){
		CombatParams params;
		params.impactEffect = effect;
		Combat::doCombatMana(creature, pos, area, minChange, maxChange, params);

		lua_pushnumber(L, LUA_NO_ERROR);
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_AREA_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}
	return 1;
}

int LuaState::luaDoTargetCombatMana(lua_State *L)
{
	//doTargetCombatMana(cid, target, min, max, effect)
	uint8_t effect = (uint8_t)popNumber(L);
	int32_t maxChange = (int32_t)popNumber(L);
	int32_t minChange = (int32_t)popNumber(L);
	uint32_t targetCid = popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);

		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	Creature* target = env->getCreatureByUID(targetCid);
	if(target){
		CombatParams params;
		params.impactEffect = effect;
		Combat::doCombatMana(creature, target, minChange, maxChange, params);

		lua_pushnumber(L, LUA_NO_ERROR);
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}
	return 1;
}

int LuaState::luaDoAreaCombatCondition(lua_State *L)
{
	//doAreaCombatCondition(cid, pos, area, condition, effect)
	uint8_t effect = (uint8_t)popNumber(L);
	uint32_t conditionId = popNumber(L);
	uint32_t areaId = popNumber(L);
	PositionEx pos;
	popPosition(L, pos);
	uint32_t cid = (uint32_t)popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);
		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	const Condition* condition = env->getConditionObject(conditionId);
	if(condition){

		const AreaCombat* area = env->getCombatArea(areaId);

		if(area || areaId == 0){
			CombatParams params;
			params.impactEffect = effect;
			params.condition = condition;
			Combat::doCombatCondition(creature, pos, area, params);

			lua_pushnumber(L, LUA_NO_ERROR);
		}
		else{
			throwLuaException(getErrorDesc(LUA_ERROR_AREA_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
		}
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_CONDITION_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}
	return 1;
}

int LuaState::luaDoTargetCombatCondition(lua_State *L)
{
	//doTargetCombatCondition(cid, target, condition, effect)
	uint8_t effect = (uint8_t)popNumber(L);
	uint32_t conditionId = popNumber(L);
	uint32_t targetCid = popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);

		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	Creature* target = env->getCreatureByUID(targetCid);
	if(target){
		const Condition* condition = env->getConditionObject(conditionId);
		if(condition){
			CombatParams params;
			params.impactEffect = effect;
			params.condition = condition;
			Combat::doCombatCondition(creature, target, params);

			lua_pushnumber(L, LUA_NO_ERROR);
		}
		else{
			throwLuaException(getErrorDesc(LUA_ERROR_CONDITION_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
		}
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}
	return 1;
}

int LuaState::luaDoAreaCombatDispel(lua_State *L)
{
	//doAreaCombatDispel(cid, pos, area, type, effect)
	uint8_t effect = (uint8_t)popNumber(L);
	ConditionType_t dispelType = (ConditionType_t)popNumber(L);
	uint32_t areaId = popNumber(L);
	PositionEx pos;
	popPosition(L, pos);
	uint32_t cid = (uint32_t)popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);

		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	const AreaCombat* area = env->getCombatArea(areaId);
	if(area || areaId == 0){
		CombatParams params;
		params.impactEffect = effect;
		params.dispelType = dispelType;
		Combat::doCombatDispel(creature, pos, area, params);

		lua_pushnumber(L, LUA_NO_ERROR);
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_AREA_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}
	return 1;
}

int LuaStateLuaState::luaDoTargetCombatDispel(lua_State *L)
{
	//doTargetCombatDispel(cid, target, type, effect)
	uint8_t effect = (uint8_t)popNumber(L);
	ConditionType_t dispelType = (ConditionType_t)popNumber(L);
	uint32_t targetCid = popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = NULL;

	if(cid != 0){
		creature = env->getCreatureByUID(cid);

		if(!creature){
			throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			lua_pushnumber(L, LUA_ERROR);
			return 1;
		}
	}

	Creature* target = env->getCreatureByUID(targetCid);
	if(target){
		CombatParams params;
		params.impactEffect = effect;
		params.dispelType = dispelType;
		Combat::doCombatDispel(creature, target, params);

		lua_pushnumber(L, LUA_NO_ERROR);
	}
	else{
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
	}

	return 1;
}

int LuaState::luaDoChallengeCreature(lua_State *L)
{
	//doChallengeCreature(cid, target)
	uint32_t targetCid = popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = env->getCreatureByUID(cid);

	if(!creature){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Creature* target = env->getCreatureByUID(targetCid);
	if(!target){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	target->challengeCreature(creature);
	lua_pushnumber(L, LUA_NO_ERROR);
	return 1;
}

int LuaState::luaDoConvinceCreature(lua_State *L)
{
	//doConvinceCreature(cid, target)
	uint32_t targetCid = popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = env->getCreatureByUID(cid);

	if(!creature){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Creature* target = env->getCreatureByUID(targetCid);
	if(!target){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	target->convinceCreature(creature);
	lua_pushnumber(L, LUA_NO_ERROR);
	return 1;
}

int LuaState::luaGetMonsterTargetList(lua_State *L)
{
	//getMonsterTargetList(cid)

	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = env->getCreatureByUID(cid);
	if(!creature){
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Monster* monster = creature->getMonster();
	if(!monster){
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	lua_newtable(L);
	uint32_t i = 1;
	const CreatureList& targetList = monster->getTargetList();
	for(CreatureList::const_iterator it = targetList.begin(); it != targetList.end(); ++it){
		if(monster->isTarget(*it)){
			uint32_t targetCid = env->addThing(*it);
			lua_pushnumber(L, i);
			lua_pushnumber(L, targetCid);
			lua_settable(L, -3);
			++i;
		}
	}

	return 1;
}

int LuaState::luaGetMonsterFriendList(lua_State *L)
{
	//getMonsterFriendList(cid)

	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = env->getCreatureByUID(cid);
	if(!creature){
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Monster* monster = creature->getMonster();
	if(!monster){
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	lua_newtable(L);
	uint32_t i = 1;
	Creature* friendCreature;
	const CreatureList& friendList = monster->getFriendList();
	for(CreatureList::const_iterator it = friendList.begin(); it != friendList.end(); ++it){
		friendCreature = *it;
		if(!friendCreature->isRemoved() && friendCreature->getPosition().z == monster->getPosition().z){
			uint32_t friendCid = env->addThing(*it);
			lua_pushnumber(L, i);
			lua_pushnumber(L, friendCid);
			lua_settable(L, -3);
			++i;
		}
	}

	return 1;
}

int LuaState::luaDoSetMonsterTarget(lua_State *L)
{
	//doSetMonsterTarget(cid, target)
	uint32_t targetCid = popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = env->getCreatureByUID(cid);
	if(!creature){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Monster* monster = creature->getMonster();
	if(!monster){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Creature* target = env->getCreatureByUID(targetCid);
	if(!target){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	if(!monster->isSummon()){
		monster->selectTarget(target);
	}

	lua_pushnumber(L, LUA_NO_ERROR);
	return 1;
}

int LuaState::luaDoMonsterChangeTarget(lua_State *L)
{
	//doMonsterChangeTarget(cid)
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = env->getCreatureByUID(cid);
	if(!creature){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Monster* monster = creature->getMonster();
	if(!monster){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	if(!monster->isSummon()){
		monster->searchTarget(true);
	}

	lua_pushnumber(L, LUA_NO_ERROR);
	return 1;
}

int LuaState::luaDoAddCondition(lua_State *L)
{
	//doAddCondition(cid, condition)

	uint32_t conditionId = popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = env->getCreatureByUID(cid);
	if(!creature){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Condition* condition = env->getConditionObject(conditionId);
	if(!condition){
		throwLuaException(getErrorDesc(LUA_ERROR_CONDITION_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	creature->addCondition(condition->clone());
	lua_pushnumber(L, LUA_NO_ERROR);
	return 1;
}

int LuaState::luaDoRemoveCondition(lua_State *L)
{
	//doRemoveCondition(cid, type)

	ConditionType_t conditionType = (ConditionType_t)popNumber(L);
	uint32_t cid = popNumber(L);

	ScriptEnviroment* env = getScriptEnv();

	Creature* creature = env->getCreatureByUID(cid);
	if(!creature){
		throwLuaException(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnumber(L, LUA_ERROR);
		return 1;
	}

	Condition* condition = creature->getCondition(conditionType, CONDITIONID_COMBAT);
	if(!condition){
		condition = creature->getCondition(conditionType, CONDITIONID_DEFAULT);
	}

	if(condition){
		creature->removeCondition(condition);
	}

	lua_pushnumber(L, LUA_NO_ERROR);
	return 1;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Town

int LuaState::lua_Town_getTemplePosition()
{
	Town* town = popTown();
	pushPosition(town->getTemplePosition());
	return 1;
}

int LuaState::lua_Town_getID()
{
	Town* town = popTown();
	pushInteger(town->getTownID());
	return 1;
}

int LuaState::lua_Town_getName()
{
	Town* town = popTown();
	pushString(town->getName());
	return 1;
}

int LuaState::lua_Town_getHouse()
{
	Houses& houses = Houses::getInstance();

	uint32_t houseid = 0;
	std::string name;

	if(isNumber()){
		houseid = popInteger();
		std::ostringstream os;
		os << houseid;
		name = os.str();
	}
	else{
		name = popString();

		for(HouseMap::iterator it = houses.getHouseBegin(); it != houses.getHouseEnd(); ++it){
			if(it->second->getName() == name)
				houseid = it->second->getHouseId();
		}
		if(houseid == 0)
			throw Script::Error("Town.getHouse : No house by the name '" + name + "' exists.");
	}
	Town* town = popTown();

	House* house = houses.getHouse(houseid);

	if(!house)
		throw Script::Error("Town.getHouse : No house by the name '" + name + "' exists.");
	if(house->getTownId() != town->getTownID())
		throw Script::Error("Town.getHouse : No house by the name '" + name + "' exists belongs to the town " + town->getName());

	pushHouse(house);
	return 1;
}

int LuaState::lua_Town_getHouses()
{
	Town* town = popTown();

	Houses& houses = Houses::getInstance();

	newTable();
	int n = 1;
	for(HouseMap::iterator it = houses.getHouseBegin(); it != houses.getHouseEnd(); ++it){
		if(it->second->getTownId() == town->getTownID()){
			pushHouse(it->second);
			setField(-2, n++);
		}
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// House

int LuaState::lua_House_cleanHouse()
{
	House* house = popHouse();
	house->cleanHouse();
	pushBoolean(true);
	return 1;
}

int LuaState::lua_House_getDoors()
{
	House* house = popHouse();

	newTable();
	int n = 1;
	for(HouseDoorList::iterator hit = house->getHouseDoorBegin(), end = house->getHouseDoorEnd();
		hit != end; ++hit)
	{
		pushThing(*hit);
		setField(-2, n++);
	}

	return 1;
}

int LuaState::lua_House_getExitPosition()
{
	House* house = popHouse();
	pushPosition(house->getEntryPosition());
	return 1;
}

int LuaState::lua_House_getID()
{
	House* house = popHouse();
	pushUnsignedInteger(house->getHouseId());
	return 1;
}

int LuaState::lua_House_getInvitedList()
{
	House* house = popHouse();
	std::string list;
	if(!house->getAccessList(GUEST_LIST, list))
		pushNil();
	// Function is overloaded in lua to return table
	pushString(list);
	return 1;
}

int LuaState::lua_House_getName()
{
	House* house = popHouse();
	pushString(house->getName());
	return 1;
}

int LuaState::lua_House_getPaidUntil()
{
	House* house = popHouse();
	pushUnsignedInteger(house->getPaidUntil());
	return 1;
}

int LuaState::lua_House_getRent()
{
	House* house = popHouse();
	pushInteger(house->getRent());
	return 1;
}

int LuaState::lua_House_getSubownerList()
{
	House* house = popHouse();
	std::string list;
	if(!house->getAccessList(SUBOWNER_LIST, list))
		pushNil();
	// Function is overloaded in lua to return table
	pushString(list);
	return 1;
}

int LuaState::lua_House_getTiles()
{
	House* house = popHouse();
	
	newTable();
	int n = 1;
	for(HouseTileList::iterator hit = house->getHouseTileBegin(), end = house->getHouseTileEnd();
		hit != end; ++hit)
	{
		pushTile(*hit);
		setField(-2, n++);
	}

	return 1;
}

int LuaState::lua_House_getTown()
{
	House* house = popHouse();
	
	pushTown(Towns::getInstance().getTown(house->getTownId()));
	return 1;
}
int LuaState::lua_House_kickPlayer()
{
	Player* who = popPlayer();
	House* house = popHouse();
	pushBoolean(house->kickPlayer(NULL, who->getName()));
	return 1;
}

int LuaState::lua_House_isInvited()
{
	Player* player = popPlayer();
	House* house = popHouse();
	pushBoolean(house->isInvited(player));
	return 1;
}

int LuaState::lua_House_setOwner()
{
	uint32_t guid = popUnsignedInteger();
	House* house = popHouse();

	house->setHouseOwner(guid);

	return 1;
}

int LuaState::lua_House_setInviteList()
{
	std::ostringstream list;

	// Flatten list
	pushNil();
	while(lua_next(state, -2) != 0) {
		list << popString() << "\n";
	}
	House* house = popHouse();

	house->setAccessList(GUEST_LIST, list.str());

	return 1;
}

int LuaState::lua_House_setSubownerList()
{
	std::ostringstream list;

	// Flatten list
	pushNil();
	while(lua_next(state, -2) != 0) {
		list << popString() << "\n";
	}
	House* house = popHouse();

	house->setAccessList(SUBOWNER_LIST, list.str());

	return 1;
}

int LuaState::lua_House_setPaidUntil()
{
	uint32_t until = popUnsignedInteger();
	House* house = popHouse();
	house->setPaidUntil(until);
	pushBoolean(true);
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
// Channel

int LuaState::lua_Channel_getID()
{
	ChatChannel* channel = popChannel();
	push(channel->getId());
	return 1;
}

int LuaState::lua_Channel_getName()
{
	ChatChannel* channel = popChannel();
	push(channel->getName());
	return 1;
}

int LuaState::lua_Channel_getUsers()
{
	ChatChannel* channel = popChannel();
	newTable();
	int n = 1;
	for(UsersMap::const_iterator iter = channel->getUsers().begin(); iter != channel->getUsers().end(); ++iter)
	{
		pushThing(iter->second);
		setField(-2, n++);
	}
	return 1;
}

int LuaState::lua_Channel_addUser()
{
	Player* user = popPlayer();
	ChatChannel* channel = popChannel();
	g_game.playerOpenChannel(user->getID(), channel->getId());
	return 1;
}

int LuaState::lua_Channel_removeUser()
{
	Player* user = popPlayer();
	ChatChannel* channel = popChannel();
	g_game.playerCloseChannel(user->getID(), channel->getId());
	user->sendClosePrivate(channel->getId());
	return 1;
}

int LuaState::lua_Channel_talk()
{
	std::string text = popString();
	int t = popInteger();
	/*Player* user = */popPlayer(ERROR_PASS);
	ChatChannel* channel = popChannel();
	channel->talk(NULL, (SpeakClass)t, text);
	pushBoolean(true);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// (Class) Waypoint

int LuaState::lua_getWaypointByName()
{
	std::string name = popString();

	Waypoint_ptr wp = g_game.getMap()->waypoints.getWaypointByName(name);
	if(wp){
		pushWaypoint(wp);
	}
	else{
		pushNil();
	}
	return 1;
}

int LuaState::lua_Waypoint_getPosition()
{
	Waypoint_ptr wp = popWaypoint();

	pushPosition(wp->pos);
	return 1;
}

int LuaState::lua_Waypoint_getName()
{
	Waypoint_ptr wp = popWaypoint();

	pushString(wp->name);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// (Class) Game

int LuaState::lua_sendMagicEffect()
{
	uint32_t type = popUnsignedInteger();
	Position pos = popPosition();
	
	g_game.addMagicEffect(pos, type);
	pushBoolean(true);
	return 1;
}

int LuaState::lua_getTile()
{
	int z = popInteger();
	int y = popInteger();
	int x = popInteger();
	pushTile(g_game.getTile(x, y, z));
	return 1;
}

int LuaState::lua_getTowns()
{
	Towns& towns = Towns::getInstance();

	newTable();
	int n = 1;
	for(TownMap::const_iterator i = towns.getTownBegin(); i != towns.getTownEnd(); ++i){
		pushTown(const_cast<Town*>((*i).second));
		setField(-2, n++);
	}
	return 1;
}

int LuaState::lua_getHouses()
{
	Houses& houses = Houses::getInstance();

	newTable();
	int n = 1;
	for(HouseMap::iterator it = houses.getHouseBegin(); it != houses.getHouseEnd(); ++it){
		pushHouse(it->second);
		setField(-2, n++);
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Bit definitions

int lua_BitNot(lua_State *L)
{
	int32_t number = (int32_t)lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushnumber(L, ~number);
	return 1;
}

int lua_BitUNot(lua_State *L)
{
	uint32_t number = (uint32_t)lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushnumber(L, ~number);
	return 1;
}

#define MULTIOP(type, name, op) \
	int lua_Bit##name(lua_State *L) { \
	int i, n = lua_gettop(L); \
	type w = (type)lua_tonumber(L, -1); \
	lua_pop(L, 1); \
	for(i = 2; i <= n; ++i){ \
	w op (type)lua_tonumber(L, -1); \
	lua_pop(L, 1); \
	} \
	lua_pushnumber(L, w); \
	return 1; \
}

MULTIOP(int32_t, And, &=)
MULTIOP(int32_t, Or, |=)
MULTIOP(int32_t, Xor, ^=)
MULTIOP(uint32_t, UAnd, &=)
MULTIOP(uint32_t, UOr, |=)
MULTIOP(uint32_t, UXor, ^=)

#define SHIFTOP(type, name, op) \
	int lua_Bit##name(lua_State *L) { \
	type n2 = (type)lua_tonumber(L, -1), n1 = (type)lua_tonumber(L, -2); \
	lua_pop(L, 2); \
	lua_pushnumber(L, (n1 op n2)); \
	return 1; \
}

SHIFTOP(int32_t, LeftShift, <<)
SHIFTOP(int32_t, RightShift, >>)
SHIFTOP(uint32_t, ULeftShift, <<)
SHIFTOP(uint32_t, URightShift, >>)


#if 0
/* Unconverted lua functions...
 * Anyone feels up to the task of converting?
 */

int LuaScriptInterface::luaGetPlayerFlagValue()
{
	//getPlayerFlagValue(who, flag)
	uint32_t flagindex = popUnsignedInteger();
	Player* player = popPlayer();

	if(flagindex < PlayerFlag_LastFlag){
		pushBoolean(player->hasFlag((PlayerFlags)flagindex));
	}
	else{
		reportLuaError("No valid flag index.");
		pushBoolean(false);
	}
	return 1;
}

int LuaScriptInterface::luaDoPlayerRemoveItem()
{
	//doPlayerRemoveItem(cid, itemid, count, <optional> subtype)
	int32_t parameters = lua_gettop(m_luaState);

	int32_t subType = -1;
	if(parameters > 3){
		subType = popNumber();
	}

	uint32_t count = popInteger();
	uint16_t itemId = (uint16_t)popUnsignedInteger();
	Player* player = popPlayer();

	pushBoolean(g_game.removeItemOfType(NULL, player, itemId, count, subType));

	return 1;
}

int LuaScriptInterface::luaDoFeedPlayer()
{
	//doFeedPlayer(uid, food)
	int32_t food = (int32_t)popInteger();
	Player* player = popPlayer();

	player->addDefaultRegeneration(food * 1000);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoTeleportThing()
{
	//doTeleportThing(uid, newpos)
	PositionEx pos = popPosition();
	Thing* thing = popThing();

	pushBoolean(g_game.internalTeleport(thing, pos) == RET_NOERROR);
	return 1;
}

int LuaScriptInterface::luaDoCreatureSay()
{
	//doPlayerSay(cid, text, type)
	uint32_t type = popUnsignedInteger();
	std::string text = popString();
	Player* player = popPlayer();

	g_game.internalCreatureSay(player,(SpeakClasses)type, text);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoPlayerAddSkillTry()
{
	//doPlayerAddSkillTry(cid, skillid, n)
	uint32_t n = popUnsignedInteger();
	uint32_t skillid = popUnsignedInteger();
	Player* player = popPlayer();

	if(skillid < SKILL_FIRST || skillid > SKILL_LAST) {
		throwLuaException("Invalid skill index!");
	}
	player->addSkillAdvance((skills_t)skillid, n);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoRelocate()
{
	//doRelocate(pos, posTo)
	//Moves all moveable objects from pos to posTo

	PositionEx toPos = popPositionEx();
	PositionEx fromPos = popPositionEx();

	Tile* fromTile = g_game.getTile(fromPos.x, fromPos.y, fromPos.z);
	if(!fromTile){
		throwLuaException(getErrorDesc(LUA_ERROR_TILE_NOT_FOUND));
	}

	Tile* toTile = g_game.getTile(toPos.x, toPos.y, toPos.z);
	if(!toTile){
		throwLuaException(getErrorDesc(LUA_ERROR_TILE_NOT_FOUND));
	}

	int32_t thingCount = fromTile->getThingCount();
	for(int32_t i = thingCount - 1; i >= 0; --i){
		Thing* thing = fromTile->__getThing(i);
		if(thing){
			if(Item* item = thing->getItem()){
				if(item->isPushable()){
					g_game.internalTeleport(NULL, item, toPos);
				}
			}
			else if(Creature* creature = thing->getCreature()){
				g_game.internalTeleport(NULL, creature, toPos);
			}
		}
	}

	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoPlayerSendTextMessage()
{
	//doPlayerSendTextMessage(cid, MessageClasses, message)
	std::string text = popString();
	uint32_t messageClass = popUnsignedInteger();
	Player* player = popPlayer();

	player->sendTextMessage((MessageClasses)messageClass, text);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoSendAnimatedText()
{
	//doSendAnimatedText(pos, text, color)
	uint32_t color = popUnsignedInteger();
	std::string text = popString();
	PositionEx pos = popPosition();

	ScriptEnviroment* env = getScriptEnv();

	SpectatorVec list;
	SpectatorVec::iterator it;

	if(pos.x == 0xFFFF){
		pos = env->getRealPos();
	}

	g_game.addAnimatedText(pos, color, text);
	return 1;
}

int LuaScriptInterface::luaGetPlayerSkill()
{
	//getPlayerSkill(cid, skillid)
	uint32_t skillid = popUnsignedInteger();
	const Player* player = popPlayer();

	if(skillid < SKILL_FIRST || skillid > SKILL_LAST){
		throwLuaException("No valid skillId");
	}
	uint32_t value = player->skills[skillid][SKILL_LEVEL];
	pushNumber(value);
	return 1;
}

int LuaScriptInterface::luaGetPlayerLossPercent()
{
	//getPlayerLossPercent(cid, lossType)
	uint8_t lossType = (uint8_t)popUnsignedInteger();
	const Player* player = popPlayer();

	if(lossType > LOSS_LAST){
		throwLuaException("No valid lossType");
	}
	uint32_t value = player->getLossPercent((lossTypes_t)lossType);
	pushNumber(value);
	return 1;
}

int LuaScriptInterface::luaDoPlayerSetLossPercent()
{
	//doPlayerSetLossPercent(cid, lossType, newPercent)
	uint32_t newPercent = popUnsignedInteger();
	uint8_t lossType = (uint8_t)popUnsignedInteger();
	Player* player = popPlayer();

	if(lossType > LOSS_LAST){
		throwLuaException("Invalid valid lossType");
	}
	if(newPercent > 100){
		reportLuaError("lossPercent value higher than 100, truncated to 100");
		newPercent = 100;
	}
	player->setLossPercent((lossTypes_t)lossType, newPercent);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoSetCreatureDropLoot()
{
	//doSetCreatureDropLoot(cid, doDrop)
	bool doDrop = popBoolean();
	Creature* creature = popCreature();

	creature->setDropLoot(doDrop);
	return 1;
}

int LuaScriptInterface::luaDoShowTextDialog()
{
	//doShowTextDialog(cid, itemid, text)
	std::string text = popString();
	uint32_t itemId = popUnsignedInteger();
	Player* player = popPlayer();

	player->setWriteItem(NULL, 0);
	player->sendTextWindow(itemId, text);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaGetItemRWInfo()
{
	//getItemRWInfo(uid)
	const Item* item = popItem();

	uint32_t rwflags = 0;
	if(item->isReadable())
		rwflags |= 1;

	if(item->canWriteText()){
		rwflags |= 2;
	}

	pushUnsignedInteger(rwflags);
	return 1;
}

int LuaScriptInterface::luaGetTileItemById()
{
	//getTileItemById(pos, itemId, <optional> subType)
	uint32_t parameters = lua_gettop(m_luaState);

	int32_t subType = -1;
	if(parameters > 2){
		subType = popInteger();
	}

	int32_t itemId = popInteger();

	PositionEx pos = popPosition();

	Tile* tile = g_game.getTile(pos.x, pos.y, pos.z);
	if(!tile){
		pushThing(NULL);
		return 1;
	}

	Item* item = g_game.findItemOfType(tile, itemId, false, subType);
	if(!item){
		pushThing(NULL);
		return 1;
	}

	pushThing(item);

	return 1;
}

int LuaScriptInterface::luaGetTileItemByType()
{
	//getTileItemByType(pos, type)
	uint32_t rType = popUnsignedInteger();

	if(rType >= ITEM_TYPE_LAST){
		throwLuaException("Not a valid item type");
		pushThing(NULL);
		return 1;
	}

	PositionEx pos = popPosition();

	Tile* tile = g_game.getTile(pos.x, pos.y, pos.z);
	if(!tile){
		pushThing(NULL);
		return 1;
	}

	for(uint32_t i = 0; i < tile->getThingCount(); ++i){
		if(Item* item = tile->__getThing(i)->getItem()){
			const ItemType& it = Item::items[item->getID()];
			if(it.type == (ItemTypes_t) rType){
				pushThing(item);
				return 1;
			}
		}
	}

	pushThing(NULL);
	return 1;
}

int LuaScriptInterface::luaGetTileThingByPos()
{
	//getTileThingByPos(pos)

	PositionEx pos = popPositionEx();

	ScriptEnviroment* env = getScriptEnv();

	Tile* tile = g_game.getTile(pos.x, pos.y, pos.z);
	if(!tile){
		if(pos.stackpos == -1){
			pushInteger(-1);
			return 1;
		}
		else{
			pushThing(NULL);
			return 1;
		}
	}

	if(pos.stackpos == -1){
		pushNumber(tile->getThingCount());
		return 1;
	}

	Thing* thing = tile->__getThing(pos.stackpos);
	if(!thing){
		pushThing(NULL);
		return 1;
	}

	pushThing(thing);
	return 1;
}

int LuaScriptInterface::luaGetTopCreature()
{
	//getTopCreature(pos)

	PositionEx pos = popPositionEx();

	Tile* tile = g_game.getTile(pos.x, pos.y, pos.z);
	if(!tile){
		pushThing(NULL);
		return 1;
	}

	Thing* thing = tile->getTopCreature();
	if(!thing || !thing->getCreature()){
		pushThing(NULL);
		return 1;
	}

	pushThing(thing);
	return 1;
}

int LuaScriptInterface::luaDoCreateItem()
{
	//doCreateItem(itemid [, subtype/count], pos)
	//Returns uid of the created item, only works on tiles.

	uint32_t parameters = lua_gettop(m_luaState);

	PositionEx pos;
	popPosition(pos);

	uint32_t count = 0;
	if(parameters > 2){
		count = popUnsignedInteger();
	}

	uint32_t itemId = popUnsignedInteger();

	Tile* tile = g_game.getTile(pos);
	if(!tile){
		tile = new Tile(pos);
		g_game.getMap()->setTile(pos, newtile);
	}

	const ItemType& it = Item::items[itemId];
	if(it.stackable && count > 100){
		throwLuaException("Stack count cannot be higher than 100.");
	}

	Item* newItem = Item::CreateItem(itemId, count);

	ReturnValue ret = g_game.internalAddItem(NULL, tile, newItem, INDEX_WHEREEVER, FLAG_NOLIMIT);
	if(ret != RET_NOERROR){
		delete newItem;
		pushThing(NULL);
		return 1;
	}

	// RTODO: MAKE ME RETURN STACKED ITEM!
	if(newItem->getParent()){
		pushThing(newItem);
	}
	else{
		//stackable item stacked with existing object, newItem will be released
		pushThing(NULL);
	}
	return 1;
}

int LuaScriptInterface::luaDoCreateItemEx()
{
	//doCreateItemEx(itemid, <optional> count/subtype)
	//Returns uid of the created item

	int32_t parameters = lua_gettop(m_luaState);

	uint32_t count = 0;
	if(parameters > 1){
		count = popUnsignedInteger();
	}

	uint32_t itemId = popUnsignedInteger();

	ScriptEnviroment* env = getScriptEnv();

	const ItemType& it = Item::items[itemId];
	if(it.stackable && count > 100){
		throwLuaException("Stack count cannot be higher than 100.");
	}

	Item* newItem = Item::CreateItem(itemId, count);
	if(!newItem){
		throwLuaException("Can not create item with id 0.");
	}

	newItem->setParent(VirtualCylinder::virtualCylinder);
	env->addTempItem(newItem);

	pushThing(newItem);
	return 1;
}

int LuaScriptInterface::luaDoCreateTeleport()
{
	//doCreateTeleport(teleportID, positionToGo, createPosition)
	Position createPos = popPosition();
	Position toPos = popPosition();
	uint32_t itemId = popUnsignedInteger();

	Tile* tile = g_game.getMap()->getTile(createPos);
	if(!tile){
		throwLuaException(getErrorDesc(LUA_ERROR_TILE_NOT_FOUND));
	}

	Item* newItem = Item::CreateItem(itemId);
	Teleport* newTp = newItem->getTeleport();

	if(!newTp){
		delete newItem;
		throwLuaException("ID is not a teleport");
	}

	newTp->setDestPos(toPos);

	ReturnValue ret = g_game.internalAddItem(NULL, tile, newTp, INDEX_WHEREEVER, FLAG_NOLIMIT);
	if(ret != RET_NOERROR){
		delete newItem;
		pushThing(NULL);
	}

	if(newItem->getParent()){
		pushThing(newItem);
	}
	else{
		// stackable item stacked with existing object, newItem will be released
		// note: this makes no sense, since when are teleports stackable?
		pushThing(NULL);
	}
	return 1;
}

int LuaScriptInterface::luaGetPlayerStorageValue()
{
	//getPlayerStorageValue(cid, valueid)
	uint32_t key = popUnsignedInteger();
	Player* player = popPlayer();

	ScriptEnviroment* env = getScriptEnv();
	int32_t value;
	if(player->getStorageValue(key, value)){
		pushInteger(value);
	}
	else{
		pushInteger(-1);
	}
	return 1;
}

int LuaScriptInterface::luaSetPlayerStorageValue()
{
	//setPlayerStorageValue(cid, valueid, newvalue)
	int32_t value = popInteger();
	uint32_t key = popUnsignedInteger();
	Player* player = popPlayer();

	if(IS_IN_KEYRANGE(key, RESERVED_RANGE)){
		std::stringstream error_str;
		error_str << "Accesing to reserverd range: "  << key;
		throwLuaException(error_str.str());
	}

	player->addStorageValue(key,value);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoSetItemActionId()
{
	//doSetItemActionID(uid, actionid)
	uint32_t actionid = popUnsignedInteger();
	Item* item = popItem();

	item->setActionId(actionid);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoSetItemText()
{
	//doSetItemText(uid, text)
	std::string text = popString();
	Item* item = popItem();
	item->setText(text);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoSetItemSpecialDescription()
{
	//doSetItemSpecialDescription(uid, desc)
	std::string text = popString();
	Item* item = popItem();

	if(text != ""){
		item->setSpecialDescription(text);
	}
	else{
		item->resetSpecialDescription();
	}
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaGetTilePzInfo()
{
	//getTilePzInfo(pos)
	Position pos = popPosition();

	Tile *tile = g_game.getMap()->getTile(pos);

	pushBoolean(tile && tile->hasFlag(TILESTATE_PROTECTIONZONE));
	return 1;
}

int LuaScriptInterface::luaGetTileHouseInfo()
{
	//getTileHouseInfo(pos)
	Position pos = popPosition();

	Tile *tile = g_game.getMap()->getTile(pos);

	if(HouseTile* houseTile = dynamic_cast<HouseTile*>(tile)){
		House* house = houseTile->getHouse();
		if(house){
			pushUnsignedInteger(house->getHouseId());
		}
		else{
			pushUnsignedInteger(0);
		}
	}
	else {
		pushBoolean(false);
	}
	return 1;
}

int LuaScriptInterface::luaDoCreateMonster()
{
	//doCreateMonster(name, pos)
	PositionEx pos = popPosition();
	std::string name = popString();

	Monster* monster = Monster::createMonster(name);

	if(!monster){
		std::string error_str = "Monster name(" + name + ") not found";
		throwLuaException(error_str);
	}

	if(!g_game.placeCreature(monster, (Position&)pos)){
		delete monster;
		std::string error_str = (std::string)"Can not summon monster: " + name;
		throwLuaException(error_str);
	}

	pushThing(monster);
	return 1;
}

int LuaScriptInterface::luaDoSummonCreature()
{
	//doSummonCreature(creature, name, pos)
	PositionEx pos = popPositionEx();
	std::string name = popString();
	Creature* creature = popCreature();

	Monster* monster = Monster::createMonster(name);
	if(!monster){
		std::string error_str = (std::string)"Monster name(" + name + (std::string)") not found";
		throwLuaException(error_str);
	}

	creature->addSummon(monster);
	if(!g_game.placeCreature(monster, (Position&)pos)){
		creature->removeSummon(monster);
		delete monster;
		pushBoolean(false);
		return 1;
	}

	pushThing(monster);

	return 1;
}

int LuaScriptInterface::luaDoRemoveCreature()
{
	//doRemoveCreature(cid)
	Creature* creature = popCreature();

	//Players will get kicked without restrictions
	Player* player = creature->getPlayer();
	if(player){
		player->kickPlayer();
	}
	//Monsters/NPCs will get removed
	else{
		g_game.removeCreature(creature);
	}
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoPlayerRemoveMoney()
{
	//doPlayerRemoveMoney(cid, money)
	uint32_t money = popInteger();
	Player* player = popPlayer();
	if(money < 0) {
		pushBoolean(false);
		return 1;
	}
	pushBoolean(g_game.removeMoney(player, money));
	return 1;
}

int LuaScriptInterface::luaDoPlayerSetTown()
{
	//doPlayerSetTown(cid, towind)
	Town* town = popTown();
	Player* player = popPlayer();
	player->masterPos = town->getTemplePosition();
	player->setTown(town->getTownID());
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoPlayerSetVocation()
{
	//doPlayerSetVocation(cid, voc)
	uint32_t voc = popInteger();
	Player* player = popPlayer();

	player->setVocation(voc);
	pushBoolean(true);

	return 1;
}

int LuaScriptInterface::luaDoPlayerAddSoul()
{
	//doPlayerAddSoul(cid, soul)
	int32_t addsoul = popInteger();
	Player* player = popPlayer();

	player->changeSoul(addsoul);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaGetHouseOwner()
{
	//getHouseOwner(house)
	House* house = popHouse();
	pushUnsignedInteger(house->getHouseOwner());
	return 1;
}

int LuaScriptInterface::luaGetHouseName()
{
	//getHouseName(house)
	House* house = popHouse();
	pushString(house->getName());
	return 1;
}

int LuaScriptInterface::luaGetHouseEntry()
{
	//getHouseEntry(houseid)
	House* house = popHouse();
	pushPosition(house->getEntryPosition());
	return 1;
}

int LuaScriptInterface::luaGetHouseRent()
{
	//getHouseRent(house)
	House* house = popHouse();
	pushUnsignedInteger(house->getRent());
	return 1;
}

int LuaScriptInterface::luaGetHouseTown()
{
	//getHouseTown(house)
	House* house = popHouse();
	Town* town = Towns::getInstance().getTown(house->getTownId());
	pushTown(town);
	return 1;
}

int LuaScriptInterface::luaGetHouseAccessList()
{
	//getHouseAccessList(house, listid)
	uint32_t listid = popUnsignedInteger();
	House* house = popHouse();

	std::string list;
	if(house->getAccessList(listid, list)){
		pushString(list);
	}
	else{
		throwLuaException("Invalid listid.");
	}
	return 1;
}

int LuaScriptInterface::luaGetHouseByPlayerGUID()
{
	//getHouseByPlayerGUID(playerGUID)
	uint32_t guid = popUnsignedInteger();
	pushHouse(Houses::getInstance().getHouseByPlayerId(guid));
	return 1;
}

int LuaScriptInterface::luaGetHouseTilesSize()
{
	//getHouseTilesSize(house)
	House* house = popHouse();
	pushNumber(house->getHouseTileSize());
	return 1;
}

int LuaScriptInterface::luaSetHouseAccessList()
{
	//setHouseAccessList(house, listid, listtext)
	std::string list = popString();
	uint32_t listid = popUnsignedInteger();
	House* house = popHouse();

	house->setAccessList(listid, list);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaSetHouseOwner()
{
	//setHouseOwner(house, owner)
	uint32_t guid;
	Player* player = popPlayer(NULL, LUA_PASS);
	if(!player) {
		// It is a GUID?
		guid = popUnsignedInteger();
	} else {
		guid = player->getGUID();
	}
	House* house = popHouse();

	house->setHouseOwner(guid);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaGetWorldType()
{
	//getWorldType()
	switch(g_game.getWorldType()){
		case WORLD_TYPE_NO_PVP:
			pushNumber(1);
			break;
		case WORLD_TYPE_PVP:
			pushNumber(2);
			break;
		case WORLD_TYPE_PVP_ENFORCED:
			pushNumber(3);
			break;
		default:
			throwLuaException("Unknown world type");
			break;
	}
	return 1;
}

int LuaScriptInterface::luaGetWorldTime()
{
	//getWorldTime()
	uint32_t time = g_game.getLightHour();
	pushNumber(time);
	return 1;
}

int LuaScriptInterface::luaGetWorldLight()
{
	//getWorldLight()
	LightInfo lightInfo;
	g_game.getWorldLightInfo(lightInfo);

	pushNumber(lightInfo.level);
	pushNumber(lightInfo.color);
	return 2;
}

int LuaScriptInterface::luaGetWorldCreatureCount()
{
	//getWorldCreatureCount(type)
	//0 players, 1 monsters, 2 npcs, 3 all
	uint32_t type = popUnsignedInteger();
	switch(type){
		case 0:
			pushNumber(g_game.getPlayersOnline());
			break;
		case 1:
			pushNumber(g_game.getMonstersOnline());
			break;
		case 2:
			pushNumber(g_game.getNpcsOnline());
			break;
		case 3:
			pushNumber(g_game.getCreaturesOnline());
			break;
		default:
			throwLuaException("Invalid creature type.");
			break;
	}
	return 1;
}

int LuaScriptInterface::luaGetWorldUpTime()
{
	//getWorldUpTime()
	uint32_t uptime = 0;

	Status* status = Status::instance();
	if(status){
		uptime = status->getUptime();
	}

	pushNumber(uptime);
	return 1;
}

int LuaScriptInterface::luaBroadcastMessage()
{
	//broadcastMessage([messageClass,] message)
	uint32_t parameters = lua_gettop(m_luaState);
	std::string message = popString();
	uint32_t type = MSG_STATUS_WARNING;
	if(parameters == 2) {
		type = popUnsignedInteger();
		if(type >= 0x12 && type <= 0x19) {
		throwLuaException("Not valid messageClass.");
		}
	}
	g_game.anonymousBroadcastMessage(MSG_STATUS_WARNING, message);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaGetCreatureLight()
{
	//getCreatureLight(creature)
	Creature* creature = popCreature();

	LightInfo lightInfo;
	creature->getCreatureLight(lightInfo);
	pushNumber(lightInfo.level);
	pushNumber(lightInfo.color);//color
	return 2;
}

int LuaScriptInterface::luaGetPlayerItemById()
{
	//getPlayerItemByID(cid, deepSearch, itemID [, subType])
	uint32_t parameters = lua_gettop(m_luaState);

	int32_t subType = -1;
	if(parameters > 3){
		subType = popInteger(L);
	}

	uint32_t itemId = popUnsignedInteger(L);
	bool deepSearch = popBoolean();
	Player* player = popPlayer();

	Item* item = g_game.findItemOfType(player, itemId, deepSearch, subType);
	pushThing(item);
	return 1;
}

int LuaScriptInterface::luaNumberToVariant(lua_State *L)
{
	//numberToVariant(number)
	LuaVariant var;
	var.type = VARIANT_NUMBER;
	var.number = popNumber(L);

	LuaScriptInterface::pushVariant(L, var);
	return 1;
}

int LuaScriptInterface::luaStringToVariant(lua_State *L)
{
	//stringToVariant(string)
	LuaVariant var;
	var.type = VARIANT_STRING;
	var.text = popString(L);

	LuaScriptInterface::pushVariant(L, var);
	return 1;
}

int LuaScriptInterface::luaPositionToVariant(lua_State *L)
{
	//positionToVariant(pos)
	LuaVariant var;
	var.type = VARIANT_POSITION;
	popPosition(L, var.pos);

	LuaScriptInterface::pushVariant(L, var);
	return 1;
}

int LuaScriptInterface::luaTargetPositionToVariant(lua_State *L)
{
	//targetPositionToVariant(pos)
	LuaVariant var;
	var.type = VARIANT_TARGETPOSITION;
	popPosition(L, var.pos);

	LuaScriptInterface::pushVariant(L, var);
	return 1;
}

int LuaScriptInterface::luaVariantToNumber(lua_State *L)
{
	//variantToNumber(var)
	LuaVariant var = popVariant(L);

	uint32_t number = 0;
	if(var.type == VARIANT_NUMBER){
		number = var.number;
	}

	lua_pushnumber(L, number);
	return 1;
}

int LuaScriptInterface::luaVariantToString(lua_State *L)
{
	//variantToString(var)
	LuaVariant var = popVariant(L);

	std::string text = "";
	if(var.type == VARIANT_STRING){
		text = var.text;
	}

	lua_pushstring(L, text.c_str());
	return 1;
}

int LuaScriptInterface::luaVariantToPosition(lua_State *L)
{
	//luaVariantToPosition(var)
	LuaVariant var = popVariant(L);

	PositionEx pos(0, 0, 0, 0);
	if(var.type == VARIANT_POSITION || var.type == VARIANT_TARGETPOSITION){
		pos = var.pos;
	}

	pushPosition(L, pos, pos.stackpos);
	return 1;
}

int LuaScriptInterface::luaDoChangeSpeed()
{
	//doChangeSpeed(creature, delta)
	int32_t delta = popInteger();
	Creature* creature = popCreature();
	g_game.changeSpeed(creature, delta);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaSetCreatureOutfit()
{
	//doSetCreatureOutfit(creature, outfit)
	Outfit_t outfit;
	outfit.lookType = getField("type");
	outfit.lookTypeEx = getField("extra");
	outfit.lookHead = getField("head");
	outfit.lookBody = getField("body");
	outfit.lookLegs = getField("legs");
	outfit.lookFeet = getField("feet");
	outfit.lookAddons = getField("addons");
	pop(); // outfit table
	Creature* creature = popCreature();

	creature->setDefaultOutfit(outfit);
	g_game.internalCreatureChangeOutfit(creature, outfit);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaGetCreatureOutfit()
{
	//getCreatureOutfit(creature)
	Creature* creature = popCreature();
	const Outfit_t outfit = creature->getCurrentOutfit();

	lua_newtable(m_luaState);
	setField("type", outfit.lookType);
	setField("extra", outfit.lookTypeEx);
	setField("head", outfit.lookHead);
	setField("body", outfit.lookBody);
	setField("legs", outfit.lookLegs);
	setField("feet", outfit.lookFeet);
	setField("addons", outfit.lookAddons);
	return 1;
}

int LuaScriptInterface::luaGetGlobalStorageValue()
{
	//getGlobalStorageValue(valueid)
	uint32_t key = popUnsignedInteger();

	ScriptEnviroment* env = getScriptEnv();

	int32_t value;
	if(env->getGlobalStorageValue(key, value)){
		pushInteger(value);
	}
	else{
		pushInteger(-1);
	}
	return 1;
}

int LuaScriptInterface::luaSetGlobalStorageValue()
{
	//setGlobalStorageValue(valueid, newvalue)
	int32_t value = popInteger();
	uint32_t key = popUnsignedInteger();

	ScriptEnviroment* env = getScriptEnv();
	env->addGlobalStorageValue(key, value);
	pushBoolean(true)
	return 1;
}

int LuaScriptInterface::luaCountPlayerDepotItems()
{
	//countPlayerDepotItems(player, depot)
	uint32_t depotid = 0;
	if(isInteger()) {
		depotid = popUnsignedInteger();
	} else {
		depotid = popTown()->getTownID;
	}
	Player* player = popPlayer();

	const Depot* depot = player->getDepot(depotid, true);
	if(depot){
		pushUnsignedInteger(depot->getItemHoldingCount());
	}
	else{
		pushUnsignedInteger(0);
	}
	return 1;
}

int LuaScriptInterface::luaDoPlayerSetGuildRank()
{
	//doPlayerSetGuildRank(player, rank)
	std::string rank = popString();
	Player* player = popPlayer();

	player->setGuildRank(rank);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoPlayerSetGuildNick()
{
	//doPlayerSetGuildNick(player, nick)
	std::string rank = popString();
	Player* player = popPlayer();

	player->setGuildNick(rank);
	pushBoolean(true);
	return true;
}

int LuaScriptInterface::luaGetGuildId()
{
	//getGuildID(guild_name)
	std::string name = popString();
	uint32_t guildId;
	if(IOPlayer::instance()->getGuildIdByName(guildId, std::string(name))){
		pushUnsignedInteger(guildId);
	}
	else{
		pushUnsignedInteger(0);
	}
	return 1;
}

int LuaScriptInterface::luaGetPlayerByName()
{
	//getPlayerByName(name)
	std::string name = popString();
	pushThing(g_game.getPlayerByName(name));
	return 1;

}

int LuaScriptInterface::luaGetPlayerGUIDByName()
{
	//getPlayerGUIDByName(name)
	std::string name = popString();

	Player* player = g_game.getPlayerByName(name);
	uint32_t value = 0;

	if(player){
		value = player->getGUID();
	}
	else{
		uint32_t guid;
		if(IOPlayer::instance()->getGuidByName(guid, name)){
			value = guid;
		}
	}

	pushUnsignedInteger(value);
	return 1;
}

int LuaScriptInterface::luaGetContainerSize()
{
	//getContainerSize(container)
	Container* container = popContainer();
	pushInteger(container->size());
	return 1;
}

int LuaScriptInterface::luaGetContainerCap()
{
	//getContainerCap(container)
	Container* container = popContainer();
	pushInteger(container->capacity());
	return 1;
}

int LuaScriptInterface::luaGetContainerItem()
{
	//getContainerItem(container, slot)
	uint32_t slot = popUnsignedInteger();
	Container* container = popContainer();
	pushThing(container->getItem(slot));
	return 1;

}

int LuaScriptInterface::luaDoAddContainerItem()
{
	//doAddContainerItem(container, item)
	Item* item = popItem();
	Container* container = popContainer();

	Cylinder* old_parent = item->getParent();
	if(old_parent != container) {
		ReturnValue ret = g_game.internalMoveItem(NULL, old_parent, container, INDEX_WHEREEVER, item, item->getItemCount());
		if(ret != RET_NOERROR) {
			pushBoolean(false);
		}
	}
	pushBoolean(true);
}

int LuaScriptInterface::luaGetDepotId()
{
	//getDepotID(depot)
	Depot* depot = popDepot();
	pushUnsignedInteger(depot->getDepotId());
	return 1;
}

int LuaScriptInterface::luaDoPlayerAddOutfit()
{
	//doPlayerAddOutfit(player, looktype, addon)
	int addon = (int)popNumber();
	int looktype = (int)popNumber();
	Player* player = popPlayer();

	player->addOutfit(looktype, addon);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoPlayerRemOutfit(lua_State *L)
{
	//doPlayerRemOutfit(player, looktype, addon)
	int addon = (int)popNumber(L);
	int looktype = (int)popNumber(L);
	Player* player = popPlayer();

	player->remOutfit(looktype, addon);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaDoSetCreatureLight()
{
	//doSetCreatureLight(creature, lightLevel, lightColor, time)
	int32_t time = popInteger();
	uint32_t color = popUnsignedInteger();
	uint32_t level = popUnsignedInteger();
	Creature* creature = popCreature();

	color = std::max(color, 255);
	level = std::max(level, 255);

	Condition* condition = Condition::createCondition(CONDITIONID_COMBAT, CONDITION_LIGHT, time, level | (color << 8));
	creature->addCondition(condition);
	return 1;
}

int LuaScriptInterface::luaGetCreatureName()
{
	//getCreatureName(creature)
	Creature* creature = popCreature();
	pushString(creature->getName());
	return 1;
}

int LuaScriptInterface::luaIsItemStackable()
{
	//isItemStackable(item/itemid)
	uint32_t itemid;
	if(isNumber()) {
		itemid = popUnsignedInteger();
	} else {
		itemid = popItem()->getID();
	}
	const ItemType& it = Item::items[itemid];
	pushBoolean(it.stackable);
	return 1;
}

int LuaScriptInterface::luaIsItemRune()
{
	//isItemRune(item/itemid)
	uint32_t itemid;
	if(isNumber()) {
		itemid = popUnsignedInteger();
	} else {
		itemid = popItem()->getID();
	}
	const ItemType& it = Item::items[itemid];
	pushBoolean(it.isRune());
	return 1;
}

int LuaScriptInterface::luaIsItemDoor()
{
	//isItemDoor(item/itemid)
	uint32_t itemid;
	if(isNumber()) {
		itemid = popUnsignedInteger();
	} else {
		itemid = popItem()->getID();
	}
	const ItemType& it = Item::items[itemid];
	pushBoolean(it.isDoor());
	return 1;
}

int LuaScriptInterface::luaIsItemContainer()
{
	//isItemContainer(item/itemid)
	uint32_t itemid;
	if(isNumber()) {
		itemid = popUnsignedInteger();
	} else {
		itemid = popItem()->getID();
	}
	const ItemType& it = Item::items[itemid];
	pushBoolean(it.isContainer());
	return 1;
}

int LuaScriptInterface::luaIsItemFluidContainer()
{
	//isItemFluidContainer(item/itemid)
	uint32_t itemid;
	if(isNumber()) {
		itemid = popUnsignedInteger();
	} else {
		itemid = popItem()->getID();
	}
	const ItemType& it = Item::items[itemid];
	pushBoolean(it.isFluidContainer());
	return 1;
}

int LuaScriptInterface::luaGetItemName(lua_State *L)
{
	//getItemName(item/itemid)
	uint32_t itemid;
	if(isNumber()) {
		itemid = popUnsignedInteger();
	} else {
		itemid = popItem()->getID();
	}
	const ItemType& it = Item::items[itemid];
	pushString(it.name);
	return 1;
}

int LuaScriptInterface::luaGetItemDescriptions()
{
	//getItemDescriptions(item/itemid)
	//returns the name, the article and the plural name of the item
	uint32_t itemid;
	if(isNumber()) {
		itemid = popUnsignedInteger();
	} else {
		itemid = popItem()->getID();
	}
	const ItemType& it = Item::items[itemid];

	lua_newtable(m_luaState);
	setField("name", it.name);
	setField("article", it.article);
	setField("plural", it.pluralName);
	return 1;
}

int LuaScriptInterface::luaGetItemWeight()
{
	//getItemWeight(item)
	uint32_t itemid;
	if(isNumber()) {
		itemid = popUnsignedInteger();
		const ItemType& it = Item::items[itemid];
		pushFloat(it.weight);
	} else {
		Item* item = popItem();
		pushFloat(item->getWeight());
	}
	return 1;
}

int LuaScriptInterface::luaGetDataDirectory()
{
	pushString(g_config.getString(ConfigManager::DATA_DIRECTORY));
	return 1;
}

int LuaScriptInterface::luaGetCreatureSpeed()
{
	//getCreatureSpeed(creature)
	Creature* creature = popCreature();
	pushInteger(creature->getSpeed());
	return 1;
}

int LuaScriptInterface::luaGetCreatureBaseSpeed()
{
	//getCreatureBaseSpeed(creature)
	Creature* creature = popCreature();
	pushInteger(creature->getBaseSpeed());
	return 1;
}

int LuaScriptInterface::luaGetCreatureTarget(lua_State *L)
{
	//getCreatureTarget(creature)
	Creature* creature = popCreature();
	pushThing(creature->getAttackedCreature());
	return 1;
}

int LuaScriptInterface::luaIsPremium()
{
	//isPremium(player)
	Player* player = popPlayer();
	pushBoolean(player->isPremium());
	return 1;
}

int LuaScriptInterface::luaHasCondition()
{
	//hasCondition(creature, conditionid)
	ConditionType_t conditionType = (ConditionType_t)popUnsignedInteger();
	Creature* creature = popCreature();

	pushBoolean(creature->hasCondition(conditionType));
	return 1;
}

int LuaScriptInterface::luaDoSendDistanceShoot()
{
	//doSendDistanceShoot(frompos, topos, type)
	uint8_t type = std::max(255, popUnsignedInteger());
	PositionEx toPos = popPosition();
	PositionEx fromPos = popPosition();

	ScriptEnviroment* env = getScriptEnv();

	if(fromPos.x == 0xFFFF){
		fromPos = env->getRealPos();
	}
	if(toPos.x == 0xFFFF){
		toPos = env->getRealPos();
	}

	g_game.addDistanceEffect(fromPos, toPos, type);
	pushBoolean(true);
	return 1;
}

int LuaScriptInterface::luaGetCreatureMaster()
{
	//getCreatureMaster(cid)
	//returns the creature's master or itself if the creature isn't a summon
	Creature* slave = popCreature();
	pushThing(creature->getMaster());
	return 1;
}

int LuaScriptInterface::luaGetCreatureSummons()
{
	//getCreatureSummons(creature)
	//returns a table with all the summons of the creature
	Creature* master = popCreature();

	lua_newtable(L);
	const std::list<Creature*>& summons = creature->getSummons();
	std::list<Creature*>::const_iterator it = summons.begin();
	uint32_t i = 0;
	for( ; it != summons.end(); ++it, ++i){
		pushUnsignedInteger(i);
		pushCreature(*it);
		lua_settable(L, -3);
	}

	return 1;
}

#endif