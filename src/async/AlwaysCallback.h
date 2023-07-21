#pragma once

#include <fluffycoin/log/Log.h>

#include <functional>

namespace fluffycoin::async
{

/**
 * A callback that will always be called exactly once.
 * If the main callback is never triggered, a failsafe
 * is triggered and an error is logged.
 */
template<typename FuncType>
class AlwaysCallback
{
    public:
        AlwaysCallback()
        {
            responded = true;
        }
        AlwaysCallback(std::function<FuncType> f, std::function<void()> failsafe)
            : f(std::move(f))
            , failsafe(std::move(failsafe))
        {
            responded = false;
        }
        AlwaysCallback(AlwaysCallback &&rhs)
        {
            responded = true;
            operator=(std::move(rhs));
        }
        AlwaysCallback(const AlwaysCallback &) = delete;
        AlwaysCallback &operator=(AlwaysCallback &&rhs)
        {
            if (this != &rhs)
            {
                fin();
                this->f = std::move(rhs.f);
                this->responded = rhs.responded;
                this->failsafe = std::move(rhs.failsafe);
                rhs.responded = true;
            }
            return (*this);
        }
        AlwaysCallback &operator=(const AlwaysCallback &) = delete;
        ~AlwaysCallback()
        {
            fin();
        }

        template<typename... Args>
        void operator()(Args&&... args)
        {
            if (responded)
            {
                log::error("AlwaysCallback triggered multiple times.");
                return;
            }

            responded = true;
            f(std::forward<Args>(args)...);
        }

    private:
        void fin()
        {
            if (responded)
                return;

            responded = true;
            if (failsafe)
            {
                try {
                    failsafe();
                    log::error("AlwaysCallback triggered default routine.");
                } catch (...) { }
            }
        }

        bool responded;
        std::function<FuncType> f;
        std::function<void()> failsafe;
};

}
