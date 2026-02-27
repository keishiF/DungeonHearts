#pragma once
#include <memory>

template<typename T>
class StateMachine;
class Camera;

// T:ステートマシンを使うクラス
template<typename T>
class StateNode
{
public:

	void Init(std::weak_ptr<StateMachine<T>> machine, std::weak_ptr<T> parent)
	{
		m_machine = machine;
		m_parent = parent;
	}

	virtual void OnEntry () abstract;
	virtual void OnUpdate(std::shared_ptr<Camera> camera) abstract;
	virtual void OnLeave () abstract;

protected:
	std::weak_ptr<StateMachine<T>> m_machine;
	std::weak_ptr<T> m_parent;
};

// T:ステートマシンを使うクラス
template<typename T>
class StateMachine : public std::enable_shared_from_this<StateMachine<T>>
{
public:

	void Init(std::weak_ptr<T> parent, std::shared_ptr<StateNode<T>> initState)
	{
		m_parent = parent;
		ChangeState(initState);
	}

	void Update(std::shared_ptr<Camera> camera)
	{
		m_nowState->OnUpdate(camera);
	}

	void ChangeState(std::shared_ptr<StateNode<T>> nextState)
	{
		if (m_nowState)
		{
			m_nowState->OnLeave();
		}
		m_nowState = nextState;
		m_nowState->Init(this->weak_from_this(), m_parent);
		m_nowState->OnEntry();
	}

	std::shared_ptr<StateNode<T>> GetCurrentState() { return m_nowState; }

private:
	std::weak_ptr<T> m_parent;
	std::shared_ptr< StateNode<T> > m_nowState;
};

