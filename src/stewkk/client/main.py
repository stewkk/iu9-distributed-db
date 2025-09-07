#!/usr/bin/env python3

from Client import Client

def main():
    client = Client("localhost", 50051)
    client.Insert("oaoa", "yay")
    print(client.Get("oaoa"))

if __name__ == "__main__":
    main()
