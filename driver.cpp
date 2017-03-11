#include "rushhour.h"
#include <cstdio> /* sscanf */
#include <iostream>
#include <regex>    /* regular expression */
#include <vector>
#include <tuple>
#include <set>

// for driver use only
// you may reuse code - copy-paste and change class name to rushhour.h/cpp
class ParkingLot {
    private:
        unsigned height             = 0;            // size of parking lot
        unsigned width              = 0;            // size of parking lot
        unsigned ** parking_lot     = nullptr;      // parking lot 
        Direction exit_direction    = undefined;    // exit direction
        unsigned car                = 0;            // car to be navigated
        std::string filename        = std::string();// filename for data

    public:
        friend std::ostream& operator<<( std::ostream& os, ParkingLot const& pl );
        void Print( std::string const& filename_out );
        unsigned const& Car() const { return car; }
        Direction const& Dir() const { return exit_direction; }
        
        ParkingLot( std::string const&  filename );
        ~ParkingLot();
        void makeMove( std::tuple< unsigned, Direction, unsigned > move );
        std::vector< std::tuple<unsigned, Direction, unsigned> > Solve() 
        { 
            return SolveRushHour( filename ); 
        }
        std::vector< std::tuple<unsigned, Direction, unsigned> > SolveOptimally() 
            { return SolveRushHourOptimally( filename ); }
        bool IsSolved() const;
        int Check( std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol );
        int CheckBrief( std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol );
};

void run_test( char const * filename, bool optimal )
{
    try {
        ParkingLot pl( filename );
        std::vector< std::tuple<unsigned, Direction, unsigned> > sol;
        if ( optimal ) {
            sol = pl.SolveOptimally( );
        } else {
            sol = pl.Solve( );
        }
        pl.CheckBrief( sol );
    } catch ( char const * msg ) {
        std::cerr << " in driver " << msg << std::endl;
        throw;
    }
}

void test0() { run_test( "level.0", 1 ); }
void test1() { run_test( "level.1", 1 ); }
void test2() { run_test( "level.2", 1 ); }
void test3() { run_test( "level.3", 1 ); }
void test4() { run_test( "level.4", 1 ); }
void test5() { run_test( "level.5", 1 ); }
void test6() { run_test( "level.6", 1 ); }

int main( int argc, char ** argv ) 
{
    //////////////////////////////////////////////////////////////////////
    // no command arguments - display help                              //
    //////////////////////////////////////////////////////////////////////
    if ( argc == 1 ) {                                                  //
        std::cout << "Usage ./" << argv[0]                              //
            << " <level> <optional bool - optimal=1, any=0 (default)\n";//
        return 1;                                                       //
    }                                                                   //
    //////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////
    // single command argument - run predefined test                    //
    //////////////////////////////////////////////////////////////////////
    if ( argc == 2 ) {                                                  //
        void (*pTests[])() = { test0,test1,test2,test3,test4,test5,     //
            test6                                                       //
        };                                                              //
        pTests[atoi( argv[1] )]();                                      //
        return 0;                                                       //
    }                                                                   //
    //////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////
    // else read filename and optimal flag                              //
    // run predefined test                                              //
    //////////////////////////////////////////////////////////////////////
    int optimal = 0;                                                    //
    std::sscanf( argv[2], "%i", &optimal );                             //
    if ( optimal != 0 ) {                                               //
        optimal = 1;                                                    //
    }                                                                   //
    std::string filename( argv[1] );                                    //
                                                                        //
    try {                                                               //
        ParkingLot pl( filename );                                      //
        std::cout << "Initial position (solve by moving car "           //
            << pl.Car() << " " << pl.Dir() << "):\n";                   //
        std::cout << pl;                                                //
        std::vector< std::tuple<unsigned, Direction, unsigned> > sol;   //
        if ( optimal ) {                                                //
            sol = pl.SolveOptimally( );                                 //
        } else {                                                        //
            sol = pl.Solve( );                                          //
        }                                                               //
        pl.Check( sol );                                                //
    } catch ( char const * msg ) {                                      //
        std::cerr << " in driver " << msg << std::endl;                 //
        throw;                                                          //
    }                                                                   //
    //////////////////////////////////////////////////////////////////////
}



// ParkingLot implementation
ParkingLot::ParkingLot( std::string const&  filename ) : filename(filename)
{
    std::ifstream infile ( filename );
    std::string   data; // all data from input file

    if ( ! infile.is_open()) {
        std::cerr << "Errors in input file: cannot open \"" << filename << "\"" << std::endl;
        throw "Errors in input file: cannot open";
    }

    // read the whole file into a string
    while (! infile.eof() ) {
        std::string line;
        std::getline( infile, line );
        data.append( line );
        data.append( " " );
    }

    infile.close();

    // parse data
    std::smatch match;

    if ( std::regex_match( data, match, std::regex( ".*width\\s+(\\d+).*", std::regex_constants::icase ) ) && match.size() == 2 ) {
        // The first sub_match is the whole string; the next
        // sub_match is the first parenthesized expression.
        width = std::stoi( match[1] );
        data = std::regex_replace( data, std::regex( "width\\s+\\d+", std::regex_constants::icase ), "");
    } else {
        std::cerr << "Errors in input file: cannot find \"width\"" << std::endl;
        throw "Errors in input file: cannot find \"width\"";
    }
    if ( std::regex_match( data, match, std::regex( ".*height\\s+(\\d+).*", std::regex_constants::icase ) ) && match.size() == 2 ) {
        height = std::stoi( match[1] );
        data = std::regex_replace( data, std::regex( "height\\s+\\d+", std::regex_constants::icase ), "");
    } else {
        std::cerr << "Errors in input file: cannot find \"height\"" << std::endl;
        throw "Errors in input file: cannot find \"height\"";
    }
    if ( std::regex_match( data, match, std::regex( ".*car\\s+(\\d+).*", std::regex_constants::icase ) ) && match.size() == 2 ) {
        car = std::stoi( match[1] );
        data = std::regex_replace( data, std::regex( "car\\s+\\d+", std::regex_constants::icase ), "");
    } else {
        std::cerr << "Errors in input file: cannot find \"car\"" << std::endl;
        throw "Errors in input file: cannot find \"car\"";
    }
    if ( std::regex_match( data, match, std::regex( ".*exit\\s+([a-z]+).*", std::regex_constants::icase ) ) && match.size() == 2 ) {
        std::string dir_str = match[1].str();
        if      ( dir_str == "left" )   { exit_direction = left; }
        else if ( dir_str == "right" )  { exit_direction = right; }
        else if ( dir_str == "up" )     { exit_direction = up; }
        else if ( dir_str == "down" )   { exit_direction = down; }
        else                            { throw "unknown exit direction "; }
        data = std::regex_replace( data, std::regex( "exit\\s+[a-z]+", std::regex_constants::icase ), "");
    } else {
        std::cerr << "Errors in input file: cannot find \"exit\"" << std::endl;
        throw "Errors in input file: cannot find \"exit\"";
    }

    // only cells are left in the data
    std::regex re_cell("(\\d+)"); // each cell is a number or dot
    std::sregex_iterator cell_matches_begin = std::sregex_iterator( data.begin(), data.end(), re_cell );
    std::sregex_iterator cell_matches_end   = std::sregex_iterator();

    // should have exactly height*width numbers and dots
    if ( std::distance( cell_matches_begin , cell_matches_end ) == height*width ) {
        unsigned * parking_lot_data = new unsigned [height*width];
        parking_lot = new unsigned*[height];
        for ( unsigned i=0; i<height; ++i ) {
            parking_lot[i] = parking_lot_data + i*width;
        }
        unsigned pos = 0;
        for (std::sregex_iterator iter = cell_matches_begin; iter != cell_matches_end; ++iter ) {
            parking_lot_data [pos++] = std::stoi( (*iter).str() );
        }
    } else {
        std::cerr << "Errors in input file: number of cells should be " << height << "*" << width << ". Found " << std::distance( cell_matches_begin , cell_matches_end ) << std::endl;
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
void ParkingLot::makeMove( std::tuple< unsigned, Direction, unsigned > move )
{
    int d = std::get<1>( move ); // convert direction to int
    int di = (d-1)*((3-d)%2);    // see comment before function
    int dj = (d-2)*(d%2);        // see comment before function
    int scan_direction = di+dj; // -1 (up,left) or 1 (down,right)

    unsigned num_positions = std::get<2>( move );
    unsigned car = std::get<0>( move );

    unsigned i_start = ( scan_direction == 1 )?height-1:0;
    unsigned j_start = ( scan_direction == 1 )?width-1 :0;
    for ( unsigned step = 0; step<num_positions; ++step ) { // move car 1 position at a time
        for ( unsigned i=i_start; i<height; i-=scan_direction ) {
            for ( unsigned j=j_start; j<width; j-=scan_direction ) {
                if ( parking_lot[i][j] == car ) {
                    parking_lot[i][j] = 0;
                    // check if legal
                    if ( i+di >= height && j+dj >= width ) {
                        throw( "Car moved outside of parking lot" );
                        return;
                    }
                    if ( parking_lot[i+di][j+dj] > 0 ) {
                        throw( "Car moved on top of another car" );
                        return;
                    }
                    parking_lot[i+di][j+dj] = car;
                }
            }
        }
    }
}

std::ostream& operator<<( std::ostream& os, ParkingLot const& pl ) {
//    os << pl.height << std::endl;
//    os << pl.width << std::endl;
    for ( unsigned i=0; i<pl.height; ++i ) {
        for ( unsigned j=0; j<pl.width; ++j ) {
            os << pl.parking_lot[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

bool ParkingLot::IsSolved() const 
{
    unsigned i_car_pos = height;
    unsigned j_car_pos = width;
    Orientation orientation = horisontal;
    bool done = false;
    for ( unsigned i=0; i<height && !done; ++i ) {
        for ( unsigned j=0; j<width && !done; ++j ) {
            if ( parking_lot[i][j] == car ) {
                if ( i_car_pos == height ) { // first time seeing car
                    i_car_pos = i;
                    j_car_pos = j;
                } else { //second time - ready to make decision 
                    if ( j_car_pos == j ) { // default is horisontal
                        orientation = vertical;
                    }
                    done = true;
                }
            }
        }
    }

    bool solved = false;
    switch ( exit_direction ) {
        case up:    solved = orientation==vertical   && parking_lot[0][j_car_pos] == car; break;
        case left:  solved = orientation==horisontal && parking_lot[i_car_pos][0] == car; break;
        case down:  solved = orientation==vertical   && parking_lot[height-1][j_car_pos] == car; break;
        case right: solved = orientation==horisontal && parking_lot[i_car_pos][width-1] == car; break;
        default: break;
    }
    return solved;
}

int ParkingLot::Check( std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol ) 
{
    try {
        // Print( "solution.txt" );
        for ( std::tuple<unsigned, Direction, unsigned> const& m : sol ) {
            std::cout << "Car " << std::get<0>( m ) << " to be moved " << std::get<1>( m ) << " " << std::get<2>( m ) << " positions\n";
            makeMove( m );
            // Print( "solution.txt" );
            std::cout << *this;
        }
        bool final_pos = IsSolved( );
        std::cout << "Number of steps = " << sol.size() << std::endl;
        std::cout << "Solved = " << final_pos << std::endl;
        return final_pos;
    } catch ( char const * msg ) {
        std::cout << "ERROR - " << msg << std::endl;
        return 1;
    }
}

int ParkingLot::CheckBrief( std::vector< std::tuple<unsigned, Direction, unsigned> > const& sol )
{
    std::cout << "Number of steps = " << sol.size() << std::endl;
    try {
        for ( std::tuple<unsigned, Direction, unsigned> const& m : sol ) {
            makeMove( m );
        }
        bool final_pos = IsSolved( );
        std::cout << "Solved = " << final_pos << std::endl;
        return final_pos;
    } catch ( char const * msg ) {
        std::cout << "ERROR - " << msg << std::endl;
        return 1;
    }
}
ParkingLot::~ParkingLot()
{
    delete [] parking_lot[0];
    delete [] parking_lot;
}

void ParkingLot::Print( std::string const& filename_out )
{
    std::ofstream os;
    os.open( filename_out, std::ofstream::out | std::ofstream::app );
    for ( unsigned i=0; i<height; ++i ) {
        for ( unsigned j=0; j<width; ++j ) {
            os << parking_lot[i][j] << " ";
        }
    }
    os << std::endl;
}
