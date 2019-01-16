#include "stdafx.h"
#include "ClientENet.h"
#include <Windows.h>

#include "application.h"
#include "globals.h"
#include "vec2.h"
#include "asserts.h"
#include <iostream>
#include <vector>
#include "entityplayer.h"
#include "NetMessageChangePos.h"
#include "NetMessageNewPlayer.h"
#include "NetMessageNewCircle.h"
#include "NetMessagePlayerId.h"
#include "NetMessageEntityDead.h"
#include "NetMessageGameSnapshot.h"
#include <map>
#include <ctime>
#include <algorithm>

using namespace ENet;

int _tmain(int argc, _TCHAR* argv[])
{
	Sleep(100);
    CClienteENet* pClient = new CClienteENet();
    pClient->Init();

    CPeerENet* pPeer = pClient->Connect("127.0.0.1", 1234, 2);

	Application * pApplication = Application::Instance();
	ASSERT(pApplication);

	IWindowManager * pWindowManager = pApplication->getWindowManager();
	ASSERT(pWindowManager);
	fprintf(stdout, "Window manager initialized.\n");

	IEventManager  * pInputManager  = pApplication->getEventManager();
	ASSERT(pInputManager);
	fprintf(stdout, "Event manager initialized.\n");

	std::map<int, Entity*> gameEntities;

	uint32_t seed = static_cast<uint32_t>(time(0));
	srand(seed);
	float red   = rand() % 256 / 255.f;
	float green = rand() % 256 / 255.f;
	float blue  = rand() % 256 / 255.f;
	float posX = static_cast<float>(rand() % (GlobalConstants::SCREEN_WIDTH + 1));
	float posY = static_cast<float>(rand() % (GlobalConstants::SCREEN_HEIGHT + 1));
	// EntityPlayer * pPlayer = new EntityPlayer(GlobalConstants::SCREEN_WIDTH/2.f, GlobalConstants::SCREEN_HEIGHT/2.f, GlobalConstants::PLAYER_INITIAL_RADIUS, GlobalConstants::PLAYER_INITIAL_SPEED, red, green, blue);
	EntityPlayer * pPlayer = new EntityPlayer(posX, posY, GlobalConstants::PLAYER_INITIAL_RADIUS, GlobalConstants::PLAYER_INITIAL_SPEED, red, green, blue);
	pPlayer->init(true);

	NetMessageNewPlayer message(*pPlayer);
	char serializedPlayerMessage[sizeof(NetMessageNewPlayer)];
	message.serialize(serializedPlayerMessage);
	pClient->SendData(pPeer, serializedPlayerMessage, sizeof(NetMessageNewPlayer), 0, true);

	// Bucle principal
	double lastTime = pWindowManager->getTime();
	int screenWidth, screenHeight;

	float count = 0.f;
	const float MOVEMENT_REQUEST_TIME = 1.f/60.f;
	bool playerDead = false;
	// El player no puede empezar a recibir mensajes hasta que no tiene id.
	bool allowedToPlay = false;
	bool serverConnected = true;

	while (!pWindowManager->windowShouldClose() && !playerDead && serverConnected) {
		// Actualizamos delta
		float deltaTime = static_cast<float>(pWindowManager->getTime() - lastTime);
		count += deltaTime;
		lastTime = pWindowManager->getTime();

		// Actualizamos tamaño de ventana
		pWindowManager->getWindowSize(screenWidth, screenHeight);
		pWindowManager->setViewport(0, 0, screenWidth, screenHeight);
		pWindowManager->setResolution(GlobalConstants::SCREEN_WIDTH, GlobalConstants::SCREEN_HEIGHT);

		/********************************/

		// Actualizamos de logica del programa
		/*if (!playerDead) {
			pPlayer->update(deltaTime);
		}*/

		/********************************/

		// Actualizamos el estado de las entidades
		if (allowedToPlay) {
			if (count >= MOVEMENT_REQUEST_TIME) {
				NetMessageChangePos message(pPlayer->getId(), pPlayer->getTargetPosX(), pPlayer->getTargetPosY());
				char serializedPosMessage[sizeof(NetMessageChangePos)];
				message.serialize(serializedPosMessage);
				pClient->SendData(pPeer, serializedPosMessage, sizeof(NetMessageChangePos), 0, true);
				count = 0.f;
			}
		}

		std::vector<CPacketENet*>  incomingPackets;
		pClient->Service(incomingPackets, 0);
		if (incomingPackets.size()) {
			for (auto packet : incomingPackets) {
				if (ENet::EPacketType::DATA == packet->GetType()) {
					NetMessage message;
					message.deserialize(reinterpret_cast<char *>(packet->GetData()));
					switch (message.Type) {
						case NetMessage::NetMessageType::NETMSG_PLAYER_ID: {
							// Se actualiza la entity de player con su id.
							NetMessagePlayerId playerIdMessage;
							playerIdMessage.deserialize(reinterpret_cast<char *>(packet->GetData()));
							pPlayer->setId(playerIdMessage.id);

							// Incluimos al player entre las entidades de juego una vez tiene id y lo autorizamos a jugar
							gameEntities[pPlayer->getId()] = pPlayer;
							allowedToPlay = true;
							
							// Se envia de vuelta el mensaje al servidor para que este envie el snapshot inicial
							pClient->SendData(pPeer, packet->GetData(), packet->GetDataLength(), packet->GetChannel(), true);

							break;
						}

						case NetMessage::NetMessageType::NETMSG_NEW_CIRCLE: {
							if (allowedToPlay) {
								// Se crea un nuevo circle.
								NetMessageNewCircle newCircleMessage;
								newCircleMessage.deserialize(reinterpret_cast<char *>(packet->GetData()));
								EntityCircle* newCircle = new EntityCircle(newCircleMessage.serializerCircle.id, newCircleMessage.serializerCircle.posX, newCircleMessage.serializerCircle.posY, newCircleMessage.serializerCircle.radius, newCircleMessage.serializerCircle.red, newCircleMessage.serializerCircle.green, newCircleMessage.serializerCircle.blue);
								newCircle->init(false);
								gameEntities[newCircle->getId()] = newCircle;
							}

							break;
						}

						case NetMessage::NetMessageType::NETMSG_NEW_PLAYER: {
							if (allowedToPlay) {
								// Se crea un nuevo player.
								NetMessageNewPlayer newPlayerMessage;
								newPlayerMessage.deserialize(reinterpret_cast<char *>(packet->GetData()));

								if (newPlayerMessage.serializedPlayer.id != pPlayer->getId()) {
									EntityPlayer* newPlayer = new EntityPlayer(newPlayerMessage.serializedPlayer.id, newPlayerMessage.serializedPlayer.posX, newPlayerMessage.serializedPlayer.posY, newPlayerMessage.serializedPlayer.radius, newPlayerMessage.serializedPlayer.speed, newPlayerMessage.serializedPlayer.red, newPlayerMessage.serializedPlayer.green, newPlayerMessage.serializedPlayer.blue);
									newPlayer->init(false);
									gameEntities[newPlayer->getId()] = newPlayer;
								}
							}

							break;
						}

						case NetMessage::NetMessageType::NETMSG_ENTITY_DEAD: {
							if (allowedToPlay) {
								// Se actualiza la entity de player con su id.
								NetMessageEntityDead entityDeadMessage;
								entityDeadMessage.deserialize(reinterpret_cast<char *>(packet->GetData()));

								// Si el player muere, se acaba la partida.
								if (entityDeadMessage.id == pPlayer->getId()) {
									playerDead = true;
								}


								if (gameEntities.end() != gameEntities.find(entityDeadMessage.id))
								{
									// Se elimina la entidad muerta
									delete gameEntities[entityDeadMessage.id];
									gameEntities.erase(entityDeadMessage.id);
								}
							}

							break;
						}

						case NetMessage::NetMessageType::NETMSG_GAME_SNAPSHOT:
						case NetMessage::NetMessageType::NETMSG_GAME_INITIAL_SNAPSHOT: {
							if (allowedToPlay) {
								// Se actualiza el estado del juego
								std::vector<int> entityIdsDead;
								for (auto& pair : gameEntities) {
									entityIdsDead.push_back(pair.first);
								}

								char* packetData = reinterpret_cast<char *>(packet->GetData());
								NetMessageGameSnapshot gameSnapshotMessage;
								gameSnapshotMessage.deserialize(packetData);
								int numEntities = gameSnapshotMessage.numEntities;
									
								if (NetMessage::NetMessageType::NETMSG_GAME_SNAPSHOT == gameSnapshotMessage.Type) {
									for (int i = 0; i < numEntities; ++i) {
										EntitySerializer* serverEntity = gameSnapshotMessage.getDeserializedEntity(packetData, i);
										// Se actualizan las entidades existentes
										if (gameEntities.end() != gameEntities.find(serverEntity->id)) {
											Entity* gameEntity = gameEntities[serverEntity->id];
											gameEntity->setPosX(serverEntity->posX);
											gameEntity->setPosY(serverEntity->posY);

											// Si es player, actualizar tamanno y velocidad
											if (Entity::E_Type::Player == gameEntity->getType()) {
												EntityPlayerSerializer* serverPlayer = static_cast<EntityPlayerSerializer*>(serverEntity);
												EntityPlayer* gamePlayer = static_cast<EntityPlayer*>(gameEntity);
												gamePlayer->setRadius(serverPlayer->radius);
												gamePlayer->setSpeed(serverPlayer->speed);
											}
										}
									}
								}
								else if (NetMessage::NetMessageType::NETMSG_GAME_INITIAL_SNAPSHOT == gameSnapshotMessage.Type) {
									for (int i = numEntities - 1; i >= 0; --i) {
										EntitySerializer* serverEntity = gameSnapshotMessage.getDeserializedEntity(reinterpret_cast<char *>(packet->GetData()), i);
										// Se incluyen en el cliente las entidades que ya estaban en juego
										if (gameEntities.end() == gameEntities.find(serverEntity->id)) {
											if (serverEntity->id != pPlayer->getId()) {
												switch (serverEntity->type) {
													case Entity::E_Type::Player: {
														EntityPlayerSerializer* serverPlayer = static_cast<EntityPlayerSerializer*>(serverEntity);
														EntityPlayer* newPlayer = new EntityPlayer(serverPlayer->id, serverPlayer->posX, serverPlayer->posY, serverPlayer->radius, serverPlayer->speed, serverPlayer->red, serverPlayer->green, serverPlayer->blue);
														newPlayer->init(false);
														gameEntities[newPlayer->getId()] = newPlayer;
														break;
													}
													case Entity::E_Type::Circle: {
														EntityCircleSerializer* serverCircle = static_cast<EntityCircleSerializer*>(serverEntity);
														EntityCircle* newCircle = new EntityCircle(serverCircle->id, serverCircle->posX, serverCircle->posY, serverCircle->radius, serverCircle->red, serverCircle->green, serverCircle->blue);
														newCircle->init(false);
														gameEntities[newCircle->getId()] = newCircle;
														break;
													}
												}

											}
										}
									}
								}
								
							}

							break;
						}
					}
				}
				else if (ENet::EPacketType::DISCONNECT == packet->GetType()) {
					serverConnected = false;
				}
			}
		}

		/********************************/

		// Pintado
		pWindowManager->clearColorBuffer(0.9f, 0.9f, 0.9f);
		for (auto gameEntity : gameEntities) {
			gameEntity.second->render();
		}

		/********************************/

		// Actualizamos ventana y eventos
		pWindowManager->swapBuffers();
		pWindowManager->pollEvents();
	}

	for (auto gameEntity : gameEntities) {
		delete gameEntity.second;
	}

	pClient->Disconnect(pPeer);

    return 0;
}

