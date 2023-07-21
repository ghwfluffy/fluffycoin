#pragma once

#include <map>
#include <memory>
#include <typeinfo>
#include <type_traits>

namespace fluffycoin
{

/**
 * Hold any number of uniquely typed things
 */
class AnyMap
{
    public:
        AnyMap() = default;
        AnyMap(AnyMap &&) = default;
        AnyMap(const AnyMap &);
        AnyMap &operator=(AnyMap &&) = default;
        AnyMap &operator=(const AnyMap &);
        ~AnyMap() = default;

        template<typename T>
        T &get() &
        {
            auto iter = data.find(hash<T>());
            if (iter == data.end())
                iter = data.emplace(hash<T>(), std::make_unique<ObjHolder<T>>(std::make_unique<T>())).first;
            return *reinterpret_cast<T *>(iter->second->get());
        }

        template<typename T,
            std::enable_if_t<std::is_move_constructible<T>::value, bool> = true>
        T get() &&
        {
            auto iter = data.find(hash<T>());
            if (iter == data.end())
                return T();
            return std::move(*reinterpret_cast<T *>(iter->second->get()));
        }

        template<typename T>
        const T &get() const &
        {
            return const_cast<AnyMap *>(this)->get<T>();
        }

        template<typename T>
        void set(T &&t)
        {
            using TType = typename std::remove_reference<T>::type;
            size_t id = hash<TType>();
            auto iter = data.find(id);
            if (iter != data.end())
                iter->second = std::make_unique<ObjHolder<TType>>(std::forward<T>(t));
            else
                data.emplace(id, std::make_unique<ObjHolder<TType>>(std::forward<T>(t)));
        }

        template<typename T>
        void set(std::unique_ptr<T> pt)
        {
            size_t id = hash<T>();
            auto iter = data.find(id);
            if (iter != data.end())
                iter->second = std::make_unique<ObjHolder<T>>(std::move(pt));
            else
                data.emplace(id, std::make_unique<ObjHolder<T>>(std::move(pt)));
        }

        template<typename T>
        bool has() const
        {
            return data.find(hash<T>()) != data.end();
        }

        template<typename T>
        void clear()
        {
            data.erase(hash<T>());
        }

        AnyMap &operator+=(AnyMap &&rhs);
        AnyMap &operator+=(const AnyMap &rhs);

    private:
        template<typename T>
        static std::size_t hash()
        {
            return typeid(T).hash_code();
        }

        class IObjHolder
        {
            public:
                virtual ~IObjHolder() = default;

                virtual std::unique_ptr<IObjHolder> clone() const = 0;

                virtual void *get() = 0;
                virtual void *get() const = 0;

            protected:
                IObjHolder() = default;
                IObjHolder(IObjHolder &&) = default;
                IObjHolder(const IObjHolder &) = default;
                IObjHolder &operator=(IObjHolder &&) = default;
                IObjHolder &operator=(const IObjHolder &) = default;
        };

        template<typename T>
        class ObjHolder : public IObjHolder
        {
            public:
                ObjHolder();
                ObjHolder(const T &t)
                {
                    pt = std::make_unique<T>(t);
                }
                ObjHolder(T &&t)
                {
                    using TType = typename std::remove_reference<T>::type;
                    pt = std::make_unique<T>(std::forward<TType>(t));
                }
                ObjHolder(std::unique_ptr<T> pt)
                {
                    this->pt = std::move(pt);
                }
                ObjHolder(ObjHolder &&) = default;
                ObjHolder(const ObjHolder &t)
                {
                    operator=(t);
                }
                ObjHolder &operator=(ObjHolder &&) = default;
                ObjHolder &operator=(const ObjHolder &t)
                {
                    if (this != &t)
                        pt = std::make_unique<T>(t);
                    return (*this);
                }
                ~ObjHolder() final = default;

                std::unique_ptr<IObjHolder> clone() const final
                {
                    return cloneInternal();
                }

                template<typename ObjType = T,
                    std::enable_if_t<std::is_copy_constructible<ObjType>::value, bool> = true>
                std::unique_ptr<IObjHolder> cloneInternal() const
                {
                    if (!pt)
                        return std::unique_ptr<ObjHolder<T>>();
                    return std::make_unique<ObjHolder<T>>(*pt.get());
                }

                template<typename ObjType = T,
                    std::enable_if_t<!std::is_copy_constructible<ObjType>::value, bool> = true>
                std::unique_ptr<IObjHolder> cloneInternal() const
                {
                    return std::unique_ptr<ObjHolder<T>>();
                }

                void *get() final
                {
                    return reinterpret_cast<void *>(pt.get());
                }

                void *get() const final
                {
                    return reinterpret_cast<void *>(pt.get());
                }

                std::unique_ptr<T> pt;
        };

        std::map<size_t, std::unique_ptr<IObjHolder>> data;
};

}
