#pragma once
#include <iostream>
#include <memory>
#include <vector>

namespace command
{
	class Game
	{
	public:

		void CreateGame()
		{
			std::cout << "You create game" << std::endl;
		}

		void  OpenGame(const std::string& name_file)
		{
			std::cout << "You open game from " << name_file << std::endl;
		}

		void SaveGame(const std::string& name_file)
		{
			std::cout << "You save game in " << name_file << std::endl;
		}

		void MakeMove(const std::string& move)
		{
			std::cout << "You make move " << move << std::endl;
		}
	};

	std::string GetPlayerInput(const std::string& prompt)
	{
		std::string input;
		std::cout << prompt << std::endl;
		std::cin >> input;
		return input;
	}

	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute() = 0;

	protected:
		Command(std::shared_ptr<Game> game)
			: game_(game)
		{}

		std::shared_ptr<Game> game_ = nullptr;
	};

	class CreateGameCommand : public Command
	{
	public:
		CreateGameCommand(std::shared_ptr<Game> game)
			: Command(game)
		{}

		void Execute() override
		{
			game_->CreateGame();
		}
	};

	class OpenGameCommand : public Command
	{
	public:
		OpenGameCommand(std::shared_ptr<Game> game)
			: Command(game)
		{}

		void Execute() override
		{
			game_->OpenGame(GetPlayerInput("Enter your name game for open"));
		}
	};

	class SaveGameCommand : public Command
	{
	public:
		SaveGameCommand(std::shared_ptr<Game> game)
			: Command(game)
		{}

		void Execute() override
		{
			game_->SaveGame(GetPlayerInput("Enter your name game for save"));
		}
	};

	class MakeMoveCommand : public Command
	{
	public:
		MakeMoveCommand(std::shared_ptr<Game> game)
			: Command(game)
		{}

		void Execute() override
		{
			game_->MakeMove(GetPlayerInput("Entr your move"));
		}
	};

	class UndoGameCommand : public Command
	{
	public:
		UndoGameCommand(std::shared_ptr<Game> game)
			: Command(game)
		{}

		void Execute() override
		{
			game_->OpenGame("Temp");
		}
	};

} // end namespace

void TestCommand()
{
	using namespace command;
	std::shared_ptr<Game> game = std::make_shared<Game>();

	std::vector<std::shared_ptr<Command>> commands;
	commands.push_back(std::make_shared<CreateGameCommand>(game));
	commands.push_back(std::make_shared<MakeMoveCommand>(game));
	commands.push_back(std::make_shared<MakeMoveCommand>(game));
	commands.push_back(std::make_shared<MakeMoveCommand>(game));
	commands.push_back(std::make_shared<SaveGameCommand>(game));
	commands.push_back(std::make_shared<UndoGameCommand>(game));
	for (const auto& command : commands)
	{
		command->Execute();
	}
}