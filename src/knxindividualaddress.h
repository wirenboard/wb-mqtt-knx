#pragma once

#include "eibclient.h"
#include "knxexception.h"
#include "wblib/exceptions.h"
#include <cstdint>
#include <sstream>
#include <wblib/utils.h>

namespace knx
{
    /// \brief KNX Individual Address Class
    ///
    /// Designed for convenient storage and conversion of a knx individual address
    class TKnxIndividualAddress
    {
    public:
        /// \brief Default constructor
        explicit TKnxIndividualAddress() = default;

        /// \brief Address constructor
        /// \param area area of a KNX individual address
        /// \param line line
        /// \param deviceAddress device address
        TKnxIndividualAddress(uint32_t area, uint32_t line, uint32_t deviceAddress);

        /// \brief Constructor from string address
        /// \param str address string. For example: "1.2.34";
        /// \param delimiter delimiter sub address for parsing string. Default '.'
        explicit TKnxIndividualAddress(const std::string& str, char delimiter = '.');

        /// \brief Constructor from EibAddress
        /// \param eibAddress KNX EIB address
        explicit TKnxIndividualAddress(eibaddr_t eibAddress);

        /// \brief Area getter
        /// \return area
        uint32_t GetArea() const;

        /// \brief Line getter
        /// \return line
        uint32_t GetLine() const;

        /// \brief Device address getter
        /// \return device address
        uint32_t GetDeviceAddress() const;

        /// \brief KNX EIB address getter
        /// \return a KNX EIB address
        eibaddr_t GetEibAddress() const;

        /// \brief Less operator
        /// \param rhs right value
        /// \return comparison result
        bool operator<(const TKnxIndividualAddress& rhs) const;

        /// \brief Equal operator
        /// \param rhs right value
        /// \return comparison result
        bool operator==(const TKnxIndividualAddress& rhs) const;

        /// \brief Not Equal operator
        /// \param rhs right value
        /// \return comparison result
        bool operator!=(const TKnxIndividualAddress& rhs) const;

        /// \brief Converting a individual address to a string
        /// \param delimiter sub address delimiter
        std::string ToString(char delimiter = '.') const;

    private:
        void Init(uint32_t area, uint32_t line, uint32_t deviceAddress);

        uint32_t Area{0};
        uint32_t Line{0};
        uint32_t DeviceAddress{0};
    };
}
