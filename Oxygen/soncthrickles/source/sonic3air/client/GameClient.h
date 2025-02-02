/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2022 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "sonic3air/client/GhostSync.h"
#include "sonic3air/client/UpdateCheck.h"

#include "oxygen_netcore/network/ServerClientBase.h"
#include "oxygen_netcore/network/ConnectionManager.h"
#include "oxygen_netcore/serverclient/Packets.h"


class GameClient : public ServerClientBase, public SingleInstance<GameClient>
{
public:
	GameClient();
	~GameClient();

	NetConnection& getServerConnection() { return mServerConnection; }
	GhostSync& getGhostSync()			 { return mGhostSync; }
	UpdateCheck& getUpdateCheck()		 { return mUpdateCheck; }

	void setupClient();
	void updateClient(float timeElapsed);

protected:
	virtual NetConnection* createNetConnection(ConnectionManager& connectionManager, const SocketAddress& senderAddress) override
	{
		// Do not allow incoming connections
		return nullptr;
	}

	virtual void destroyNetConnection(NetConnection& connection) override
	{
		RMX_ASSERT(false, "This should never get called");
	}

	virtual bool onReceivedPacket(ReceivedPacketEvaluation& evaluation) override;

private:
	enum class State
	{
		NONE,
		STARTED,
		READY,
	};

private:
	void startConnectingToServer(uint64 currentTimestamp);

private:
	UDPSocket mSocket;
	ConnectionManager mConnectionManager;
	NetConnection mServerConnection;
	State mState = State::NONE;
	uint64 mLastConnectionAttemptTimestamp = 0;

	GhostSync mGhostSync;
	UpdateCheck mUpdateCheck;
	network::GetServerFeaturesRequest mGetServerFeaturesRequest;
};
