#ifndef REGION_H
#define REGION_H

//stl
#include <vector>
#include <string>

// project
#include "main.h"

class Region
{
public:
	Region();
	Region(const int& pId, const  int& superRegion);
	virtual ~Region();

	void addNeighbor(const int& neighbor);
	void setArmies(const int& nbArmies) { armies = nbArmies; }
	void setOwner(const Player& pOwner){ owner = pOwner; }

	inline int getArmies() const { return armies; }
	inline Player getOwner() const { return owner; }
	inline int getSuperRegion() const { return superRegion; }
	int getNeighbor(const size_t& index) const ;
	int getNbNeighbors() const;
	int getID();
	//custom functions
	void set_war(int Zones, int Danger);
	int get_danger();
	int get_zones();
	void reset_war(); // resets enemy_zones and danger of regions
	int get_enemy_zones();
	void set_enemy_zones(int zones);
	int get_neutral_zones();
	void set_neutral_zones(int zones);
	void set_enemy_armies(int armies);
	int get_enemy_armies();
	void set_neutral_armies(int armies);
	int get_neutral_armies();



private:
	std::vector<int> neighbors;
	int id;
	int superRegion;
	Player owner;
	int armies;
	// custom info
	int enemy_zones; // enemy countries bordering region
	int neutral_zones;
	int danger; // number of enemy armies bordering region
	int enemy_armies;
	int neutral_armies;
	
};

#endif // REGION_H
