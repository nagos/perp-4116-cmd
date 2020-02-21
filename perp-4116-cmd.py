#!/usr/bin/env python3

import argparse
import socket
import time
import struct

handshake = b"PERP\x00\x01\x00\x00"

def send_button(sock, button, value):
        data = struct.pack('>IxBHBH', 11, 2, button, 1, value)
        sock.sendall(data)

if __name__ == "__main__":
        parser = argparse.ArgumentParser(description='PERP-4116-CMD')
        parser.add_argument('ip', help='IP Address')
        parser.add_argument('port', type=int, help='Port')
        parser.add_argument('button', type=int, help='Button')
        args = parser.parse_args()
        print(args)

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((args.ip, args.port))
        sock.sendall(handshake)
        data = sock.recv(len(handshake))
        time.sleep(0.1)

        send_button(sock, args.button, 1)
        time.sleep(1)
        send_button(sock, args.button, 0)

        time.sleep(1)
        sock.close()
