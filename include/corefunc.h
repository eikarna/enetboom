#pragma once
#include "../include/log.h"
#include "enet/enet.h"
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

using namespace std;

string SERVER_HOST("");
uint16_t SERVER_PORT = 0;
bool useNewPacket = false;

/*********  hidden vars  ********/
class GrowtopiaBot {
public:
  ENetPeer *peer;
  ENetHost *client;

  long long lastreq = 0;
  bool connected = false;
  /********** user sutff ***************/

  enum {
    PACKET_STATE = 0,
    PACKET_CALL_FUNCTION,
    PACKET_UPDATE_STATUS,
    PACKET_TILE_CHANGE_REQUEST,
    PACKET_SEND_MAP_DATA,
    PACKET_SEND_TILE_UPDATE_DATA, // 5 from 0
    PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE,
    PACKET_TILE_ACTIVATE_REQUEST,
    PACKET_TILE_APPLY_DAMAGE,
    PACKET_SEND_INVENTORY_STATE,
    PACKET_ITEM_ACTIVATE_REQUEST, // 10 from 0
    PACKET_ITEM_ACTIVATE_OBJECT_REQUEST,
    PACKET_SEND_TILE_TREE_STATE,
    PACKET_MODIFY_ITEM_INVENTORY,
    PACKET_ITEM_CHANGE_OBJECT,
    PACKET_SEND_LOCK,
    PACKET_SEND_ITEM_DATABASE_DATA, // 16 from 0
    PACKET_SEND_PARTICLE_EFFECT,
    PACKET_SET_ICON_STATE,
    PACKET_ITEM_EFFECT,
    PACKET_SET_CHARACTER_STATE, // 20 from 0
    PACKET_PING_REPLY,
    PACKET_PING_REQUEST,
    PACKET_GOT_PUNCHED,
    PACKET_APP_CHECK_RESPONSE,
    PACKET_APP_INTEGRITY_FAIL,
    PACKET_DISCONNECT, // 25 from 0
    PACKET_BATTLE_JOIN,
    PACKET_BATTLE_EVEN,
    PACKET_USE_DOOR,
    PACKET_SEND_PARENTAL, // 30 from 0
    PACKET_GONE_FISHIN,
    PACKET_STEAM,
    PACKET_PET_BATTLE,
    PACKET_NPC,
    PACKET_SPECIAL, // 35 from 0
    PACKET_SEND_PARTICLE_EFFECT_V2,
    GAME_ACTIVE_ARROW_TO_ITEM,
    GAME_SELECT_TILE_INDEX // 38 from 0
  };
  enum {
    NET_MESSAGE_UNKNOWN = 0,
    NET_MESSAGE_SERVER_HELLO,
    NET_MESSAGE_GENERIC_TEXT,
    NET_MESSAGE_GAME_MESSAGE,
    NET_MESSAGE_GAME_PACKET, // 4 from 0
    NET_MESSAGE_ERROR,
    NET_MESSAGE_TRACK,
    NET_MESSAGE_CLIENT_LOG_REQUEST,
    NET_MESSAGE_CLIENT_LOG_RESPONSE,
  };

  /*********** structs declaration *********/
  struct gameupdatepacket_t {
    uint8_t m_type;
    uint8_t m_netid;
    uint8_t m_jump_amount;
    uint8_t m_count;
    int32_t m_player_flags;
    int32_t m_item;
    int32_t m_packet_flags;
    float m_struct_flags;
    int32_t m_int_data;
    float m_vec_x;
    float m_vec_y;
    float m_vec2_x;
    float m_vec2_y;
    float m_particle_time;
    uint32_t m_state1;
    uint32_t m_state2;
    uint32_t m_data_size;
    uint32_t m_data;
  };

  /********* user funcs  *********/
  void userLoop();
  void userInit();
  // void onLoginRequested();
  void WhenConnected();
  void WhenDisconnected();
  /********* user funcs  *********/

  typedef struct gametankpacket_t {
    int32_t m_type;
    char m_data;
  } gametextpacket_t;
  /******************* enet core *********************/
  void SendPacket2(int32_t type, uint8_t *data, int32_t len) {

    auto packet = enet_packet_create(0, len + 5, ENET_PACKET_FLAG_RELIABLE);
    auto game_packet = (gametextpacket_t *)packet->data;
    game_packet->m_type = type;
    if (data)
      memcpy(&game_packet->m_data, data, len);

    memset(&game_packet->m_data + len, 0, 1);
    enet_peer_send(peer, 0, packet);
  }

  void SendPacket(int a1, string a2, ENetPeer *enetPeer) {
    if (enetPeer) {
      ENetPacket *v3 = enet_packet_create(0, a2.length() + 5, 1);
      memcpy(v3->data, &a1, 4);
      //*(v3->data) = (DWORD)a1;
      memcpy((v3->data) + 4, a2.c_str(), a2.length());

      // cout << std::hex << (int)(char)v3->data[3] << endl;
      enet_peer_send(enetPeer, 0, v3);
    }
  }

  void connectClient() {
    client = enet_host_create(
        ENET_ADDRESS_TYPE_IPV4, NULL /* create a client host */,
        1 /* only allow 1 outgoing connection */,
        1 /* allow up 2 channels to be used, 0 and 1 */,
        0 /* 56K modem with 56 Kbps downstream bandwidth */,
        0 /* 56K modem with 14 Kbps upstream bandwidth */);
    if (client == NULL) {
      logger::Error(
          "An error occurred while trying to create an ENet client host.");
      exit(EXIT_FAILURE);
    }
    ENetAddress address;

    client->checksum = enet_crc32;
    client->usingNewPacket = useNewPacket ? 1 : 0;
    enet_host_compress_with_range_coder(client);
    enet_address_set_host(&address, ENET_ADDRESS_TYPE_IPV4,
                          SERVER_HOST.c_str());
    address.port = SERVER_PORT;
    address.type = ENET_ADDRESS_TYPE_IPV4;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect(client, &address, 1, 0);
    if (peer == NULL) {
      logger::Error("No available peers for initiating an ENet connection.");
      exit(EXIT_FAILURE);
    }
    enet_host_flush(client);
  }
  /******************* enet core *********************/

  void eventLoop() {
    ENetEvent event;
    while (true) {
      while (enet_host_service(client, &event, 36000) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_NONE:
          // cout << "No event???" << endl;
          break;
        case ENET_EVENT_TYPE_CONNECT:
          WhenConnected();
          break;
        case ENET_EVENT_TYPE_DISCONNECT:
          enet_peer_reset(peer);
          enet_host_destroy(client);
          WhenDisconnected();
          break;
        case ENET_EVENT_TYPE_RECEIVE:
          // ProcessPacket(&event, peer);
          if (event.packet) {
            uint8_t firstByte =
                event.packet->dataLength > 0 ? event.packet->data[0] : 0;
            logger::Info("Got Packet type: %d, raw: %s", firstByte,
                         event.packet);
          }
          enet_packet_destroy(event.packet);
          break;
        default:
          break;
        }
      }
      userLoop();
      std::this_thread::sleep_for(
          std::chrono::milliseconds(50)); // reduce cpu usage
    }
  }
};

void init() {
  if (enet_initialize() != 0) {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
    exit(0);
  }
  atexit(enet_deinitialize);
  srand(time(NULL));

  if (useNewPacket)
    logger::Info("Using new packet method!");
}
