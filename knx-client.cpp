#include "knx-client.h"
#include "Telegram.h"
#include "observer.h"
#include "wbmqtt/utils.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>

#include "logging.h"

TKnxClient::TKnxClient(std::string url)
{
    Url = url;
}

void TKnxClient::Observe(PMqttKnxObserver observer)
{
    Observer = observer;
}

TKnxClient::~TKnxClient()
{
}

void TKnxClient::SendTelegram(std::string payload)
{
    TKnxTelegram t(payload);

    TKnxConnection Out(Url);
    if (!Out) throw TKnxException("failed to open Url: " + Url + ". Is KNXD running?");

    int res;
    if (t.IsGroup()) {
        res = EIBOpen_GroupSocket(Out, 0);
        if (res == -1) throw TKnxException("failed to open GroupSocket");

        res = EIBSendGroup(Out, t.GetDstAddr(), t.GetSize(), t.GetKnxPayload());
        if (res == -1) throw TKnxException("failed to send group telegram");

    } else {
        res = EIBOpenT_TPDU(Out, t.GetSrcAddr());
        if (res == -1) throw TKnxException("failed to open TPDU connection");

        res = EIBSendTPDU(Out, t.GetDstAddr(), t.GetSize(), t.GetKnxPayload());
        if (res == -1) throw TKnxException("failed to send group telegram");
    }
}

void TKnxClient::HandleLoopError(std::string what, unsigned int timeout)
{
    LOG(ERROR) << "Error in KNX loop: " << what;
    sleep(timeout);
}

void TKnxClient::Loop()
{

    while (true) {
        TKnxConnection In(Url);
        if (!In) {
            HandleLoopError("failed to open Url: " + Url + ". Is KNXD running?", ERROR_TIMEOUT);
            continue;
        }
        LOG(INFO) << "KNX connection succeessfull";

        int res = EIBOpenVBusmonitor(In);
        if (res == -1) {
            HandleLoopError("failed to open Busmonitor connection", 0);
            continue;
        }

        int fd = EIB_Poll_FD(In);
        if (fd == -1) {
            HandleLoopError("failed to get Poll fd", 0);
            continue;
        }

        while (true) {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(fd, &set);

            res = select(fd + 1, &set, NULL, NULL, NULL);
            if (res == -1) {
                HandleLoopError(std::string("select failed: ") + std::strerror(errno), 0);
                break;
            }

            uint8_t telegram[MAX_TELEGRAM_LENGTH] = {0};
            int len = EIBGetBusmonitorPacket(In, MAX_TELEGRAM_LENGTH, telegram);
            if (len == -1) {
                HandleLoopError("failed to read Busmonitor packet", 0);
                break;
            }

            std::stringstream ss;
            ss << "KNX Client received a telegram: ";
            ss << std::hex << std::noshowbase;
            for (int i = 0; i < len; i++) {
                ss << "0x" << std::setw(2) << std::setfill('0');
                ss << (unsigned)telegram[i] << " ";
            }
            LOG(INFO) << ss.str();

            Observer->OnTelegram(telegram, len);
        }
    }
}
