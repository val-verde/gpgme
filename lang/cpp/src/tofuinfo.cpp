/* tofuinfo.cpp - wraps gpgme tofu info
  Copyright (C) 2016 Intevation GmbH

  This file is part of GPGME++.

  GPGME++ is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  GPGME++ is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with GPGME++; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "tofuinfo.h"

#include <istream>
#include "util.h"

class GpgME::TofuInfo::Private
{
public:
    Private() {}
    Private(gpgme_tofu_info_t info)
        : mInfo(info ? new _gpgme_tofu_info(*info) : nullptr)
    {
        if (mInfo && mInfo->description) {
            mInfo->description = strdup(mInfo->description);
        }
    }

    Private(const Private &other)
        : mInfo(other.mInfo)
    {
        if (mInfo && mInfo->description) {
            mInfo->description = strdup(mInfo->description);
        }
    }

    ~Private()
    {
        if (mInfo) {
            std::free(mInfo->description);
            mInfo->description = nullptr;

            delete mInfo;
        }
    }

    gpgme_tofu_info_t mInfo;
};

GpgME::TofuInfo::TofuInfo(gpgme_tofu_info_t info)
    : d(new Private(info))
{
}

GpgME::TofuInfo::TofuInfo() : d()
{
}

bool GpgME::TofuInfo::isNull() const
{
    return !d || !d->mInfo;
}

GpgME::TofuInfo::Validity GpgME::TofuInfo::validity() const
{
    if (isNull()) {
        return ValidityUnknown;
    }
    switch (d->mInfo->validity) {
        case 0:
            return Conflict;
        case 1:
            return NoHistory;
        case 2:
            return LittleHistory;
        case 3:
            return BasicHistory;
        case 4:
            return LargeHistory;
        default:
            return ValidityUnknown;
    }
}

GpgME::TofuInfo::Policy GpgME::TofuInfo::policy() const
{
    if (isNull()) {
        return PolicyUnknown;
    }
    switch (d->mInfo->policy) {
        case GPGME_TOFU_POLICY_NONE:
            return PolicyNone;
        case GPGME_TOFU_POLICY_AUTO:
            return PolicyAuto;
        case GPGME_TOFU_POLICY_GOOD:
            return PolicyGood;
        case GPGME_TOFU_POLICY_BAD:
            return PolicyBad;
        case GPGME_TOFU_POLICY_ASK:
            return PolicyAsk;
        case GPGME_TOFU_POLICY_UNKNOWN:
            return PolicyUnknown;
    }
}

const char *GpgME::TofuInfo::description() const
{
    return isNull() ? nullptr : d->mInfo->description;
}

unsigned short GpgME::TofuInfo::signCount() const
{
    return isNull() ? 0 : d->mInfo->signcount;
}

unsigned long GpgME::TofuInfo::firstSeen() const
{
    return isNull() ? 0 : d->mInfo->firstseen;
}

unsigned long GpgME::TofuInfo::lastSeen() const
{
    return isNull() ? 0 : d->mInfo->lastseen;
}

std::ostream &GpgME::operator<<(std::ostream &os, const GpgME::TofuInfo &info)
{
    os << "GpgME::Signature::TofuInfo(";
    if (!info.isNull()) {
        os << "\n desc: "     << protect(info.description())
           << "\n validity: " << info.validity()
           << "\n policy: "   << info.policy()
           << "\n signcount: "<< info.signCount()
           << "\n firstseen: "<< info.firstSeen()
           << "\n lastseen: " << info.lastSeen()
           << '\n';
    }
    return os << ")";
}
