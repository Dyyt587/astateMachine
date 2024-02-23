#pragma once
#include<vector>
#include <iostream>
#include <cstdio>
/**
* auto start: if user do not set the start state we will use the first state added in state machine to start
*/
/*enable auto start*/
#define ASTATE_AUTO_START 0

#define ASTSTE_USE_DEBUG_TRACK_TRIGGRT 1
#define ASTSTE_USE_DEBUG_TRACK_DEFAULT_FINAL_STATE 1
#define ASTATE_DEBUG_TRACK_FINISH 1
#define ASTATE_DEBUG_TRACK_TRANSITION 1
#define ASTATE_DEBUG_TRACK_ENTRY 1
#define ASTATE_DEBUG_TRACK_EXCUTION 1
#define ASTATE_DEBUG_TRACK_EXIT 1
#define ASTATE_DEBUG_TRACK_CONDITION 1

#define ASTATE_USE_C_SUPPORT 1

#define ASTATEMACHINE_PRINTF printf
class aState;
class aStateMachine;
class transition
{
	public:
		transition(aState* to, bool(*condition)(aStateMachine* statemachine)):to(to),condition(condition){}
		~transition();

		bool trigger = true;
		class aState* to;
		bool(*condition)(aStateMachine* statemachine);
	
};
class aState
{
	friend class aStateMachine;
public:
	//包含entry excution exit translation的链表
	int(*entry)();
	int(*excution)();
	int(*exit)();

	aState(const char* _name, int(*_excution)(), int(*_entry)()=nullptr, int(*_exit)()= nullptr) :name(_name)
	{
		excution = _excution;
		entry = _entry;
		exit = _exit;
	}
	~aState();

	void showTransition()
	{
		for (int i = 0; i < transitions.size(); i++)
		{
			ASTATEMACHINE_PRINTF("transition[%d] %s -> %s\r\n",i,name, transitions[i]->to->name);
		}
	}
	const char* getName()
	{
		return name;
	}
	void addTransition(transition* _transition)
	{
		transitions.push_back(_transition);
	}
	private:
		std::vector<transition*> transitions;
		const char* name;

};
class aStateMachine
{
public:
	enum {
		stateNone,
		statePause,
		stateFinish,
		stateNumber
	};
	const char* name = "nuknown";
	std::vector<aState*> states;
	aState* current = nullptr;
	aState* previous = nullptr;
	bool started =false;
	bool finished=false;

	void finish(){finished = true;}
private:
	aState defaultFinishState = aState("finish", []()->int{
#if ASTSTE_USE_DEBUG_TRACK_DEFAULT_FINAL_STATE
		ASTATEMACHINE_PRINTF( "finish excution\r\n");
#endif
		return stateFinish;
	});

	public:
		aStateMachine() {}
		aStateMachine(const char* _name):name(_name){}
		~aStateMachine(){}
		aState* convert(const char* name)
		{
			//查找name对应的state
			for (int i = 0; i < states.size(); i++)
			{
				if (states[i]->getName() == name)
				{
					return states[i];
				}
			}
			return nullptr;
			
		}
		const char* convert(aState* state)
		{
			return state->getName();
		}
	void showStates()
	{
		//显示所有的states
		for (int i = 0; i < states.size(); i++)
		{
			ASTATEMACHINE_PRINTF("state[%d]:%s\r\n",i, states[i]->getName());
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
			to = &defaultFinishState;
			ASTATEMACHINE_PRINTF("addTransition warning: to state not found,set defaultFinshState to replace\r\n"); 
		}
		for (int i = 0; i < states.size(); i++)
		{
			if (states[i] == from) {
				
				from->addTransition(new transition(to, condition));
				return;
			}
		}
		//如果没有找到from状态,则报错
		//如果没有to状态,则默认为finsh状态
		ASTATEMACHINE_PRINTF("addTransition error: from state not found");
	}

	void addTransition(const char* fromName, const char* toName, bool(*condition)(aStateMachine*))
	{
		aState* from;
		aState* to;
		//转换为指针
		from = convert(fromName);
		to = convert(toName);
		addTransition(from, to, condition);
	}

	void addTransition(aState* from, const char* toName, bool(*condition)(aStateMachine*))
	{
		aState* to;
		//转换为指针
		to = convert(toName);
		addTransition(from, to, condition);
	}
	
	void addTransition(const char* fromName, aState* to, bool(*condition)(aStateMachine*))
	{
		aState* from;
		//转换为指针
		from = convert(fromName);
		addTransition(from, to, condition);
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

	void restart(aState* state)
	{
		started = false;
		finished = false;
		start(state);
	}
	void update()
	{
#if !ASTATE_AUTO_START
		if (!started)
		{
			ASTATEMACHINE_PRINTF("update debug: stateMachine not started\r\n");
			return;
		}
#endif
		if (finished) {
#if ASTATE_DEBUG_TRACK_FINISH
			ASTATEMACHINE_PRINTF("update debug: stateMachine finished\r\n");
#endif
			return;
		}
		if (current != nullptr)
		{
			if (current->excution != nullptr)
			{
				if (current->excution() == stateFinish)
				{
					finish();
				}
			}
			for (int i = 0; i < current->transitions.size(); i++)
			{
#if ASTATE_DEBUG_TRACK_CONDITION
				ASTATEMACHINE_PRINTF("update debug: stateMachine %s condition[%d]\r\n", current->getName(), i);
#endif
				if (current->transitions[i]->trigger == true) 
				{
					if (current->transitions[i]->condition(this))
					{
#if ASTATE_DEBUG_TRACK_TRANSITION
						ASTATEMACHINE_PRINTF("update debug: stateMachine transition %s -> %s\r\n",
							current->getName(),
							current->transitions[i]->to->getName());
#endif
						if (current->exit != nullptr)
						{
#if ASTATE_DEBUG_TRACK_EXIT
							ASTATEMACHINE_PRINTF("update debug: stateMachine %s exit\r\n", current->getName() );
#endif
							current->exit();
						}
						previous = current;
						current = current->transitions[i]->to;
						if (current->entry != nullptr)
						{
#if ASTATE_DEBUG_TRACK_ENTRY
							ASTATEMACHINE_PRINTF("update debug: stateMachine %s entry\r\n", current->getName());
#endif
							current->entry();
						}
						break;
					}

				}
#if ASTSTE_USE_DEBUG_TRACK_TRIGGRT
				else {
					ASTATEMACHINE_PRINTF("update debug: transition %s to %s not triggered",current->getName(), current->transitions[i]->to->getName());
				}
#endif
			}
		}else if(started == false)
		{
			start(states[0]);
		}else{
			/* will never reaach */
			ASTATEMACHINE_PRINTF ("update fault: stateMachine nuknown error\r\n");
		}
	}
};

#if ASTATE_USE_C_SUPPORT

#endif
