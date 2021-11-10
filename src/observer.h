#pragma once
#include "isubscriber.h"
#include <algorithm>
#include <list>
#include <memory>

namespace knx
{
    template<typename... Args> class Observer
    {
    public:
        virtual bool Subscribe(std::shared_ptr<ISubscriber<Args...>> subscriber)
        {
            auto it = std::find(SubscriberList.begin(), SubscriberList.end(), subscriber);
            if (it == SubscriberList.end()) {
                SubscriberList.push_back(subscriber);
                return true;
            }
            return false;
        }

        virtual bool Unsubscribe(std::shared_ptr<ISubscriber<Args...>> subscriber)
        {
            auto it = std::find(SubscriberList.begin(), SubscriberList.end(), subscriber);
            if (it != SubscriberList.end()) {
                SubscriberList.erase(it);
                return true;
            }
            return false;
        }

        virtual ~Observer() = default;

    protected:
        virtual void NotifyAllSubscribers(const Args&... data)
        {
            for (const auto& subscriber: SubscriberList) {
                subscriber->Notify(data...);
            }
        }

    private:
        std::vector<std::shared_ptr<ISubscriber<Args...>>> SubscriberList;
    };
}