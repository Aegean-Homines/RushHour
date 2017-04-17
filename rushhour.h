/*!
* \file RushHour.h
* \author Egemen Koku
* \date 20 Mar 2017
* \brief Interface of @b rushhour.cpp
*
* \copyright Digipen Institute of Technology
* \mainpage Rush Hour
*
*/

#ifndef RUSHHOUR_H
#define RUSHHOUR_H

#include <vector>
#include <tuple>
#include <deque>
#include <limits>
#include <fstream>
#include <regex>
#include <list>

// Keep this
enum Direction   { up, left, down, right, undefined };

// Keep this
enum Orientation { horisontal, vertical};

// Keep this
std::ostream& operator<<( std::ostream& os, Direction const& d );

// Keep this
////////////////////////////////////////////////////////////////////////////////
// global functions
std::vector< std::tuple<unsigned, Direction, unsigned> > 
SolveRushHour( std::string const& filename );

std::vector< std::tuple<unsigned, Direction, unsigned> > 
SolveRushHourOptimally( std::string const& filename );

////////////////////////////////////////////////////////////////////////////////
// your stuff
////////////////////////////////////////////////////////////////////////////////

// courtesy of Prof. Volper
struct CarInfo {
	unsigned row;
	unsigned column;
	unsigned size;
	Orientation orientation;

	bool operator==(CarInfo const & rhs) const;

	CarInfo(unsigned x = 0, unsigned y = 0, unsigned size = 2,  Orientation orientation = horisontal)
	: row(x), column(y), size(size), orientation(orientation){}

	void PrintCarInfo( unsigned carID ) const;
};

// returned solution typedef
typedef std::vector<std::tuple<unsigned, Direction, unsigned>> MoveList;

// changed "unsigned **" to this #STLRulezzz
typedef std::vector<std::vector<unsigned>> ParkingLotMap;

// typedefs for data containers
typedef std::vector<std::pair<unsigned, CarInfo>> CarLocations;
typedef std::vector<CarLocations> StateHistory; // to prevent infinite loops
typedef std::deque<std::tuple<unsigned, Direction, unsigned>> PossibleMoveVector; // Iterate through this
typedef PossibleMoveVector ReverseMoveVector; // for rolling back the map

// CLOSED LIST OPT
struct ClosedListSearchNode {
	CarLocations carLocation;
	size_t solSize;

	bool operator==(CarLocations const & rhs) const;

	ClosedListSearchNode(CarLocations location, size_t size);
};

typedef std::list<ClosedListSearchNode> ClosedList; // to cancel out some branches

/*
 * Rush Hour solving class that contains all the data needed. Called by the global functions
 */
class RushHourSolver {
private:
	ParkingLotMap parkingLot = ParkingLotMap();      // parking lot 
	unsigned height = 0;            // size of parking lot
	unsigned width = 0;            // size of parking lot
	Direction exitDirection = undefined;    // exit direction
	unsigned car = 0;            // car to be navigated
	std::string filename = std::string();// filename for data

	unsigned currentLevel = 1;
	unsigned maxIterationLevel = std::numeric_limits<unsigned>::max();
	unsigned maxLevel = std::numeric_limits<unsigned>::max();

	// Data for storing vars
	StateHistory stateHistory = StateHistory();
	ClosedList closedList = ClosedList();
	CarLocations currentCarLocations = CarLocations();

	// Helper methods
    /**
     * @brief Member function to calculate all the possible moves and their reverses in each iteration.
     *
     * @param possibleMoves The move list to be applied on the current state.
     * @param reverseMoves The move list to be applied on the resulting state to return to the current state.
     */
	void CalculatePossibleMoves(PossibleMoveVector & possibleMoves, ReverseMoveVector & reverseMoves);

	// Saves bunch of if checks
	/**
	 * @brief Member function to calculate a car's size.
	 * @param x Row of the car
	 * @param y Column of the car
	 * @param carID ID of the car
	 * @return size of the car
	 */
	unsigned CalculateHorizontalCarSize(unsigned x, unsigned y, unsigned carID);
	/**
	 * @brief Member function to calculate a car's size.
	 * @param x Row of the car
	 * @param y Column of the car
	 * @param carID ID of the car
	 * @return size of the car
	 */
	unsigned CalculateVerticalCarSize(unsigned x, unsigned y, unsigned carID);

	// debugging
	/**
	 * @brief Used for debugging to print all the moves given to it
	 * @param moves Moves to be printed
	 */
	void PrintPossibleMoves(PossibleMoveVector& moves ) const;

	/**
	 * @brief Used for debugging to print a single move.
	 * @param move Move to be printed.
	 */
	void PrintMove(std::tuple<unsigned, Direction, unsigned> const &) const;

	/**
	 * @brief Used for printing the current map.
	 */
	void PrintMap() const;

public:
	//debugging
	/**
	 * @brief Used for printing the map into a file
	 * @param filename_out file name to be used to create the file
	 */
	void Print(std::string const& filename_out) const;


	/**
	 * @brief Used for printing the current car locations
	 */
	void PrintCarLocations() const;

	/**
	 * @brief Getter for main car
	 * @return main car
	 */
	unsigned const& Car() const { return car; }

	/**
	 * @brief Getter for exit direction
	 * @return Exit direction
	 */
	Direction const& Dir() const { return exitDirection; }

	/**
	 * @brief Constructor of the class
	 * @param filename The name of the file of the map data
	 */
	RushHourSolver(std::string const&  filename);

	/**
	 * @brief Destructor of the class
	 */
	~RushHourSolver();

	/**
	 * @brief Applies given move to the map and the current car states.
	 * @param move The move to be applied.
	 */
	void makeMove(std::tuple< unsigned, Direction, unsigned > move);

	/**
	 * @brief Checks whether the current state satisfies the problem termination condition
	 * @return Whether solved or not
	 */
	bool IsSolved() const;

	/**
	 * @brief Checking if the solution is correct. This one is the verbose version.
	 * @param sol Solution to be checked
	 * @return Position of the main car
	 */
	int Check(std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol);
	/**
	 * @brief Checking if the solution is correct. This one is the brief version.
	 * @param sol Solution to be checked
	 * @return Position of the main car
	 */
	int CheckBrief(std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol);

	// My rather uncool stuff
	/**
	 * @brief Recursive method for all global rush hour methods
	 * @param solution Solution to be filled
	 * @return Whether it is solved or not
	 */
	bool SolveRushHourRec ( MoveList & solution );

	/**
	 * @brief Initializes car locations.
	 */
	void InitCarLocations();

	/**
	 * @brief For clearing closed list between each iteration.
	 */
	void ClearClosedList();

	/**
	 * @brief Setter for max iteration for IDA*
	 * @param iter New max iteration
	 */
	void MaxIteration(unsigned iter);

};


#endif

