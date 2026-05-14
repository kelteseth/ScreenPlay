// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QObject>
#include <functional>

#include "steam/steam_api.h"

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamAsyncCall
    \inmodule ScreenPlayWorkshop
    \brief RAII wrapper that connects a SteamAPICall_t to a std::function callback.

    Usage:
    \code
    auto apiCall = SteamUGC()->DeleteItem(fileId);
    SteamAsyncCall<DeleteItemResult_t>::create(apiCall, [this, fileId](auto* result, bool ioFailure) {
        // handle result
    }, this);
    \endcode

    The wrapper parents itself to the given QObject so its lifetime is
    automatically managed.  After the callback fires it calls deleteLater()
    on itself.  If the parent is destroyed first, Qt destroys this child
    which in turn destroys the CCallResult, cancelling the pending callback.
*/
template <typename ResultType>
class SteamAsyncCall : public QObject {
public:
    using Callback = std::function<void(ResultType*, bool)>;

    /*!
        \brief Creates a new SteamAsyncCall that will invoke \a callback when
               the Steam API call identified by \a apiCall completes.
               The call is parented to \a parent for automatic lifetime management.

               Returns nullptr if \a apiCall is k_uAPICallInvalid — i.e. the
               underlying Steam call failed synchronously (SteamUGC not
               initialised, no logged-in user, etc.). The callback is not
               invoked in that case; the caller must handle the failure.
    */
    static SteamAsyncCall* create(SteamAPICall_t apiCall, Callback callback, QObject* parent)
    {
        if (apiCall == k_uAPICallInvalid) {
            qCritical("SteamAsyncCall: refused k_uAPICallInvalid; the underlying Steam call failed synchronously");
            return nullptr;
        }
        return new SteamAsyncCall(apiCall, std::move(callback), parent);
    }

private:
    SteamAsyncCall(SteamAPICall_t apiCall, Callback callback, QObject* parent)
        : QObject(parent)
        , m_callback(std::move(callback))
    {
        m_callResult.Set(apiCall, this, &SteamAsyncCall::onComplete);
    }

    void onComplete(ResultType* result, bool ioFailure)
    {
        if (m_callback)
            m_callback(result, ioFailure);
        deleteLater();
    }

    CCallResult<SteamAsyncCall<ResultType>, ResultType> m_callResult;
    Callback m_callback;
};

} // namespace ScreenPlayWorkshop
