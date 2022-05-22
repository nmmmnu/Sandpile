#include <cstdint>
#include <string_view>
#include <iostream>

template<typename T, size_t SizeX, size_t SizeY = SizeX>
class Grid{
	T data_[SizeX * SizeY];

	static auto calc__(size_t const x, size_t const y){
		return y * SizeX + x;
	}

public:
	using value_type	= T;
	using size_type		= size_t;

public:
	Grid() = default;

	Grid(T const f){
		fill(f);
	}

	constexpr static auto sizeX(){
		return SizeX;
	}

	constexpr static auto sizeY(){
		return SizeY;
	}

	constexpr const auto &operator()(size_t const x, size_t const y) const{
		return data_[ calc__(x, y) ];
	}

	auto &operator()(size_t const x, size_t const y){
		return data_[ calc__(x, y) ];
	}

	void fill(T const f){
		for(auto &x : data_)
			x = f;
	}

	void clear(){
		fill(0);
	}

	void print() const{
		for(size_t y = 0; y < SizeY; ++y){
			for(size_t x = 0; x < SizeX; ++x)
				std::cout << operator()(x, y) << " ";

			std::cout << "\n";
		}
	}

	template<class CH>
	void prettyPrint(CH const &ch) const{
		for(size_t y = 0; y < SizeY; ++y){
			for(size_t x = 0; x < SizeX; ++x)
				std::cout << ch[ operator()(x, y) ];

			std::cout << "\n";
		}
	}
};



enum class NormalizeGrid_Neigbours{
	Neumann,
	Checkers
};



template<class Grid, NormalizeGrid_Neigbours neigbours = NormalizeGrid_Neigbours::Neumann>
const Grid &normalizeGrid(Grid * grid1, Grid * grid2, typename Grid::value_type maxSand){
	grid2->clear();

	bool running;

	do{
		running = false;

		// not iterating on first and last "element",
		// allow us to be sure that y - 1, y + 1, x - 1, x + 1 are inside the buffer,
		// so we skip one if statement later.
		for(size_t y = 0 + 1; y < Grid::sizeY() - 1; ++y)
			for(size_t x = 0 + 1; x < Grid::sizeX() - 1; ++x){
				auto &g1 = *grid1;
				auto &g2 = *grid2;

				auto const sand = g1(x, y);

				if (sand <= maxSand){
					grid2->operator()(x, y) += sand;
				}else{
					auto const sandF = sand / 4;
					auto const sandD = sand % 4;

					if (neigbours == NormalizeGrid_Neigbours::Neumann){
						g2( x - 1, y    ) += sandF;
						g2( x + 1, y    ) += sandF;
						g2( x    , y - 1) += sandF;
						g2( x    , y + 1) += sandF;
					}else{
						g2( x - 1, y - 1) += sandF;
						g2( x - 1, y + 1) += sandF;
						g2( x + 1, y - 1) += sandF;
						g2( x + 1, y + 1) += sandF;
					}

					g2(x, y) += sandD;

					running = true;
				}

				// Meanwhile we clear g1...
				g1(x, y) = 0;
			}

		// so we can swap it...
		std::swap(grid1, grid2);
	}while(running);

	// can be grid1 or grid2...
	return *grid1;
}



using sint = uint32_t;

constexpr size_t MAX = 1024;

constexpr sint InitialSand = 1'000'000;

constexpr sint MaxSand = 5;

using MyGrid = Grid<sint, MAX, MAX>;

MyGrid grid[2];

int main(){
//	grid( MyGrid::sizeX() / 4 * 1, MyGrid::sizeY() / 2 ) = InitialSand;
//	grid( MyGrid::sizeX() / 4 * 2, MyGrid::sizeY() / 2 ) = InitialSand;
//	grid( MyGrid::sizeX() / 4 * 3, MyGrid::sizeY() / 2 ) = InitialSand;

	grid[0].clear();

	grid[0]( MyGrid::sizeX() / 2, MyGrid::sizeY() / 2 ) = InitialSand;

	const auto &result = normalizeGrid(&grid[0], &grid[1], MaxSand);

	std::cout << "P2\n";
	std::cout << MyGrid::sizeX() << ' ' << MyGrid::sizeY() << "\n";
	std::cout << MaxSand << "\n";

	result.print();
}


