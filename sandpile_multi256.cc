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
	Grid(){
		clear();
	}

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

	void print(){
		for(size_t y = 0; y < SizeY; ++y){
			for(size_t x = 0; x < SizeX; ++x)
				std::cout << operator()(x, y) << " ";

			std::cout << "\n";
		}
	}

	template<class CH>
	void prettyPrint(CH const &ch){
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
void normalizeGrid(Grid *grid1, Grid *grid2, typename Grid::value_type maxSand){
	Grid *grid1_addr = grid1;

	auto addSand = [](Grid &g, size_t const x, size_t const y, auto value){
		if (x >= 0 && x < Grid::sizeX() && y >= 0 && y < Grid::sizeY())
			g(x, y) += value;
	};

	bool running;

	do{
		running = false;

		grid2->clear();

		for(size_t y = 0; y < Grid::sizeY(); ++y)
			for(size_t x = 0; x < Grid::sizeX(); ++x){
				auto const sand = grid1->operator ()(x, y);

				if (sand <= maxSand){
					grid2->operator()(x, y) += sand;
				}else{
					auto const sandF = sand / 4;
					auto const sandD = sand % 4;

					if (neigbours == NormalizeGrid_Neigbours::Neumann){
						addSand(*grid2, x - 1, y    , sandF);
						addSand(*grid2, x + 1, y    , sandF);
						addSand(*grid2, x    , y - 1, sandF);
						addSand(*grid2, x    , y + 1, sandF);
					}else{
						addSand(*grid2, x - 1, y - 1, sandF);
						addSand(*grid2, x - 1, y + 1, sandF);
						addSand(*grid2, x + 1, y - 1, sandF);
						addSand(*grid2, x + 1, y + 1, sandF);
					}

					// no need indirection here...
					grid2->operator()(x, y) += sandD;

					running = true;
				}
			}

		std::swap(grid1, grid2);
	}while(running);

	if (grid1_addr == grid2)
		*grid1 = *grid2;
}



using sint = uint32_t;

constexpr size_t MAX = 512;

constexpr sint InitialSand = 10'000'000;

constexpr sint MaxSand = 256;

using MyGrid = Grid<sint, MAX * 2, MAX>;

MyGrid grid;
MyGrid grid_temp;

int main(){
	grid( MyGrid::sizeX() / 4 * 1, MyGrid::sizeY() / 2 ) = InitialSand;
	grid( MyGrid::sizeX() / 4 * 2, MyGrid::sizeY() / 2 ) = InitialSand;
	grid( MyGrid::sizeX() / 4 * 3, MyGrid::sizeY() / 2 ) = InitialSand;

	normalizeGrid(&grid, &grid_temp, MaxSand);

	std::cout << "P2\n";
	std::cout << MyGrid::sizeX() << ' ' << MyGrid::sizeY() << "\n";
	std::cout << MaxSand << "\n";

	grid.print();
}


