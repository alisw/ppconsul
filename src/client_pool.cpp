#include <ppconsul/client_pool.h>


namespace ppconsul {

    std::unique_ptr<http::HttpClient> ClientPool::acquire()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (!m_pool.empty())
            {
                auto res = std::move(m_pool.back());
                m_pool.pop_back();
                return res;
            }
        }

        return m_factory();
    }

    void ClientPool::release(http::HttpClient *client) noexcept
    {
        assert(client);
        std::unique_ptr<http::HttpClient> cl(client);

        // We just delete client if there is no free memory for adding it back to the pool
        try
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pool.push_back(std::move(cl));
        }
        catch (const std::bad_alloc&)
        {}
    }

}
