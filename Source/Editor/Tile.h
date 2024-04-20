#pragma once
#include "../EntityComponentSystem/EntityMemoryPool.h"
#include "../Maths/Vector2.h"
#include "../EntityComponentSystem/Components.h"
#include "../EntityComponentSystem/TupleHelper.h"
#include <imgui.h>
#include <array>
#include <bitset>
#include <tuple>


namespace Engine
{
	class Texture;
	class TileAtlas;

	/// <summary>
	/// Tile is an editor representation of all the editable attributes of an entity.
	/// </summary>
	class Tile
	{
	public:
		Tile(TileAtlas& atlas, Vector2<int> index);

		template <typename T>
		const T& GetComponent() const
		{
			return std::get<T>(ComponentSliceData);
		}

		const ComponentSlice& GetComponents() const
		{
			return ComponentSliceData;
		}

		template<typename T>
		bool HasComponent()
		{
			return EnabledComponents[tuple_element_index_v<T, ComponentSlice>];
		}

		template<typename... T>
		bool HasComponents()
		{
			std::bitset<MAX_COMPONENTS> requiredComponents = (... + []()
			{
				return (1 << tuple_element_index_v<T, ComponentSlice>);
			}());

			auto componentsInBoth = requiredComponents & EnabledComponents;
			if (componentsInBoth == requiredComponents) { return true; }
			return false;
		}

		const std::bitset<MAX_COMPONENTS>& GetEnabledComponents() const
		{
			return EnabledComponents;
		}

		std::bitset<MAX_COMPONENTS>& GetEnabledComponents()
		{
			return EnabledComponents;
		}

		ComponentReferenceSlice GetReferenceSlice()
		{
			return ComponentHelper<Components>::CreateReferenceSlice(ComponentSliceData);
		}

	private:
		ComponentSlice ComponentSliceData;
		std::bitset<MAX_COMPONENTS> EnabledComponents;

		template <typename T>
		void AddComponent()
		{
			EnabledComponents[tuple_element_index_v<T, ComponentSlice>] = true;
		}

		template <typename T>
		void RemoveComponent()
		{
			EnabledComponents[tuple_element_index_v<T, ComponentSlice>] = false;
		}
	};
}
