#pragma once

#include "Potion.h"
#include <cassert>
#include <expected>
#include <utility>

namespace
{
	constexpr Potion::Type defaultPotion()
	{
		return {};
	}

	constexpr int defaultPotionCost()
	{
		return Potion::Store::getInfo<defaultPotion()>().cost;
	}

	constexpr Potion::Type invalidPotion()
	{
		return Potion::Type{ -1 };
	}

	bool canBuyDefaultPotion(const Potion::Player& potionPlayer);
}

namespace Potion::Test
{
	void compiletimePurchase()
	{
		Potion::Player player{ "Bob" };

		while (canBuyDefaultPotion(player))
		{
			std::expected purchase{ player.purchase<defaultPotion()>() };
			assert(purchase);
		}

		std::expected purchase{ player.purchase<defaultPotion()>() };
		assert(purchase.error() == Player::PurchaseError::notEnoughGold);
	}

	void runtimePurchase()
	{
		Potion::Player player{ "Bob" };

		while (canBuyDefaultPotion(player))
		{
			std::expected purchase{ player.purchase(defaultPotion()) };
			assert(purchase);
		}

		std::expected purchase1{ player.purchase(defaultPotion()) };
		assert(purchase1.error() == Player::PurchaseError::notEnoughGold);
		std::expected purchase2{ player.purchase(invalidPotion()) };
		assert(purchase2.error() == Player::PurchaseError::potionUnavailable);
	}

	void printStore()
	{
		Potion::Store::print();
	}

	void printPlayerInventory()
	{
		Potion::Player player{ "Bob" };
		std::expected purchase{ player.purchase(defaultPotion()) };
		assert(purchase);
		player.printInventory();
	}
}

namespace
{
	bool canBuyDefaultPotion(const Potion::Player& potionPlayer)
	{
		return potionPlayer.getGold() >= defaultPotionCost();
	}
}