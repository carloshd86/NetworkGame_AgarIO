#include "stdafx.h"
#include "ServerENet.h"
#include <Windows.h>

#include "NetMessageNewCircle.h"
#include "NetMessageNewPlayer.h"
#include "NetMessagePlayerId.h"
#include "NetMessageEntityDead.h"
#include "NetMessageChangePos.h"
#include "NetMessageGameSnapshot.h"
#include "entityplayer.h"
#include "entitycircle.h"
#include <map>
#include <ctime>
#include "globals.h"
#include <set>

using namespace ENet;

void growPlayer(EntityPlayer* player);
void makePlayerPlayerCollision(EntityPlayer* player1, EntityPlayer* player2);
void makePlayerCircleCollision(EntityPlayer* player, EntityCircle* circle);
void checkCollisions(std::map<int, Entity*>& gameEntities);

int _tmain(int argc, _TCHAR* argv[])
{
    CServerENet* pServer = new CServerENet();
	int numPlayers = 0;
	int numCircles = 0;
	std::map<int, Entity*> gameEntities;
	std::map<ENetPeer*, int> peerEntities;
	std::vector<int> deadIds;

    if (pServer->Init(1234, 5))
    {
		uint32_t seed = static_cast<uint32_t>(time(0));
		srand(seed);

		int currentEntityId = 0;

		float snapshotTimeCounter    = 0.f;
		float spawnCircleTimeCounter = 0.f;
		float deltaTime = 0.f;
		const float SEND_SNAPSHOT_MILLIS = 1000.f/60.f;
		const float SPAWN_CIRCLE_MILLIS  = 1000.f;
		const float SLEEP_MILLIS         = 1000.f/60.f;
		const int   MAX_CIRCLES          = 20;

        while (true)
        {
			// Actualizamos delta
			snapshotTimeCounter += SLEEP_MILLIS;
			spawnCircleTimeCounter += SLEEP_MILLIS;
			deltaTime = static_cast<float>(SLEEP_MILLIS/1000.f);

			/********************************/

			// Actualizacion del estado de las entidades
			deadIds.clear();
			for (auto& gameEntity : gameEntities) {
				gameEntity.second->update(deltaTime);
			}

			checkCollisions(gameEntities);

			for (auto& gameEntity : gameEntities) {
				if (gameEntity.second->getDead()) {
					// Se envia a todos los jugadores el mensaje de entidad muerta y se saca del listado
					NetMessageEntityDead entityDeadMessage(gameEntity.second->getId());
					char serializedMessage[sizeof(NetMessageEntityDead)];
					entityDeadMessage.serialize(serializedMessage);
					pServer->SendAll(serializedMessage, sizeof(NetMessageEntityDead), 0, true);

					// Se marca la entidad para eliminar
					deadIds.push_back(gameEntity.second->getId());
				}
			}

			// Se eliminan las entidades que han muerto del mapa
			for (int entityId : deadIds) {
				if(Entity::E_Type::Circle == gameEntities[entityId]->getType()) numCircles--;
				delete gameEntities[entityId];
				gameEntities.erase(entityId);
			}

			// Acciones periodicas del servicio
			if (snapshotTimeCounter >= SEND_SNAPSHOT_MILLIS) {
				snapshotTimeCounter = 0;
				if (numPlayers) {
					// Se envia a todos los clientes un snapshot de la partida
					NetMessageGameSnapshot gameSnapshotMessage(gameEntities);
					size_t messageLength = gameSnapshotMessage.getSerialBufferSize();
					char* serializedMessage = new char[messageLength];
					gameSnapshotMessage.serialize(serializedMessage);
					pServer->SendAll(serializedMessage, messageLength, 0, false);
					delete[] serializedMessage;
				}
			}

			if (spawnCircleTimeCounter >= SPAWN_CIRCLE_MILLIS) {
				spawnCircleTimeCounter = 0.f;

				if (numCircles < MAX_CIRCLES) {
					// Se incrementa el id de entidad
					currentEntityId++;

					// Se crea el circulo
					float posX = static_cast<float>(rand() % (GlobalConstants::SCREEN_WIDTH + 1));
					float posY = static_cast<float>(rand() % (GlobalConstants::SCREEN_HEIGHT + 1));
					float red = rand() % 256 / 255.f;
					float green = rand() % 256 / 255.f;
					float blue = rand() % 256 / 255.f;
					EntityCircle* newCircle = new EntityCircle(currentEntityId, posX, posY, GlobalConstants::CIRCLE_RADIUS, red, green, blue);
					gameEntities[currentEntityId] = newCircle;

					numCircles++;

					// Se envia a todos los jugadores el mensaje de nueva entidad
					NetMessageNewCircle newCircleMessage(*newCircle);
					char serializedMessage[sizeof(NetMessageNewCircle)];
					newCircleMessage.serialize(serializedMessage);
					pServer->SendAll(serializedMessage, sizeof(NetMessageNewCircle), 0, true);
				}
			}
			
			// Lectura de paquetes entrantes
            std::vector<CPacketENet*>  incomingPackets;
            pServer->Service(incomingPackets, 0);

			if (incomingPackets.size()) {
				for (auto packet : incomingPackets) {
					if (ENet::EPacketType::DATA == packet->GetType()) {
						NetMessage message;
						message.deserialize(reinterpret_cast<char *>(packet->GetData()));
						switch (message.Type) {
							case NetMessage::NetMessageType::NETMSG_NEW_PLAYER: {
								// Se incrementa el id de entidad
								currentEntityId++;

								// Se guarda el nuevo player
								NetMessageNewPlayer newPlayerMessageRec;
								newPlayerMessageRec.deserialize(reinterpret_cast<char *>(packet->GetData()));
								EntityPlayerSerializer serializedPlayer = newPlayerMessageRec.serializedPlayer;
								EntityPlayer * newPlayer = new EntityPlayer(currentEntityId, serializedPlayer.posX, serializedPlayer.posY, serializedPlayer.radius, serializedPlayer.speed, serializedPlayer.red, serializedPlayer.green, serializedPlayer.blue);
								gameEntities[currentEntityId] = newPlayer;
								peerEntities[packet->GetPeer()->GetENetPeer()] = currentEntityId;

								// Se incrementa el contador de jugadores
								++numPlayers;

								// Se envia a todos los jugadores el mensaje de nueva entidad
								NetMessageNewPlayer newPlayerMessageSend(*newPlayer);
								char serializedPlayerMessage[sizeof(NetMessageNewPlayer)];
								newPlayerMessageSend.serialize(serializedPlayerMessage);
								pServer->SendAll(serializedPlayerMessage, sizeof(NetMessageNewPlayer), packet->GetChannel(), true);

								// Se envia al player su id
								NetMessagePlayerId sendIdMessage(currentEntityId);
								char serializedIdMessage[sizeof(NetMessagePlayerId)];
								sendIdMessage.serialize(serializedIdMessage);
								pServer->SendData(packet->GetPeer(), serializedIdMessage, sizeof(NetMessagePlayerId), packet->GetChannel(), true);

								break;
							}

							case NetMessage::NetMessageType::NETMSG_PLAYER_ID: {
								// Se envia al player que ya ha recibido su id el snapshot inicial de la partida
								NetMessageGameSnapshot gameSnapshotMessage(gameEntities);
								gameSnapshotMessage.Type = NetMessage::NetMessageType::NETMSG_GAME_INITIAL_SNAPSHOT;
								size_t messageLength = gameSnapshotMessage.getSerialBufferSize();
								char* serializedMessage = new char[messageLength];
								gameSnapshotMessage.serialize(serializedMessage);
								pServer->SendData(packet->GetPeer(), serializedMessage, messageLength, packet->GetChannel(), true);
								delete[] serializedMessage;
							}

							case NetMessage::NetMessageType::NETMSG_CHANGE_POS: {
								NetMessageChangePos changePosMessage;
								changePosMessage.deserialize(reinterpret_cast<char *>(packet->GetData()));
								if (gameEntities.end() != gameEntities.find(changePosMessage.id)) {
									Entity* gameEntity = gameEntities[changePosMessage.id];
									if (Entity::E_Type::Player == gameEntity->getType()) {
										EntityPlayer* gamePlayer = static_cast<EntityPlayer*>(gameEntity);
										gamePlayer->setTargetPosX(changePosMessage.targetPosX);
										gamePlayer->setTargetPosY(changePosMessage.targetPosY);
									}
								}
							}
						}
					}
					else if (ENet::EPacketType::DISCONNECT == packet->GetType()) {
						// Se mata la entidad
						int entityId = peerEntities[packet->GetPeer()->GetENetPeer()];
						if (gameEntities.end() != gameEntities.find(entityId)) {
							gameEntities[entityId]->setDead(true);
						}
						peerEntities.erase(packet->GetPeer()->GetENetPeer());
					}
				}
			}

			/********************************/

			Sleep(static_cast<DWORD>(SLEEP_MILLIS));
        }
    }
    else
    {
        fprintf(stdout, "Server could not be initialized.\n");
    }

	for (auto player : gameEntities) {
		delete player.second;
	}

    return 0;
}

// *************************************************

void growPlayer(EntityPlayer* player) {
	player->setRadius(player->getRadius() + GlobalConstants::PLAYER_RADIUS_INCREMENT);
	float speed = player->getSpeed() - GlobalConstants::PLAYER_SPEED_DECREMENT;
	if (speed < GlobalConstants::PLAYER_MIN_SPEED) speed = GlobalConstants::PLAYER_MIN_SPEED;
	player->setSpeed(speed);
}

// *************************************************

void makePlayerPlayerCollision(EntityPlayer* player1, EntityPlayer* player2) {
	if (player1->getRadius() > player2->getRadius()) {
		growPlayer(player1);
		player2->setDead(true);
	}
	else if (player2->getRadius() > player1->getRadius()) {
		growPlayer(player2);
		player1->setDead(true);
	}
}

// *************************************************

void makePlayerCircleCollision(EntityPlayer* player, EntityCircle* circle) {
	growPlayer(player);
	circle->setDead(true);
}

// *************************************************

void checkCollisions(std::map<int, Entity*>& gameEntities) {
	std::set<std::pair<int, int>> checkedCollisions;
	for (auto& gameEntity1 : gameEntities) {
		for (auto& gameEntity2 : gameEntities) {
			if (gameEntity1.first != gameEntity2.first && !gameEntity1.second->getDead() && !gameEntity2.second->getDead()) {
				std::pair<int, int> collisionCheck = std::make_pair(gameEntity1.first, gameEntity2.first);
				if (checkedCollisions.end() == checkedCollisions.find(collisionCheck)) {
					std::pair<int, int> collisionCheckInverse = std::make_pair(gameEntity2.first, gameEntity1.first);
					checkedCollisions.insert(collisionCheck);
					checkedCollisions.insert(collisionCheckInverse);

					if (gameEntity1.second->collides(*gameEntity2.second)) {
						switch(gameEntity1.second->getType()) {
							case Entity::E_Type::Circle: {
								if (Entity::E_Type::Player == gameEntity2.second->getType()) {
									makePlayerCircleCollision(static_cast<EntityPlayer*>(gameEntity2.second), static_cast<EntityCircle*>(gameEntity1.second));
								}
								break;
							}
							case Entity::E_Type::Player: {
								if (Entity::E_Type::Circle == gameEntity2.second->getType()) {
									makePlayerCircleCollision(static_cast<EntityPlayer*>(gameEntity1.second), static_cast<EntityCircle*>(gameEntity2.second));
								}
								if (Entity::E_Type::Player == gameEntity2.second->getType()) {
									makePlayerPlayerCollision(static_cast<EntityPlayer*>(gameEntity1.second), static_cast<EntityPlayer*>(gameEntity2.second));
								}
								break;
							}
						}
					}
				}
			}
		}
	}
}