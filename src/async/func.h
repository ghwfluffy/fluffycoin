#pragma once

#include <memory>

namespace fluffycoin::async
{

template<typename>
class func;

/**
 * Like std::function but only supported move not copy
 */
template<typename Ret, typename... Args>
class func<Ret(Args...)>
{
    private:
        struct ICallable
        {
            virtual ~ICallable() = default;
            virtual Ret call(Args...) = 0;
        };

        template<typename Callable>
        struct CallableWrapper : ICallable
        {
            Callable callable;

            CallableWrapper(Callable&& callable)
                : callable(std::move(callable))
            {}

            Ret call(Args... args) override
            {
                return callable(std::forward<Args>(args)...);
            }
        };

        std::unique_ptr<ICallable> ptr;

    public:
        func() = default;

        template<typename Callable>
        func(Callable &&callable)
            : ptr(std::make_unique<CallableWrapper<Callable>>(std::move(callable)))
        {}

        func(const std::function<Ret(Args...)> &func)
            : ptr(std::make_unique<CallableWrapper<std::function<Ret(Args...)>>>(func))
        {}

        func(const func &) = delete;
        func(func &&rhs) noexcept
        {
            operator=(std::move(rhs));
        }

        func& operator=(const func &) = delete;
        func &operator=(func &&rhs) noexcept
        {
            if (this != &rhs)
                ptr = std::move(rhs.ptr);

            return *this;
        }

        ~func() = default;

        Ret operator()(Args... args)
        {
            if (ptr)
                return ptr->call(std::forward<Args>(args)...);
            throw std::bad_function_call();
        }

        explicit operator bool() const noexcept
        {
            return bool(ptr);
        }
};

}
