/*
 * Copyright (C) 2017 Igalia S.L.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "RemoteInspectorServer.h"

#if ENABLE(REMOTE_INSPECTOR)

#include "RemoteInspectorUtils.h"
#include <gio/gio.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/Vector.h>
#include <wtf/glib/GUniquePtr.h>

namespace Inspector {

static uint64_t generateConnectionID()
{
    static uint64_t connectionID = 0;
    return ++connectionID;
}

const SocketConnection::MessageHandlers& RemoteInspectorServer::messageHandlers()
{
    static NeverDestroyed<const SocketConnection::MessageHandlers> messageHandlers = SocketConnection::MessageHandlers({
    { "DidClose", std::pair<CString, SocketConnection::MessageCallback> { { },
        [](SocketConnection& connection, GVariant*, gpointer userData) {
            auto& inspectorServer = *static_cast<RemoteInspectorServer*>(userData);
            inspectorServer.connectionDidClose(connection);
        }}
    },
    { "SetTargetList", std::pair<CString, SocketConnection::MessageCallback> { "(a(tsssb)b)",
        [](SocketConnection& connection, GVariant* parameters, gpointer userData) {
            auto& inspectorServer = *static_cast<RemoteInspectorServer*>(userData);
            inspectorServer.setTargetList(connection, parameters);
        }}
    },
    { "SetupInspectorClient", std::pair<CString, SocketConnection::MessageCallback> { "(ay)",
        [](SocketConnection& connection, GVariant* parameters, gpointer userData) {
            auto& inspectorServer = *static_cast<RemoteInspectorServer*>(userData);
            GRefPtr<GVariant> backendCommandsHash;
            g_variant_get(parameters, "(@ay)", &backendCommandsHash.outPtr());
            auto* backendCommands = inspectorServer.setupInspectorClient(connection, g_variant_get_bytestring(backendCommandsHash.get()));
            connection.sendMessage("DidSetupInspectorClient", g_variant_new("(@ay)", backendCommands));
        }}
    },
    { "Setup", std::pair<CString, SocketConnection::MessageCallback> { "(tt)",
        [](SocketConnection& connection, GVariant* parameters, gpointer userData) {
            auto& inspectorServer = *static_cast<RemoteInspectorServer*>(userData);
            guint64 connectionID, targetID;
            g_variant_get(parameters, "(tt)", &connectionID, &targetID);
            inspectorServer.setup(connection, connectionID, targetID);
        }}
    },
    { "FrontendDidClose", std::pair<CString, SocketConnection::MessageCallback> { "(tt)",
        [](SocketConnection& connection, GVariant* parameters, gpointer userData) {
            auto& inspectorServer = *static_cast<RemoteInspectorServer*>(userData);
            guint64 connectionID, targetID;
            g_variant_get(parameters, "(tt)", &connectionID, &targetID);
            inspectorServer.close(connection, connectionID, targetID);
        }}
    },
    { "SendMessageToFrontend", std::pair<CString, SocketConnection::MessageCallback> { "(ts)",
        [](SocketConnection& connection, GVariant* parameters, gpointer userData) {
            auto& inspectorServer = *static_cast<RemoteInspectorServer*>(userData);
            guint64 targetID;
            const char* message;
            g_variant_get(parameters, "(t&s)", &targetID, &message);
            inspectorServer.sendMessageToFrontend(connection, targetID, message);
        }}
    },
    { "SendMessageToBackend", std::pair<CString, SocketConnection::MessageCallback> { "(tts)",
        [](SocketConnection& connection, GVariant* parameters, gpointer userData) {
            auto& inspectorServer = *static_cast<RemoteInspectorServer*>(userData);
            guint64 connectionID, targetID;
            const char* message;
            g_variant_get(parameters, "(tt&s)", &connectionID, &targetID, &message);
            inspectorServer.sendMessageToBackend(connection, connectionID, targetID, message);
        }}
    }
    });
    return messageHandlers;
}

RemoteInspectorServer& RemoteInspectorServer::singleton()
{
    static RemoteInspectorServer server;
    return server;
}

RemoteInspectorServer::~RemoteInspectorServer()
{
    if (m_service)
        g_signal_handlers_disconnect_matched(m_service.get(), G_SIGNAL_MATCH_DATA, 0, 0, nullptr, nullptr, this);
}

bool RemoteInspectorServer::start(const char* address, unsigned port)
{
    m_service = adoptGRef(g_socket_service_new());
    g_signal_connect(m_service.get(), "incoming", G_CALLBACK(incomingConnectionCallback), this);

    GRefPtr<GSocketAddress> socketAddress = adoptGRef(g_inet_socket_address_new_from_string(address, port));
    GUniqueOutPtr<GError> error;
    if (!g_socket_listener_add_address(G_SOCKET_LISTENER(m_service.get()), socketAddress.get(), G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, nullptr, nullptr, &error.outPtr())) {
        g_warning("Failed to start remote inspector server on %s:%u: %s\n", address, port, error->message);
        return false;
    }

    return true;
}

gboolean RemoteInspectorServer::incomingConnectionCallback(GSocketService*, GSocketConnection* connection, GObject*, RemoteInspectorServer* inspectorServer)
{
    inspectorServer->incomingConnection(SocketConnection::create(GRefPtr<GSocketConnection>(connection), messageHandlers(), inspectorServer));
    return TRUE;
}

void RemoteInspectorServer::incomingConnection(Ref<SocketConnection>&& connection)
{
    ASSERT(!m_connections.contains(connection.ptr()));
    m_connections.add(WTFMove(connection));
}

void RemoteInspectorServer::setTargetList(SocketConnection& remoteInspectorConnection, GVariant* parameters)
{
    ASSERT(m_connections.contains(&remoteInspectorConnection));
    auto addResult = m_remoteInspectorConnectionToIDMap.add(&remoteInspectorConnection, 0);
    if (addResult.isNewEntry) {
        addResult.iterator->value = generateConnectionID();
        m_idToRemoteInspectorConnectionMap.add(addResult.iterator->value, &remoteInspectorConnection);
    }

    gboolean remoteAutomationEnabled;
    GRefPtr<GVariant> targetList;
    g_variant_get(parameters, "(@a(tsssb)b)", &targetList.outPtr(), &remoteAutomationEnabled);
    SocketConnection* clientConnection = remoteAutomationEnabled && m_automationConnection ? m_automationConnection : m_clientConnection;
    if (!clientConnection)
        return;

    clientConnection->sendMessage("SetTargetList", g_variant_new("(t@a(tsssb))", addResult.iterator->value, targetList.get()));
}

GVariant* RemoteInspectorServer::setupInspectorClient(SocketConnection& clientConnection, const char* clientBackendCommandsHash)
{
    ASSERT(!m_clientConnection);
    m_clientConnection = &clientConnection;

    GVariant* backendCommands;
    if (strcmp(clientBackendCommandsHash, backendCommandsHash().data())) {
        auto bytes = Inspector::backendCommands();
        backendCommands = g_variant_new_bytestring(static_cast<const char*>(g_bytes_get_data(bytes.get(), nullptr)));
    } else
        backendCommands = g_variant_new_bytestring("");

    // Ask all remote inspectors to push their target lists to notify the new client.
    for (auto* remoteInspectorConnection : m_remoteInspectorConnectionToIDMap.keys())
        remoteInspectorConnection->sendMessage("GetTargetList", nullptr);

    return backendCommands;
}

void RemoteInspectorServer::setup(SocketConnection& clientConnection, uint64_t connectionID, uint64_t targetID)
{
    ASSERT(m_clientConnection == &clientConnection || m_automationConnection == &clientConnection);
    ASSERT(m_idToRemoteInspectorConnectionMap.contains(connectionID));
    if (&clientConnection == m_automationConnection) {
        m_automationTargets.add(std::make_pair(connectionID, targetID));
        RemoteInspector::singleton().setup(targetID);
        return;
    }

    m_inspectionTargets.add(std::make_pair(connectionID, targetID));
    m_idToRemoteInspectorConnectionMap.get(connectionID)->sendMessage("Setup", g_variant_new("(t)", targetID));
}

void RemoteInspectorServer::close(SocketConnection& clientConnection, uint64_t connectionID, uint64_t targetID)
{
    ASSERT(m_clientConnection == &clientConnection || m_automationConnection == &clientConnection);
    ASSERT(m_idToRemoteInspectorConnectionMap.contains(connectionID));
    if (&clientConnection == m_automationConnection) {
        // FIXME: automation.
        return;
    }

    ASSERT(m_inspectionTargets.contains(std::make_pair(connectionID, targetID)));
    m_idToRemoteInspectorConnectionMap.get(connectionID)->sendMessage("FrontendDidClose", g_variant_new("(t)", targetID));
    m_inspectionTargets.remove(std::make_pair(connectionID, targetID));
}

void RemoteInspectorServer::connectionDidClose(SocketConnection& clientConnection)
{
    ASSERT(m_connections.contains(&clientConnection));
    if (&clientConnection == m_automationConnection) {
        for (auto connectionTargetPair : m_automationTargets)
            close(clientConnection, connectionTargetPair.first, connectionTargetPair.second);
        m_automationConnection = nullptr;
    } else if (&clientConnection == m_clientConnection) {
        for (auto connectionTargetPair : m_inspectionTargets)
            close(clientConnection, connectionTargetPair.first, connectionTargetPair.second);
        m_clientConnection = nullptr;
    } else if (m_remoteInspectorConnectionToIDMap.contains(&clientConnection)) {
        uint64_t connectionID = m_remoteInspectorConnectionToIDMap.take(&clientConnection);
        m_idToRemoteInspectorConnectionMap.remove(connectionID);
        // Send an empty target list to the clients.
        Vector<SocketConnection*> clientConnections = { m_automationConnection, m_clientConnection };
        for (auto* connection : clientConnections) {
            if (!connection)
                continue;
            connection->sendMessage("SetTargetList", g_variant_new("(t@a(tsssb))", connectionID, g_variant_new_array(G_VARIANT_TYPE("(tsssb)"), nullptr, 0)));
        }
    }
    m_connections.remove(&clientConnection);
}

void RemoteInspectorServer::sendMessageToBackend(SocketConnection& clientConnection, uint64_t connectionID, uint64_t targetID, const char* message)
{
    ASSERT(m_clientConnection == &clientConnection || m_automationConnection == &clientConnection);
    ASSERT(m_idToRemoteInspectorConnectionMap.contains(connectionID));
    if (&clientConnection == m_automationConnection) {
        RemoteInspector::singleton().sendMessageToTarget(targetID, message);
        return;
    }

    m_idToRemoteInspectorConnectionMap.get(connectionID)->sendMessage("SendMessageToTarget", g_variant_new("(t&s)", targetID, message));
}

void RemoteInspectorServer::sendMessageToFrontend(SocketConnection& remoteInspectorConnection, uint64_t targetID, const char* message)
{
    ASSERT(m_connections.contains(&remoteInspectorConnection));
    ASSERT(m_remoteInspectorConnectionToIDMap.contains(&remoteInspectorConnection));

    uint64_t connectionID = m_remoteInspectorConnectionToIDMap.get(&remoteInspectorConnection);
    auto connectionTargetPair = std::make_pair(connectionID, targetID);
    ASSERT(m_automationTargets.contains(connectionTargetPair) || m_inspectionTargets.contains(connectionTargetPair));
    SocketConnection* clientConnection = m_inspectionTargets.contains(connectionTargetPair) ? m_clientConnection : m_automationConnection;
    ASSERT(clientConnection);
    clientConnection->sendMessage("SendMessageToFrontend", g_variant_new("(tt&s)", connectionID, targetID, message));
}

} // namespace Inspector

#endif // ENABLE(REMOTE_INSPECTOR)
