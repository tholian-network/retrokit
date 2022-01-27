/*
 * Copyright (C) 2019 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "FeaturePolicy.h"

#include "DOMWindow.h"
#include "Document.h"
#include "HTMLIFrameElement.h"
#include "HTMLNames.h"
#include "HTMLParserIdioms.h"
#include "SecurityOrigin.h"

namespace WebCore {

using namespace HTMLNames;

static const char* policyTypeName(FeaturePolicy::Type type)
{
    switch (type) {
    case FeaturePolicy::Type::SyncXHR:
        return "SyncXHR";
    }
    ASSERT_NOT_REACHED();
    return "";
}

bool isFeaturePolicyAllowedByDocumentAndAllOwners(FeaturePolicy::Type type, const Document& document, LogFeaturePolicyFailure logFailure)
{
    auto& topDocument = document.topDocument();
    auto* ancestorDocument = &document;
    while (ancestorDocument != &topDocument) {
        if (!ancestorDocument) {
            if (logFailure == LogFeaturePolicyFailure::Yes && document.domWindow())
                document.domWindow()->printErrorMessage(makeString("Feature policy '", policyTypeName(type), "' check failed."));
            return false;
        }

        auto* ownerElement = ancestorDocument->ownerElement();
        if (is<HTMLIFrameElement>(ownerElement)) {
            const auto& featurePolicy = downcast<HTMLIFrameElement>(ownerElement)->featurePolicy();
            if (!featurePolicy.allows(type, ancestorDocument->securityOrigin().data())) {
                if (logFailure == LogFeaturePolicyFailure::Yes && document.domWindow()) {
                    auto& allowValue = downcast<HTMLIFrameElement>(ownerElement)->attributeWithoutSynchronization(HTMLNames::allowAttr);
                    document.domWindow()->printErrorMessage(makeString("Feature policy '", policyTypeName(type), "' check failed for iframe with origin '", document.securityOrigin().toString(), "' and allow attribute '", allowValue, "'."));
                }
                return false;
            }
        }

        ancestorDocument = ancestorDocument->parentDocument();
    }

    return true;
}

static bool isAllowedByFeaturePolicy(const FeaturePolicy::AllowRule& rule, const SecurityOriginData& origin)
{
    switch (rule.type) {
    case FeaturePolicy::AllowRule::Type::None:
        return false;
    case FeaturePolicy::AllowRule::Type::All:
        return true;
    case FeaturePolicy::AllowRule::Type::List:
        return rule.allowedList.contains(origin);
    }
    ASSERT_NOT_REACHED();
    return false;
}

static inline void processOriginItem(Document& document, FeaturePolicy::AllowRule& rule, StringView item)
{
    if (rule.type == FeaturePolicy::AllowRule::Type::None)
        return;

    item = item.stripLeadingAndTrailingMatchedCharacters(isHTMLSpace<UChar>);
    // FIXME: Support 'src'.
    if (item == "'src'")
        return;

    if (item == "*") {
        rule.type = FeaturePolicy::AllowRule::Type::All;
        return;
    }

    if (item == "'self'") {
        rule.allowedList.add(document.securityOrigin().data());
        return;
    }
    if (item == "'none'") {
        rule.type = FeaturePolicy::AllowRule::Type::None;
        return;
    }
    URL url { { }, item.toString() };
    if (url.isValid())
        rule.allowedList.add(SecurityOriginData::fromURL(url));
}

static inline void updateList(Document& document, FeaturePolicy::AllowRule& rule, StringView value)
{
    // We keep the empty string value equivalent to '*' for existing websites.
    if (value.isEmpty()) {
        rule.type = FeaturePolicy::AllowRule::Type::All;
        return;
    }

    while (!value.isEmpty()) {
        auto position = value.find(isHTMLSpace<UChar>);
        if (position == notFound) {
            processOriginItem(document, rule, value);
            return;
        }

        processOriginItem(document, rule, value.substring(0, position));
        value = value.substring(position + 1).stripLeadingAndTrailingMatchedCharacters(isHTMLSpace<UChar>);
    }
}

FeaturePolicy FeaturePolicy::parse(Document& document, const HTMLIFrameElement& iframe, StringView allowAttributeValue)
{
    UNUSED_PARAM(iframe);
    FeaturePolicy policy;
    bool isSyncXHRInitialized = false;
    for (auto allowItem : allowAttributeValue.split(';')) {
        auto item = allowItem.stripLeadingAndTrailingMatchedCharacters(isHTMLSpace<UChar>);
        if (item.startsWith("sync-xhr")) {
            isSyncXHRInitialized = true;
            updateList(document, policy.m_syncXHRRule, item.substring(9));
            continue;
        }
    }

    if (!isSyncXHRInitialized)
        policy.m_syncXHRRule.type = AllowRule::Type::All;

    return policy;
}

bool FeaturePolicy::allows(Type type, const SecurityOriginData& origin) const
{
    switch (type) {
    case Type::SyncXHR:
        return isAllowedByFeaturePolicy(m_syncXHRRule, origin);
    }
    ASSERT_NOT_REACHED();
    return false;
}

}
