#pragma once

#include <array>
#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Potion
{
	using SignedIndex = std::ptrdiff_t;

	enum class Type : SignedIndex
	{
		healing,
		mana,
		speed,
		invisibility,
		fireResistance,

		maxTypes
	};

	struct Item
	{
		std::string_view name{};
		int cost{};
	};
}

namespace Potion
{
	constexpr bool valid(Potion::Type potionType)
	{
		return (potionType >= Type{}) && (potionType < Type::maxTypes);
	}

	constexpr auto operator+(Potion::Type potionType) noexcept
	{
		return std::to_underlying(potionType);
	}

	class Store
	{
	public:
		enum class Error
		{
			invalidPotion,

			maxErrors
		};

		template <Potion::Type potionType>
		static constexpr Potion::Item getInfo();

		static constexpr auto getInfo(Potion::Type potionType)
			-> std::expected<Potion::Item, Store::Error>;

		static void print();

	private:
		static constexpr std::array s_store{
			Potion::Item{.name{"healing"},         .cost{20}},
			Potion::Item{.name{"mana"},            .cost{30}},
			Potion::Item{.name{"speed"},           .cost{12}},
			Potion::Item{.name{"invisibility"},    .cost{50}},
			Potion::Item{.name{"fire resistance"}, .cost{40}},
		};

		static_assert(std::ssize(s_store) == +Potion::Type::maxTypes,
			"Length of array s_store doesn't match Potion::Type::maxTypes.");
	};

	template <Potion::Type potionType>
	constexpr Potion::Item Store::getInfo()
	{
		// For better error messages.
		static_assert(valid(potionType),
			"Potion type out of bounds.");

		return std::get<+potionType>(s_store);
	}

	constexpr auto Store::getInfo(Potion::Type potionType)
		-> std::expected<Potion::Item, Store::Error>
	{
		if (!valid(potionType))
			return std::unexpected(Store::Error::invalidPotion);

		return std::data(s_store)[+potionType];
	}
}

namespace Potion
{
	class Player
	{
	public:
		enum class PurchaseError
		{
			notEnoughGold,
			potionUnavailable,

			maxErrors
		};

		explicit Player(std::string_view name = "");
		const std::string& getName() const;
		int getGold() const;
		int getAmount(Potion::Type potionType) const;

		template <Potion::Type potionType>
		auto purchase() -> std::expected<void, PurchaseError>;

		auto purchase(Potion::Type potionType)
			-> std::expected<void, PurchaseError>;

		void printInventory();

	private:
		std::string m_name{};
		std::vector<int> m_inventory{};
		int m_gold{};
	};

	consteval auto CONSTEVAL(auto value)
	{
		return value;
	}

	template <Potion::Type potionType>
	auto Player::purchase() -> std::expected<void, PurchaseError>
	{
		if (Player::getGold() < Store::getInfo<potionType>().cost)
			return std::unexpected(PurchaseError::notEnoughGold);

		m_gold -= Store::getInfo<potionType>().cost;
		++std::data(m_inventory)[CONSTEVAL(+potionType)];
		return {};
	}
}