#!/usr/bin/env python3

import grpc


async def test_get(app_client):
    app_client.Insert("oaoa", "yaya")

    response = app_client.Get("oaoa")

    assert response == "yaya"


async def test_not_found(app_client):
    response = app_client.Get("oaoa")

    assert response == (grpc.StatusCode.NOT_FOUND, 'TODO')
