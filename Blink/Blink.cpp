#include "Blink.h"

    struct {
        std::string blink_zero = "tcp.DstPort == 25565 && tcp.PayloadLength > 35 && tcp.PayloadLength < 1400"; //Blocks outbound packets
        std::string blink_one = "tcp.SrcPort == 25565"; //Blocks inbound packets
        std::vector<std::string> packets;
        std::vector<UINT> packet_len;
        std::vector<WINDIVERT_ADDRESS> addr;
        bool blink_loop = false;
    }blink_vectors;

    void soundSignal() { //Allows you to know when you're blinking
        while (blink_vectors.blink_loop) {
            Beep(100, 30);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

    void blink(int mode, int input_delay, int release_delay) {

        HANDLE handle = NULL;
        UINT packetLen;
        char packet[0xFFFF];
        WINDIVERT_ADDRESS addr;
        unsigned int blink_timer = 0;

        switch (mode) {
        case 0:
            handle = WinDivertOpen(blink_vectors.blink_zero.c_str(), WINDIVERT_LAYER_NETWORK, 0, 0); //Blink that allows you to see players move
            break;
        case 1: //Blink that won't allow you te see player move
        case 2: //Fake lag
            handle = WinDivertOpen(blink_vectors.blink_one.c_str(), WINDIVERT_LAYER_NETWORK, 0, 0);
            break;
        }

        while (blink_vectors.blink_loop) {

            if (WinDivertRecv(handle, packet, sizeof(packet), &packetLen, &addr)) { //Capture packets
                blink_vectors.packets.push_back(std::string(packet, packetLen));
                blink_vectors.packet_len.push_back(packetLen);
                blink_vectors.addr.push_back(addr);
            }

            if (mode == 2) {
                auto start = std::chrono::high_resolution_clock::now();
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
                auto end = std::chrono::high_resolution_clock::now();
                blink_timer += (end - start).count() / 1000000;
            }

            if (blink_timer >= input_delay) {
                for (int i = 0; i < blink_vectors.packets.size(); i++) {
                    WinDivertSend(handle, blink_vectors.packets[i].c_str(), blink_vectors.packet_len[i], NULL, &blink_vectors.addr[i]);
                }
                blink_timer = 0;
                blink_vectors.packets.clear();
                blink_vectors.packet_len.clear();
                blink_vectors.addr.clear();
                WinDivertClose(handle);
                std::this_thread::sleep_for(std::chrono::milliseconds(release_delay));
                handle = WinDivertOpen(blink_vectors.blink_one.c_str(), WINDIVERT_LAYER_NETWORK, 0, 0);
            }

            if (mode != 2) {
                Beep(100, 30);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        for (int i = 0; i < blink_vectors.packets.size(); i++) {
            WinDivertSend(handle, blink_vectors.packets[i].c_str(), blink_vectors.packet_len[i], NULL, &blink_vectors.addr[i]); //Releases captured packets
        }

        WinDivertClose(handle);
    }

    void runBlink(int mode, int capture_delay, int release_delay) { //Capture delay and release delay are variables use for fakelag
        if (!blink_vectors.blink_loop) {
            blink_vectors.blink_loop = true;
            std::thread(soundSignal).detach();
            std::thread(blink, mode, capture_delay, release_delay).detach();
        }
        else blink_vectors.blink_loop = false;
    }