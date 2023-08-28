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
	};

	// Example
	// using Components = TypeList<Position, Velocity, Zoom>;
	// ComponentHelper<Components>::Pool Pool;
	// ComponentHelper<Components>::Slice Slice;
}