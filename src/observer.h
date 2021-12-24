#pragma once
#include "isubscriber.h"
#include <algorithm>
#include <memory>
#include <vector>

namespace knx
{
    /// \brief Base templated class for implementing the observer pattern
    template<typename... Args> class TObserver
    {
    public:
        /// Add subscriber to subscriber list
        /// \param subscriber pointer to subscriber
        /// \return the subscriber has not been added because it is already in the list
        virtual bool Subscribe(PSubscriber<Args...> subscriber)
        {
            auto it = std::find(SubscriberList.begin(), SubscriberList.end(), subscriber);
            if (it == SubscriberList.end()) {
                SubscriberList.push_back(subscriber);
                return true;
            }
            return false;
        }

        /// Remove subscriber to subscriber list
        /// \param subscriber pointer to subscriber
        /// \return the subscriber was not deleted because it is not in the list
        virtual bool Unsubscribe(PSubscriber<Args...> subscriber)
        {
            auto it = std::find(SubscriberList.begin(), SubscriberList.end(), subscriber);
            if (it != SubscriberList.end()) {
                SubscriberList.erase(it);
                return true;
            }
            return false;
        }

        /// Destructor
        virtual ~TObserver() = default;

    protected:
        /// Notify all subscribers
        /// \param data constant data reference
        virtual void NotifyAllSubscribers(const Args&... data)
        {
            for (const auto& subscriber: SubscriberList) {
                subscriber->Notify(data...);
            }
        }

    private:
        std::vector<PSubscriber<Args...>> SubscriberList;
    };

    template<typename... Args> using PObserver = std::shared_ptr<TObserver<Args...>>;
}