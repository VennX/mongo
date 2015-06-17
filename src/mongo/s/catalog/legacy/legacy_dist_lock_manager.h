/**
 *    Copyright (C) 2015 MongoDB Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the GNU Affero General Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#pragma once

#include <boost/thread.hpp>
#include <map>
#include <memory>

#include "mongo/bson/oid.h"
#include "mongo/client/connection_string.h"
#include "mongo/s/catalog/dist_lock_manager.h"
#include "mongo/s/catalog/legacy/distlock.h"
#include "mongo/s/catalog/legacy/legacy_dist_lock_pinger.h"
#include "mongo/stdx/memory.h"

namespace mongo {

    class DistributedLock;

    class LegacyDistLockManager: public DistLockManager {
    public:
        explicit LegacyDistLockManager(ConnectionString configServer);

        virtual ~LegacyDistLockManager() = default;

        virtual void startUp() override;
        virtual void shutDown() override;

        virtual StatusWith<DistLockManager::ScopedDistLock> lock(
                StringData name,
                StringData whyMessage,
                stdx::chrono::milliseconds waitFor,
                stdx::chrono::milliseconds lockTryInterval) override;

        // For testing only.
        void enablePinger(bool enable);

    protected:

        virtual void unlock(const DistLockHandle& lockHandle) BOOST_NOEXCEPT override;

        virtual Status checkStatus(const DistLockHandle& lockHandle) override;

    private:

        const ConnectionString _configServer;

        stdx::mutex _mutex;
        boost::condition_variable _noLocksCV;
        std::map<DistLockHandle, std::unique_ptr<DistributedLock>> _lockMap;

        std::unique_ptr<LegacyDistLockPinger> _pinger;

        bool _isStopped;

        // For testing only.
        bool _pingerEnabled;
    };

}
