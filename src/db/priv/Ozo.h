#pragma once

#include <ozo/ozo.h>
#include <ozo/error.h>
#include <ozo/transaction.h>

namespace fluffycoin::db::priv
{

namespace Ozo
{
    using ConnectionPool = ozo::connection_pool<
        ozo::connection_info<
            ozo::oid_map_t<>, ozo::none_t>,
        ozo::thread_safety<true> >;

    using ConnectionProvider = ozo::connection_provider<
        ozo::connection_pool<
            ozo::connection_info<ozo::oid_map_t<>, ozo::none_t>,
            ozo::thread_safety<true> >&>;

    using Connection = std::shared_ptr<
        ozo::pooled_connection<
            yamail::resource_pool::handle<ozo::connection_rep<ozo::oid_map_t<>, ozo::none_t> >,
            boost::asio::io_context::basic_executor_type<std::allocator<void>, 0ul> > >;

    using Transaction = ozo::transaction<
        std::shared_ptr<
            ozo::pooled_connection<
                yamail::resource_pool::handle<
                    ozo::connection_rep<ozo::oid_map_t<>, ozo::none_t> >,
                    boost::asio::io_context::basic_executor_type<std::allocator<void>, 0ul> > >,
        boost::hana::detail::map_impl<boost::hana::detail::hash_table<>,
        boost::hana::basic_tuple<> > >;

    template<typename Conn>
    std::string error(ozo::error_code ec, Conn conn)
    {
        std::stringstream ss;
        ss << ec.message();
        if (!ozo::is_null_recursive(conn))
            ss << " - " << ozo::error_message(conn) << " (" << ozo::get_error_context(conn) << ")";
        return ss.str();
    }
}

}
