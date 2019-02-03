#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <string>

// TODO scenario and test functions

/* This code implements the Game of Life into C++, with the rules of 
	- If a live cell has 2 or 3 live neighbours it stays alive, otherwise it dies
	- If a dead cell has 3 live neighbours it becomes alive, otherwise it dies
   This is also known as the rule set B3/S2,3. This code allows for simulation of 
   the Game of Life, where the coordinates of a LiveCell are stored and can be
   printed on the command line.

   I have made the assumptions that not all cells need to be considered (i.e. we 
   don't need a grid of booleans) and so a container with the coordinates of the
   live cells is enough to simulate the problem. I also assumed that no 
   visualisations were needed as none were asked for, and so all outputs are just
   the lists of the coordinates printed on the command line, with some extra 
   information pertaining to the current generation etc. for easier readability.
*/

/* A structure that stores a pair of unsigned integers, intended to be used as the
   coordinates for the live cells in the Game of Life implementation. 

   X - an integer used for the horizontal coordinate of a live cell
   Y - an integer used for the vertical coordinate of a live cell
*/
struct Coordinate
{
	int X, Y; // coordinates for live cells


	// constructors
	
	// intialises coordinates (0,0)
	Coordinate() { int X = 0, Y = 0; }

	// initalise a chosen pair of coordinates
	Coordinate(int X_, int Y_) { X = X_, Y = Y_; }


	/* equivalence operator for Coordinate - returns true if each of the
	   components are pairwise equivalent. 
	*/
	bool operator==(Coordinate xy)
	{
		if (X == xy.X && Y == xy.Y) return true;
		else return false;
	}


	/* comparison operator for lexicographic ordering - returns true if the LHS is
	   lexicographically less than the RHS 
	*/ 
	bool operator<(Coordinate xy) const
	{
		if (X < xy.X) return true;
		else if (X == xy.X && Y < xy.Y) return true;
		else return false;
	}


	/* overloaded print operator for allowing the printing of Coordinate in an 
	   output of the form "(X,Y)" 
	*/
	friend std::ostream &operator<<(std::ostream &out, const Coordinate &xy);
};
 

// friend required to implement print as a member function
std::ostream &operator<<(std::ostream &out, const Coordinate &xy)
{
	out << "(" << xy.X << "," << xy.Y << ")";
	return out;
}


/* a class which implements the game of life and allows the game to be simulated
   on the command line (or another stream output)

   the Life class contains sets of type Coordinate (with LexComparator to define
   the ordering) and an unsigned integer gen which stores the current generation 
   of the game

   LiveCells - a set of type Coordinate and ordering LexComparator which
			   stores the coordinates of the live cells of the current
			   generation. Stored publicly so we can initialise a game in
			   a main() function.
   gen - an unsigned integer which stores the current generation of the
	     game. A new game is always initiliased with current generation 0.
   LiveCellsNextGen - a set of type Coordinate and ordering LexComparator 
					  which is used to store the coordinates of the live
					  cells of the next generation. Stored privately so the
					  only way it can be edited is by the member functions
					  defining the rules of the game.
*/
class Life
{
public:
	

	/* constructor - intialises an empty game with current generation 0. LiveCells
	   is intiialised in main function using the set function insert() to include
	   the live cells of an initial configuration
	*/
	Life(std::set<Coordinate> LiveCells_)
	{
		LiveCells = LiveCells_;
		gen = 0;
	}
	

	// friend required to implement print as a member function
	friend std::ostream& operator<<(std::ostream& out, const Life& Game);

	
	/* advances the game to the next generation.
	
	   This function uses the functions Birth() and SurivavlAndDeath() to 
	   determine the state of the game for the next generation, and then sets 
	   LiveCellsNextGen to be LiveCells (i.e. sets the next generation to be the
	   current one), empties LiveCellsNextGen to be used for determining the next
	   state and advances the generation by 1, (i.e. add one to gen)
	*/
	void AdvanceOne()
	{
		
		Birth(); // add dead cells that become alive to next gen
		SurvivalAndDeath(); // add live cells that stay live to next gen
		LiveCells = LiveCellsNextGen; // set next gen to current gen
		// empty next gen container
		LiveCellsNextGen.erase(LiveCellsNextGen.begin(), LiveCellsNextGen.end());
		gen += 1; // advance generation
	}


	/* advances to the game to generation n for some given unsigned integer n.
	   The function runs AdvanceOne() until gen is equal to n. 
	*/
	void AdvanceTo(unsigned n)
	{
		while (gen < n) AdvanceOne();
	}
	

	/* this function returns the current generation of the game.
	
	@output gen - the member gen which defines the current generation of the game
	*/
	unsigned Gen()
	{
		return gen;
	}

	/* this function returns the number of live cells in the current generation
	
	@output LiveCells.size() - the size of the member set LiveCells
	*/
	unsigned cardLiveCells()
	{
		return LiveCells.size();
	}

private:
	/* LiveCells stores the coordinates of the live cells for the current of the
	   game
	*/
	std::set<Coordinate> LiveCells;

	/* gen stores the current generation of the game */
	unsigned gen;

	/* stores the Coordinates for the state of the game in the next generation. */
	std::set<Coordinate> LiveCellsNextGen;


	/* returns a set of the 9 neighbouring cells of a given Coordinate xy

	   Given some live cell, it has eight neighbouring cells. This function
	   generates a set of type Coordinate and ordering LexComparator which stores
	   the coordinates of the neighbours of a given live cell.

	   @param xy - a Coordinate of some cell. Given we are trying to determine
				   the state of a cell for the next generation, this value will
				   be a Coordinate either of a live cell or its neighbour
	   @output nbrs - a set of Coordinates of the neighbours of the input xy
	*/
	std::set<Coordinate> Neighbours(Coordinate xy)
	{
		// coordinates initialised in lexicographic order
		Coordinate SW(xy.X - 1, xy.Y - 1); // southwest neighbour
		Coordinate W(xy.X - 1, xy.Y); // west neighbour
		Coordinate NW(xy.X - 1, xy.Y + 1); // northwest neighbour
		Coordinate S(xy.X, xy.Y - 1); // south neighbour
		Coordinate N(xy.X, xy.Y + 1); // north neighbour
		Coordinate SE(xy.X + 1, xy.Y - 1); // southeast neighbour
		Coordinate E(xy.X + 1, xy.Y); // east neighbour
		Coordinate NE(xy.X + 1, xy.Y + 1); // northeast neighbour
		// initialise set of neighours
		std::set<Coordinate> nbrs{ SW, W, NW, S, N, SE, E, NE };
		return nbrs;
	}

	/* returns the set of coordinates of dead cells that may become alive at the
	   next generation

	   For each coordinate in the set of live cells, the function generates the
	   set of its neighbours and inserts it into a set dn. The set difference of
	   dn and LiveCells is taken to ensure that this set contains only the
	   Coordinates of the neighbours of live cells that are dead in the current
	   generation

	   @output dn - the set of neighbours of live cells of the current generation
					that are considered dead in the current generation.
	*/
	std::set<Coordinate> AllDeadNeighbours()
	{
		std::set<Coordinate> dn, deadnbrs;
		std::set<Coordinate>::iterator it;
		// for each coordinate, generate its set of neighbours and insert into dn
		for (it = LiveCells.begin(); it != LiveCells.end(); it++)
		{
			std::set<Coordinate> nbrs = Neighbours(*it);
			dn.insert(nbrs.begin(), nbrs.end());
		}
		// take set difference so deadnbrs contains only dead neighbours
		std::set_difference(dn.begin(), dn.end(),
			LiveCells.begin(), LiveCells.end(),
			std::inserter(deadnbrs, deadnbrs.end()));
		return deadnbrs;
	}

	/* includes the dead cells that will become alive in the next generation in
	   LiveCellsNextGen

	   This function uses the set DeadCells generated by the function
	   DeadNeighbours() and determines which cells in this set are going to be
	   alive next generation. For each Coordinate in DeadCells, we use the
	   function Neighbours(_) to generate the set nbrs of its neighbours, and use
	   set_intersection on nbrs and LiveCells. If the size of this intersection
	   is 3, the given coordinate in DeadCells is added to the set defining
	   the next generation LiveCellsNextGen, otherwise it is not.
	*/
	void Birth()
	{
		// generate set of dead neighbours
		std::set<Coordinate> count, nbrs,
			DeadCells = AllDeadNeighbours();
		std::set<Coordinate>::iterator it;
		// if a dead cell has 3 live neighbours, it becomes alive next gen
		for (it = DeadCells.begin(); it != DeadCells.end(); it++)
		{
			nbrs = Neighbours(*it);
			std::set_intersection(nbrs.begin(), nbrs.end(), LiveCells.begin(),
				LiveCells.end(), std::inserter(count, count.end()));
			
			if (count.size() == 3) LiveCellsNextGen.insert(*it);
			
			count.clear();
		}
	}

	/* includes the live cells that stay alive in the next generation in the set
	   LiveCellsNextGen.

	   For each Coordinate in the set LiveCells, it's set of neighbours nbrs is
	   generated using the Neighbours(_) function, and then the set_intersection
	   is taken of nbrs and LiveCells. If this intersection has size 2 or 3, then
	   the given Coordinate from LiveCells is included in LiveCellsNextGen,
	   otherwise it is not.
	*/
	void SurvivalAndDeath()
	{
		std::set<Coordinate>::iterator it;
		std::set<Coordinate> count, nbrs;
		// if a live cell has 2 or 3 live neighbours, it becomes alive next gen
		for (it = LiveCells.begin(); it != LiveCells.end(); it++)
		{
			nbrs = Neighbours(*it);
			std::set_intersection(nbrs.begin(), nbrs.end(), LiveCells.begin(),
			LiveCells.end(), std::inserter(count, count.end()));
			
			if (count.size() == 2 || count.size() == 3)
			{
				LiveCellsNextGen.insert(*it);
			}
			
			count.clear();
		}
	}
};

/* overloaded print operator which prints a set of LiveCells in the form
	   {(X1,Y1),(X2,Y2),...,(Xn,Yn),}
*/
std::ostream& operator<<(std::ostream& out, const Life& Game)
{
	if (Game.LiveCells.size() == 0)
	{
		out << "{}";
		return out;
	}
	else
	{
		std::set<Coordinate>::iterator it;
		out << "{";
		for (it = Game.LiveCells.begin(); it != Game.LiveCells.end(); it++)
		{
			out << *it << " , ";
		}
		out << "\b\b\b" << "}";
		return out;
	}
}

/* This function centres a string with asterisks either side of the centred string
   to pad out the empty space.
   
   Takes an input of a string and the length of the string (with padding) wanted
   and returns a string of the given length with asterisks either side to pad out
   the length
   
   @param in - the string we wish to centre
   @param length - an unsigned integer stating the length we want the output to be
   
   @output pad+in+pad - return the string with the padding *...* on either side to
   achieve the given length 
*/
std::string Centre(std::string const &in, unsigned length)
{
	assert(length >= in.length()); // assert length to be at least length of in
	std::string pad((length - in.length()) / 2, 42);
	return pad + in + pad;
}

/* The following series of functions test the various scenarios given in the 
   candidate instructions. In each of the functions, the scenario is tested where
   a cell's state is considered at the next stage. For simplicity, in each of the
   scenarios we look at what happens to the cell (0,0) in particular between 
   states. Other cells may survive/die/be born, but the scenarios are built to 
   look at what happens at the origin.
*/

/* Initialises an empty game. At the next generation, it is expected that there
   are still no live cells. 
*/
void Scenario0()
{
	Life Game0({});
	std::cout << "Game at generation " << Game0.Gen() << ":\n" << Game0 << "\n";
	Game0.AdvanceOne();
	std::cout << "Game at generation " << Game0.Gen() << ":\n" << Game0 << "\n";
}

/* Initialises a game where (0,0) has only one live neighbour. As a result, we 
   expect that (0,0) is no longer live at the next generation, so it is removed
   from LiveCells.
*/
void Scenario1()
{
	Life Game1({ {0,0},{1,1} });
	std::cout << "Game at generation " << Game1.Gen() << ":\n" << Game1 << "\n";
	Game1.AdvanceOne();
	std::cout << "Game at generation " << Game1.Gen() << ":\n" << Game1 << "\n";
}

/* Initialises a game where (0,0) has 4 live neighbours. Hence we expect at the
   next generation that (0,0) is no longer live since it has neither 2 or 3
   live neighbours, and so it is no longer contained in LiveCells.
*/
void Scenario2()
{
	Life Game2({ {0,0},{1,1},{1,-1},{-1,1},{-1,-1} });
	std::cout << "Game at generation " << Game2.Gen() << ":\n" << Game2 << "\n";
	Game2.AdvanceOne();
	std::cout << "Game at generation " << Game2.Gen() << ":\n" << Game2 << "\n";
}

/* Initialises a game where (0,0) has 2 live neighbours. As a result, (0,0) is 
   still considered to be alive next generation, and so is included in LiveCells.
   This would also be the case if (0,0) had 3 live neighbours. 
*/
void Scenario3()
{
	Life Game3({ {0,0},{1,1},{1,0} });
	std::cout << "Game at generation " << Game3.Gen() << ":\n" << Game3 << "\n";
	Game3.AdvanceOne();
	std::cout << "Game at generation " << Game3.Gen() << ":\n" << Game3 << "\n";
}

/* Initialises a game where (0,0) is not considered live, but 3 cells neighbouring
   (0,0) are live. As a result, we expect (0,0) to be included in LiveCells at the
   next generation.
*/
void Scenario4()
{
	Life Game4({ {1,0},{-1,0},{0,1} });
	std::cout << "Game at generation " << Game4.Gen() << ":\n" << Game4 << "\n";
	Game4.AdvanceOne();
	std::cout << "Game at generation " << Game4.Gen() << ":\n" << Game4 << "\n";
}

/* Scenario 5 is excluded - it is equivalent to Scenario 1. */

/* The following initialises the state of the game known as the "Blinker" - a 3 
   cell line configuration. It is initialised in a horizontal state centered at
   (0,0), and the expected outcome of advancing to the next generation is that 
   the configuration flips between horizontal and vertical configurations centred
   at (0,0) i.e. if generation 0 has a horizontal configuration, then generation 1
   has a vertical configuration and generation 2 has a horizontal 1, identical to 
   generation 0.*/
void Scenario6()
{
	Life Game6({ {0,0},{1,0},{-1,0} });
	std::cout << "Game at generation " << Game6.Gen() << ":\n" << Game6 << "\n";
	Game6.AdvanceOne();
	std::cout << "Game at generation " << Game6.Gen() << ":\n" << Game6 << "\n";
	Game6.AdvanceOne();
	std::cout << "Game at generation " << Game6.Gen() << ":\n" << Game6 << "\n";
}

/* the following main function allows prints the information of the various
   scenarios to screen, with details added for easier readability. A game at 
   generation 0 is simply the initial configuration of the game.
*/
int main()
{
	std::cout << Centre("SCENARIO_0",50) << "\n";
	Scenario0();
	std::cout << "\n" << Centre("SCENARIO_1", 50) << "\n";
	Scenario1();
	std::cout << "\n" << Centre("SCENARIO_2", 50) << "\n";
	Scenario2();
	std::cout << "\n" << Centre("SCENARIO_3", 50) << "\n";
	Scenario3();
	std::cout << "\n" << Centre("SCENARIO_4", 50) << "\n";
	Scenario4();
	std::cout << "\n" << Centre("SCENARIO_5", 50) << "\n";
	std::cout << "Omitted - similar to scenario 0.\n";
	std::cout << "\n" << Centre("SCENARIO_6", 50) << "\n";
	Scenario6();
	return 0;
}