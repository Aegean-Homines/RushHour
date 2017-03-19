#include <iostream>
#include "rushhour.h"
#include <string>

#define LOG_ENABLED 0

#if LOG_ENABLED
void LOG() {
	std::cout << std::endl;
}

// Helper function for printing lines and debugging (because boy is it necessary)
template<typename Word, typename ...Rest>
void LOG(Word && word, Rest && ...rest) {
	std::cout << std::forward<Word>(word) << " ";
	LOG(std::forward<Rest>(rest)...);
}
#else
void LOG() {
}

// Helper function for printing lines and debugging (because boy is it necessary)
template<typename Word, typename ...Rest>
void LOG(Word && word, Rest && ...rest) {
	static_cast<void>(word);
	LOG(std::forward<Rest>(rest)...);
}
#endif

// Keep this
std::ostream& operator<<( std::ostream& os, Direction const& d ) {
    switch ( d ) {
        case up:    os << "up "; break;
        case left:  os << "left "; break;
        case down:  os << "down "; break;
        case right: os << "right "; break;
        default:    os << "undefined"; break;
    }
    return os;
}

MoveList SolveRushHour(std::string const & filename)
{
	RushHourSolver rh(filename);
	rh.InitCarLocations();
	MoveList allMoves;
	if(!rh.SolveRushHourRec(allMoves)) {
		std::cout << "Rush hour solution couldn't found" << std::endl;
	}
	return allMoves;
}

MoveList SolveRushHourOptimally ( std::string const& filename ) {

	RushHourSolver rh(filename);
	rh.InitCarLocations();
	unsigned maxLevel = 1;
	MoveList allMoves;
	bool done = false;
	do {
		rh.MaxIteration(++maxLevel);
		done = rh.SolveRushHourRec(allMoves);
	} while (!done);

	return allMoves;
}

bool CarInfo::operator== ( CarInfo const& rhs ) const {
	return (row == rhs.row
		&& column == rhs.column
		&& size == rhs.size
		&& orientation == rhs.orientation);
}

void CarInfo::PrintCarInfo ( unsigned carID ) const {
	std::string orientationInString = orientation == horisontal ? "Horizontal" : "Vertical";
	std::cout << std::endl;
	std::cout << "CarID: " << carID << " is at (" << row << ", " << column << ")" << std::endl;
	std::cout << "Car Size: " << size << " with a " << orientationInString << " orientation." << std::endl;
	std::cout << std::endl;
}

// ParkingLot implementation
RushHourSolver::RushHourSolver(std::string const&  filename) : filename(filename)
{
	std::ifstream infile(filename);
	std::string   data; // all data from input file

	if (!infile.is_open()) {
		std::cerr << "Errors in input file: cannot open \"" << filename << "\"" << std::endl;
		throw "Errors in input file: cannot open";
	}

	// read the whole file into a string
	while (!infile.eof()) {
		std::string line;
		std::getline(infile, line);
		data.append(line);
		data.append(" ");
	}

	infile.close();

	// parse data
	std::smatch match;

	if (std::regex_match(data, match, std::regex(".*width\\s+(\\d+).*", std::regex_constants::icase)) && match.size() == 2) {
		// The first sub_match is the whole string; the next
		// sub_match is the first parenthesized expression.
		width = std::stoi(match[1]);
		data = std::regex_replace(data, std::regex("width\\s+\\d+", std::regex_constants::icase), "");
	}
	else {
		std::cerr << "Errors in input file: cannot find \"width\"" << std::endl;
		throw "Errors in input file: cannot find \"width\"";
	}
	if (std::regex_match(data, match, std::regex(".*height\\s+(\\d+).*", std::regex_constants::icase)) && match.size() == 2) {
		height = std::stoi(match[1]);
		data = std::regex_replace(data, std::regex("height\\s+\\d+", std::regex_constants::icase), "");
	}
	else {
		std::cerr << "Errors in input file: cannot find \"height\"" << std::endl;
		throw "Errors in input file: cannot find \"height\"";
	}
	if (std::regex_match(data, match, std::regex(".*car\\s+(\\d+).*", std::regex_constants::icase)) && match.size() == 2) {
		car = std::stoi(match[1]);
		data = std::regex_replace(data, std::regex("car\\s+\\d+", std::regex_constants::icase), "");
	}
	else {
		std::cerr << "Errors in input file: cannot find \"car\"" << std::endl;
		throw "Errors in input file: cannot find \"car\"";
	}
	if (std::regex_match(data, match, std::regex(".*exit\\s+([a-z]+).*", std::regex_constants::icase)) && match.size() == 2) {
		std::string dir_str = match[1].str();
		if (dir_str == "left") { exitDirection = left; }
		else if (dir_str == "right") { exitDirection = right; }
		else if (dir_str == "up") { exitDirection = up; }
		else if (dir_str == "down") { exitDirection = down; }
		else { throw "unknown exit direction "; }
		data = std::regex_replace(data, std::regex("exit\\s+[a-z]+", std::regex_constants::icase), "");
	}
	else {
		std::cerr << "Errors in input file: cannot find \"exit\"" << std::endl;
		throw "Errors in input file: cannot find \"exit\"";
	}

	// only cells are left in the data
	std::regex re_cell("(\\d+)"); // each cell is a number or dot
	std::sregex_iterator cell_matches_begin = std::sregex_iterator(data.begin(), data.end(), re_cell);
	std::sregex_iterator cell_matches_end = std::sregex_iterator();


	// should have exactly height*width numbers and dots
	if (std::distance(cell_matches_begin, cell_matches_end) == height*width) {
		parkingLot.resize(height);
		unsigned heightIter = 0;
		unsigned counter = 0;
		for (std::sregex_iterator iter = cell_matches_begin; iter != cell_matches_end; ++iter, ++counter) {
			heightIter = counter / width;
			parkingLot[heightIter].push_back(std::stoi((*iter).str()));
		}
	}
	else {
		std::cerr << "Errors in input file: number of cells should be " << height << "*" << width << ". Found " << std::distance(cell_matches_begin, cell_matches_end) << std::endl;
		throw "Errors in input file: number of cells";
	}

}

// Direction is an enum (see header):
// +------------>j
// |    ^ 0
// |    |
// |1<--+-->3     
// |    |
// |    V 2
// V        update (i,j) given 1 step in direction d
// i        i += (d-1)*((3-d)%2)    j += (d-2)*(d%2)
// 0 = -1,0
// 1 = 0,-1
// 2 = 1,0
// 3 = 0,1
//

//                                move = car,    direction, num positions
void RushHourSolver::makeMove(std::tuple< unsigned, Direction, unsigned > move)
{
	int d = std::get<1>(move); // convert direction to int
	int deltaRow = (d - 1)*((3 - d) % 2);    // see comment before function
	int deltaColumn = (d - 2)*(d % 2);        // see comment before function
	int scan_direction = deltaRow + deltaColumn; // -1 (up,left) or 1 (down,right)

	unsigned num_positions = std::get<2>(move);
	unsigned car = std::get<0>(move);



	// I changed this function. Since I'm storing car info, the moment I find the car ID
	// I can just move numbers based on the size and the orientation information of the car
	for(auto & pair:currentCarLocations) {
		if(pair.first == car) {
			CarInfo const & carInfo = pair.second;

			unsigned row_start = (deltaRow != 0 && scan_direction == 1) ? carInfo.row + carInfo.size - 1 : carInfo.row;
			unsigned column_start = (deltaColumn != 0 && scan_direction == 1) ? carInfo.column + carInfo.size - 1 : carInfo.column;
			unsigned row_end = row_start + deltaRow * num_positions;
			unsigned column_end = column_start + deltaColumn * num_positions;

			for (unsigned counter = 0; counter < carInfo.size; ++counter) {

				parkingLot[row_start][column_start] = 0;

				// check if legal
				if (row_end >= height && column_end >= width) {
					throw("Car moved outside of parking lot");
				}
				if (parkingLot[row_end][column_end] > 0) {
					throw("Car moved on top of another car");
				}

				parkingLot[row_end][column_end] = car;

				row_start += -deltaRow;
				column_start += -deltaColumn;
				row_end += -deltaRow;
				column_end += -deltaColumn;

			}
			
			pair.second.row = carInfo.row + deltaRow * num_positions;
			pair.second.column = carInfo.column + deltaColumn * num_positions;
			//UpdateCarInfo(car, carInfo.row + deltaRow * num_positions, carInfo.column + deltaColumn * num_positions);
		}
	}
	
}

bool RushHourSolver::IsSolved() const
{
	unsigned i_car_pos = height;
	unsigned j_car_pos = width;
	Orientation orientation = horisontal;
	bool done = false;
	for (unsigned i = 0; i<height && !done; ++i) {
		for (unsigned j = 0; j<width && !done; ++j) {
			if (parkingLot[i][j] == car) {
				if (i_car_pos == height) { // first time seeing car
					i_car_pos = i;
					j_car_pos = j;
				}
				else { //second time - ready to make decision 
					if (j_car_pos == j) { // default is horisontal
						orientation = vertical;
					}
					done = true;
				}
			}
		}
	}

	bool solved = false;
	switch (exitDirection) {
	case up:    solved = orientation == vertical   && parkingLot[0][j_car_pos] == car; break;
	case left:  solved = orientation == horisontal && parkingLot[i_car_pos][0] == car; break;
	case down:  solved = orientation == vertical   && parkingLot[height - 1][j_car_pos] == car; break;
	case right: solved = orientation == horisontal && parkingLot[i_car_pos][width - 1] == car; break;
	default: break;
	}
	return solved;
}

int RushHourSolver::Check(std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol)
{
	try {
		for (std::tuple<unsigned, Direction, unsigned> const& m : sol) {
			std::cout << "Car " << std::get<0>(m) << " to be moved " << std::get<1>(m) << " " << std::get<2>(m) << " positions\n";
			makeMove(m);
		}
		bool final_pos = IsSolved();
		std::cout << "Number of steps = " << sol.size() << std::endl;
		std::cout << "Solved = " << final_pos << std::endl;
		return final_pos;
	}
	catch (char const * msg) {
		std::cout << "ERROR - " << msg << std::endl;
		return 1;
	}
}

int RushHourSolver::CheckBrief(std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol)
{
	std::cout << "Number of steps = " << sol.size() << std::endl;
	try {
		for (std::tuple<unsigned, Direction, unsigned> const& m : sol) {
			makeMove(m);
		}
		bool final_pos = IsSolved();
		std::cout << "Solved = " << final_pos << std::endl;
		return final_pos;
	}
	catch (char const * msg) {
		std::cout << "ERROR - " << msg << std::endl;
		return 1;
	}
}

bool RushHourSolver::SolveRushHourRec ( MoveList & solution )
{
	if (currentLevel > maxIterationLevel)
		return false;


	if(IsSolved())
		return true;

	PossibleMoveVector possibleMoves;
	ReverseMoveVector reverseMoves;

	CalculatePossibleMoves(possibleMoves, reverseMoves);

	while(!possibleMoves.empty()) {

		std::tuple<unsigned, Direction, unsigned> move = possibleMoves.back();
		possibleMoves.pop_back();
		makeMove(move);

		// never seen this state
		if(std::find(stateHistory.begin(), stateHistory.end(), currentCarLocations) == stateHistory.end()) {
			PrintMove(move);

			stateHistory.push_back(currentCarLocations);
			solution.push_back(move);
			++currentLevel;
			if (SolveRushHourRec(solution))
				return true;
			--currentLevel;
			solution.pop_back();
			stateHistory.pop_back();
		}

		std::tuple<unsigned, Direction, unsigned> reverseMove = reverseMoves.back();
		reverseMoves.pop_back();

		makeMove(reverseMove);

	}
	return false;
}

//#TODO need refactoring bad. This entire function is a terrible code block
void RushHourSolver::InitCarLocations ( ) {
	// build the initial carInfo vector
	for (unsigned i = 0; i < height; ++i) {
		for (unsigned j = 0; j < width; ++j) {
			if (parkingLot[i][j] != 0) {
				unsigned carID = parkingLot[i][j];
				unsigned size;

				// Edge cases
				if(j == 0 && parkingLot[i][j + 1] == carID) {
					//Horizontal
					size = CalculateHorizontalCarSize(i, j, carID);
					CarInfo info(i, j, size, horisontal);
					currentCarLocations.push_back(std::make_pair(carID, info));
				}else if(i == 0 && parkingLot[i + 1][j] == carID) {
					// Vertical
					size = CalculateVerticalCarSize(i, j, carID);
					CarInfo info(i, j, size, vertical);
					currentCarLocations.push_back(std::make_pair(carID, info));

				}else if (j != width - 1 &&  parkingLot[i][j + 1] == carID && parkingLot[i][j - 1] != carID) {
					// Horizontal in between
					size = CalculateHorizontalCarSize(i, j, carID);
					CarInfo info(i, j, size, horisontal);
					currentCarLocations.push_back(std::make_pair(carID, info));

				}else if(i != height - 1 && parkingLot[i + 1][j] == carID && parkingLot[i - 1][j] != carID) {
					// Vertical in between
					size = CalculateVerticalCarSize(i, j, carID);
					CarInfo info(i, j, size, vertical);
					currentCarLocations.push_back(std::make_pair(carID, info));
				}

			}
		}
	}
	stateHistory.push_back(currentCarLocations);

	PrintCarLocations();
}

void RushHourSolver::MaxIteration ( unsigned iter ) {
	maxIterationLevel = iter;
}

unsigned RushHourSolver::CalculateHorizontalCarSize ( unsigned x, unsigned y, unsigned carID) {
	unsigned counter = 0;
	while(y + counter < width && parkingLot[x][y + (counter)] == carID) {
		++counter;
	}
	return counter;

}

unsigned RushHourSolver::CalculateVerticalCarSize ( unsigned x, unsigned y, unsigned carID ) {
	unsigned counter = 0;
	while (x + counter < height && parkingLot[x + (counter)][y] == carID) {
		++counter;
	}
	return counter;
}

void RushHourSolver::PrintPossibleMoves(PossibleMoveVector& moves ) const
{
#if LOG_ENABLED
	std::cout << std::endl;
	for (unsigned i = moves.size() - 1; i != -1; --i) {
		PrintMove(moves[i]);
	}
#else
	static_cast<void>(moves);
#endif
}

void RushHourSolver::PrintMove ( std::tuple<unsigned, Direction, unsigned> const & move) const {
#if LOG_ENABLED
	unsigned carID = std::get<0>(move);
	Direction direction = std::get<1>(move); // convert direction to int
	unsigned positions = std::get<2>(move);

	std::cout << "Car with ID: " << carID << " can move " << positions << " steps towards " << direction << std::endl;
#else
	static_cast<void>(move);
#endif// LOG_ENABLED

}

void RushHourSolver::PrintMap ( ) const {
#if LOG_ENABLED
	std::cout << std::endl;
	std::cout << "    ";
	for (unsigned i = 0; i < width; ++i) {
		std::cout << i << " ";
	}
	std::cout << std::endl;
	std::cout << "   ------------" << std::endl;
	for (unsigned i = 0; i<height; ++i) {
		std::cout << i << " | ";
		for (unsigned j = 0; j<width; ++j) {
			std::cout << parkingLot[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif
}

RushHourSolver::~RushHourSolver()
{
}

// #TODO This function also needs refactoring bad.
void RushHourSolver::CalculatePossibleMoves (PossibleMoveVector& possibleMoves, ReverseMoveVector& reverseMoves) {
	CarLocations::iterator iter = currentCarLocations.begin();
	CarLocations::iterator end = currentCarLocations.end();

	while(iter != end) {
		CarInfo * carInfo = &iter->second;
		unsigned carColumn = carInfo->column;
		unsigned carRow = carInfo->row;
		unsigned carID = iter->first;

		if(carInfo->orientation == horisontal) {
			unsigned counter = 1;
			unsigned last = carColumn + carInfo->size;
			while (last < width && parkingLot[carRow][last++] == 0) {
				possibleMoves.push_front(std::tuple<unsigned, Direction, unsigned>(carID, right, counter));
				reverseMoves.push_front(std::tuple<unsigned, Direction, unsigned>(carID, left, counter));
				++counter;
			}
			int begin = carColumn - 1;
			counter = 1;
			while (begin > -1 && parkingLot[carRow][begin--] == 0) {
				possibleMoves.push_front(std::tuple<unsigned, Direction, unsigned>(carID, left, counter));
				reverseMoves.push_front(std::tuple<unsigned, Direction, unsigned>(carID, right, counter));
				++counter;
			}
		}else {
			unsigned counter = 1;
			unsigned last = carRow + carInfo->size;
			while (last < height && parkingLot[last++][carColumn] == 0) {
				possibleMoves.push_front(std::tuple<unsigned, Direction, unsigned>(carID, down, counter));
				reverseMoves.push_front(std::tuple<unsigned, Direction, unsigned>(carID, up, counter));
				++counter;
			}
			int begin = carRow - 1;
			counter = 1;
			while (begin > -1 && parkingLot[begin--][carColumn] == 0) {
				possibleMoves.push_front(std::tuple<unsigned, Direction, unsigned>(carID, up, counter));
				reverseMoves.push_front(std::tuple<unsigned, Direction, unsigned>(carID, down, counter));
				++counter;
			}
		}

		++iter;
	}

}

void RushHourSolver::UpdateCarInfo ( unsigned carID, unsigned newRow, unsigned newColumn )
{
	(void)carID;
	(void)newRow;
	(void)newColumn;


	/*try {
		CarInfo & carInfo = currentCarLocations.at(carID);
		carInfo.row = newRow;
		carInfo.column = newColumn;
	}catch(...) {
		throw("Car ID invalid!");
	}*/

}

void RushHourSolver::Print(std::string const& filename_out) const
{
	std::ofstream os;
	os.open(filename_out, std::ofstream::out | std::ofstream::app);
	os << std::endl;
	for (unsigned i = 0; i<height; ++i) {
		for (unsigned j = 0; j<width; ++j) {
			os << parkingLot[i][j] << " ";
		}
		os << std::endl;
	}
	os << std::endl;
}

void RushHourSolver::PrintCarLocations ( ) const {
#if LOG_ENABLED
	CarLocations::const_iterator iter = currentCarLocations.begin();
	CarLocations::const_iterator end = currentCarLocations.end();
	while (iter != end) {
		iter->second.PrintCarInfo(iter->first);
		++iter;
	}
#endif // LOG_ENABLED

}

