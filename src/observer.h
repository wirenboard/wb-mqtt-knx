#pragma once
#include "isubscriber.h"
#include <algorithm>
#include <list>
#include <memory>

namespace knx
{
    template<typename... Args> class TObserver
    {
    public:
        virtual bool Subscribe(PSubscriber<Args...> subscriber)
        {
            auto it = std::find(SubscriberList.begin(), SubscriberList.end(), subscriber);
            if (it == SubscriberList.end()) {
                SubscriberList.push_back(subscriber);
                return true;
            }
            return false;
        }

        virtual bool Unsubscribe(PSubscriber<Args...> subscriber)
        {
            auto it = std::find(SubscriberList.begin(), SubscriberList.end(), subscriber);
            if (it != SubscriberList.end()) {
                SubscriberList.erase(it);
                return true;
            }
            return false;
        }

        virtual ~TObserver() = default;

    protected:
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