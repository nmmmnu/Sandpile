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
		for(size_t y = 0 + 1; y < Grid::sizeY() - 1; ++y){
			for(size_t x = 0 + 1; x < Grid::sizeX() - 1; ++x){
				auto const sand = grid(x, y);

				if (sand > maxSand){
					auto const sandF = sand / 4;
				//	auto const sandD = sand % 4;

					if constexpr(neigbours == NormalizeGrid_Neigbours::Neumann){
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
		}

		if (++count % 256 == 0)
			std::cerr << count << ' ' << running << ' ' << (initialSand - fill) << '\n';
	}while(running);

	return grid;
}



using sint = uint64_t;

constexpr size_t MAX = 1024 * 4;

constexpr sint InitialSand = 1'000'000'000;

constexpr sint MaxSand = 0b0'1111'1111'1111;

using MyGrid = Grid<sint, MAX, MAX>;

MyGrid grid;

int main(){
	grid.clear();

	grid( MyGrid::sizeX() / 3, MyGrid::sizeY() / 2 ) = InitialSand;

	const auto &result = normalizeGrid(grid, MaxSand, InitialSand);

	auto cc = [](auto, auto pixel, auto mask, auto shift){
		return ( ( pixel & mask ) >> (4 * shift) ) * 16;
	};

	std::cout << "P3\n";
	std::cout << MyGrid::sizeX() << ' ' << MyGrid::sizeY() << "\n";
	std::cout << 255 << "\n";

	for(size_t y = 0; y < MyGrid::sizeY(); ++y){
		for(size_t x = 0; x < MyGrid::sizeX(); ++x){
			auto const pixel = result(x, y);

			std::cout << cc('R', pixel, 0b0'1111'0000'0000, 2) << " ";
			std::cout << cc('G', pixel, 0b0'0000'1111'0000, 1) << " ";
			std::cout << cc('B', pixel, 0b0'0000'0000'1111, 0) << " ";
		}

		std::cout << "\n";
	}

}


