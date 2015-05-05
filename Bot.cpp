// stl
#include <iostream>
#include <string>

//project
#include "Bot.h"

//tools
#include "tools/StringManipulation.h"

Bot::Bot() :
armiesLeft(0), timebank(0), timePerMove(0), maxRounds(0), parser(this), phase(NONE), moved(false)
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
	std::cout << startingRegionsreceived.front() << std::endl;

	//

}

void Bot::placeArmies()
{
	//test
	print_warzones();
	// if warzone DNE place armies equally on all zones. 

	std::vector<std::string> moves;
	

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

	std::cout << string::join(moves) << std::endl;
}

std::vector<std::string> Bot::transfers()
{
	std::vector<std::string> moves;
	for (int i = 0; i < ownedRegions.size(); i++)
	{
		// if region is capable of transfering (2 defenders)
		if (regions[ownedRegions[i]].getArmies() > 2)
		{
			//start 
			int highest_threat = regions[ownedRegions[i]].get_danger();
			int neighborid = regions[ownedRegions[i]].getNeighbor(0);
			//for all neighbors 
			for (int j = 0; j <regions[ownedRegions[i]].getNbNeighbors(); j++)
			{
				int next = regions[ownedRegions[i]].getNeighbor(j);
				if (regions[next].getOwner() == ME)
				{
					highest_threat = (regions[next].get_danger() > highest_threat) ? regions[next].get_danger() : highest_threat;
					neighborid = (regions[next].get_danger() > highest_threat) ? next : neighborid;
				}
				

			}
			// if a neighbor exists with a higher threat level
			if (highest_threat > regions[ownedRegions[i]].get_danger() || highest_threat == 0)
			{
				// if no threat all bordering regions are frendly so transfer to a random one of them. 
				if (highest_threat == 0)
				{
					neighborid = regions[ownedRegions[i]].getNeighbor(rand() % regions[ownedRegions[i]].getNbNeighbors());
				}

				std::stringstream move;
				int armies = (regions[ownedRegions[i]].getArmies() - 2);

				

				move << botName << " attack/transfer " << ownedRegions[i] << " "
					<< neighborid << " " << armies;
				subArmies(ownedRegions[i], armies);
				//any armies added here can't be transfered again and so are not added here. 
				//addArmies(neighborid, armies);
				moves.push_back(move.str());
			}
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
			
			// decide if/ where to attack
			//1. if surrounded only by neutral regions: attack sucessful in same super region, or random
			if (regions[warzones[i]].get_enemy_zones() == 0)
			{
				
				//search for a neutral army in same super region or pick random enemy. 
				for (int r = 0; r < regions[warzones[i]].getNbNeighbors(); r++)
				{
					// armies needed to conquer a region
					int needed = std::ceil(((double)(regions[regions[warzones[i]].getNeighbor(r)].getArmies()) *(double)1.61));

					if (regions[regions[warzones[i]].getNeighbor(r)].getOwner() == NEUTRAL)						
					{
						if (regions[regions[warzones[i]].getNeighbor(r)].getSuperRegion() == regions[warzones[i]].getSuperRegion()
							&& regions[warzones[i]].getArmies() > needed)
						{
							// attack with needed troops. 
							// best_moves
							
							std::stringstream move;

							move << botName << " attack/transfer " << warzones[i] << " "
								<< regions[warzones[i]].getNeighbor(r) << " "
								<< (needed);
							subArmies(warzones[i], needed);
							moves.push_back(move.str());
						}
						
						// otherwise if we can take another region not in our super region save it as a backup move. 
						else if (regions[warzones[i]].getArmies() > needed)
						{
							okay_moves.push_back(std::pair<int,int>(needed, regions[warzones[i]].getNeighbor(r)));
							
						}

					}//
					
				}//end searching through neighbors
				// combine best_moves and moves to actual moves
				std::sort(okay_moves.begin(), okay_moves.end());
				int i = 0;
				while (i < okay_moves.size() && regions[warzones[i]].getArmies() > okay_moves[i].first + 1)
				{
					std::stringstream move;

					move << botName << " attack/transfer " << warzones[i] << " "
						<< okay_moves[i].second << " "
						<< (okay_moves[i].first);
					subArmies(warzones[i], okay_moves[i].first);
					moves.push_back(move.str());
					i++;
				}
			} // end only neutral regions.


			//2. if enemy_warzones >= 1; : attack sucessful in same super region || random sucess
			else if (regions[warzones[i]].get_enemy_zones() >= 1)
			{
				
				// priority goes: in same super region, enemies, neutral.
				// search through neighbors
				for (int r = 0; r < regions[warzones[i]].getNbNeighbors(); r++)
				{

					// armies needed to conquer a region
					int needed = std::ceil(((double)(regions[regions[warzones[i]].getNeighbor(r)].getArmies()) *(double)1.61));

					//if super region
					if (regions[regions[warzones[i]].getNeighbor(r)].getSuperRegion() == regions[warzones[i]].getSuperRegion())
					{
						//if enemy -- priority
						if (regions[regions[warzones[i]].getNeighbor(r)].getOwner() == ENEMY
							&& regions[warzones[i]].getArmies() > needed)
						{
							std::stringstream move;

							move << botName << " attack/transfer " << warzones[i] << " "
								<< regions[warzones[i]].getNeighbor(r) << " "
								<< (needed);
							subArmies(warzones[i], needed);
							moves.push_back(move.str());
						}
						//if neutral -- 2nd priority
						else if (regions[regions[warzones[i]].getNeighbor(r)].getOwner() == NEUTRAL
							&& regions[warzones[i]].getArmies() > needed)
						{
							okay_moves.push_back(std::pair<int, int>(needed, regions[warzones[i]].getNeighbor(r)));

						}

					}// end same super region

					else if (regions[regions[warzones[i]].getNeighbor(r)].getOwner() != ME &&
						regions[warzones[i]].getArmies() > regions[regions[warzones[i]].getNeighbor(r)].getArmies() + 2)
					{
						alright_moves.push_back(std::pair<int, int>(needed, regions[warzones[i]].getNeighbor(r)));
					}
				}// done searching through neighbors
				
				// combine moves. 
				std::sort(okay_moves.begin(), okay_moves.end());
				int i = 0;
				while (i < okay_moves.size() && regions[warzones[i]].getArmies() > okay_moves[i].first + 1)
				{
					std::stringstream move;

					move << botName << " attack/transfer " << warzones[i] << " "
						<< okay_moves[i].second << " "
						<< (okay_moves[i].first);
					subArmies(warzones[i], okay_moves[i].first);
					moves.push_back(move.str());
					i++;
				}

				if (i == okay_moves.size()) // if it took all the okay moves do alrigh moves
				{
					std::sort(alright_moves.begin(), alright_moves.end());
					int i = 0;
					while (i < alright_moves.size() && regions[warzones[i]].getArmies() < alright_moves[i].first + 1 )
					{
						std::stringstream move;

						move << botName << " attack/transfer " << warzones[i] << " "
							<< alright_moves[i].second << " "
							<< (alright_moves[i].first);
						subArmies(warzones[i], alright_moves[i].first);
						moves.push_back(move.str());
						i++;
					}

				}
			}
		}// end if they have > 3 armies
	}

	



	
	
	
	/*
	//for all enemy regions visible
	for (int i = 0; i < enemy_regions.size(); i++)
	{
		int needed = std::ceil(((double)regions[enemy_regions[i]].getArmies() *(double)1.5));
		if (needed < 2){ needed = 2; }
			int neighborid = regions[enemy_regions[i]].getNeighbor(0);
			for (int j = 0; j <regions[enemy_regions[i]].getNbNeighbors(); j++)
			{
				int next = regions[enemy_regions[i]].getNeighbor(j);
				if (regions[next].getOwner() == ME)
				{
					// if there are enough of our armies to conquer their region attack. 
					if (regions[next].getArmies() > (needed + 1) )
					{
						std::stringstream move;

						move << botName << " attack/transfer " << next << " "
							<< enemy_regions[i] << " "
							<< (needed);
						subArmies(next, needed);
						moves.push_back(move.str());
					}
					
				}
			}
	}
	*/
	return moves;
}


void Bot::makeMoves()
{
	// START HERE!
	/*
	//default_behavior
	std::vector<std::string> moves;
	for (size_t j = 0; j < ownedRegions.size(); ++j)
	{
		std::stringstream move;
		int i = ownedRegions[j];
		if (regions[i].getArmies() <= 1)
			continue;

		int target = regions[i].getNeighbor(std::rand() % regions[i].getNbNeighbors());
		// prefer enemy regions
		for (unsigned k = 0; k < 5; ++k)
		{
			if (regions[target].getOwner() != ME) break;
			target = regions[i].getNeighbor(std::rand() % regions[i].getNbNeighbors());
		}
		move << botName << " attack/transfer " << i << " "
			<< target << " "
			<< (regions[i].getArmies() - 1);
		moves.push_back(move.str());
	}

	std::cout << string::join(moves) << std::endl;
	*/
	
	
	
	/// Output No moves when you have no time left or do not want to commit any moves.
	// std::cout << "No moves "  << std::endl;
	/// Anatomy of a single move
	//  std::cout << botName << " attack/transfer " << from << " " << to << " "<< armiesMoved;
	/// When outputting multiple moves they must be seperated by a comma
	
	
	std::vector<std::string> moves;

	std::vector<std::string> convoy = transfers();
	std::vector<std::string> charge = attacks();
	std::vector<std::string>::iterator itr;
	
	for (itr = charge.begin(); itr != charge.end(); itr++)
	{
		moves.push_back(*itr);
	}
	for(itr = convoy.begin(); itr != convoy.end(); itr++)
	{
		//moves.push_back(*itr);
	}




	/*
	


	// for each region owned
	for (size_t j = 0; j < ownedRegions.size(); ++j)
	{
		std::stringstream move;
		int i = ownedRegions[j];
		//skip over regions with only 1 army on them (because you can't move the last soldier)
		if (regions[i].getArmies() <= 1)
			continue;
		// decides random region to invade
		int target = regions[i].getNeighbor(std::rand() % regions[i].getNbNeighbors());
		
		// prefer enemy regions
		for ( unsigned k = 0; k < 5; ++k)
		{
			if(regions[target].getOwner() != ME) break;
			target = regions[i].getNeighbor(std::rand() % regions[i].getNbNeighbors());
		}
		// attack with full army. 
		move << botName << " attack/transfer " << i << " "
				<< target << " "
				<< (regions[i].getArmies() - 1);
		moves.push_back(move.str());
	}
	*/
	//check for no moves
	if (moves.empty())
	{
		std::stringstream move;

		 move << botName << " No moves";
		moves.push_back(move.str());
	}



	std::cout << string::join(moves) << std::endl;

}

void Bot::addRegion(const unsigned& noRegion, const unsigned& noSuperRegion)
{
	while (regions.size() <= noRegion)
	{
		regions.push_back(Region());
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
	while (superRegions.size() <= noSuperRegion)
	{
		superRegions.push_back(SuperRegion());
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

void Bot::addStartingRegion(const unsigned& noRegion)
{
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
			regions[regions[noRegion].getNeighbor(i)].set_war(regions[regions[noRegion].getNeighbor(i)].get_zones() + 1, regions[regions[noRegion].getNeighbor(i)].get_danger() + regions[noRegion].getArmies());
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
		insert_warzone(ownedRegions[i]);
		enemy_armies += regions[ownedRegions[i]].get_danger();
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