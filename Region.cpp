//stl
#include <stdio.h>
#include <iostream>
//project
#include "Region.h"
#include "SuperRegion.h"

Region::Region()
	: id(0)
	, superRegion(0)
	, owner(NEUTRAL)
	, armies(0)
	, enemy_zones(0)
	, danger(0)
	, enemy_armies(0)
	, neutral_armies(0)
	, neutral_zones(0)
{
}

Region::Region(const int& pId, const int& pSuperRegion)
	: id(pId)
	, superRegion(pSuperRegion)
	, owner(NEUTRAL)
	, armies(0)
	, enemy_zones(0)
	, danger(0)
	, enemy_armies(0)
	, neutral_armies(0)
	, neutral_zones(0)
{
}

Region::~Region()
{
}

void Region::addNeighbor(const int& neighbor)
{
	neighbors.push_back(neighbor);
}

int Region::getNbNeighbors() const
{
	return neighbors.size();
}

int Region::getNeighbor(const size_t& index) const
{
	return neighbors.at(index);
}

int Region::getID()
{
	return id;
}

//custom functions
void Region::set_war(int Zones, int Danger)
{
	enemy_zones = Zones;
	danger = Danger;
}

void Region::reset_war()
{
	enemy_zones = 0;
	danger = 0;
}

int Region::get_danger()
{
	return danger;
}
int Region::get_zones()
{
	return enemy_zones;
}

int Region::get_enemy_zones()
{
	return enemy_zones;
}
void  Region::set_enemy_zones(int zones)
{
	enemy_zones = zones;
}
int Region::get_neutral_zones()
{
	return neutral_zones;
}
void Region::set_neutral_zones(int zones)
{
	neutral_zones = zones;
}

void Region::set_enemy_armies(int armies)
{
	enemy_armies = armies;
}
int Region::get_enemy_armies()
{
	return enemy_armies;
}
void Region::set_neutral_armies(int armies)
{
	neutral_armies = armies;
}
int Region::get_neutral_armies()
{
	return neutral_armies;
}
