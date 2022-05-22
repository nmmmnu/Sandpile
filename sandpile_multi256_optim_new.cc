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
const Grid &normalizeGrid(Grid &grid, typename Grid::value_type maxSand, typename Grid::value_type initialSand){
	ssize_t running;
	ssize_t fill;
	ssize_t count = 0;

	do{
		fill = 0;

		running = 0;

		// not iterating on first and last "element",
		// allow us to be sure that y - 1, y + 1, x - 1, x + 1 are inside the buffer,
		// so we skip one if statement later.
		for(size_t y = 0 + 1; y < Grid::sizeY() - 1; ++y)
			for(size_t x = 0 + 1; x < Grid::sizeX() - 1; ++x){
				auto const sand = grid(x, y);

				if (sand > maxSand){
					auto const sandF = sand / 4;
				//	auto const sandD = sand % 4;

					if (neigbours == NormalizeGrid_Neigbours::Neumann){
						grid(x - 1, y    ) += sandF;
						grid(x + 1, y    ) += sandF;
						grid(x    , y - 1) += sandF;
						grid(x    , y + 1) += sandF;
					}else{
						grid(x - 1, y - 1) += sandF;
						grid(x - 1, y + 1) += sandF;
						grid(x + 1, y - 1) += sandF;
						grid(x + 1, y + 1) += sandF;
					}

					grid(x, y) -= sandF * 4;

					++running;
				}else if(sand > 0){
					fill += sand;
				}

			}

		if (++count % 256 == 0)
			std::cerr << count << ' ' << running << ' ' << (initialSand - fill) << '\n';
	}while(running);

	// can be grid1 or grid2...
	return grid;
}



using sint = uint32_t;

constexpr size_t MAX = 4096;

constexpr sint InitialSand = 50'000'000;

constexpr sint MaxSand = 5;

using MyGrid = Grid<sint, MAX, MAX>;

MyGrid grid;

int main(){
//	grid( MyGrid::sizeX() / 4 * 1, MyGrid::sizeY() / 2 ) = InitialSand;
//	grid( MyGrid::sizeX() / 4 * 2, MyGrid::sizeY() / 2 ) = InitialSand;
//	grid( MyGrid::sizeX() / 4 * 3, MyGrid::sizeY() / 2 ) = InitialSand;

	grid.clear();

	grid( MyGrid::sizeX() / 2, MyGrid::sizeY() / 2 ) = InitialSand;

	const auto &result = normalizeGrid(grid, MaxSand, InitialSand);

	std::cout << "P2\n";
	std::cout << MyGrid::sizeX() << ' ' << MyGrid::sizeY() << "\n";
	std::cout << MaxSand << "\n";

	result.print();
}


