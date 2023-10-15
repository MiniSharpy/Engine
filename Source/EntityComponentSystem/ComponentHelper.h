#pragma once
#include <tuple>
#include <vector>

namespace Engine
{
	// https://stackoverflow.com/a/37558067
	template<typename... Ts>
	struct TypeList { };

	template<class> struct ComponentHelper;

	template<template<typename...Args> class t, typename ...Ts>
	struct ComponentHelper<t<Ts...>> {
		using Pool = std::tuple<std::vector<Ts>...>;
		using Slice = std::tuple<Ts...>;
		using ReferenceSlice = std::tuple<Ts&...>;

		// Sometimes it's necessary to capture a copy of an entity, or to set up an entity later, and so to avoid virtual overhead the functions are necessary.
		static ReferenceSlice CreateReferenceSlice(Slice& slice)
		{
			return std::tie(std::get<Ts>(slice) ...);
		}

		static ReferenceSlice CreateReferenceSlice(Pool& pool, size_t id)
		{
			return std::tie( std::get<std::vector<Ts>>(pool)[id] ... );
		}

		static Slice CreateSlice(Pool& pool, size_t id)
		{
			return std::make_tuple(std::get<std::vector<Ts>>(pool)[id] ...);
		}
	};

	// Example
	// using Components = TypeList<Position, Velocity, Zoom>;
	// ComponentHelper<Components>::Pool Pool;
	// ComponentHelper<Components>::Slice Slice;
}