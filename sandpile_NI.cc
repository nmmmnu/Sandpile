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

template<class Grid>
void normalizeGrid(Grid &grid1, Grid &grid2, typename Grid::value_type maxSand){
	auto addSand = [](Grid &g, size_t const x, size_t const y, auto value){
		if (x >= 0 && x < g.sizeX() && y >= 0 && y < g.sizeY())
			g(x, y) += value;
	};

	bool running;

	grid2 = grid1;

	do{
		running = false;

		for(size_t y = 0; y < Grid::sizeX(); ++y)
			for(size_t x = 0; x < Grid::sizeY(); ++x){
				auto const sand = grid1(x, y);

				if (sand > maxSand){
					addSand(grid2, x - 1, y    , 1);
					addSand(grid2, x + 1, y    , 1);
					addSand(grid2, x    , y - 1, 1);
					addSand(grid2, x    , y + 1, 1);

					// no need indirection here...
					grid2(x, y) += - 4;

					running = true;
				}
			}

		grid1 = grid2;
	}while(running);
}



using sint = uint32_t;

constexpr size_t MAX = 512;

constexpr sint InitialSand = 500'000;

constexpr sint MaxSand = 4;

using MyGrid = Grid<sint, MAX>;

MyGrid grid;
MyGrid grid_temp;

int main(){
	grid( MyGrid::sizeX() / 2, MyGrid::sizeY() / 2 ) = InitialSand;

	normalizeGrid(grid, grid_temp, MaxSand);

//	constexpr std::string_view ch[]{ " ", "░", "▒", "▓", "█" };
	constexpr std::string_view ch[]{ "0 ", "64 ", "128 ", "192 ", "255 " };

	std::cout << "P2\n";
	std::cout << MyGrid::sizeX() << ' ' << MyGrid::sizeY() << "\n";
	std::cout << 255 << "\n";

	grid.prettyPrint(ch);
}


