#pragma once
#include<vector>
#include <iostream>

class aState;
class aStateMachine;
class transition
{
	public:
		transition(aState* to, bool(*condition)(aStateMachine* statemachine)):to(to),condition(condition)
		{
		}
		~transition();

		void (*trigger)() = nullptr;
		class aState* to;
		bool(*condition)(aStateMachine* statemachine);
	
};
class aState
{
public:
	//包含entry excution exit translation的链表
	void(*entry)();
		void(*excution)();
		void(*exit)();
	std::vector<transition*> transitions;
	const char* name;
	aState(const char* _name, void(*_excution)(), void(*_entry)()=nullptr, void(*_exit)()= nullptr) :isFinish(false), isStart(false), name(_name)
	{
		excution = _excution;
		entry = _entry;
		exit = _exit;
	}
	void showTransition()
	{
		for (int i = 0; i < transitions.size(); i++)
		{
			std::cout << "transition[" << i << "]"<<name<<"->" << transitions[i]->to->name << std::endl;
		}
	}
	~aState();
	private:
		bool isFinish;
		bool isStart;

};
class aStateMachine
{
public:
	std::vector<aState*> states;
	aState* current;
	aState* previous;
bool started;
private:
	aState finishState = aState("finish", [](){
		std::cout << "finish excution" << std::endl; 
		//started = false;
	});

	public:
		aStateMachine()
		{

	}
		~aStateMachine()
		{

	}
	void showStates()
	{
		//显示所有的states
		for (int i = 0; i < states.size(); i++)
		{
			std::cout << "state[" << i << "]:" << states[i]->name << std::endl;
			states[i]->showTransition();
		}
	}
	void addState(aState* state)
	{
		states.push_back(state);
	}
	void addTransition(aState* from, aState* to, bool(*condition)(aStateMachine*))
	{
		if (to == nullptr)
		{
			//TODO:创建一个finish状态
			to = &finishState;
			std::cout << "addTransition warning: to state not found set to finshState" << std::endl;
		}
		for (int i = 0; i < states.size(); i++)
		{
			if (states[i] == from) {
				from->transitions.push_back(new transition(to, condition));
				return;
			}
		}
		//如果没有找到from状态,则报错
		//如果没有to状态,则默认为finsh状态
		std::cout << "addTransition error: from state not found" << std::endl;
	}
	void start(aState* state)
	{
		current = state;
		if (current->entry != nullptr)
		{
			current->entry();
		}
		started = true;
	}
	void update()
	{
		if (!started)
		{
			std::cout << "update error: stateMachine not started" << std::endl;
			return;
		}
		if (current != nullptr)
		{
			if (current->excution != nullptr)
			{
				current->excution();
			}
			for (int i = 0; i < current->transitions.size(); i++)
			{
				if (current->transitions[i]->condition(this))
				{
					if (current->exit != nullptr)
					{
						current->exit();
					}
					previous = current;
					current = current->transitions[i]->to;
					if (current->entry != nullptr)
					{
						current->entry();
					}
					break;
				}
			}
		}else if(started == false)
		{
			start(states[0]);
		}else{
			std::cout << "update info: stateMachine finished" << std::endl;
		}
	}


};

