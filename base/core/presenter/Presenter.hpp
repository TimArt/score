#pragma once
#include <core/presenter/CommandQueue.hpp>

namespace iscore
{
	class Command;
	class CustomCommand;
	class Model;
	class View;
	class Presenter : public QObject
	{
		public:
			Presenter(iscore::Model* model, iscore::View* view);

			void addCustomCommand(CustomCommand*);

		private slots:
			void applyCommand(Command*);

		private:
			CommandQueue m_commandQueue;
			Model* m_model;
			View* m_view;

			std::vector<CustomCommand*> m_customCommands;
	};
}
