#pragma once

#include <utility>

namespace Text {

template <typename>
class FunctorRefWrapper;

template <typename Ret, typename... Args>
class FunctorRefWrapper<Ret(Args...)> {
	public:
		template <typename Functor>
		FunctorRefWrapper(Functor& func)
				: m_wrapper([](void* pFunctor, Args... args) -> Ret {
					return (*reinterpret_cast<Functor*>(pFunctor))(args...);
				})
				, m_pFunctor(&func) {}

		FunctorRefWrapper(FunctorRefWrapper&&) = delete;
		FunctorRefWrapper(const FunctorRefWrapper&) = delete;

		void operator=(FunctorRefWrapper&&) = delete;
		void operator=(const FunctorRefWrapper&) = delete;

		template <typename... Args2>
		Ret operator()(Args2&&... args) {
			return m_wrapper(m_pFunctor, std::forward<Args2>(args)...);
		}
	private:
		Ret (*m_wrapper)(void*, Args...){};
		void* m_pFunctor{};
};

}
