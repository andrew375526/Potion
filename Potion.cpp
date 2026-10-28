#include "Potion.h"
#include "Random.h"
#include <cassert>
#include <expected>
#include <iostream>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
	constexpr auto potionName(Potion::Type potionType)
		-> std::expected<std::string_view, Potion::Store::Error>
	{
		const std::expected potion{ Potion::Store::getInfo(potionType) };

		if (potion)
			return potion->name;

		if (potion.error() == Potion::Store::Error::invalidPotion)
			return std::unexpected(Potion::Store::Error::invalidPotion);

		std::unreachable();
	}

	std::ostream& operator<<(std::ostream& out, const Potion::Item& potion);
	constexpr int g_minStartingGold{ 80 };
	constexpr int g_maxStartingGold{ 120 };
}

namespace Potion
{
	void Store::print()
	{
		for (auto i{ 0z }; i < std::ssize(s_store); ++i)
			std::cout << i << ") " << std::data(s_store)[i] << "\n";
	}
}

namespace Potion
{
	Player::Player(std::string_view name)
		: m_name{ name }
		, m_inventory{ std::vector<int>(static_cast<std::size_t>(CONSTEVAL(+Potion::Type::maxTypes))) }
		, m_gold{ Random::get(g_minStartingGold, g_maxStartingGold) }
	{
	}

	const std::string& Player::getName() const
	{
		return m_name;
	}

	int Player::getGold() const
	{
		return m_gold;
	}

	int Player::getAmount(Potion::Type potionType) const
	{
		return valid(potionType) ? std::data(m_inventory)[+potionType] : 0;
	}

	auto Player::purchase(Potion::Type potionType)
		-> std::expected<void, PurchaseError>
	{
		const std::expected potion{ Store::getInfo(potionType) };

		if (potion && potion->cost > Player::getGold())
			return std::unexpected(PurchaseError::notEnoughGold);

		if (potion)
		{
			m_gold -= potion->cost;
			++std::data(m_inventory)[+potionType];
			return {};
		}

		if (potion.error() == Store::Error::invalidPotion)
			return std::unexpected(PurchaseError::potionUnavailable);

		std::unreachable();
	}

	void Player::printInventory()
	{
		for (auto i{ 0z }; i < std::ssize(m_inventory); ++i)
		{
			const auto potionCount{ getAmount(Potion::Type{ i }) };

			if (potionCount)
			{
				std::cout << potionCount << "x potion of "
					<< *potionName(Potion::Type{ i }) << "\n";
			}
		}
	}
}

namespace
{
	std::ostream& operator<<(std::ostream& out, const Potion::Item& potion)
	{
		return out << potion.name << " costs " << potion.cost;
	}
}