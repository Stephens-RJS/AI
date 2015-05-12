// stl
#include <iostream>
#include <string>

//project
#include "Bot.h"

//tools
#include "tools/StringManipulation.h"

Bot::Bot() :
armiesLeft(0), timebank(0), timePerMove(0), maxRounds(0), parser(this), phase(NONE), moved(false), next_pick(0)
{
}

Bot::~Bot()
{

}

void Bot::playGame()
{
	parser.parseInput();
}

void Bot::pickStartingRegion()
{
	// START HERE!
	// right now automatically picks first choice
	// START HERE!
	std::cout << startingRegionsreceived.front() << std::endl;

	/*
	//search starting regions recieved for priority's place. 
	bool found = false;
	while (!found)
	{
		for (int r : startingRegionsreceived)
		{
			if (r == priority[next_pick])
			{
				found = true;
				std::cout << r << std::endl;
				next_pick++;
				return;
			}
		}
		next_pick++;
	}
	*/



}
	
	
void Bot::placeArmies()
{
	//test
	//print_warzones();
	// if warzone DNE place armies equally on all zones. 

	std::vector<std::string> moves;
	if (places.size() > 0)
	{
		int z = 0;
		//run through the places that need armies for attacking. 
		while (places.size() > z && places[z].first <= armiesLeft)
		{
			std::stringstream move;
			move << botName << " place_armies " << places[z].second << " " << armiesLeft;
			moves.push_back(move.str());
			addArmies(places[z].second, places[z].first);
			armiesLeft -= places[z].first;
			z++;
		}
		if (armiesLeft != 0 && places.size() > z)
		{
			std::stringstream move;
			move << botName << " place_armies " << places[z].second << " " << armiesLeft;
			moves.push_back(move.str());
			armiesLeft -= armiesLeft;
		}
		else
		{
			std::stringstream move;
			z--;
			move << botName << " place_armies " << places[z].second << " " << armiesLeft;
			moves.push_back(move.str());
			armiesLeft -= armiesLeft;
		}

	}
	// if no priorities
	else{
		// rules for if no warzones exist. 
		if (regions[warzones[0]].get_danger() == 0)
		{
			int armies = armiesLeft / ownedRegions.size();
			for (int i = 0; i < ownedRegions.size() && armiesLeft != 0; i++)
			{

				std::stringstream move;
				if (armiesLeft >= armies)
				{
					move << botName << " place_armies " << ownedRegions[i] << " " << armies;
					moves.push_back(move.str());
					addArmies(ownedRegions[i], armies);
					armiesLeft -= armies;
				}
				else
				{
					move << botName << " place_armies " << ownedRegions[i] << " " << armiesLeft;
					moves.push_back(move.str());
					addArmies(ownedRegions[i], armiesLeft);
					armiesLeft -= armiesLeft;
				}

			}
			if (armiesLeft != 0)
			{
				std::stringstream move;
				move << botName << " place_armies " << ownedRegions[0] << " " << armiesLeft;
				moves.push_back(move.str());
				addArmies(ownedRegions[0], armiesLeft);
				armiesLeft -= armiesLeft;
			}

		}

		//allocate armies according to warzone priority
		else
		{
			double Total = enemy_armies;
			double start_armies = armiesLeft;
			for (int i = 0; i < warzones.size() && armiesLeft != 0; i++)
			{
				std::stringstream move;
				int armies = std::ceil((double)((double)(regions[warzones[i]].get_danger() / Total) * start_armies));

				if (armies > armiesLeft)
				{
					armies = armiesLeft;
				}



				move << botName << " place_armies " << warzones[i] << " " << armies;
				moves.push_back(move.str());
				addArmies(warzones[i], armies);
				armiesLeft -= armies;



			}
			if (armiesLeft != 0)
			{
				std::stringstream move;
				move << botName << " place_armies " << warzones[0] << " " << armiesLeft;
				moves.push_back(move.str());
				addArmies(warzones[0], armiesLeft);
				armiesLeft -= armiesLeft;
			}


		}
	}

	std::cout << string::join(moves) << std::endl;
		
	places.clear();
}

std::vector<std::string> Bot::transfers()
{
	std::vector<std::string> moves;

	
	for (int i = 0; i < ownedRegions.size(); i++)
	{
		// if region is capable of transfering (2 defenders) and does not border an enemy
		if (regions[ownedRegions[i]].getArmies() > 1 && regions[ownedRegions[i]].get_enemy_zones() == 0)
		{
			//start 
			int highest_threat = 0;
			int neighborid = regions[ownedRegions[i]].getNeighbor(rand() % regions[ownedRegions[i]].getNbNeighbors());
			//for all neighbors 
			for (int j = 0; j <regions[ownedRegions[i]].getNbNeighbors(); j++)
			{
				int next = regions[ownedRegions[i]].getNeighbor(j);
				if (regions[next].getOwner() == ME)
				{
					highest_threat = (regions[next].get_enemy_armies() > highest_threat) ? regions[next].get_enemy_armies() : highest_threat;
					neighborid = (regions[next].get_enemy_armies() > highest_threat) ? next : neighborid;
				}
				

			}
		

				std::stringstream move;
				int armies = (regions[ownedRegions[i]].getArmies() - 1);
				move << botName << " attack/transfer " << ownedRegions[i] << " "
					<< neighborid << " " << armies;
				subArmies(ownedRegions[i], armies);
				//any armies added here can't be transfered again and so are not added here. 
				//addArmies(neighborid, armies);
				moves.push_back(move.str());
			
		}
		
	}
	
	return moves;
}

std::vector<std::string> Bot::attacks()
{
	std::vector<std::pair<int,int>> okay_moves; // armies, regions
	std::vector<std::pair<int, int>> alright_moves;// lowest priority moves

	//std::vector<std::string > best_moves;
	std::vector<std::string> moves;
	
	// search through all of my warzones.
	for (int i = 0; i < warzones.size(); i++)
	{

		// if they can attack. ie >3 armies 
		if (regions[warzones[i]].getArmies() >= 3)
		{
			
			// sets checking for less than ideal moves
				
			bool important = false;
				
				// priority goes: in same super region, enemies, neutral.

				// search through neighbors
				for (int r = 0; r < regions[warzones[i]].getNbNeighbors(); r++)
				{
					
					//elimination check do not attack a place if it has already been attacked
					if (!attacked[regions[warzones[i]].getNeighbor(r)])
					{


						// armies needed to conquer a region
						int needed = std::ceil(((double)(regions[regions[warzones[i]].getNeighbor(r)].getArmies()) *(double)1.61));
						if (needed < 2){ needed = 2; }
						
						//if super region
						if (regions[regions[warzones[i]].getNeighbor(r)].getSuperRegion() == regions[warzones[i]].getSuperRegion())
						{


							
							// only do if there are enough armies to attack this neighbor
							if (regions[warzones[i]].getArmies() > needed)
							{
								//if enemy -- priority
								if (regions[regions[warzones[i]].getNeighbor(r)].getOwner() == ENEMY)
								{
									important = true;
									std::stringstream move;

									move << botName << " attack/transfer " << warzones[i] << " "
										<< regions[warzones[i]].getNeighbor(r) << " "
										<< (needed);
									subArmies(warzones[i], needed);
									attacked[regions[warzones[i]].getNeighbor(r)] = true;
									moves.push_back(move.str());
								}
								//if neutral -- 2nd priority
								else if (regions[regions[warzones[i]].getNeighbor(r)].getOwner() == NEUTRAL
									&& regions[warzones[i]].getArmies() > needed)
								{
									okay_moves.push_back(std::pair<int, int>(needed, regions[warzones[i]].getNeighbor(r)));

								}

							}// end enough armies
							// if super region priority but not enough armies, add to targets. 
							else
							{
								targets.push_back(std::pair<int, double>(regions[warzones[i]].getNeighbor(r), 0.0));
							}


						}// end same super region
						else if (regions[regions[warzones[i]].getNeighbor(r)].getOwner() != ME
							&& regions[warzones[i]].getArmies() > needed)
							{
								alright_moves.push_back(std::pair<int, int>(needed, regions[warzones[i]].getNeighbor(r)));
							}
						
					
					
					}// done if already attacked. 
				}// done searching through neighbors
					
				
				// if no important moves have been made. 
				if (!important){
					// combine moves. 
					std::sort(okay_moves.begin(), okay_moves.end());
					int j = 0;
					while (j < okay_moves.size() && regions[warzones[i]].getArmies() > okay_moves[j].first + 1)
					{
						std::stringstream move;

						move << botName << " attack/transfer " << warzones[i] << " "
							<< okay_moves[j].second << " "
							<< (okay_moves[j].first);
						subArmies(warzones[i], okay_moves[j].first);
						moves.push_back(move.str());
						attacked[okay_moves[j].second] = true;
						j++;
					} 

					
						std::sort(alright_moves.begin(), alright_moves.end());
						j = 0;
						while (j < alright_moves.size() && regions[warzones[i]].getArmies() > alright_moves[j].first + 1)
						{
							std::stringstream move;

							move << botName << " attack/transfer " << warzones[i] << " "
								<< alright_moves[j].second << " "
								<< (alright_moves[j].first);
							subArmies(warzones[i], alright_moves[j].first);
							moves.push_back(move.str());
							attacked[alright_moves[j].second] = true;
							j++;
						}

				
			//	}
			}
		}// end if they have > 3 armies
	}

	return moves;
}

std::vector<std::string> Bot::finishIt()
{
	std::vector<std::string> moves;
	std::vector<std::string> final_moves;
	std::vector<std::pair<int, int>> used;
	// search through all enemy_regions
	for (int i = 0; i < enemy_regions.size(); i++)
	{
		int found = 0;
		// armies needed to conquer a region
		int needed = std::ceil(((double)(regions[enemy_regions[i]].getArmies()) *(double)1.61));
		if (needed < 2){ needed = 2; }
		// only do if there are enough armies to attack this neighbor

		// run surround functionality
		int id = enemy_regions[i];

		// iterate through target's neighbors
		for (int i = 0; i < regions[id].getNbNeighbors() && found < needed * 2; i++)
		{
			if (regions[regions[id].getNeighbor(i)].getOwner() == ME)
			{
				if (regions[regions[id].getNeighbor(i)].getArmies() >= 4)
				{
					std::stringstream move;
					move << botName << " attack/transfer " << regions[id].getNeighbor(i) << " "
						<< id << " "
						<< regions[regions[id].getNeighbor(i)].getArmies() - 2;
					moves.push_back(move.str());
					used.push_back(std::pair<int, int>(regions[id].getNeighbor(i), regions[regions[id].getNeighbor(i)].getArmies() - 2));
					found += regions[regions[id].getNeighbor(i)].getArmies() - 2;
				}
			}

		}
		if (found > needed)
		{
			
			std::vector<std::string>::iterator itr;
			for (itr = moves.begin(); itr != moves.end(); itr++)
			{
				final_moves.push_back(*itr);
			}

			for (std::pair<int, int> sub : used)
			{
				subArmies(sub.first, sub.second);
			}
		}
		else
		{
			targets.push_back(std::pair<int, double>(id, 0.0));

		}
	}





	return final_moves;
}

// does everything needed to capture a region by invading from multiple adjacent regions. 
std::vector<std::string> Bot::surround(bool* can, int id, int* required, int* place)
{

	std::vector<std::string> moves;
	std::vector<std::pair<int, int>> used;


	int needed = std::ceil(((double)(regions[id].getArmies()) *(double)1.61));
	if (needed < 2){ needed = 2; }
	int found = 0;
	*can = false;
	for (int i = 0; i < regions[id].getNbNeighbors() && found < needed; i++)
	{
		if (regions[regions[id].getNeighbor(i)].getOwner() == ME)
		{
			*place = regions[id].getNeighbor(i);
			if (regions[regions[id].getNeighbor(i)].getArmies() >= 4)
			{

				std::stringstream move;
				move << botName << " attack/transfer " << regions[id].getNeighbor(i) << " "
					<< id << " "
					<< regions[regions[id].getNeighbor(i)].getArmies() - 2;
				moves.push_back(move.str());
				used.push_back(std::pair<int, int>(regions[id].getNeighbor(i), regions[regions[id].getNeighbor(i)].getArmies() - 2));
				found += regions[regions[id].getNeighbor(i)].getArmies() - 2;
			}
		}
		
	}

	if (found >= needed)
		*can = true;
	if (*can)
	{
		for (std::pair<int, int> sub : used)
		{
			subArmies(sub.first, sub.second);
		}
		attacked[id] = true;
	}
	*required = needed - found;

	return moves;
}

void Bot::process()
{
	// globalmoves
	bool can = false;
	int required = 0;
	int place = 0;

	for (int i = 0; i < targets.size(); i++)
	{
		std::vector<std::string> att = surround(&can, targets[i].first, &required, &place );
		if (can)
		{
			for (std::string X : att)
			{
				priority_attacks.push_back(X);
			}

			//global.push_back(att);
		}
		//if not enought to attack update queue on where to put armies. 
		else
		{
			places.push_back(std::pair<int, int>(required, place));
		}
	}
	std::sort(places.begin(), places.end());
	targets.clear();
}

void Bot::makeMoves()
{
	// set moves to be the priority attacks we have set up previously. 
	std::vector<std::string> moves = priority_attacks;

	std::vector<std::string> convoy = transfers();
	std::vector<std::string> charge;

	if (regions.size() - ownedRegions.size() <= 5)
	{
		charge = finishIt();
	}
	else
	{
		 charge = attacks();
	}
	
	std::vector<std::string>::iterator itr;
	
	for (itr = charge.begin(); itr != charge.end(); itr++)
	{
		priority_attacks.push_back(*itr);
	}
	for(itr = convoy.begin(); itr != convoy.end(); itr++)
	{
		priority_attacks.push_back(*itr);
	}

	//check for no priority_attacks
	if (priority_attacks.empty())
	{
		std::stringstream move;

		 move << botName << " No moves";
		priority_attacks.push_back(move.str());
	}


	
	std::cout << string::join(priority_attacks) << std::endl;
	priority_attacks.clear();
}

void Bot::addRegion(const unsigned& noRegion, const unsigned& noSuperRegion)
{
	while (regions.size() <= noRegion)
	{
		regions.push_back(Region());
		attacked.push_back(false);
	}
	regions[noRegion] = Region(noRegion, noSuperRegion);
	superRegions[noSuperRegion].addRegion(noRegion);
}
void Bot::addNeighbors(const unsigned& noRegion, const unsigned& neighbors)
{
	regions[noRegion].addNeighbor(neighbors);
	regions[neighbors].addNeighbor(noRegion);
}
void Bot::addWasteland(const unsigned &noRegion)
{
	wastelands.push_back(noRegion);
}
void Bot::addSuperRegion(const unsigned& noSuperRegion, const int&reward)
{
	// nosuper is super id, reward is reward
	while (superRegions.size() <= noSuperRegion)
	{
		superRegions.push_back(SuperRegion());
		std::vector<int> X;
		starting_sort.push_back(std::pair<double, std::vector<int>>(0, X));
	}
	superRegions[noSuperRegion] = SuperRegion(reward);
	

}

void Bot::setBotName(const std::string& name)
{
	botName = name;
}
void Bot::setOpponentBotName(const std::string& name)
{
	opponentBotName = name;
}
void Bot::setArmiesLeft(const int& nbArmies)
{
	armiesLeft = nbArmies;
}
void Bot::setTimebank(const int &newTimebank)
{
	timebank = newTimebank;
}
void Bot::setTimePerMove(const int &newTimePerMove)
{
	timePerMove = newTimePerMove;
}
void Bot::setMaxRounds(const int &newMaxRounds)
{
	maxRounds = newMaxRounds;
}

void Bot::clearStartingRegions()
{
	startingRegionsreceived.clear();

}

void Bot::prioritize()
{
	// turn first values into percentages
	for (int i = 0; i < superRegions.size(); i++)
	{
		starting_sort[i].first /= superRegions[i].size();
	}
	std::sort(starting_sort.begin(), starting_sort.end());
	int x = 0;
	while (x < starting_sort.size())
	{
		int y = 0;
		while (y < starting_sort[x].second.size())
		{
			priority.push_back(starting_sort[x].second[y]);
			y++;
		}
		x++;
	}
	




}


void Bot::addStartingRegionnow(const unsigned& noRegion)
{
	startingRegionsreceived.push_back(noRegion);
}

void Bot::addStartingRegion(const unsigned& noRegion)
{

	starting_sort[regions[noRegion].getSuperRegion()].first++;
	starting_sort[regions[noRegion].getSuperRegion()].second.push_back(noRegion);	
	startingRegionsreceived.push_back(noRegion);




}

void Bot::addOpponentStartingRegion(const unsigned& noRegion)
{
	opponentStartingRegions.push_back(noRegion);
}
void Bot::opponentPlacement(const unsigned & noRegion, const int & nbArmies)
{
	// suppress unused variable warnings
	(void) noRegion;
	(void) nbArmies;

	// TODO: STUB
}
void Bot::opponentMovement(const unsigned &noRegion, const unsigned &toRegion, const int &nbArmies)
{
	// suppress unused variable warnings
	(void) noRegion;
	(void) toRegion;
	(void) nbArmies;

	// TODO: STUB
}

void Bot::startDelay(const int& delay)
{
	// suppress unused variable warnings
	(void) delay;
	// TODO: STUB
}
void Bot::setPhase(const Bot::Phase pPhase)
{
	phase = pPhase;
}
void Bot::executeAction()
{
	if (phase == NONE)
		return;
	if (phase == Bot::PICK_STARTING_REGION)
	{
		pickStartingRegion();
	}
	else if (phase == Bot::PLACE_ARMIES)
	{
		placeArmies();
	}
	else if (phase == Bot::ATTACK_TRANSFER)
	{
		makeMoves();
	}
	phase = NONE;
}

void Bot::updateRegion(const unsigned& noRegion, const  std::string& playerName, const int& nbArmies)
{
	
	Player owner;
	if (playerName == botName)
	{
		owner = ME;
		ownedRegions.push_back(noRegion);
		// keep track of our total number of armies
		my_armies += nbArmies;

	}
	else if (playerName == opponentBotName)
	{
		owner = ENEMY;
		enemy_regions.push_back(noRegion);
		//enemy_armies += nbArmies;
	}
		
	else
	{
		owner = NEUTRAL;
		enemy_regions.push_back(noRegion);
		//enemy_armies += nbArmies;
	}
		
	regions[noRegion].setArmies(nbArmies);
	regions[noRegion].setOwner(owner);
	
	// update the neighbors and warzone stuff
	
	for (int i = 0; i < regions[noRegion].getNbNeighbors(); i++)
	{
		// update surrounding regions of this one with info;
		
		if (regions[regions[noRegion].getNeighbor(i)].getOwner() != owner)
		{
			regions[regions[noRegion].getNeighbor(i)].set_war(regions[regions[noRegion].getNeighbor(i)].get_zones() + 1, regions[regions[noRegion].getNeighbor(i)].get_danger() + nbArmies);
			//test
			//std::cout << "neighbor region " << regions[regions[noRegion].getNeighbor(i)].getID() << " \nwarzones " << regions[regions[noRegion].getNeighbor(i)].get_zones() << " \ndanger: " << regions[regions[noRegion].getNeighbor(i)].get_danger() << "\n";
			//insert_warzone(regions[noRegion].getNeighbor(i));
		}

		if (owner == NEUTRAL)
		{
			regions[regions[noRegion].getNeighbor(i)].set_neutral_armies(regions[regions[noRegion].getNeighbor(i)].get_neutral_armies() + nbArmies);
			regions[regions[noRegion].getNeighbor(i)].set_neutral_zones(regions[regions[noRegion].getNeighbor(i)].get_neutral_zones() + 1);

		}
		if (owner == ENEMY)
		{
			regions[regions[noRegion].getNeighbor(i)].set_enemy_armies(regions[regions[noRegion].getNeighbor(i)].get_enemy_armies() + nbArmies);
			regions[regions[noRegion].getNeighbor(i)].set_enemy_zones(regions[regions[noRegion].getNeighbor(i)].get_enemy_zones() + 1);

		}
	}
	

}

void Bot::queue_priority()
{
	std::vector<std::pair<int, double>> sorted;

	//sort and eliminate targets that are the same to prevent redundant attacks;
	std::sort(targets.begin(), targets.end());
	targets.erase(std::unique(targets.begin(), targets.end()), targets.end());

	
	// iterate over potential targets 
	for (int i = 0; i < targets.size(); i++)
	{
		// get thier superRegion info
		int super = regions[targets[i].first].getSuperRegion();
		// get uncontrolled regions
		std::vector<int> ex_regions = superRegions[super].get_regions();
		int unconquered = 0;
		int un_enemy = 0; // 
		int ours = 0;
		
		//search the info of the targets super region. 
		for (int s = 0; s < ex_regions.size(); s++)
		{
			// find the number of unconquered regions,
			// and enemy armies in that super region we must still conquer
			if (regions[ex_regions[s]].getOwner() != ME)
			{
				unconquered++;
				un_enemy += regions[ex_regions[s]].getArmies();
			}
		}
		// set priority of this target based on its super region info. 
		targets[i].second = (double)superRegions[super].get_reward()
			/ (double)(unconquered * un_enemy);

		// place target into sorted vector of targets.
		if (sorted.size() == 0 || targets[i].second < sorted[sorted.size() -1].second )
		{
			sorted.push_back(targets[i]);
		}

		else
		{
			std::vector<std::pair<int, double>>::iterator itr = sorted.begin();
			while (targets[i].second  < itr->second && itr != sorted.end())
			{
				itr++;
			}
			sorted.insert(itr, targets[i]);
		}
	}//end for loop

	targets.clear();
	targets = sorted;
}





void Bot::reset_attacked()
{
	for (int i = 0; i < attacked.size(); i++)
	{
		attacked[i] = false;
	}
	priority_attacks.clear();

}

void Bot::addArmies(const unsigned& noRegion, const int& nbArmies)
{
	my_armies += nbArmies;
	regions[noRegion].setArmies(regions[noRegion].getArmies() + nbArmies);
}
void Bot::subArmies(const unsigned& noRegion, const int& nbArmies)
{
	my_armies -= nbArmies;
	regions[noRegion].setArmies(regions[noRegion].getArmies() - nbArmies);
}
void Bot::moveArmies(const unsigned& noRegion, const unsigned& toRegion, const int& nbArmies)
{
	if (regions[noRegion].getOwner() == regions[toRegion].getOwner() && regions[noRegion].getArmies() > nbArmies)
	{
		regions[noRegion].setArmies(regions[noRegion].getArmies() - nbArmies);
		regions[toRegion].setArmies(regions[toRegion].getArmies() + nbArmies);
	}
	else if (regions[noRegion].getArmies() > nbArmies)
	{
		regions[noRegion].setArmies(regions[noRegion].getArmies() - nbArmies);
		if (regions[toRegion].getArmies() - std::round(nbArmies * 0.6) <= 0)
		{
			regions[toRegion].setArmies(nbArmies - std::round(regions[toRegion].getArmies() * 0.7));
			regions[toRegion].setOwner(regions[noRegion].getOwner());
		}
		else
		{
			regions[noRegion].setArmies(
					regions[noRegion].getArmies() + nbArmies - std::round(regions[toRegion].getArmies() * 0.7));
			regions[toRegion].setArmies(regions[toRegion].getArmies() - std::round(nbArmies * 0.6));
		}
	}
}

void Bot::resetRegionsOwned()
{
	ownedRegions.clear();
}

 std::vector<int> Bot::sortwarzones(std::vector<int> zones)
 {
	 int n = zones.size();
	 // base case
	 if (n == 1)
		 return zones;
	 int m = zones.size() / 2;
	 // set_up recursive vectors
	 std::vector<int>::const_iterator first = zones.begin();
	 std::vector<int>::const_iterator last = zones.end();
	 std::vector<int>::const_iterator middle = zones.begin();
	 
	 middle += m;
	 std::vector<int> zoneA(first, middle);
	 std::vector<int> zoneB(middle, last);
	 //recurse
	 zoneA = sortwarzones(zoneA);
	 zoneB = sortwarzones(zoneB);

// merge sorted sub - arrays using temp array
	 std::vector<int> zone;
	 int i = 0;//iterater for A
	 int j = 0;// iterator for B
	int k = zoneB.size();
	while (i < m && j < k)
	{
		int X = (regions[zoneA[i]].get_danger() > regions[zoneB[k]].get_danger()) 
			? zoneA[i++] : zoneB[j++];
		zone.push_back(X);
	}
		
	while (j < k)
		zone.push_back(zoneB[j++]);

		return zone;

 }

void Bot::createWarzones()
{
	for (int i = 0; i < ownedRegions.size(); i++)
	{
		//test
		//std::cout << i <<": region " << ownedRegions[i] <<  " danger" 
		//	<< regions[ownedRegions[i]].get_danger() << "\n";
		if (regions[ownedRegions[i]].get_zones() > 0)
		{
			insert_warzone(ownedRegions[i]);
			enemy_armies += regions[ownedRegions[i]].get_danger();
		}
			
	}
	
}

void Bot::reset_stats()
{
	my_armies = 0;
	enemy_armies = 0;
	enemy_regions.clear();
	warzones.clear();
	int i = 0;
	while (i < regions.size())
	{
		regions[i++].reset_war();
	}
}

void Bot::insert_warzone(int W)
{
	// if warzones is empty or 
	if (warzones.empty() || (regions[W].get_danger() < regions[warzones[warzones.size() -1]].get_danger()))
		warzones.push_back(W);
	else
	{
		std::vector<int>::iterator itr = warzones.begin();
		while (regions[W].get_danger() < regions[*itr].get_danger() && itr != warzones.end())
		{
			itr++;
		}
		warzones.insert(itr, W);
		
	}


}

void Bot::print_warzones()
{
	for (int i = 0; i < warzones.size(); i++)
	{
		std::cout << "warzone: " << regions[warzones[i]].getID() <<
			" \n\tzones: " << regions[warzones[i]].get_zones() <<
			"\n\tdanger: " << regions[warzones[i]].get_danger() <<"\n";

	}
}