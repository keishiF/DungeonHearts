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

	// 初期化
	void Init(std::weak_ptr<StateMachine<T>> machine, std::weak_ptr<T> parent)
	{
		m_machine = machine;
		m_parent = parent;
	}

	// ステート繊維時に一度呼ばれる処理
	virtual void OnEntry () abstract;
	// 更新処理
	virtual void OnUpdate(std::shared_ptr<Camera> camera) abstract;
	// ステート離脱時に一度呼ばれる処理
	virtual void OnLeave () abstract;

protected:
	// ステートからステートマシン本体へアクセスするための弱参照
	std::weak_ptr<StateMachine<T>> m_machine;

	// ステートから親へアクセスするための弱参照
	std::weak_ptr<T> m_parent;
};

// T:ステートマシンを使うクラス
template<typename T>
class StateMachine : public std::enable_shared_from_this<StateMachine<T>>
{
public:

	// 初期化
	void Init(std::weak_ptr<T> parent, std::shared_ptr<StateNode<T>> initState)
	{
		m_parent = parent;
		ChangeState(initState);
	}

	// 更新処理
	// 現在のステートの OnUpdate を呼び出す
	void Update(std::shared_ptr<Camera> camera)
	{
		m_nowState->OnUpdate(camera);
	}

	// ステート遷移
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

	// 現在のステートを取得（読み取り用）
	std::shared_ptr<StateNode<T>> GetCurrentState() { return m_nowState; }

private:
	// 親（ステートを所有するクラス）への弱参照
	std::weak_ptr<T> m_parent;

	// 現在のステート
	std::shared_ptr< StateNode<T> > m_nowState;
};

