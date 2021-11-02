#pragma once
#include "isubscriber.h"
#include <algorithm>
#include <list>
#include <memory>

namespace knx
{
    template<typename T> class Observer
    {
    public:
        virtual bool Subscribe(std::shared_ptr<ISubscriber<T>> subscriber) noexcept
        {
            auto it = std::find(SubscriberList.begin(), SubscriberList.end(), subscriber);
            if (it == SubscriberList.end()) {
                SubscriberList.push_back(subscriber);
                return true;
            }
            return false;
        }

        virtual bool Unsubscribe(std::shared_ptr<ISubscriber<T>> subscriber) noexcept
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
        virtual void NotifyAllSubscribers(const T& data) noexcept
        {
            for (const auto& subscriber: SubscriberList) {
                subscriber->Notify(data);
            }
        }

    private:
        std::vector<std::shared_ptr<ISubscriber<T>>> SubscriberList;
    };
}