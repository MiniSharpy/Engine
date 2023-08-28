#pragma once
#include <array>
#include <bitset>
#include "../Maths/Vector2.h"
#include "../EntityComponentSystem/Components.h"
#include "../Core/Texture.h"
#include "EntityComponentSystem/TupleHelper.h"
#include <optional>
#include <imgui.h>

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
		void TileEditor();

		template <typename T>
		const T& GetComponent() const
		{
			return std::get<T>(Components);
		}

		const ComponentSlice& GetComponents() const
		{
			return Components;
		}

		const std::bitset<64>& GetEnabledComponents() const
		{
			return EnabledComponents;
		}

	private:
		Vector2<int> Index;
		Vector2<int> Size;
		ComponentSlice Components;
		std::bitset<64> EnabledComponents;
		std::bitset<64> MandatoryComponents;
		static constexpr int MaxTableColumn = 5;

		Vector2<int> Snapping = { 64, 64 };


		void CollisionEditor();

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

		template <typename T>
		void SetComponentUI()
		{
			constexpr size_t index = tuple_element_index_v<T, ComponentSlice>;
			const T& component = std::get<T>(Components);
			const std::string label = typeid(component).name(); // Result is implementation dependent, MSVC is clear and understandable.

			constexpr int column = index % MaxTableColumn;
			if (column == 0)
			{
				ImGui::TableNextRow();
			}
			ImGui::TableSetColumnIndex(column);

			ImGui::BeginDisabled(MandatoryComponents[index]);
			{
				bool isEnabled = EnabledComponents[index];
				ImGui::Checkbox(("##" + label).c_str(), &isEnabled);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				{
					ImGui::SetTooltip("%s", label.c_str());
				}
				EnabledComponents[index] = isEnabled;
			}
			ImGui::EndDisabled();
		}

		template <typename T>
		void ComponentEditor() {}
	};

	template<>
	void Tile::ComponentEditor<Position>();

	template<>
	void Tile::ComponentEditor<Collider>();
}