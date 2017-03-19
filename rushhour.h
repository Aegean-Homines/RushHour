#ifndef RUSHHOUR_H
#define RUSHHOUR_H

#include <vector>
#include <tuple>
#include <deque>


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
	unsigned carID;
	Orientation orientation;

	bool operator==(CarInfo const & rhs) const;

	CarInfo(unsigned x = 0, unsigned y = 0, unsigned size = 2, unsigned carID = 1, Orientation orientation = horisontal)
	: row(x), column(y), size(size), carID(carID), orientation(orientation){}

	void PrintCarInfo() const;
};

// returned solution typedef
typedef std::vector<std::tuple<unsigned, Direction, unsigned>> MoveList;

// changed "unsigned **" to this #STLRulezzz
typedef std::vector<std::vector<unsigned>> ParkingLotMap;

// typedefs for data containers
typedef std::vector<CarInfo> CarLocations;
typedef std::vector<CarLocations> StateHistory; // to prevent infinite loops
typedef std::deque<std::tuple<unsigned, Direction, unsigned>> PossibleMoveVector; // Iterate through this
typedef PossibleMoveVector ReverseMoveVector; // for rolling back the map

class RushHourSolver {
private:
	ParkingLotMap parkingLot = ParkingLotMap();      // parking lot 
	unsigned height = 0;            // size of parking lot
	unsigned width = 0;            // size of parking lot
	Direction exitDirection = undefined;    // exit direction
	unsigned car = 0;            // car to be navigated
	std::string filename = std::string();// filename for data

	// Data for storing vars
	StateHistory stateHistory = StateHistory();
	CarLocations currentCarLocations = CarLocations();

	// Helper methods
	void CalculatePossibleMoves(PossibleMoveVector & possibleMoves, ReverseMoveVector & reverseMoves);

	void UpdateCarInfo ( unsigned carID, unsigned newRow, unsigned newColumn );
	// Saves bunch of if checks
	unsigned CalculateHorizontalCarSize(unsigned x, unsigned y, unsigned carID);
	unsigned CalculateVerticalCarSize(unsigned x, unsigned y, unsigned carID);

	// debugging
	void PrintPossibleMoves(PossibleMoveVector& moves ) const;
	void PrintMove(std::tuple<unsigned, Direction, unsigned> const &) const;
	void PrintMap() const;

public:
	//debugging
	void Print(std::string const& filename_out) const;
	void PrintCarLocations() const;
	unsigned const& Car() const { return car; }
	Direction const& Dir() const { return exitDirection; }

	RushHourSolver(std::string const&  filename);
	~RushHourSolver();
	void makeMove(std::tuple< unsigned, Direction, unsigned > move);
	bool IsSolved() const;
	int Check(std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol);
	int CheckBrief(std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol);

	// My rather uncool stuff
	bool SolveRushHourRec ( MoveList & solution );
	void InitCarInfoArray();

	MoveList SolveRushHourOptimallyRec();


};


#endif
