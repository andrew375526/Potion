#include "Input.h"
#include "Potion.h"
#include <expected>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace
{
	enum class InputError
	{
		NaN,
		invalidSelection,

		maxErrors
	};

	std::string getName(std::string_view prompt)
	{
		std::cout << prompt;
		std::string name{};
		std::getline(std::cin >> std::ws, name);
		return name;
	}

	constexpr bool isNum(char input)
	{
		return (input >= '0') && (input <= '9');
	}

	constexpr std::expected<int, InputError> charToInt(char input)
	{
		if (!isNum(input)) return std::unexpected(InputError::NaN);
		return input - '0';
	}

	constexpr auto charToPotionType(char input)
		-> std::expected<Potion::Type, InputError>
	{
		if (!isNum(input)) return std::unexpected(InputError::NaN);

		if (!valid(Potion::Type{ *charToInt(input) }))
			return std::unexpected(InputError::invalidSelection);

		return Potion::Type{ *charToInt(input) };
	}

	constexpr std::string_view invalidMessage()
	{
		return "That is an invalid input. Try again: ";
	}

	std::optional<Potion::Type> getPotionType(std::string_view prompt)
	{
		while (true)
		{
			std::cout << prompt;
			char input{};
			std::cin >> input;

			if (!std::cin)
			{
				Input::clearFailedExtraction();
				prompt = invalidMessage();
				continue;
			}

			if (Input::hasUnextractedInput())
			{
				Input::clearBuffer();
				prompt = invalidMessage();
				continue;
			}

			if (input == 'q') return {}; // q to quit.
			Input::clearBuffer();        // Clear extraneous input.
			const std::expected selectedPotionType{ charToPotionType(input) };

			if (selectedPotionType) return *selectedPotionType;

			if (selectedPotionType.error() == InputError::NaN
				|| selectedPotionType.error() == InputError::invalidSelection)
			{
				Input::clearFailedExtraction();
				prompt = invalidMessage();
				continue;
			}
		}
	}

	void makePurchase(Potion::Player& player, Potion::Type potionType)
	{
		const std::expected purchase{ player.purchase(potionType) };

		if (purchase)
		{
			std::cout << "You purchased a potion of "
				<< Potion::Store::getInfo(potionType)->name
				<< ". You have " << player.getGold() << " gold left.\n\n";

			return;
		}

		if (purchase.error() == Potion::Player::PurchaseError::notEnoughGold)
		{
			std::cout << "You can not afford that.\n\n";
			return;
		}

		if (purchase.error()
			== Potion::Player::PurchaseError::potionUnavailable)
		{
			std::cout << "That potion is unavailable.\n\n";
			return;
		}

		std::unreachable();
	}

	void shop(Potion::Player& player)
	{
		while (true)
		{
			std::cout << "Here is our selection for today:\n";
			Potion::Store::print();

			constexpr std::string_view prompt{ "Enter the number of the potion"
				" you'd like to buy, or \'q\' to quit: " };

			const std::optional selectedPotion{ getPotionType(prompt) };

			if (!selectedPotion)
			{
				std::cout << "\nYour inventory contains:\n";
				player.printInventory();

				std::cout << "You escaped with " << player.getGold()
					<< " gold remaining.\n\n";

				break;
			}

			makePurchase(player, *selectedPotion);
		}
	}
}

#ifdef _DEBUG
#include "PotionTest.h"
#endif

int main()
{
#ifdef _DEBUG
	{
		std::cout << "TESTS:\n";
		Potion::Test::compiletimePurchase();
		Potion::Test::runtimePurchase();
		std::cout << "TESTS ENDED\n\n";
	}
#endif

	std::cout << "Welcome to Roscoe's potion emporium!\n";
	Potion::Player player{ getName("Enter your name: ") };

	std::cout << "Hello, " << player.getName() << ", you have "
		<< player.getGold() << " gold.\n\n";

	shop(player);
	std::cout << "Thanks for shopping at Roscoe's potion emporium!\n";
	return 0;
}