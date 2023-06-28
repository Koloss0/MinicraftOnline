#pragma once

#include <core/common.h>
#include <unordered_map>
#include <vector>
#include <cstdint>

typedef uint8_t TileID; // 256 possible IDs

struct TileData
{
	struct FloorSourceData
	{
		const std::vector<IntRect> connected_dr =  {};
		const std::vector<IntRect> connected_dlr = {};
		const std::vector<IntRect> connected_dl =  {};
		const std::vector<IntRect> connected_udr = {};
		const std::vector<IntRect> connected_all = {};
		const std::vector<IntRect> connected_udl = {};
		const std::vector<IntRect> connected_ur =  {};
		const std::vector<IntRect> connected_ulr = {};
		const std::vector<IntRect> connected_ul =  {};
	};

	struct WallSourceData
	{
		const std::vector<IntRect> connected_dr =  {};
		const std::vector<IntRect> connected_dlr = {};
		const std::vector<IntRect> connected_dl =  {};
		const std::vector<IntRect> connected_udr = {};
		const std::vector<IntRect> connected_all = {};
		const std::vector<IntRect> connected_udl = {};
		const std::vector<IntRect> connected_ur =  {};
		const std::vector<IntRect> connected_ulr = {};
		const std::vector<IntRect> connected_ul =  {};
		
		const std::vector<IntRect> connected_ul_corner = {};
		const std::vector<IntRect> connected_ur_corner = {};
		const std::vector<IntRect> connected_dl_corner = {};
		const std::vector<IntRect> connected_dr_corner = {};
	};

	const FloorSourceData floor_sources;
	const WallSourceData wall_sources;
	const unsigned char palette_index;
	const bool is_connective;
	const bool is_solid;
	const bool is_fluid;
};

const TileData::FloorSourceData BASIC_FLOOR_SOURCES = {
	{
		{0, -40, 8, 8}
	},
	{
		{8, -40, 8, 8}
	},
	{
		{16, -40, 8, 8}
	},
	{
		{0, -48, 8, 8}
	},
	{
		{0, -24, 8, 8},
		{8, -24, 8, 8},
		{0, -32, 8, 8},
		{8, -32, 8, 8},
	},
	{
		{16, -48, 8, 8}
	},
	{
		{0, -56, 8, 8}
	},
	{
		{8, -56, 8, 8}
	},
	{
		{16, -56, 8, 8}
	},
};

const TileData::WallSourceData BASIC_WALL_SOURCES = {
	{
		{24, -40, 8, 8}
	},
	{
		{32, -40, 8, 8}
	},
	{
		{40, -40, 8, 8}
	},
	{
		{24, -48, 8, 8}
	},
	{
		{32, -48, 8, 8}
	},
	{
		{40, -48, 8, 8}
	},
	{
		{24, -52, 8, 8}
	},
	{
		{32, -52, 8, 8}
	},
	{
		{40, -52, 8, 8}
	},
	{
		{48, -40, 8, 8}
	},
	{
		{56, -40, 8, 8}
	},
	{
		{48, -48, 8, 8}
	},
	{
		{56, -48, 8, 8}
	},
};

extern const TileData TILES[];
