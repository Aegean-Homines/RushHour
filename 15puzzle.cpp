//g++ -std=c++11 15.cpp -O3 -pedantic -Wall -Wextra
#include <tuple>
#include <set>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>
#include <map> //multimap
#include <cmath>        // std::abs

enum Direction { up, right, down, left };

class FifteenSolver {
	int ** board = nullptr;
	int * data = nullptr;
	int height = 0;
	int width = 0;
	int num_moves = 0;        // move counter (does not count undo's
public:
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	FifteenSolver(int ** b, int h, int w) : height(h), width(w)
	{
		data = new int[height*width];
		board = new int*[height];
		for (int i = 0; i < height; ++i) {
			board[i] = data + i*width;
		}
		for (int i = 0; i<h; ++i) {
			for (int j = 0; j<w; ++j) {
				board[i][j] = b[i][j];
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////
	// main function: sets up variable and calls recursive DFS
	std::vector< Direction > Solve(int max_depth)  // add max level ????
	{
		std::vector< Direction > solution;
		std::set< std::vector< int > > history;

		std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
		DFS(solution, history, 1, max_depth);
		std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << "Time elapsed " << elapsed_seconds.count() << std::endl;
		std::cout << "Number of moves " << num_moves << std::endl;
		std::cout << "Moves per second " << num_moves / elapsed_seconds.count() << std::endl;

		return solution;
	}
private:
	////////////////////////////////////////////////////////////////////////////
	bool DFS(
		std::vector< Direction > & solution,        // will be populated if solution is found
		std::set< std::vector< int > > & history,   // positions/board seen so far
		int depth,                                  // current depth = solution.size()
		int const& max_depth)                     // depth at which to give up
	{
		//        std::cout << "Start DFS\n";
		//        Print();
		if (AreWeDone()) { // base case
						   //            std::cout << "Done\n";
			return true;
		}
		if (depth > max_depth) { // depth limit
								 //            std::cout << "Max depth\n";
			return false;
		}

		std::vector< std::tuple< int, int, Direction > > possible_moves = PossibleMoves();

		//        for ( auto const& m : possible_moves ) {
		//            enum { z_i=0, z_j=1, Dir=2 }; // enumerate 0,1,2 for readabilty
		//            int zero_i = std::get< z_i >( m );
		//            int zero_j = std::get< z_j >( m );
		//            std::cout << "Move " << zero_i << "," << zero_j << " ";
		//            switch ( std::get< Dir >( m ) ) {
		//                case up:    std::cout << "up"       << std::endl; break;
		//                case down:  std::cout << "down"     << std::endl; break;
		//                case left:  std::cout << "left"     << std::endl; break;
		//                case right: std::cout << "right"    << std::endl; break;
		//            }
		//        }

		for (std::tuple< int, int, Direction > const& m : possible_moves) {
			enum { z_i = 0, z_j = 1, Dir = 2 }; // enumerate 0,1,2 for readabilty

												// coordinates of 0 - same for all moves in this iteration (may be optimized)
			int zero_i = std::get< z_i >(m);
			int zero_j = std::get< z_j >(m);

			// make move
			++num_moves;
			//            if ( num_moves %10000 ) Print();
			switch (std::get< Dir >(m)) {
			case up:    std::swap(board[zero_i][zero_j], board[zero_i - 1][zero_j]); break;
			case down:  std::swap(board[zero_i][zero_j], board[zero_i + 1][zero_j]); break;
			case left:  std::swap(board[zero_i][zero_j], board[zero_i][zero_j - 1]); break;
			case right: std::swap(board[zero_i][zero_j], board[zero_i][zero_j + 1]); break;
			}
			//            std::cout << "After move:\n";
			//            Print();

			// save to history 
			// 1) convert to std::vector 
			std::vector< int > current_position;
			for (int k = 0; k<height*width; ++k) {
				int i = k / width;
				int j = k%width;
				current_position.push_back(board[i][j]);
			}

			// 2) check if already in history
			auto it_pos = history.find(current_position);
			if (it_pos == history.end()) { // not found, i.e. new position
				auto it_insert = history.insert(current_position).first; // guaranteed to insert

																		 // call recursively 
																		 // if recursive call retuns true - solution found, update solution and return 
				if (DFS(solution, history, depth + 1, max_depth)) {
					// this terminates search (since we found solution)
					// as we go up the search tree insert current moves from all recursive calls
					solution.push_back(std::get< Dir >(m));  // update solution
					return true;
				}

				// if here, DFS returned false, no solution found, delete current position/board from history -- or do we?????
				history.erase(it_insert);
			}

			// undo move
			switch (std::get< Dir >(m)) {
			case up:    std::swap(board[zero_i][zero_j], board[zero_i - 1][zero_j]); break;
			case down:  std::swap(board[zero_i][zero_j], board[zero_i + 1][zero_j]); break;
			case left:  std::swap(board[zero_i][zero_j], board[zero_i][zero_j - 1]); break;
			case right: std::swap(board[zero_i][zero_j], board[zero_i][zero_j + 1]); break;
			}
			//            std::cout << "After undo:\n";
			//            Print();
		}

		return false;
	}

	////////////////////////////////////////////////////////////////////////////
	// true if current state is the goal
	bool AreWeDone() const
	{
		for (int k = 0; k<height*width - 1; ++k) {
			int i = k / width;
			int j = k%width;
			if (board[i][j] != k + 1) {
				//                std::cout << "board" << i << " " << j << " is " << board[i][j] << " not " << k+1 << std::endl;
				return false;
			}
		}
		return true;
	}

	////////////////////////////////////////////////////////////////////////////
	// vector of possible moves, move is a triple zero_i, zero_j, direction
	std::vector< std::tuple<int, int, Direction> > PossibleMoves() const
	{
		// find zero
		int zero_i = -1;
		int zero_j = -1;
		for (int i = 0; i<height; ++i) {
			for (int j = 0; j<width; ++j) {
				if (board[i][j] == 0) { zero_i = i; zero_j = j; }
			}
		}

		// which of the 4 moves are possible
		std::vector< std::tuple<int, int, Direction> > possible_moves;
		if (zero_i < height - 1) { possible_moves.push_back(std::tuple< int, int, Direction >(zero_i, zero_j, down)); }
		if (zero_j < width - 1) { possible_moves.push_back(std::tuple< int, int, Direction >(zero_i, zero_j, right)); }
		if (zero_i > 0) { possible_moves.push_back(std::tuple< int, int, Direction >(zero_i, zero_j, up)); }
		if (zero_j > 0) { possible_moves.push_back(std::tuple< int, int, Direction >(zero_i, zero_j, left)); }

		return possible_moves;
	}
	////////////////////////////////////////////////////////////////////////////
	void PrintLine() const
	{
		for (int k = 0; k<height*width; ++k) {
			std::cout << data[k] << " ";
		}
		std::cout << std::endl;
	}
public:
	////////////////////////////////////////////////////////////////////////////
	~FifteenSolver() {
		delete[] data;
		delete[] board;
	}
	////////////////////////////////////////////////////////////////////////////
	void Print() const
	{
		for (int i = 0; i<height; ++i) {
			for (int j = 0; j<width; ++j) {
				std::cout << std::setw(3) << board[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class FifteenChecker {
	int ** board = nullptr;
	int * data = nullptr;
	int height = 0;
	int width = 0;
	bool show_steps = false;
public:
	////////////////////////////////////////////////////////////////////////////
	FifteenChecker(int ** b, int h, int w, bool ss = false) : height(h), width(w), show_steps(ss)
	{
		data = new int[height*width];
		board = new int*[height];
		for (int i = 0; i < height; ++i) {
			board[i] = data + i*width;
		}
		for (int i = 0; i<height; ++i) {
			for (int j = 0; j<width; ++j) {
				board[i][j] = b[i][j];
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////
	~FifteenChecker() {
		delete[] data;
		delete[] board;
	}
	////////////////////////////////////////////////////////////////////////////
	void Print() const
	{
		for (int i = 0; i<height; ++i) {
			for (int j = 0; j<width; ++j) {
				std::cout << std::setw(3) << board[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}
	////////////////////////////////////////////////////////////////////////////
	void Check(std::vector< Direction > const& sol)
	{
		Print();

		// find zero location
		int zero_i = -1;
		int zero_j = -1;
		for (int i = 0; i<height; ++i) {
			for (int j = 0; j<width; ++j) {
				if (board[i][j] == 0) { zero_i = i; zero_j = j; }
			}
		}

		// solution is in reverse (since I used push_back - see DFS)
		std::vector< Direction >::const_reverse_iterator it = sol.rbegin(), it_e = sol.rend();
		for (; it != it_e; ++it) {
			Direction const& dir = *it;
			// make the move
			switch (dir) {
			case up:    if (show_steps) std::cout << "move up\n";   std::swap(board[zero_i][zero_j], board[zero_i - 1][zero_j]); --zero_i; break;
			case down:  if (show_steps) std::cout << "move down\n"; std::swap(board[zero_i][zero_j], board[zero_i + 1][zero_j]); ++zero_i; break;
			case left:  if (show_steps) std::cout << "move left\n"; std::swap(board[zero_i][zero_j], board[zero_i][zero_j - 1]); --zero_j; break;
			case right: if (show_steps) std::cout << "move right\n";std::swap(board[zero_i][zero_j], board[zero_i][zero_j + 1]); ++zero_j; break;
			}
			if (show_steps) Print();
		}
		std::cout << "Done = " << AreWeDone() << std::endl;
	}
	////////////////////////////////////////////////////////////////////////////
	bool AreWeDone() const
	{
		for (int k = 0; k<height*width - 1; ++k) {
			int i = k / width;
			int j = k%width;
			if (board[i][j] != k + 1) {
				std::cout << "board" << i << " " << j << " is " << board[i][j] << " not " << k + 1 << std::endl;
				return false;
			}
		}
		return true;
	}
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool IsEvenPermutation(int * data, int size)
{
	int count = 0;
	for (int i = 0; i<size; ++i) {
		if (data[i] > 0) {
			for (int j = i + 1; j<size; ++j) {
				if (data[j] > 0) {
					if (data[j]<data[i]) {
						++count;
					}
				}
			}
		}
	}
	//std::cout << "count " << count << " parity " << ( count%2 ? "odd":"even" )<< std::endl;
	return !(count % 2); // true if even
}


#include <random>
int mainfake()
{
	int height = 3, width = 3;

	int * data = new int[height*width];
	int ** board = new int*[height];
	for (int i = 0; i < height; ++i) {
		board[i] = data + i*width;
	}

	// init to solved position
	for (int k = 0; k < height*width; ++k) {
		data[k] = k + 1;
	}
	board[height - 1][width - 1] = 0;

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<> dis(0, height*width - 1); // direction to move zero
	do {
		for (int k = 0; k< height*width * log(height*width); ++k) { // NlogN swap to simulate good shuffle
			std::swap(data[dis(gen)], data[dis(gen)]);
		}
	} while (!IsEvenPermutation(data, height*width));

	// if you want to test on a specific board - define it here:

	//hardest 3x3 (31 steps)
	//8 6 7
	//2 5 4
	//3 0 1
	//hardest 3x3 (31 steps)
	//data[0]=8; data[1]=6; data[2]=7; data[3]=2; data[4]=5; data[5]=4; data[6]=3; data[7]=0; data[8]=1;
	//6 4 7
	//8 5 0
	//3 2 1
	//data[0]=6; data[1]=4; data[2]=7; data[3]=8; data[4]=5; data[5]=0; data[6]=3; data[7]=2; data[8]=1;

	// easy 20 steps
	// 5   6   2 
	// 1   8   7 
	// 3   4   0 
	//data[0]=5; data[1]=6; data[2]=2; data[3]=1; data[4]=8; data[5]=7; data[6]=3; data[7]=4; data[8]=0;

	// trivial 2 steps
	// 1   2   3 
	// 4   0   6 
	// 7   5   8 
	//data[0]=1; data[1]=2; data[2]=3; data[3]=4; data[4]=0; data[5]=6; data[6]=7; data[7]=5; data[8]=8;

	FifteenSolver fs(board, height, width);
	fs.Print();

	// maximum necessary steps to solve mxn puzzle (if solvable)
	//    .n\m...1...2...3...4...5...6...7...8...9
	//    .----------------------------------------
	//    .1.|...0...1...2...3...4...5...6...7...8
	//    .2.|...1...6..21..36..55..80.108.140
	//    .3.|...2..21..31..53..84
	//    .4.|...3..36..53..80
	//    .5.|...4..55..84
	//    .6.|...5..80
	//    .7.|...6.108
	//    .8.|...7.140
	//    .9.|...8
	std::vector< Direction> sol;

	//sol = fs.Solve( std::numeric_limits<int>::max() ); // will almost definitely blow the stack
	sol = fs.Solve(100); // max depth level
						 // play with the maximum depth parameter to get good understanding of how to solve optimally

	FifteenChecker fc(board, height, width, false);
	fc.Check(sol);
	std::cout << "solution length " << sol.size() << std::endl;

	delete[] data;
	delete[] board;

	return 0;
}