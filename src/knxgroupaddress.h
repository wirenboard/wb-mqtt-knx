#pragma once

#include "eibclient.h"
#include "knxexception.h"
#include "wblib/exceptions.h"
#include <cstdint>
#include <sstream>
#include <wblib/utils.h>

namespace knx
{
    /// \brief KNX Group Address Class
    ///
    /// Designed for convenient storage and conversion of a group address
    class TKnxGroupAddress
    {
    public:
        /// \brief Default constructor
        explicit TKnxGroupAddress() = default;

        /// \brief Triple address constructor
        /// \param main main group of a KNX group address
        /// \param middle middle group
        /// \param sub sub group
        TKnxGroupAddress(uint32_t main, uint32_t middle, uint32_t sub);

        /// \brief Double address constructor
        /// \param main main group of a KNX group address
        /// \param sub wide sub group of a KNX group address
        TKnxGroupAddress(uint32_t main, uint32_t sub);

        /// \brief Constructor from string address
        /// \param str address string. For example: "1/2/34"; "1/234"; "2305"
        explicit TKnxGroupAddress(const std::string& str);

        /// \brief Constructor from EibAddress
        /// \param eibAddress KNX EIB address
        explicit TKnxGroupAddress(eibaddr_t eibAddress);

        /// \brief Main group getter
        /// \return main group
        uint32_t GetMainGroup() const;

        /// \brief Middle group getter
        /// \return middle group
        uint32_t GetMiddleGroup() const;

        /// \brief Sub group getter
        /// \return sub group
        uint32_t GetSubGroup() const;

        /// \brief KNX EIB address getter
        /// \return a KNX EIB address
        eibaddr_t GetEibAddress() const;

        /// \brief Less operator
        /// \param rhs right value
        /// \return comparison result
        bool operator<(const TKnxGroupAddress& rhs) const;

        /// \brief Equal operator
        /// \param rhs right value
        /// \return comparison result
        bool operator==(const TKnxGroupAddress& rhs) const;

        /// \brief Not Equal operator
        /// \param rhs right value
        /// \return comparison result
        bool operator!=(const TKnxGroupAddress& rhs) const;

        /// \brief Converting a group address to a string
        std::string ToString() const;

    private:
        void Init(uint32_t main, uint32_t middle, uint32_t sub);

        void Init(uint32_t main, uint32_t sub);

        void Init(uint32_t address);

        uint32_t MainGroup{0};
        uint32_t MiddleGroup{0};
        uint32_t SubGroup{0};
    };
}
